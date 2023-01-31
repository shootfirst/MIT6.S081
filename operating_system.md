# Operating System

## 进程管理

#### 系统调用接口

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

+ 保证子进程先执行，除非子进程execv或者exit，然后父进程往下执行，记住是接着子进程往下执行

+ 注意子进程在父进程调用vfork的那个函数里绝对不能return，只能exit或者execv

+ 在子进程execv或者exit之前和父进程共享地址空间


##### clone

```

```





## 进程通信、信号和锁


## 内存管理

#### 系统调用

##### brk

##### mmap

##### munmap

##### mlock 

##### munlock 

##### mprotect

##### getpagesize

##### swapon

##### swapoff

##### mincore

##### movepage

##### mbind

##### migratepages

## 文件管理

#### 系统调用

##### open

##### read

##### write

##### pread

##### pwrite

##### readv

##### writev

##### mkdir

##### chdir

##### rename

##### lseek

##### stat

##### getdents

##### dup

##### link

##### unlink

##### symlink

##### pipe

##### fcntl

##### flock

##### close

##### fsync


## IO管理

#### 系统调用

##### ioctl

##### select

##### poll

##### epoll_create

##### epoll_ctl

##### epoll_wait


## 网络管理

#### 系统调用

##### socket

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


















