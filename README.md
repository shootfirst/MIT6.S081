# MIT6.S081
麻省理工2020年秋操作系统实验


## util

这是入门的第一个实验，是写五个利用内核系统调用的用户态程序。

### sleep

实验很简单，会调用内核提供的系统调用sleep即可。注意非法参数的传递，

- 如果只传递了一个参数，没有指定要sleep的时间，则使用exit(-1)异常退出

- 否则，我们调用sleep系统调用接口，使用atoi将char*类型转换为int类型

### pingpong

实验难度我觉得要远大于sleep，可是不知为啥官方把两个都标记为easy。要写对这个实验，首先需要对内核进程的创建有了解。需要注意的是，调用fork创建子进程时，不仅仅是内存空间内容
一模一样，文件描述符也是一样的。这就是利用管道进行进程间通信的关键所在。首先这个父子进程间通信，我使用了两个管道，其实一个管道即可做到。父进程先在写端写字节，然后子进程
读。子进程读完后，又可以在相同写端写字节，然后父进程读之。因为之前做过本校的os实验，本校os实验需要两个管道，所以受此影响。另外还需要注意的是：pingpong的打印检查很严格
（其实凡是涉及到打印检查的都很严格，空格，标点符号都要和题目给的保持一致，我就是因为received写成recieved，不给我通过，我硬生生卡了两小时）

- 创建两个管道

- 新建char内存缓冲区（就是char数组）存储要传递进管道的信息

- 调用fork系统调用，返回为0则是子进程，使用buffer读取ping管道，打印相关内容，再向pong管道写入内容

- 返回的是子进程id号则是父进程，先使用buffer写入管道ping，然后再从pong管道读取内容打印之

### primes

实验是要求实现质数筛子，这是这个算法的示意图：
![](sieve.png)


- 先保存2-36至数组pipeline当中

- 调用primesieve函数筛选。该函数创建子进程，父进程负责将pipeline通过管道传递给子进程，而子进程每次从管道读取一个数，按照此算法：

p = get a number from father
print p
loop:
    n = get a number from father
    if (p does not divide n)
        send n to pipeline

- 第一次将读取的数p打印并且保存，后面从父进程读取的数据若不能整除p则传递到pipeline数组中

- 结束筛选后将pipeline数组传递给下一个primessieve函数，相当于是传递给下一组管道进行新一轮筛选

- 递归调用结束的条件是没有一个数可以被送进pipeline数组
        
### find

这个实验挺有意思。不过对linux相关文件系统部分没有了解可能会耗费一些时间去阅读官方推荐的ls.c文件以及相关函数与数据结构。


- 首先对文件类型进行判断，若是文件则直接打印其路径内容

- 若是目录则通过dirent获取该目录下的所有目录项，依次进行递归find查找，注意.和..需要跳过，它们分别表示父目录和当前目录，若不跳过则会导致无限递归栈溢出

- 递归调用时注意将当前目录项的文件名也接到当前路径之后，这样才能打印出正确的路径名

- 最后别忘了关闭文件哦。

### xargs

这实验一开始看上去我是有点小疑惑的。一开始我是不清楚xargs前面的字符串是否也包含在argv中。后面通过打印发现是不是的。该实验是要把输入命令A改成执行命令B

A: x xargs y
B：y x

其中"xargs y"是argv的内容，"xargs"是argv[0]，而"x"则还位于标准输入。我们将其读取然后拼接到新argv中。我们需要构建新的argv数组B，然后执行之。

- 首先将老argv中的第2~n(n是其长度)个参数先copy过去，copy过程先注意要为字符串分配空间。第一个是xrags，第二个是xargs要执行的命令。所以我们从第二个开始copy

- copy完之后，xargs前面的x我们也将其加入(看成整体除非遇到换行符)。最后我们在新argv数组最后加上0表示结束。

一共花费时间：7小时（主要是pingpong坑人，还有最后一个实验一开始看不懂）

## syscall

该实验很简单。设计的很好，第一部分能让我们大概了解xv6系统调用的过程，而第二部分能通过对kmem中freelist字段的使用和proc数组的使用，对xv6内存管理和进程管理有初步了解。

### System call tracing

该部分是让我们实现破产版的strace，打印系统调用名称和调用的进程id。首先说明sys_trace的实现

- 将传入的参数syscallbit通过argint从寄存器a0中获取，我在这里解释下为啥系统调用参数要隐藏起来，使用argint类似的函数获取，这是因为要保证统一的函数指针格式，将所有
  系统调用的指针存入系统调用数组，方便xv6直接通过系统调用号作为下标来调用对应系统调用
  
