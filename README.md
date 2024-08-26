# xv6-labs-2022-kai
Tongji University 2024 OS summer homework
OS课设实验报告
Lab1：Xv6 and Unix utilities
1.	Sleep (easy)
实验目的
实现UNIX的sleep程序，时间为整数个时钟周期数，并且时钟周期由用户指定。
实验步骤
1.	在Makefile文件的UPROGS下添加命令 $U/_sleep\。
2.	在user中创立sleep.c文件，编写代码，如图：
 
输入参数，调用系统sleep函数即可实现。
2. pingpong（easy）
实验目的
使用 UNIX 系统调用编写一个程序 pingpong ，在一对管道上实现两个进程之间的通信。父进程应该通过第一个管道给子进程发送一个信息 “ping”，子进程接收父进程的信息后打印 "< pid >: received ping" ，其中是其进程 ID 。然后子进程通过另一个管道发送一个信息 “pong” 给父进程，父进程接收子进程的信息然后打印 "<pid>: received pong" ，然后退出。
实验步骤
1. 在Makefile文件的UPROGS下添加命令 $U/_pingpong\。
2. 在user目录下创建pingpong.c文件，编写代码如图：
 

3. primes（moderate）
实验目的
使用 pipe 和 fork 来创建管道。第一个进程将数字 2 到 35 送入管道中。对于每个质数，创建一个进程，从其左邻通过管道读取，并在另一条管道上写给右邻。
实验步骤
1. 在UPROGS加一行 $U/_primes\。
2. 编写代码。
4. find（moderate）
实验目的
编写一个简单的find程序，可以查找文件。
实验步骤
1. 在 Makefile 文件中， UPROGS 项追加一行 $U/_find\。
2. 编写代码。
5. xargs（moderate）
实验目的
编写一个xargs程序，从标准输入中读取行并为每一行运行一个命令，将该行作为命令的参数提供。解决方案放在 user/xargs.c 中。
实验步骤
1. 在 Makefile 文件中， UPROGS 项追加一行 $U/_xargs\ 。
2. 新建xargs.c，编写代码。
6. 实验中遇到的问题和解决方法
1. 在编写primes函数时，最开始不知道如何存取管道内部的数，后来看到应该先利用第二个作业的函数来解决。
7. 实验心得
本次实验解决了一些基本的函数功能的实现，让我对操作系统的实现有了更深的了解，明白了操作系统与一般程序的差别，并且对管道的理解更加深刻。

Lab2：system calls
实验目的
添加一个系统调用跟踪功能，该功能可以在以后的实验中为你提供帮助。
你将创建一个新的 trace 系统调用来控制跟踪。
它应该有一个参数，一个整数“mask(掩码)”，其指定要跟踪的系统调用。例如，为了跟踪 fork 系统调用，程序调用 trace (1 << SYS_fork) ，其中 SYS_fork 是来自 kernel/syscall.h 的系统调用号。
如果掩码中设置了系统调用的编号，则必须修改 xv6 内核以在每个系统调用即将返回时打印出一行。
该行应包含 进程 ID 、 系统调用名称 和 返回值 ；您不需要打印系统调用参数。 trace 系统调用应该为调用它的进程和它随后派生的任何子进程启用跟踪，但不应影响其他进程。

实验步骤
1. 在 kernel/syscall.h 添加宏定义：#define SYS_trace  22
2. 在 user/usys.pl 文件加入语句entry("trace");
3. 把新增的 trace 系统调用添加到函数指针数组 *syscalls[] 上。
4. 在文件开头给内核态的系统调用 trace 加上声明，在 kernel/syscall.c 加上：
extern uint64 sys_trace(void);
5. 在 kernel/sysproc.c 给出 sys_trace 函数的具体实现，如图：
 
6. 在 kernel/syscall.c 中定义static char *syscall_names[] = {
  "", "fork", "exit", "wait", "pipe", 
  "read", "kill", "exec", "fstat", "chdir", 
  "dup", "getpid", "sbrk", "sleep", "uptime", 
  "open", "write", "mknod", "unlink", "link", 
  "mkdir", "close", "trace"};
6. 编写syscall函数：
 
7. 在fork函数中，添加父进程mask代码
8. 在 Makefile 的 UPROGS 中添加：$U/_trace\

实验中遇到的问题及解决方法
1. 最开始，我不是很懂git切换分支的使用方法，总是无法切换新分支，因此实验一天都没有做好，但后来又莫名其妙的可以切换分支了（？

