// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

#define FREE_PAGE_NUM 32723
uint8 referencecnt[FREE_PAGE_NUM];

// if return 255, means the pa is too big
uint8
getrefcnt(uint64 pa) 
{ 
  int index = PGROUNDDOWN(pa) / PGSIZE - PGROUNDUP((uint64)end) / PGSIZE;
  if (index >= FREE_PAGE_NUM)
    return 255;
    
  return referencecnt[index];
}

// if return 255, means the pa is too big
uint8
addrefcnt(uint64 pa) 
{ 
  int index = PGROUNDDOWN(pa) / PGSIZE - PGROUNDUP((uint64)end) / PGSIZE;
  if (index >= FREE_PAGE_NUM)
    return 255;
  
  ++referencecnt[index];
  return referencecnt[index];
}

// if return 255, means the pa is too big
uint8
subrefcnt(uint64 pa) 
{ 
  int index = PGROUNDDOWN(pa) / PGSIZE - PGROUNDUP((uint64)end) / PGSIZE;
  if (index >= FREE_PAGE_NUM)
    return 255;
  
  if (referencecnt[index] == 0)
    return 0;

  --referencecnt[index];
  return referencecnt[index];
}



void
kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE){
    referencecnt[PGROUNDDOWN((uint64)p) / PGSIZE - PGROUNDUP((uint64)end) / PGSIZE] = 1;
    // here we will sub to 0
    kfree(p);
  }
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");
  
  subrefcnt((uint64)pa);
  // we only free the page when reference is zero
  if (getrefcnt((uint64)pa) > 0) {
    return;
  }

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r)
    addrefcnt((uint64)r);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}
