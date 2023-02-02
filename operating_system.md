# Operating System

## 进程管理

#### 核心系统调用接口

##### fork

```
pid_t fork()
```

+ 创建进程，返回值若为0，则为子进程；若为正数，则返回值为子进程id；若小于0，则失败

##### execv

```
int execv(const char* filename, char * const argv[], char *const envp[])
```

+ 加载，执行可执行文件路径为filename的文件，传入的参数为argv，环境变量为envp。

+ 该函数一般和fork一起配合使用，如终端shell执行文件。

+ 该函数一般不返回，因为进程地址空间早就发生变换

##### wait

```
pid_t wait(int * wsstatus)
```

+ 如果调用进程并无之前未被等待的子进程终止，调用将一直阻塞，直到某个子进程终止。如果调用时已有子进程终止，wait()将立即返回

+ wsstatus存储子进程状态，返回值为子进程id，出错时，wait()返回-1。可能的错误原因之一是调用进程并无之前未被等待的子进程

##### exit

```
void exit(int status)
```

+ 退出当前进程，传入的status作为退出码，0为正常

##### nanosleep

int nanosleep(struct timespec * rqtp, struct timespec * rmtp)

+ 第一个参数表明需要睡眠的时间，第二个表示如果没有睡眠到第一个参数指定的值（如信号中断），则记录剩余还需睡眠的值

+ 睡眠被中断则返回-1

##### vfork

```
pid_t vfork();
```

和fork区别

+ 保证子进程先执行，父进程挂起。除非子进程execv或者exit，然后父进程往才会执行

+ 注意子进程在父进程调用vfork的那个函数里绝对不能return，只能exit或者execv

+ 在子进程execv或者exit之前和父进程共享地址空间


##### clone

```
int clone(int (*func)(void*), void *child_stack, int flags, void *func_arg, ... /*pid_t *ptid, struct user_desc *tls, pid_t *ctid*/);
```

+ 按flags创建一个从func开始的进程，栈空间为child_stack，函数参数为func_arg

+ 成功则返回进程id，-1则表示出错

+ flag表示创建的进程是否和当前进程共享地址空间，是否共享文件描述符，是否共享信号处理，是否位于同一个线程组等


#### 进程通信系统调用接口

##### 管道

pipe

##### 消息队列

msgget、msgsnd、msgrcv、msgctl

##### 共享内存

shmget、shmat、shmdt、shmctl

##### 信号量

semget、semop、semctl


#### 信号相关系统调用

##### sigaction

```
int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
```

+ 检查或者修改指定信号相关联的处理动作

+ signum是信号类型，act是要修改的处理动作，oldact则是之前的处理动作，如果没有则是NULL

##### sigreturn

```
int sigreturn(...);
```

+ 恢复信号处理函数之前的用户态上下文

+ 在信号处理函数结束后调用，处理函数的return地址就是sigreturn系统调用地址（由os设定）

##### sigprocmask

```
int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
```

+ 设置信号阻塞或不阻塞

+ how是指定修改方式，set则是新设的信号集，oldset则是老的信号集

##### kill

```
int kill(pid_t pid, int sig);
```

+ 向进程id为pid的进程发送sig信号


#### 锁相关系统调用

##### futex

```
int futex (int *uaddr, int op, int val, const struct timespec *timeout,int *uaddr2, int val3);
```

+ futes是 Fast Userspace muTEXes的缩写，为了避免进程进入临界区检查是否能够获取锁，需要陷入内核造成不必要开销而设立的系统调用

+ 具体流程是同步的进程之间通过共享内存，futex变量就位于这段共享的内存进程先去查看共享内存中的futex变量，进程进入或者退出连接去时，如果没有竞争发生，则只修改futex变量

+ 若有竞争发生，执行系统调用去完成相应的处理(wait 或者 wake up)。简单的说，futex就是通过在用户态的检查，如果了解到没有竞争就不用陷入内核了

+ futex内部维护了一个队列，在线程挂起前会线程插入到其中，同时对于队列中的每个节点都有一个标识，代表该线程关联锁的uaddr

