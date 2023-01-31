# Operating System

## 进程管理

#### 系统调用接口

##### fork

##### execv

##### wait

##### exit

##### nanosleep

##### clone

##### vfork



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


