接下来说明如何使用strace

- 通过静态数组记录syscallid和sycallname

- 然后在trap进内核，调用syscall后的时候，通过系统调用id打印出当前系统调用的名称和进程id

### Sysinfo

该部分是打印出当前空闲内存数目和状态不是UNUSED的进程个数（看清楚了，是不是UNUSED的进程个数，我在这里也是没看清，可恶）

- 前者可以通过数freelist的个数，再乘以4096即可

- 而后者也是可以直接通过遍历proc数组，判断其状态得出数据

- 需要注意的就是由于内核使用的页表和用户态不一样，所以要参考stat中，使用copyout方法，遍历用户页表将该虚拟地址转化为物理地址，再将sysinfo拷贝到该物理地址。
  因为内核是一一映射的。具体我就不解释，看xv6book对内存空间解释的图片，见下一节pgtbl。

一共花费时间：2小时

## pgtbl

### Print a page table

参考feewalk对页表的访问

+ 若该页表想存储的是页表，我们通过深度首先打印点，然后打印页表项，打印完后，我们进入该页表项递归打印子页表，这有点像二叉树的中序遍历

+ 若是叶子，那我们通过深度打印点，然后打印页表项，结束

### A kernel page table per process

每个进程一个页表，说实话，这个搞人心态，也许是我写的太急了，前前后后花了两天。首先在此说明下xv6的页表，xv6是一个内核页表供内核使用，该页表进行了一一映射，但是最底部的物理
地址没有映射，原因见图，而进程的用户页表则是进行虚拟地址映射，这样一旦进入内核态，xv6必须大费周章地将该虚拟地址先转化为物理地址，然后通过该物理地址的一一映射，因为内核的
页表没有进行该映射。这样去获取该地址的数据。这样时间开销很大，所以本实验和下个实验就是让我们实现每个进程一个内核页表，这里成为进程内核页表，上面的是全局内核页表。让每个
进程内核页表使用底虚拟地址实现用户页表的虚拟映射，这样mmu可以直接使用该映射进行转化，大大提升速度。本次实验主要是实现每一个进程都有一个进程内核页表

+ 新建创建进程内核页表方法proc_kpagetable
    - 创建空的页表
    - 参考全局内核页表对其进行一一映射
    
+ 删去procinit中的内核栈映射

+ 修改kvmpa，将kernel_pagetable改成我们新建的进程内核页表即可，这样在内核态我们使用访问虚拟地址的也是进程内核页表，这点很容易漏！！！我就是这里卡我两天

+ 在scheduler中，切换进程的时候同时切换到其内核页表，若没有进程可切换则切换至全局内核页表

+ 在allocproc中调用proc_kpagetable新建进程内核页表，对内核栈进行映射

+ 新建释放进程内核页表方法proc_freekpagetable
    - 对全局映射进行解映射，这样接下来的摧毁页表就不会删除释放这些一一映射的物理页面
    - 摧毁页表及其叶子节点
    
+ 在freeproc中解映射内核栈，调用proc_freekpagetable

### Simplify copyin/copyinstr

这里就很简单了，第二部分实现了每个进程一个进程内核页表，但是还没有实现对用户态地址的映射，这里我们实现之

+ 首先我们按指导书要求修改copyin和copyinstr

+ 在外面改变用户页表的映射时，相应的进程内核页表也需要改变映射，exec、fork、sbrk，这里应该是很简单了

+ 在userinit中我们也要对其进程内核页表映射，这里时第一个进程，不能调用fork，所以我们需要映射

+ 最后我们实现一个映射方法copyuserptb2kernelptb，通过传入老的size大小和新的size大小，我们进行对应的映射和解映射即可

一共花费时间：24小时（主要是第二部分，呜呜）


## traps

这个实验说实话和信号系统有点类似哈哈

### RISC-V assembly

不bb

### Backtrace 

打印函数调用栈，首先我刻画riscv栈结构图

    <-----fp
ra

save fp

+ 我们获取返回地址和老的fp直接使用fp-8和fp-16即可，打印完ra后，我们切换到老的fp继续打印，循环此操作

+ 我们什么时候结束循环呢，要知道xv6栈的大小是固定的，为一个页，所以我们取当前页顶作为最大地址即可，fp不能超出最大地址

### Alarm

写之前需要了解xv6trap的处理机制

#### sys_sigalarm系统调用

+ 保存阈值和handler处理函数即可

#### 时间中断处理：

+ 每次时间中断，我们将次数加1

+ 若时间中断次数到达阈值并且此时没有进行handler处理

+ 我们保存trapframe供sigreturn恢复上下文使用