实验心得
之前一直不能理解普通的函数调用和系统调用有什么区别，这次因为要实现系统调用，要先顺着系统调用的过程把一个新的系统调用在各种文件中注册一遍。这个过程中就对系统调用清楚了很多。

Lab3：Page tables
1. Print a page table
实验目的
按层次输出多级页表的页表项，定义函数vmprint()，参数为level-2级页表地址pagetable_t，并输出pid为1的进程的用户页表

实验步骤
增加一个新函数，使得其可以在系统刚启动时打印多级页表。
 
A kernel page table per process (hard)
实验目的
实现每个进程一个内核页表

实验步骤
1. 在进程数据结构中增加内核页表域，位于proc.h
 
2. 参考kvminit定义user_kvminit初始化进程内核页表
 
3. 初始化进程内核页表
 
4. 每个进程内核页表映射进程的内核栈
每个进程有两个栈，一个是用户栈，当进程在用户态执行时使用该栈，当该进程陷入后，使用该进程的内核栈，在之前只有一个内核页表时，proc.c/procinit函数中分配进程内核栈物理空间，内核页表对进程内核栈进行映射
5. 修改proc.c/scheduler
6. 释放进程
7. 释放进程内核页表
实验中遇到的问题和解决方法
在进行实验时，总是在第二个实验完成之后第一个实验失灵，目前还没有找到解决方法。

实验心得
通过本次实验，我对多级页表有了更深刻的理解。但仍有一些未解决的问题。

Lab4：Trap
1. RISC-V assembly
实验目的
了解一些RISC-V程序集很重要，你在6.004中接触过这些程序集。在你的xv6存储库中有一个文件user/call.c。使用make fs.img命令对其进行编译，并在user/call.asm中生成程序的可读汇编版本。
阅读call.asm中函数g、f和main的代码。RISC-V的说明手册在参考地址上。以下是你应该回答的一些问题(将答案存储在文件answers-traps.txt中)

实验步骤
nswers-traps.txt已回答。

2. Backtrace
实验目的
当栈上的某一点发生错误，可以通过回溯得到上方的函数调用列表。现需要在kernel/printf.c中实现backtrace()函数，并在系统调用sys_sleep中插入对该函数的调用。

