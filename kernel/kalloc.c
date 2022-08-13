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
} kmem[NCPU];

void
kinit()
{
  char buf[10];
  for (int i = 0; i < NCPU; i++) {
    snprintf(buf, 10, "kmem%d", i);
    initlock(&kmem[i].lock, buf);
  }
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
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

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  int i = (((uint64)pa - PGROUNDUP((uint64)end)) / PGSIZE) % NCPU;
  
  acquire(&kmem[i].lock);
  r->next = kmem[i].freelist;
  kmem[i].freelist = r;
  release(&kmem[i].lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  push_off();
  int cpu = cpuid();
  pop_off();

  acquire(&kmem[cpu].lock);
  r = kmem[cpu].freelist;
  if(r)
    kmem[cpu].freelist = r->next;
  else 
  {
    struct run *p;
    for (int m = 1; m < NCPU; m++) {
      int i = (m + cpu) % NCPU;
      acquire(&kmem[i].lock);
      p = kmem[i].freelist;
      if (p == 0) {
        release(&kmem[i].lock);
        continue;
      }
      kmem[cpu].freelist = p;
      for (int j = 0; j < 128; j++) {
        if(p->next)
          p = p->next;
        else 
          break;
      }
      kmem[i].freelist = p->next;
      p->next = 0;
      release(&kmem[i].lock);
      break;
      
    }
      
    r = kmem[cpu].freelist;
    if (r) 
      kmem[cpu].freelist = r->next;
  }
  release(&kmem[cpu].lock);
  
  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}


