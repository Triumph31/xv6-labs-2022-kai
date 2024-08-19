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
/*// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf*
bget(uint dev, uint blockno)
{
  struct buf *b;
  int hi = hash(blockno);
 
  // Find cache from bucket hi.
  acquire(&bcache.bucket_lock[hi]);
  for(b = bcache.bucket[hi].next; b != &bcache.bucket[hi]; b = b->next) {
    if(b->dev == dev && b->blockno == blockno) {
      b->refcnt++;
      release(&bcache.bucket_lock[hi]);
      acquiresleep(&b->lock);
      return b;
    }
  }
  // Release lock hi, avoid deadlock.
  release(&bcache.bucket_lock[hi]);

  // Steal lock makes the eviction serialized in bget.
  acquire(&bcache.lock);

  // Check again cache to maintain the invariant that at most one copy of each block is cached.
  acquire(&bcache.bucket_lock[hi]);
  for(b = bcache.bucket[hi].next; b != &bcache.bucket[hi]; b = b->next) {
    if(b->dev == dev && b->blockno == blockno) {
      b->refcnt++;
      release(&bcache.bucket_lock[hi]);
      release(&bcache.lock);
      acquiresleep(&b->lock);
      return b;
    }
  }
  release(&bcache.bucket_lock[hi]);
 
  struct buf *minb = 0;
  uint min_ticks = ~0;

  // Find Recycle the least recently used (LRU) unused buffer.
  for(int i = 0; i < NBUCKET; i++) {
    acquire(&bcache.bucket_lock[i]);
    int find = 0;
    for(b = bcache.bucket[i].next; b != &bcache.bucket[i]; b = b->next) {
      if(b->refcnt == 0 && b->ticks < min_ticks) {
        if(minb != 0) {
          int last = hash(minb->blockno);
          if(last != i)
            release(&bcache.bucket_lock[last]);
        }
       
        min_ticks = b->ticks;
        minb = b;
        find = 1;
      }
    }

    if(!find)
      release(&bcache.bucket_lock[i]);
  }

  if(minb == 0)
    panic("bget: no buffers");
 
  int minb_i = hash(minb->blockno);

  minb->dev = dev;
  minb->blockno = blockno;
  minb->valid = 0;
  minb->refcnt = 1;

  if (minb_i != hi) {
    minb->prev->next = minb->next;
    minb->next->prev = minb->prev;
  }
  release(&bcache.bucket_lock[minb_i]);
 
  if(minb_i != hi) {
    // Move the buf from original bucket to bucket hi.
    acquire(&bcache.bucket_lock[hi]);

    minb->next = bcache.bucket[hi].next;
    minb->prev = &bcache.bucket[hi];
    bcache.bucket[hi].next->prev = minb;
    bcache.bucket[hi].next = minb;

    release(&bcache.bucket_lock[hi]);
  }

  release(&bcache.lock);

  acquiresleep(&minb->lock);

  return minb;
}*/
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"

#define NBUCKET 13

struct {
  struct spinlock lock;
  struct buf buf[NBUF];

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
  struct buf head;

  struct buf bucket[NBUCKET];
  struct spinlock bucket_lock[NBUCKET];
} bcache;

int
hash(uint blockno)
{
  return blockno % NBUCKET;
}