实验步骤
1. 根据实验提示①，将backtrace的原型添加到kernel/defs.h。
void backtrace(void);
接着要将backtrace的定义添加到kernel/printf.c中。由于现在才开编写该函数，只知道首先要打印一行backtrace:\n。
void backtrace(void) {
  printf("backtrace:\n");
2、 根据实验提示②，将r_fp的定义添加到kernel/riscv.h。

static inline uint64 r_fp()
{
  uint64 x;
  asm volatile("mv %0, s0" : "=r" (x) );
  return x;
}
接着要在backtrace中调用该函数。
void backtrace(void) {
  printf("backtrace:\n");
  uint64 fp = r_fp();
  // ......后面继续完成其它功能
}
3、 根据实验提示③，返回地址位于fp-8处，前一个栈帧的帧指针位于fp-16。于是我们需要循环地打印返回地址，然后移动到前一个栈帧。
void backtrace(void) {
  printf("backtrace:\n");
  uint64 fp = r_fp();
  while( ){
	  uint64 ra = *(uint64*)(fp-8);
	  printf("%p\n", ra);
	  fp = *(uint64*)(fp-16);
  }
}
4、 根据实验提示④，需要为循环添加终止条件。xv6中用一个页来存储栈，且向低地址扩展，所以当fp到达最高地址时说明到达栈底。
void backtrace(void) {
  printf("backtrace:\n");
  uint64 fp = r_fp();
  while(fp < PGROUNDUP(fp))
  {
    uint64 ra = *(uint64*)(fp-8);
    printf("%p\n", ra);
    fp = *(uint64*)(fp-16);
  }
}
5、根据实验提示⑤，在kernel/printf.c的painc中添加backtrace的调用。
void panic(char *s)
{
  // ......
  printf("\n");
  backtrace(); //添加
  panicked = 1; // freeze uart output from other CPUs
 // ......
}
根据实验目的，在kernel/sysproc.c中的系统调用sys_sleep里面也添加对backtrace的调用。
uint64
sys_sleep(void)
{
 // ......
	release(&tickslock);
	backtrace();//添加
  	return 0;
}
3. Alarm
实验目的
你将向xv6添加一个功能，该功能会在进程使用CPU时定期提醒它。

实验步骤
1. 在Makefile的UPROGS中添加$U/_alarmtest\
2. 在user/user.h中放入sigalarm和sigreturn的声明。
3 更新user/usys.pl(生成user/usys.S)。
更新kernel/syscall.h
更新kernel/syscall.c。
4 在kernel/sysproc.c中编写sys_sigreturn，返回0。
5 将警报间隔和指向警报处理函数的指针存储在proc结构中。
6 定义一个跟踪自上次调用警报处理程序以来已经过去了多少滴答的成员变量ticks。
在kernel/proc.c中的allocproc()中初始化proc字段。
7 去usertrap中进行处理。
----如果有时钟中断，我们操作进程的警报tick，因此在if (which_dev==2)情况下进行处理。
----如果tick没到达设定的间隔，则将tick++。
----如果tick达到设定的间隔，则将tick清零，同时转去相应的处理程序handler。此处主要考虑如何调用处理函数handler。在usertrap中页表已经切换为内核页表，而handler仍然是用户页表的函数虚拟地址，因此不能直接调用。这里我们将p->trapfram->epc置为p->handler，这样在返回到用户空间时，程序计数器为handler定时函数的地址，便达到了执行定时函数的目的。(这里做的其实就是在内核态进行赋值，返回到用户态再执行。)

4. 实验遇到的问题和解决方法
5. 实验心得
通过本次实验，我对于定时中断的处理有了更深的理解。如果需要实现定时中断处理函数，这可能涉及到操作硬件定时器和设置中断处理程序，从而也加深了对中断处理机制的了解。

Lab5：Lazy page allocation
实验目的
有些程序申请分配的内存比实际使用的要多（例如，实现稀疏数组），或者为了以后的不时之需而分配内存。为了让sbrk()在这些情况下更快地完成，复杂的内核会延迟分配用户内存。也就是说，sbrk()不分配物理内存，只是记住分配了哪些用户地址，并在用户页表中将这些地址标记为无效。当进程第一次尝试使用延迟分配中给定的页面时，CPU生成一个页面错误（page fault），内核通过分配物理内存、置零并添加映射来处理该错误。您将在这个实验室中向xv6添加这个延迟分配特性。
1. Eliminate allocation from sbrk() (easy)
实验目的
更改 kernel/sysproc.c 中的 sys_sbrk() 函数，把原来只要申请就分配的逻辑改成申请时仅进行标注，即更改进程的 sz 字段。

实验步骤
更改代码即可。
2. Lazy allocation (moderate)
实验目的
仿照上一个实验中的中断处理操作，在 kernel/trap.c 中，找到中断处理的逻辑进行更改。

实验步骤
1. 处理中断，分配内存
系统调用的中断码是 8，page fault 的中断码是 13 和 15。因此，这里我们对 r_scause() 中断原因进行判断，如果是 13 或是 15，则说明没有找到地址。错误的虚拟地址被保存在了 STVAL 寄存器中，我们取出该地址进行分配。如果申请物理地址没成功或者虚拟地址超出范围了，那么杀掉进程。
2. 处理uvmunmap的报错
uvmunmap 是在释放内存时调用的，由于释放内存时，页表内有些地址并没有实际分配内存，因此没有进行映射。如果在 uvmunmap 中发现了没有映射的地址，直接跳过就行，不需要 panic。

3. Lazytests and Usertests (moderate)
实验目的
通过所有的测试用例
4.实验中遇到的问题及解决方法
1. 在实现懒分配（Lazy Allocation）时，操作系统并不会在进程分配虚拟内存时立即分配物理页。只有在实际访问该页时（通常通过页错误的方式）才会进行物理页的分配。这就要求正确地处理页错误。
2. 在实现懒分配时，确保每个分配的页面有正确的权限设置（例如只读、读写）。
5. 实验心得
懒分配实验通过让我理解了实现延迟物理页分配的方法，帮助我更好地理解虚拟内存管理和页表机制的工作原理。
Lab6：Copy-on-Write Fork for xv6
实验目的
在 xv6 操作系统中实现写时复制（Copy-on-Write，COW）的 fork 功能。传统的 fork() 系统调用会复制父进程的整个用户空间内存到子进程，而 COW fork() 则通过延迟分配和复制物理内存页面，只在需要时才进行复制，从而提高性能和节省资源。通过这个实验，你将了解如何使用写时复制技术优化进程的 fork 操作。

实验步骤
1. 查看 kernel/riscv.h ，了解一些对于页表标志 (page table flags) 有用的宏和定义。
2. 修改 uvmcopy() 函数，将父进程的物理页面映射到子进程，而不是分配新的页面。同时，清除父子进程的 PTE 中的 PTE_W 标志，使得页面变为只读状态。
3. 在 vm.c 文件中修改 uvmcopy 函数。为了使用 useReference 引用计数，需要提前在文件开头声明。
4. 修改 usertrap() 函数，使其能够识别页面故障（page fault）。当发生 COW 页面的页面故障时，使用 kalloc() 分配一个新的页面，将旧页面复制到新页面中，然后在父进程和子进程的 PTE 中设置 PTE_W 标志，允许写入。如果一个 COW 页错误发生时，没有剩余可用内存，那么该进程应该被杀死。
5. 确保当物理页面的最后一个 PTE 引用消失时释放它。可以为每个物理页面维护一个“引用计数”，表示指向该页面的用户页表数量。使用一个固定大小的整数数组来保存这些 reference count 。在 kalloc() 分配页面时，将 reference count 设置为 1；在 fork() 时，将 reference count 增加；在页面被从页表中删除时，将 reference count 减少；只有当 reference count 为 0 时，才将页面放回空闲列表。
6. 在trap.c中定义 cowhandler 函数进行检查。只有存在的、被标记为 COW且属于用户级别的，才可以被分配内存。如果页是只读的，若在此时尝试对其进行写入，就会返回 -1，最终被 kill 。
7. 修改 copyout() 函数，使其在遇到 COW 页面时使用与页面故障相同的方式进行处理。

实验中遇到的问题及解决方法
1. 在usertraps函数的修改上，在trap.c中的usertrap中拦截写页异常，那么利用什么标志呢？
根据riscv手册(riscv-privileged),查到当scause寄存器为15时，为写页异常

实验心得
在实验过程中，我遇到了关于处理页面故障的问题。刚开始时，我对于如何确定页面故障以及如何获取相应的异常代码和地址信息感到困惑。然而，通过查阅 RISC-V 架构规范和相关文档，我逐渐理解了异常处理的流程。

Lab7：Multithreading
实验目的
实现一个用户态的线程库；尝试使用线程来为程序提速；并且尝试实现一个同步屏障
1. Uthread: switching between threads (moderate)
实验目的
设计并实现一个用户态线程的上下文切换机制（其实类似于协程）。
实验步骤
1. 定义context结构，用于保存上下文寄存器
当从一个线程切换到另一个线程时，需要保存切换前线程的上下文，恢复切换后线程的上下文。在保存时可以参考xv6中内核线程的切换，只用保存被调用者保存的寄存器即可，因为需要调用者保存的寄存器都已经存储在线程的栈中了。
除此之外还应保存两个重要的寄存器：ra、sp，ra相当于保存了pc寄存器，用户恢复到之前切换的地方继续执行；sp保存了线程栈顶指针，每个线程都需要在栈中执行。
2. 线程切换汇编
3. 线程初始化
4. 线程切换
2. Using threads
实验目的
在hash表中使用多线程和锁，用于加快程序运行速度。在linux中使用pthread库来完成，并非xv6。
实验步骤
1. race condition
对hash表执行插入元素的代码如下所示：
 
2. 定义互斥锁
定义并初始化线程互斥锁，如下所示：
 
3. 为insert操作加锁
在put函数中为insert函数加互斥锁，由于put之前基本都是读操作，不用加锁，虽然e->value = value;是修改操作，但不会产生冲突，一定是后修改的线程会覆盖先修改的线程，也不用加锁，只有insert才需要加锁，代码如下所示：
 

3. Barrier
实验目的
使用pthread的条件等待实现线程屏障(barrier)：所有的线程都需要在该点等待直到所有其他线程也都到达了该点。
实验步骤
每一轮barrier结构维护当前到达屏障的线程数目，当未全部达到时，先到的线程调用pthread_cond_wait在条件上等待，当最后一个线程到达时，再调用pthread_cond_broadcast唤醒所有等待的线程，之后所有线程进行新一轮的执行。
在barrier()函数实现的过程中，同样涉及多个线程对于barrier共享数据结构的修改，因此应该加锁进行同步。
实验中遇到的问题及解决方法
1. 线程的创建与管理：可以为每个新线程分配一块独立的栈空间。可以通过在进程的地址空间中为线程栈预留区域来实现。
实验心得
通过实现多线程功能，我深刻理解了线程的概念及其与进程的区别。线程在一个进程的地址空间内执行，使得共享数据更加方便。

Lab8：Lock
1. Memory allocator (moderate)
实验目的
通过修改内存分配器的设计，以减少锁竞争，从而提高多核系统中的性能。具体来说，需要实现每个CPU都有一个独立的自由列表（free list），每个列表都有自己的锁。这样可以让不同CPU上的内存分配和释放操作可以并行进行，从而减少锁的争用。还需要实现当一个CPU的自由列表为空时，能够从其他CPU的自由列表中获取部分内存。

实验步骤
1. 定义每个CPU一个空闲物理页链表
将原本的单链表扩展为每个CPU一个链表，并在kinit()函数中初始化每个锁。
2. 修改kalloc()函数
当申请物理页时，kalloc()被调用，由于现在每个CPU一个链表，因此需要通过cpuid()函数获取到当前调用kalloc()线程所属CPU的编号，之后从该CPU的链表中去获取一个空闲物理页；同时要注意只有中断关闭时通过cpuid()获取CPU号才是安全的，否则可能由于中断，之后的代码被调度到其他CPU上去执行了。
在当前CPU链表不为空时，首先对当前CPU链表加锁，并从链表中获取到空闲物理页。由于每个CPU一个锁，因此可以使得多个CPU同时调用kalloc()函数并分配各自链表上的空闲物理页，提高了并行度。
在当前CPU链表为空时，依次遍历其他CPU的链表，当遇到非空的CPU链表，则窃取该链表头部的空闲物理页，窃取过程中应加上被窃取CPU链表的锁。
有个细节是在窃取之前，应先释放当前CPU链表的锁，防止之后窃取过程发生死锁，所窃取的物理页之后直接作为返回值返回了，也不需要再操作当前链表。在最初的尝试中，想法是每次从其他非空的CPU链表中窃取一半的空闲物理页到当前CPU链表，但是无奈需要同时加上两个锁，会发生死锁。
3. 修改kfree()函数
kfree()函数的修改相对简单，只要把被释放的物理页放到当前CPU链表的头部即可。

2. Buffer cache (hard)
实验目的
修改buffer cache的数据结构和加锁策略来减少争用，提高并行度。

实验步骤
1. 修改数据结构为hash表
之前的buffer采用双向链表组织，为降低锁的粒度，可以使用hash表来组织，根据块号(blockno)进行hash，将buffer分布到相应的bucket中去。每个hash bucket中的buffer同样采用双向链表组织。
为了方便，每个bucket链表有一个buf作为dummy结点，用作链表头。
每个bucket有一个锁，这样获取和分配时可直接针对某个bucket进行，实现不同bucket间的并行。
同时每个buffer维护一个ticks作为时间戳，用来实现LRU算法。
2. 修改bget()函数
bget()函数可分为两步进行：
第一步：首先查找是否存在给定dev和blockno的buffer，如果存在，则使用此buffer并返回；
第二步：如果不存在，则寻找一个LRU未使用的buffer并返回。
3. 修改brelse()函数
brelse()的修改则比较简单，直接将buffer的引用数-1，如果之后未被引用的话，更新ticks，表示为最新的未使用的buffer，用于LRU算法寻找最近最少使用的unused buffer。
4. 修改bpin()和bunpin()
更新buffer的引用数时，应加锁，引用数为共享变量

实验中遇到的问题及解决方法
死锁可能在尝试获取多个锁的过程中发生，特别是当一个 CPU 持有某个锁并尝试获取另一个锁，而此时另一个 CPU 也在尝试获取这两个锁。
可以尽量避免同时持有多个锁。在 kalloc 和 bget 的实现中，避免在遍历所有 CPU 的空闲页链表或者所有的缓存桶时同时持有多个锁。需要先释放当前持有的锁再去获取另一个锁。此外，在 bget 中使用大锁（bcache.lock）来串行化第二步的淘汰过程，这样可以避免跨多个桶同时持有多个锁的情况。

实验心得
通过实验，我学到了多核系统中的并发编程和锁的管理策略，也深入了解了多线程环境下的同步和并发控制。

Lab9：file system
1. Large files
实验目的
当前的xv6文件被限制在268个块内， 每一块1024字节，这个限制由于xv6的inode结点包含12个直接块号，一个单一的间接块号，间接块号可以存储256个数据块块号，因此总共是12 + 256 = 268块。
该部分需要增加xv6文件的最大大小，通过为xv6引入二级间接块号来增加文件大小最大值，二级间接块号包括256个一级间接块号，每个一级间接块号又可以存储256个数据块块号，因此最大文件可以达到65803 块（256 * 256 + 256 + 11块，牺牲了一个直接块号作为二级间接块号地址）。

实验步骤
1. 修改inode结构
由于需要引入二级间接块号，需要牺牲一个直接块号，因此需要修改inode的结构以修改布局，修改后inode有11个直接块号、1个一级间接块号地址、1个二级间接块号地址。
2. 修改fs.c/bmap()
bmap()函数将文件内块号映射为磁盘块号，如果不存在则会为其从bitmap寻找一空闲数据块，并将寻找到的数据块号添加到inode中，建立相应的映射。
如果文件内块号位于一级间接块号的范围内，则应该先根据一级间接块号地址找到存放一级间接块号的数据块，如果不存在则分配，再根据一级间接块号中的块号地址，找到具体的数据块，如果不存在则分配。
3. 修改fs.c/itrunc()
有增加映射就有删除映射，itrunc()用于删除整个文件内容，则需要查找inode，删除所占有的所有数据块，包括一级间接块、二级间接块。
4. 修改file.c/filewrite()
xv6为了维持文件系统的一致性，对于文件系统的调用都将被作为一个事务来完成，由于日志大小限制，一个事务同时更新的数据块数量也受到了限制，对于大文件的写来说，将被切分成多次写，每次写几个数据块作为一个事务进行提交。
对于文件的一次写，可能需要为其分配数据块，那么存储间接块号的数据块、inode数据块都需要更新，每个事务实际使用的数据块数量应该考虑到同时更新的这些数据块；而由于添加了二级间接块号的数据块，那么同时又可能需要更新二级间接块号的数据块，因此最终每个事务使用的最大数据块又应该从每个事务的日志最大块数 -1。
2. Symbolic links
实验目的
该部分为xv6增加符号链接（软链接），符号链接通过文件路径链接另一个文件；当一个符号链接打开时，内核打开实际链接到的文件。符号链接像硬链接，但是硬链接被限制指向同一个磁盘上，符号链接可以在不同的磁盘设备上。
实验步骤
1. 增加系统调用
按照之前Lab添加系统调用的方式添加symlink系统调用。
2. 增加符号链接文件类型
3. 创建sysfile.c/sys_symlink()系统调用函数
符号链接是一个特殊的文件，文件类型为T_SYMLINK，该文件只需要存储另一个文件的路径即可，因此首先先创建一个符号链接文件，再将所链接文件路径写到符号链接文件中即可。
4. 修改sysfile.c/sys_open()增加链接文件打开方式
进程使用open()系统调用打开符号链接文件时，可以有两种打开方式：
•	打开模式为O_NOFOLLOW时，直接打开符号链接文件的内容，也即另一个文件的文件路径，直接按照一般文件打开即可
•	打开模式不加O_NOFOLLOW时，则打开所链接的文件，如果所链接的文件又是一个链接文件，这应该递归地去找到真正的文件

实验中遇到的问题及解决方法
在实现双重间接块（doubly indirect blocks）时，可能会遇到一些逻辑错误。例如，计算文件块号（block number）对应的双重间接块时，可能会搞混直接块、一级间接块和二级间接块之间的关系。
解决方法：
•  在实现 bmap() 函数时，仔细检查计算双重间接块的逻辑。
•  确保计算出的块号正确地映射到相应的双重间接块号，并正确地在内存中读取或分配这些块。
•  使用 panic() 和 printf() 调试打印来帮助检查代码执行的中间结果，确保逻辑正确。

实验心得
通过Lab 9的实现，我对文件系统的扩展设计有了更深的理解，也明白了一些调试技巧。

Lab10: mmap
实验目的
为xv6实现简易版的mmap和munmap系统调用，mmap允许对进程的地址空间进行详细控制，可以用来进程间共享内存、将文件映射到进程地址空间等。

实验步骤
1. 定义 vma 结构体，其中包含了 mmap 映射的内存区域的各种必要信息，比如开始地址、大小、所映射文件、文件内偏移以及权限等。
并且在 proc 结构体末尾为每个进程加上 16 个 vma 空槽。
2. 实现 mmap 系统调用。函数原型请参考 man mmap。函数的功能是在进程的 16 个 vma 槽中，找到可用的空槽，并且顺便计算所有 vma 中使用到的最低的虚拟地址（作为新 vma 的结尾地址 vaend，开区间），然后将当前文件映射到该最低地址下面的位置（vastart = vaend - sz）。
3. 在 proc.c 中添加处理进程 vma 的各部分代码。
•	让 allocproc 初始化进程的时候，将 vma 槽都清空
•	freeproc 释放进程时，调用 vmaunmap 将所有 vma 的内存都释放，并在需要的时候写回磁盘
•	fork 时，拷贝父进程的所有 vma，但是不拷贝物理页

实验中遇到的问题及解决方法
在 xv6 的用户地址空间中，heap 的范围一直从 stack 到 trapframe。为了防止 mmap 映射的文件与进程本身使用的内存空间冲突，选择将 mmap 映射的文件映射到尽可能高的位置（即在 trapframe 下方）。

实验心得
通过mmap实验，我对虚拟内存管理有了更深的了解，也看到了懒加载的效果，同时对操作系统的内核有了更深刻的理解。

Lab11：network driver
实验目的
熟悉系统驱动与外围设备的交互、内存映射寄存器与 DMA 数据传输，实现与 E1000 网卡交互的核心方法：transmit 与 recv。
实验步骤
int
e1000_transmit(struct mbuf *m)
{
  acquire(&e1000_lock); // 获取 E1000 的锁，防止多进程同时发送数据出现 race

  uint32 ind = regs[E1000_TDT]; // 下一个可用的 buffer 的下标
  struct tx_desc *desc = &tx_ring[ind]; // 获取 buffer 的描述符，其中存储了关于该 buffer 的各种信息
  // 如果该 buffer 中的数据还未传输完，则代表我们已经将环形 buffer 列表全部用完，缓冲区不足，返回错误
  if(!(desc->status & E1000_TXD_STAT_DD)) {
    release(&e1000_lock);
    return -1;
  }
  
  // 如果该下标仍有之前发送完毕但未释放的 mbuf，则释放
  if(tx_mbufs[ind]) {
    mbuffree(tx_mbufs[ind]);
    tx_mbufs[ind] = 0;
  }

  // 将要发送的 mbuf 的内存地址与长度填写到发送描述符中
  desc->addr = (uint64)m->head;
  desc->length = m->len;
  // 设置参数，EOP 表示该 buffer 含有一个完整的 packet
  // RS 告诉网卡在发送完成后，设置 status 中的 E1000_TXD_STAT_DD 位，表示发送完成。
  desc->cmd = E1000_TXD_CMD_EOP | E1000_TXD_CMD_RS;
  // 保留新 mbuf 的指针，方便后续再次用到同一下标时释放。
  tx_mbufs[ind] = m;

  // 环形缓冲区内下标增加一。
  regs[E1000_TDT] = (regs[E1000_TDT] + 1) % TX_RING_SIZE;
  
  release(&e1000_lock);
  return 0;
}

static void
e1000_recv(void)
{
  while(1) { // 每次 recv 可能接收多个包

    uint32 ind = (regs[E1000_RDT] + 1) % RX_RING_SIZE;
    
    struct rx_desc *desc = &rx_ring[ind];
    // 如果需要接收的包都已经接收完毕，则退出
    if(!(desc->status & E1000_RXD_STAT_DD)) {
      return;
    }

    rx_mbufs[ind]->len = desc->length;
    
    net_rx(rx_mbufs[ind]); // 传递给上层网络栈。上层负责释放 mbuf

    // 分配并设置新的 mbuf，供给下一次轮到该下标时使用
    rx_mbufs[ind] = mbufalloc(0); 
    desc->addr = (uint64)rx_mbufs[ind]->head;
    desc->status = 0;

    regs[E1000_RDT] = ind;
  }

}

实验中遇到的问题及解决方法
本实验比较简单，除了读文档有一些困难外，其他没什么问题

实验心得
通过本次实验，我对网络连接、数据发送等内容有了更深的了解，也对整个xv6系统有了一个更加整体的理解。
