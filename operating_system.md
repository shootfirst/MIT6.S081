# Operating System

## 进程管理

#### 基本系统调用接口

fork execv wait exit nanosleep vfork clone

#### 进程状态

+ 进程的创建

+ 进程的调度

+ 进程的阻塞

+ 进程陷入内核

+ 进程恢复用户态

+ 进程的摧毁

#### 进程与线程的区别

+ clone是如何创建线程（理解的关键）

+ 内核线程与用户线程

#### 调度算法

+ 先来先服务调度算法

+ 最短作业优先调度算法

+ 高响应比优先调度算法

+ 多级反馈队列调度算法

+ 时间片轮转调度算法

+ cfs调度算法

+ linux o1调度算法

#### 并行与并发

#### 进程通信

##### 管道

pipe

##### 消息队列

msgget、msgsnd、msgrcv、msgctl

##### 共享内存

shmget、shmat、shmdt、shmctl

##### 信号量

semget、semop、semctl


#### 信号

信号处理的时机，是进程陷入内核，从内核态返回时检查处理

##### 相关系统调用

sigaction sigreturn sigprocmask kill


#### 同步与互斥

##### 类型

+ 锁

+ 信号量

+ 条件变量

##### 系统调用

为了避免进程进入临界区检查是否能够获取锁，需要陷入内核造成不必要开销而设立的系统调用 

futex


#### 死锁

##### 产生条件

##### 解决





## 内存管理

#### 核心系统调用接口

brk mmap munmap 

#### 内存分配与回收

##### 伙伴分配系统

##### 用户态

mallco与free，基于brk和mmap系统调用

##### 内核态

+ vmalloc

+ kmalloc

slab分配器

#### 虚拟内存

##### 多级页表

##### 硬件支持

##### 优缺点

##### swap机制

##### lazy机制

##### 相关系统调用

mlock munlock mincore mprotect 

#### 进程地址空间


## IO管理

#### VFS

linux下一切皆文件，linux一共有7种类型的文件

+ 普通文件

+ 目录文件

+ 块设备文件

+ 字符设备文件

+ 套接字文件

+ 管道文件

+ 链接文件

file结构体与inode结构体

#### 文件管理

open read write close mkdir chdir rename lseek stat getdents link unlink symlink dup readv pread writev pwrite fsync sync

#### IO设备管理

ioctl 

#### 网络管理

socket bind listen connect accept receive send shutdown

#### IO多路复用

select poll epoll_create epoll_ctl epoll_wait

#### 异步IO

io_uring_setup io_uring_enter io_uring_control