void
binit(void)
{
  struct buf *b;

  initlock(&bcache.lock, "bcache");
  
  for(int i=0;i<NBUCKET;i++){
    initlock(&bcache.bucket_lock[i],"bcache.bucket");
  }
  for(int i=0;i<NBUCKET;i++){
    bcache.bucket[i].next=&bcache.bucket[i];
    bcache.bucket[i].prev=&bcache.bucket[i];
  }

  // Create linked list of buffers
  //bcache.head.prev = &bcache.head;
  //bcache.head.next = &bcache.head;
  for(b = bcache.buf; b < bcache.buf+NBUF; b++){
    int i=hash(b->blockno);
    b->next = bcache.bucket[i].next;
    b->prev = &bcache.bucket[i];

    bcache.bucket[i].next->prev = b;
    bcache.bucket[i].next = b;
    b->ticks=ticks;
    initsleeplock(&b->lock, "buffer");
  }
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
/*static struct buf*
bget(uint dev, uint blockno)
{
  struct buf *b;

  acquire(&bcache.lock);

  // Is the block already cached?
  for(b = bcache.head.next; b != &bcache.head; b = b->next){
    if(b->dev == dev && b->blockno == blockno){
      b->refcnt++;
      release(&bcache.lock);
      acquiresleep(&b->lock);
      return b;
    }
  }

  // Not cached.
  // Recycle the least recently used (LRU) unused buffer.
  for(b = bcache.head.prev; b != &bcache.head; b = b->prev){
    if(b->refcnt == 0) {
      b->dev = dev;
      b->blockno = blockno;
      b->valid = 0;
      b->refcnt = 1;
      release(&bcache.lock);
      acquiresleep(&b->lock);
      return b;
    }
  }
  panic("bget: no buffers");
}*/

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
/*static struct buf*
bget(uint dev, uint blockno)
{
  struct buf *b;
  int hi = hash(blockno);
 
  // Find cache from bucket hi.
  acquire(&bcache.bucket_lock[hi]);
  for(b = bcache.bucket[hi].next; b != &bcache.bucket[hi]; b = b->next) {
    if(b->dev == dev && b->blockno == blockno) {
      b->refcnt++;
      release(&bcache.bucket_lock[hi]);
      acquiresleep(&b->lock);
      return b;
    }
  }
  // Release lock hi, avoid deadlock.
  release(&bcache.bucket_lock[hi]);

  // Steal lock makes the eviction serialized in bget.
  acquire(&bcache.lock);

  // Check again cache to maintain the invariant that at most one copy of each block is cached.*/
 // Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf*
bget(uint dev, uint blockno)
{
  struct buf *b;
  int hi = hash(blockno);
 
  // Find cache from bucket hi.
  acquire(&bcache.bucket_lock[hi]);
  for(b = bcache.bucket[hi].next; b != &bcache.bucket[hi]; b = b->next) {
    if(b->dev == dev && b->blockno == blockno) {
      b->refcnt++;
      release(&bcache.bucket_lock[hi]);
      acquiresleep(&b->lock);
      return b;
    }
  }
  // Release lock hi, avoid deadlock.
  release(&bcache.bucket_lock[hi]);

  // Steal lock makes the eviction serialized in bget.
  acquire(&bcache.lock);

  // Check again cache to maintain the invariant that at most one copy of each block is cached.
  acquire(&bcache.bucket_lock[hi]);
  for(b = bcache.bucket[hi].next; b != &bcache.bucket[hi]; b = b->next) {
    if(b->dev == dev && b->blockno == blockno) {
      b->refcnt++;
      release(&bcache.bucket_lock[hi]);
      release(&bcache.lock);
      acquiresleep(&b->lock);
      return b;
    }
  }
  release(&bcache.bucket_lock[hi]);
 
  struct buf *minb = 0;
  uint min_ticks = ~0;

  // Find Recycle the least recently used (LRU) unused buffer.
  for(int i = 0; i < NBUCKET; i++) {
    acquire(&bcache.bucket_lock[i]);
    int find = 0;
    for(b = bcache.bucket[i].next; b != &bcache.bucket[i]; b = b->next) {
      if(b->refcnt == 0 && b->ticks < min_ticks) {
        if(minb != 0) {
          int last = hash(minb->blockno);
          if(last != i)
            release(&bcache.bucket_lock[last]);
        }
       
        min_ticks = b->ticks;
        minb = b;
        find = 1;
      }
    }

    if(!find)
      release(&bcache.bucket_lock[i]);
  }

  if(minb == 0)
    panic("bget: no buffers");
 
  int minb_i = hash(minb->blockno);

  minb->dev = dev;
  minb->blockno = blockno;
  minb->valid = 0;
  minb->refcnt = 1;

  if (minb_i != hi) {
    minb->prev->next = minb->next;
    minb->next->prev = minb->prev;
  }
  release(&bcache.bucket_lock[minb_i]);
 
  if(minb_i != hi) {
    // Move the buf from original bucket to bucket hi.
    acquire(&bcache.bucket_lock[hi]);

    minb->next = bcache.bucket[hi].next;
    minb->prev = &bcache.bucket[hi];
    bcache.bucket[hi].next->prev = minb;
    bcache.bucket[hi].next = minb;

    release(&bcache.bucket_lock[hi]);
  }

  release(&bcache.lock);

  acquiresleep(&minb->lock);

  return minb;
} /*acquire(&bcache.bucket_lock[hi]);
  for(b = bcache.bucket[hi].next; b != &bcache.bucket[hi]; b = b->next) {
    if(b->dev == dev && b->blockno == blockno) {
      b->refcnt++;
      release(&bcache.bucket_lock[hi]);
      release(&bcache.lock);
      acquiresleep(&b->lock);
      return b;
    }
  }
  release(&bcache.bucket_lock[hi]);
 
  struct buf *minb = 0;
  uint min_ticks = ~0;

  // Find Recycle the least recently used (LRU) unused buffer.
  for(int i = 0; i < NBUCKET; i++) {
    acquire(&bcache.bucket_lock[i]);
    int find = 0;
    for(b = bcache.bucket[i].next; b != &bcache.bucket[i]; b = b->next) {
      if(b->refcnt == 0 && b->ticks < min_ticks) {
        if(minb != 0) {
          int last = hash(minb->blockno);
          if(last != i)
            release(&bcache.bucket_lock[last]);
        }
       
        min_ticks = b->ticks;
        minb = b;
        find = 1;
      }
    }

    if(!find)
      release(&bcache.bucket_lock[i]);
  }

  if(minb == 0)
    panic("bget: no buffers");
 
  int minb_i = hash(minb->blockno);

  minb->dev = dev;
  minb->blockno = blockno;
  minb->valid = 0;
  minb->refcnt = 1;

  if (minb_i != hi) {
    minb->prev->next = minb->next;
    minb->next->prev = minb->prev;
  }
  release(&bcache.bucket_lock[minb_i]);
 
  if(minb_i != hi) {
    // Move the buf from original bucket to bucket hi.
    acquire(&bcache.bucket_lock[hi]);

    minb->next = bcache.bucket[hi].next;
    minb->prev = &bcache.bucket[hi];
    bcache.bucket[hi].next->prev = minb;
    bcache.bucket[hi].next = minb;

    release(&bcache.bucket_lock[hi]);
  }

  release(&bcache.lock);

  acquiresleep(&minb->lock);

  return minb;
}*/

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

  releasesleep(&b->lock);

  int hi=hash(b->blockno);

  acquire(&bcache.bucket_lock[hi]);
  b->refcnt--;
  if (b->refcnt == 0) {
    // no one is waiting for it.
    b->ticks=ticks;
  }
  release(&bcache.bucket_lock[hi]);
}
  /*thread_switch needs to save/restore only the callee-save registers . Why?

    因为我们是通过thread_swtich以函数调用的形式切换线程，抛去线程相关的寄存器ra、sp需要被保存，剩下的需要保存的，就是可能被 [被调用过程改变的寄存器]，也即 被调用者保存的寄存器.
    因为我们将线程切换包装成了函数调用的形式，正常C code中 A调用B（A函数B函数都是用C代码写的），那么编译器编译之后，会生成如下汇编代码：A调用B之前会自动的保存调用者保存的寄存器，进入B之后，B会自动的保存被调用者保存的寄存器。那么在本节中，B就是swtch函数，可是由于该函数是汇编编写，无需如Ccode一样编译成汇编，也就不会生成：进入B之后，自动的保存被调用者保存的寄存器 的 汇编代码。故这个swtch函数中，保存被调用者保存的寄存器的函数是由我们用汇编编写的。故swtch函数中需要保存 reg sa , sp 以及 被调用者保存寄存器
    而相比之下 trampoline中 就不只是保存 被调用者保存的寄存器；因为user thread 切换成 kernel thread 并不是 C code中调用的函数调用，也即这个上下文切换并不是以函数调用的形式，程序在运行时可能随时会通过trampoline（如定时器中断）进入内核（如在正在执行密集计算指令的时候），并不是C代码通过函数调用进行的切换，属于是在某函数运行的中间突然触发切换，本身该函数的C代码编译后也并没有对这种情况进行处理，故我们在trampoline需要保存大量寄存器，既有caller 也有 callee(被调用者保存)
    uthread程序执行流程：
            main thread : thread_init -> thread_createabc -> thread_scheduler -> thread a/b/c
            thread a/b/c: running func -> yield -> running func
            end: scheduler -> no runnable process -> exit(-1)

        This sets a breakpoint at line 60 of uthread.c. The breakpoint may (or may not) be triggered before you even run uthread. How could that happen?

                因为gdb的实现依赖于监视pc寄存器，我们在b some_func的时候实际上是记录的某个地址。如果uthread内的指令地址与内核的指令地址有重复，那么当内核运行到这个地址的时候就会触发本应该在uthread内的断点。此外，很容易验证不同的用户态程序也会干扰。比如在uthread内部的0x3b之类的地址打下个断点，再运行ls或者其他用户态程序，如果在0x3b地址的指令是合法的，那么也会触发本应该在uthread程序内部的断点。*/

void
bpin(struct buf *b) {
  int hi=hash(b->blockno);
  acquire(&bcache.bucket_lock[hi]);
  b->refcnt++;
  release(&bcache.bucket_lock[hi]);
}

void
bunpin(struct buf *b) {
  int hi = hash(b->blockno);
  acquire(&bcache.bucket_lock[hi]);
  b->refcnt--;
  release(&bcache.bucket_lock[hi]);
}


