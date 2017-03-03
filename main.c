/* main.c */


#include "common.h"
//#include <sys/msg.h>//以后尽量不用消息队列
extern void socket_recv_pthread(void);
extern void socket_send_thread(void);


extern int USDK_device_init(void);
extern void USDK_device_txloop(void);
extern void USDK_device_rxloop(void);
extern void iAudio_rx(void);
extern void iAudio_init();
extern void key_monitor();
extern void light_init();


extern void notify_light_init();

extern void notify_light_ctrl(int cmd);
static int Create_Thread_Task(void){
	pthread_t thrd;
	int ret;

	/*------------------------socket------------------------*/
	ret=pthread_create(&thrd,NULL,(void *) socket_recv_pthread,NULL);
	if(ret!=0){
		printf ("Create psocket_recv_pthread! err\n");
		return FALSE;
	}
	  /*-----------------key_monitor  */
    ret=pthread_create(&thrd,NULL,(void *)key_monitor,NULL);
	if(ret!=0){
		printf ("Create key_monitor thread  error\n");
		return FALSE;
	}
	/*---------------------Usdk---------------------*/

	USDK_device_init();
	ret=pthread_create(&thrd,NULL,(void *) USDK_device_txloop,NULL);
	if(ret!=0){
		printf ("Create USDK_device_txloop! err\n");
		return FALSE;
	}
  
	return TRUE;
}


void Dev_modules_init(void){
    notify_light_init();
    notify_light_ctrl(2);//红灯闪烁
    iAudio_init();

	printf("Dev_modules_init!\n");

}

//----------------------------------------main----------------------------
int main(int argc, char **argv)
{
//	int ret;
//	unsigned long filesize = -1;
	
	printf("Intelligent Audio Version \02/27/2017 10:45:11 AM 添加空气魔方版\n");
    FILE *fp;
    time_t timep;
    time(&timep);
    if((fp=fopen(LOG_FILE_PATH, "w+")) == NULL){
        printf("open LOG_FILE_PATH failed!\n ");
    }
    fprintf(fp, "Working Day:%s IntelligentAudio Version  V12/27/2016 11:42:30 AM\n" , ctime(&timep));
		
    fclose(fp);

	if(!Create_Thread_Task()){
		printf("TaskInit error!\n");
		return FALSE;
	}

    Dev_modules_init();
	while(1){
		sleep(1000);
	}
    
}