+ 当用户态调用futex_wake时，只需要遍历这个等待队列，把带有相同uaddr的节点所对应的进程唤醒就行了

+ uaddr就是用户态下共享内存的地址，里面存放的是一个对齐的整型计数器

+ op存放着操作类型，val则存储操作相关的值


## 内存管理

#### 核心系统调用接口

##### brk

```
int brk(void * addr)
```

+ 修改程序间断点的位置为addr，初始值为数据段结尾处（end符号地址），即修改堆区结尾处地址

##### mmap

```
void* mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
```

+ addr: 指定映射被放置的虚拟地址，如果NULL，那么内核会为映射分配一个合适的地址。如果非NULL，则内核在选择地址映射时会将该参数值作为一个提示信息来处理。

+ length：要映射的长度

+ prot: 参数掩码，用于指定映射上的保护信息

+ flags：指定映射类型，有匿名映射，文件映射等

+ fd：要映射文件的文件描述符

+ offset：文件偏移

+ 返回映射内存起始位置

##### munmap

```
int munmap(void *addr, size_t len);
```

+ 去除地址为addr，长度为len的mmap映射


#### 虚拟内存相关

##### mlock 

```
int mlock(const void *addr, size_t len, unsigned int flags);
```

+ 由addr和len涉及到的所有内存块都会锁定到内存而不会换出到swap分区

+ mlock系统调用允许程序在物理内存上锁住它的部分或全部地址空间，这将阻止Linux将这个内存页调度到交换空间

+ 一个严格时间相关的程序可能会希望锁住物理内存，因为内存页面调出调入的时间延迟可能太长或过于不可预知

+ 安全性要求较高的应用程序可能希望防止敏感数据被换出到交换文件中，因为这样在程序结束后，攻击者可能从交换文件中恢复出这些数据

##### munlock 

```
int munlock(const void *addr, size_t len);
```

+ 解除这段区域的内存锁定

##### mincore

```
int mincore(void *addr, size_t length, unsigned char *vec);
```

+ 将起始地址为addr，长度为length的内存驻留信息放入vec

##### mprotect

```
int mprotect(void *addr, size_t len, int prot);
```

修改起始地址为addr，长度为len的虚拟地址的属性为prot



## IO管理


#### 文件系统调用

open read write close mkdir chdir rename lseek stat getdents link unlink symlink dup readv pread writev pwrite

##### fcntl

##### flock

##### fsync



#### IO系统调用

##### ioctl

##### select

##### poll

##### epoll_create

##### epoll_ctl

##### epoll_wait



#### 网络系统调用

##### socket

```
int socket(int domain, int type, int protocol);
```

+ 创建套接字，返回值为套接字的文件描述符

##### bind

##### listen

##### connect

##### accept

##### receive

##### send

##### shutdown









### 并行和并发区别

##### 进程线程状态

##### 上下文切换

### 进程

### 线程

##### 线程的实现

用户线程（协程）、内核线程

### 进程线程区别

### scheduler调度

##### 调度时机

##### 调度原则

##### 调度算法

先来先服务、最短作业优先、高响应比优先调度算法、时间片轮转调度算法、多级反馈队列

### 进程间通信

##### 管道

pipe

##### 消息队列

msgget、msgsnd、msgrcv、msgctl

##### 共享内存

shmget、shmat、shmdt、shmctl

##### 信号量

semget、semop、semctl

##### 信号

kill、raise、sigqueue、abort、alarm、sigaction、signal、sigpromask、sigpending、sigreturn

##### 套接字

socket、bind、listen、accept、connect

### 线程安全

##### 同步与互斥

##### 锁

##### 信号量

##### 条件变量

##### 死锁


# 内存管理

### 虚拟内存

##### 内存分段

##### 内存分页

### 进程地址空间


# 文件管理

### 索引节点inode和目录项dentry

### 超级快、索引节点、数据块

### 虚拟文件系统

### 文件系统

### 硬链接与软链接


# 设备io

# 网络管理


