+ 将epc设置为handler地址，设置当前在handler处理中

#### sys_sigreturn

+ 恢复trapframe

+ 设置当前没有在handler处理中

一共花费时间：5小时

## lazy

lazy包括很多，有exec、sbrk、mmap等，这里只写sbrk的lazy

+ 首先修改sbrk，使其若是增加size，则只是单纯增加size大小而不分配映射物理地址

+ 接下来，需要修改uvmunmap()让它不要恐惧，在找不到相关映射时继续查找即可

+ 在trap中处理lazy，通过r_scause()读取trap原因，若是13或15，则是pagefault

+ 通过r_stval()读取错误地址

+ 若错误地址大于size，则不做处理，直接杀死

+ 若错误地址落在栈的保护页，则是因为栈溢出了，也直接杀死

+ 否则，我们为此错误地址所在的虚拟页分配并且映射一个物理页即可，下面该函数allocmaponepage流程

    - 首先仍然需要判断错误地址是否合法问题，因为在copyin与copyout处也会调用，那里我们没有设立错误地址是否合法的检测
    
    - 然后我们分配一块物理页，并且映射之即可
    
+ 若allocmaponepage出错，如内存耗尽，杀死
    
+ 最后我们需要修改copyin和copyout，因为再内核转化的时候也会遇到walk出错，这个时候我们不会转移到trap处理，所以我们要自己手动调用allocmaponepage之后，在进行walk，若还是
  出错，此时可能是错误地址不合法或者内存耗尽，我们返回-1
  
一共花费时间：5小时

## cow

copy on write，以下简称cow或者奶牛

+ 一开始我们设立物理页引用数组，一开始每个物理页的引用数都是0，当被kalloc分配，或者，奶牛复制的时候，物理页引用数会加1，而当调用free的时候，将引用数减1，若引用数为0我们则
  回收之，这样我们可以像原来一样放心调用free，而不用大费周章地思考去减引用数啥的，这样会修改好多源码

+ 我们修改uvmcopy()，这会在fork的时候被调用，我们不做物理页的分配，我们只是单纯地拷贝父亲页表的叶子到子页表，同时修改页表项，加上奶牛标记而删除可写标记，最后将引用数加1

+ 在trap中处理cow，通过r_scause()读取trap原因，若是15，则是因为奶牛导致的pagefault

+ 首先在trap处理时，我们同样需要判断cow地址是否合法，只有小于size的情况下，我们才进一步处理，否则直接杀死

+ 然后我们walkcow出错页表项，它的flag必须是不可写，才是cow出错，我们进行进一步处理，否则我们照例直接杀死

+ 我们调用handlecow处理之

    - 分配新页面，将cow出错页面放心free，这里底层我们会判断引用数，上面提到了，只有引用数变为0我们才真正free，否则只是简单减少引用数
    
    - 擦除cow出错页面的奶牛flag，加上可写flag
    
    - 将cow出错的页表项修改为新分配的页面，复制cow出错页面内容到新分配页面
    

+ 若handlecow出错，如内存耗尽，杀死

+ 我们还要修改copyout，当遇到cow错误时，我们调用handlecow

一共花费时间：5小时

## thread

这个实验不是在内核中进行操作。主要是让我们用户态线程和熟悉线程安全。我觉得实验的一大亮点就是第一部分实现了用户态线程。我之前虽然是知道用户态线程和内核线程的概念，但是也只是
像高中那样背诵式的说出它们的区别，然后自己看过内核线程的实现源码，但是对用户态线程一直是很模糊，这回实现了，是真正的让我理解了内核线程和用户态线程的区别。

### switching between threads

实现用户态线程切换。

+ create的实现，我一开始的思考是保存所有的32个寄存器和func函数，但是我发现是没有必要的，只需要保存callee需要保存的14个即可，caller保存的编译器已经
  为我们自动保存了，所以这里thread_switch怎么写也就不言而喻了

+ create时注意两个特殊寄存器的初始化，ra寄存器需要保存为func指针的值，当第一次调度时会从func的第一条指令开始执行，而sp则需要保存为thread结构体中栈顶的值，注意是
  (uint64)t->stack + STACK_SIZE - 1，别忘了-1。

+ 调用thread_switch时参数顺序不要写错，我这里写错了，导致pagefault，卡了一会

### Using threads 

+ 就是最基本的用锁保护数据结构。唯一需要注意的是最好是一个bucket一个锁，这样可以大大提速

### Barrier

实现一个类似于内存屏障的东西。

+ 首先是用锁保护nthread，每个线程访问时首先需要获取锁，才能对nthread加一

