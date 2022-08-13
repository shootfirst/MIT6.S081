// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.


#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"

#define HASH_MOD_NUM  13
struct {
  struct spinlock lock;
  struct buf buf[NBUF];
  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
  // struct buf head;
  struct buf bucket[HASH_MOD_NUM];
  struct spinlock bucketlock[HASH_MOD_NUM];
} bcache;

void
binit(void)
{
  struct buf *b;

  initlock(&bcache.lock, "bcache");

  char buff[10];
  int i;
  for (i = 0; i < HASH_MOD_NUM; i++) {
    snprintf(buff, 10, "bcache%d", i);
    initlock(&bcache.bucketlock[i], buff);
    bcache.bucket[i].prev = &bcache.bucket[i];
    bcache.bucket[i].next = &bcache.bucket[i];
  }

  for(b = bcache.buf, i = 0; b < bcache.buf+NBUF; b++, i++){
    b->next = bcache.bucket[i % HASH_MOD_NUM].next;
    b->prev = &bcache.bucket[i % HASH_MOD_NUM];
    initsleeplock(&b->lock, "buffer");
    bcache.bucket[i % HASH_MOD_NUM].next->prev = b;
    bcache.bucket[i % HASH_MOD_NUM].next = b;
  }
}


// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf*
bget(uint dev, uint blockno)
{
  struct buf *b;
  int i = blockno % HASH_MOD_NUM;

  acquire(&bcache.bucketlock[i]);

  // Is the block already cached?
  for(b = bcache.bucket[i].next; b != &bcache.bucket[i]; b = b->next){
    if(b->dev == dev && b->blockno == blockno){
      b->refcnt++;
      release(&bcache.bucketlock[i]);
      acquiresleep(&b->lock);
      return b;
    }
  }

  // Not cached.
  // Recycle the least recently used (LRU) unused buffer.
  for(b = bcache.bucket[i].prev; b != &bcache.bucket[i]; b = b->prev){
    if(b->refcnt == 0) {
      b->dev = dev;
      b->blockno = blockno;
      b->valid = 0;
      b->refcnt = 1;
      release(&bcache.bucketlock[i]);
      acquiresleep(&b->lock);
      return b;
    }
  }

  // Steal from other bucket
  for (int j = 1; j < HASH_MOD_NUM; j++) {
    int k = (j + i) % HASH_MOD_NUM;
    acquire(&bcache.bucketlock[k]);
    for(b = bcache.bucket[k].prev; b != &bcache.bucket[k]; b = b->prev){
      if(b->refcnt == 0) {
        b->prev->next = b->next;
        b->next->prev = b->prev;

        b->prev = &bcache.bucket[i];
        b->next = bcache.bucket[i].next;
        bcache.bucket[i].next->prev = b;
        bcache.bucket[i].next = b;

        b->dev = dev;
        b->blockno = blockno;
        b->valid = 0;
        b->refcnt = 1;

        release(&bcache.bucketlock[i]);
        release(&bcache.bucketlock[k]);
        acquiresleep(&b->lock);
        return b;
      }
    }
    release(&bcache.bucketlock[k]);
  }
  
  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
struct buf*
bread(uint dev, uint blockno)
{
  struct buf *b;

  b = bget(dev, blockno);
  if(!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
void
brelse(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("brelse");

  int i = b->blockno % HASH_MOD_NUM;
  releasesleep(&b->lock);
  acquire(&bcache.bucketlock[i]);
  b->refcnt--;
  if (b->refcnt == 0) {
    // no one is waiting for it.
    b->next->prev = b->prev;
    b->prev->next = b->next;
    b->next = bcache.bucket[i].next;
    b->prev = &bcache.bucket[i];
    bcache.bucket[i].next->prev = b;
    bcache.bucket[i].next = b;
  }
  
  release(&bcache.bucketlock[i]);
}

void
bpin(struct buf *b) {
  acquire(&bcache.lock);
  b->refcnt++;
  release(&bcache.lock);
}

void
bunpin(struct buf *b) {
  acquire(&bcache.lock);
  b->refcnt--;
  release(&bcache.lock);
}


