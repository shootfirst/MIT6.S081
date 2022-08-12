# MIT6.S081
麻省理工2020年秋操作系统实验

首先需要说明的是各个实验代码位于不同的分支，大部分是20年的实验。其中alloc和net是19年的，因为本人觉得伙伴分配系统需要熟悉一下，19年的alloc是伙伴分配系统，还有就是net也是19年的，20
年虽然有net，但是不如19详细，19除了底层网络驱动，还有上层的socket实现。下面是我的代码实现思路。

## util

这是入门的第一个实验，是写五个利用内核系统调用的用户态程序。

### sleep

实验很简单，会调用内核提供的系统调用sleep即可。注意非法参数的传递，如果只传递了一个参数，没有指定要sleep的时间，则使用exit(-1)异常退出。

### pingpong

实验难度我觉得要远大于sleep，可是不知为啥官方把两个都标记为easy。要写对这个实验，首先需要对内核进程的创建有了解。需要注意的是，调用fork创建子进程时，不仅仅是内存空间内容一
模一样，文件描述符也是一样的。这就是利用管道进行进程间通信的关键所在。首先这个父子进程间通信，我使用了两个管道，其实一个管道即可做到。父进程先在写端写字节，然后子进程读。子
进程读完后，又可以在相同写端写字节，然后父进程读之。因为之前做过本校的os实验，本校os实验需要两个管道，所以受此影响。

### primes

实验是要求实现质数筛子，这是这个算法的示意图：



## syscall

## pgtbl

## traps

## lazy

## cow

## thread

## lock

## fs

## mmap

## net

## alloc