+ 然后判断是否是最后一个到来的线程，是则唤醒所有线程，将round加1，nthread置0，这两个别忘了

+ 否则就wait释放锁并且睡眠。注意wait被唤醒后还是会抢锁，这是条件变量wait的流程。所以在方法最后要释放锁，不然会死锁的。

一共花费时间：2小时

## lock

这个实验的核心思想就是，打破锁的粒度，像第一部分，原来整个大锁保护所有内存，现在每个cpu分得一定内存，由每个锁保护。

### Memory allocator

我们一开始均匀分给每个cpu所有内存，当其中一个cpu耗尽内存时，我们也均匀地去其他cpu处偷取一定内存

+ 首先kinit中，初始化各个cpu保护内存链表的锁

+ 在kfree中，我们通过物理页号均匀地将页表回收到各个cpu的页表

+ 在kalloc中，若，当前cpu内存足够，我们直接返回即可，若为空，我们调用steal从相邻cpu窃取内存过来，为了防止多次窃取造成程序性能下降，我们每次窃取125块物理页

### Buffer cache

原来的磁盘缓冲区块是由一个大锁保护，现在我们设立哈希表，通过计算对应缓冲区块号将其加入到对应哈希桶，每个桶由一个锁保护

+ binit首先初始化所以bucket的锁，然后开始将所有缓冲区通过计算得出的哈希值加入到各个桶

+ bget获取缓冲区

    - 首先计算处于哪个桶，看是否已经被cached了，是则直接返回
    - 不是我们则寻找一个refcnt为0的块通过判断脏位写入磁盘，然后我们使用其来装填当前缓冲区
    - 若没有一个refcnt为0，我们去其他bucket窃取空闲缓冲区块即可
    - 若其他bucket也没有，我们panic

## fs

### Large files

很简单，不想说了

### Symbolic links

#### sys_symlink

+ 首先判断要链接的文件是否是目录，如果是我们直接返回-1

+ 否则我们调用create新建该文件，类型为符号链接

+ 将链接的目录内容调用writei写入inode

#### open

+ open打开的文件如果是符号链接，并且它的打开属性不是O_NOFOLLOW，那我们就需要打开这符号链接所指向的文件

+ 我们读入path内容，开始打开path，判断其是否是符号链接，是我们则继续打开，注意为了防止循环链接，我们循环的次数最多是10次

+ 若path指向的文件不存在我们返回-1


## mmap

首先写在最前面，对于用户态地址空间，我们mmap的区域是从trapframe下的地址开始

### 数据结构

表示mmap的地址

struct VMA {
  int valid  是否被mmap
  uint64 start  mmap的起始虚拟地址
  uint64 length  mmap长度
  int prot  属性
  int flags  写回与不写回
  struct file *file 被mmap的文件，偏移量为0
}

在proc中设立两个字段

struct VMA vmavec[NVMA]  记录mmap的区域
uint64 max_vma  可供mmap的最高地址（不算上面被munmap的区域）

### allocproc

在allocproc中，我们将max_vma指针初始化，让其指向TRAPFRAME，即trapframe下的最高地址

### mmap系统调用

+ 首先获取四个主要的参数（我们只使用这四个参数）length, prot, flags, fd

+ 如果该文件不能写但是flag是要求写回并且prot属性也是可写，我们直接返回-1

+ 否则我们在vmavec中存储该mmap映射信息，其中start和length需要对齐，文件引用数+1

+ 最后更新max_vma为start值，返回

+ 注意，这里我们使用了lazy。目前是没有真正mmap的，只有发生pagefault时，我们再进行相关映射

### munmap系统调用

+ 首先获取addr和length两个参数

+ 在vmavec中查找，如果addr没有落入任何区间，返回-1

+ 否则首先判断是否是MAP_SHARED，若是我们需要写回文件

+ 然后通过判断munmap区间和mmap区间的关系，更新vmavec存储的内容，若全部munmap，我们需要将文件引用数-1

### trap处理

+ 首先判断是否位于mmap的区域，若不是我们直接杀死

+ 否则我们分配一块物理页，如果内存耗尽，我们直接杀死

+ 我们从文件中读取一块内容到该物理页上，并且完成映射即可

### exit

在exit处理中，我们应该要释放所有mmap的区域，但是这里调用uvmunmap解映射时需要注意，有可能这些区域根本没有映射，那么uvmunmap将会恐慌，我们需要修改uvmunmap的实现，
当发现没有映射时，我们继续而不是恐慌。同时别忘了减少文件引用计数

### fork

我们复制父进程的mmap区域和max_vma即可


## net

## alloc




