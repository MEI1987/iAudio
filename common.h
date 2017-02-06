#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/un.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/select.h>
#include <sys/reboot.h>
#include <linux/reboot.h>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/input.h>
#include <net/if.h>
#include <signal.h>
#include <time.h>


#undef TRUE
#define TRUE 1

#undef FALSE
#define FALSE 0

#define MAXDATASIZE 256


#define LOG_FILE 
#define LOG_FILE_PATH       "/data/misc/iAudio/config/iAudio.log" 

//#define SS01_DEV_SUPPORT
//#define IrDA_10W_DEV_SUPPOPT
#define IrDA_10W_ALARM_SUPPOPT


typedef unsigned char           uint8;
typedef signed char             int8;
typedef unsigned short int      uint16;
typedef signed short int        int16;
typedef unsigned int            uint32;
typedef signed int              int32;

typedef enum{
	TASK_MIN = 0,
	//TASK_SOCKET_RECV,
	TASK_SOCKET_SMSG,
	TASK_USDK_SMSG,
	//TASK_USDK_REPORT,
	TASK_IAUDIO_SMSG,
	TASK_MAX,
}TASK_TYPE;

typedef enum{
	MSG_MIN = 0,
	MSG_SOCKET,
	MSG_USDK,
	MSG_USDK_CLOUD,
    MSG_IAUDIO,
	MSG_MAX,
}MSG_TYPE;

typedef struct{
	long type;
	unsigned char msg[MAXDATASIZE];
	//char *msg;
}MSG;


int GetMessage(TASK_TYPE m_TaskId, MSG* m_msg);

int SendMsg(TASK_TYPE dst_task, MSG* m_msg);


#endif
