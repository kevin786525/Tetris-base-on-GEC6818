#ifndef _COMMONHEADER_H_
#define _COMMONHEADER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <errno.h>
#include <math.h>
#include <error.h>                      //(1)常规头文件

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <dirent.h>                     //(2)文件、目录管理类头文件

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <signal.h>                     //(3)进程通信类头文件

#include <pthread.h>                    //(4)线程类头文件

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>                 //(5)网络类头文件

#include <linux/input.h>                //(6)设备类头文件

#endif