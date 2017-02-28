/*
 * =====================================================================================
 *
 *       Filename:  key_monitor.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/02/2017 03:58:41 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  meishengmin (MEI), meishengmin@haier.com
 *        Company:  Haier人工智能

 * =====================================================================================
 */



#include "common.h"
//#include "iAudio.h" // 为什么头文件重复引用了Audio.h:12:2: error: unknown type name 'br_dev_handle_t'？

extern void iAudio_light_voice_ctrl(int v_name,int v_value);
extern void notify_light_ctrl(int cmd);
extern void iAudio_sendmsg(char* buf,int len);
void key_monitor(){
    printf("key monitor init \n");
    int fd;
    if((fd = open("/dev/input/event2", O_RDWR)) < 0){
        perror("Can not open keyboard input file\n");
    }
    char buf[128]={0};
    
    time_t press_t;
    //new flag
    long power_on=0;
    long power_off=0;
    
    struct timeval timeout;
    while (1) {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(fd , &rfds);
        time(&press_t);
        timeout.tv_sec = 2;//2.5s   
        timeout.tv_usec = 500000; 
        //printf("----while time ----%ld--\n",press_t);
        if((press_t-power_on>2)&&(power_on!=0)){
            printf("Light Open\n");
            iAudio_light_voice_ctrl(1,1);//必须指向一个有效的对象
            power_on=0;
        }
        else if((press_t-power_off>2)&&(power_off!=0)){
            printf("Light Close\n");
            iAudio_light_voice_ctrl(1,0);//必须指向一个有效的对象
            power_off=0;
        }
        
        int ret = select(fd +1, &rfds, NULL, NULL,&timeout);
        if (ret < 0){
            perror("select ");
            continue; 
        }
        else {
            if (FD_ISSET(fd , &rfds)) {
                int readn = read(fd, buf, sizeof(struct input_event));
                if (readn <= 0) {
                    printf("uart read error %d\n", readn);
                    continue;
                }
                struct input_event *my_event=(struct input_event*)buf;
                if(my_event->type==EV_KEY){
                     printf("This is a key:%d %d\n",my_event->code,my_event->value);

                    switch(my_event->code){
                        case 253://已修改
                            printf("voice down\n");
                            if(my_event->value==0){
                                if(power_off>0){
                                    printf("正常功能：音量减小\n");
                                    char buf_reduce[31]={0};
                                    buf_reduce[0]=0xFE;
                                    buf_reduce[1]=0x1f;
                                    buf_reduce[2]=0x01;
                                    buf_reduce[3]=0x03;
                                    buf_reduce[4]=0x02;//音箱本体`
                                    buf_reduce[5]=0x18;//音箱音乐
                                    buf_reduce[24]=0x01;
                                    buf_reduce[25]=0x0a;
                                    buf_reduce[26]=0x03;
                                    buf_reduce[28]=0x00;//减小音量
                                    buf_reduce[30]=0xFD;
                                    
                                    iAudio_sendmsg(buf_reduce,31);
                                }
                                power_off=0;
                            }
                            if(my_event->value==1){
                                power_off=press_t;
                                printf("current power_off time :%ld-------------\n",power_off);
                            }
                            break;
                        case 254:
                            //voiceup
                            printf("voice up\n");
                            if(my_event->value==0){
                                if(power_on>0){
                                    printf("正常功能：音量增加\n");
                                    char buf_add[31]={0};
                                    buf_add[0]=0xFE;
                                    buf_add[1]=0x1f;
                                    buf_add[2]=0x01;
                                    buf_add[3]=0x03;
                                    buf_add[4]=0x02;//音箱本体`
                                    buf_add[5]=0x18;//音箱音乐
                                    buf_add[24]=0x01;
                                    buf_add[25]=0x0a;
                                    buf_add[26]=0x03;
                                    buf_add[28]=0x01;//增大音量
                                    buf_add[30]=0xFD;
                                    
                                    iAudio_sendmsg(buf_add,31);
                                } 
                                power_on=0;
                            }
                            if(my_event->value==1){
                                power_on=press_t;
                            }
                            break;
                        case 250:
                     
                            if(my_event->value==0){
                                power_on=0;
                                power_off=0;
                        
                                char buf_nextsong[31]={0};
                                buf_nextsong[0]=0xFE;
                                buf_nextsong[1]=0x1f;
                                buf_nextsong[2]=0x01;
                                buf_nextsong[3]=0x03;
                                buf_nextsong[4]=0x02;//音箱本体
                                buf_nextsong[5]=0x18;//音箱音乐
                                buf_nextsong[24]=0x01;
                                buf_nextsong[25]=0x0a;
                                buf_nextsong[26]=0x06;
                                buf_nextsong[28]=0x01;//上一首
                                buf_nextsong[30]=0xFD;
                                iAudio_sendmsg(buf_nextsong,31);
                            }
                            break;
                        case 251://修改后歌曲控制非音量键不再触发安卓原始
                            if(my_event->value==1){//key pause 键反了
                                power_on=0;
                                power_off=0;
                                char buf_ksong[31]={0};
                                buf_ksong[0]=0xFE;
                                buf_ksong[1]=0x1f;
                                buf_ksong[2]=0x01;
                                buf_ksong[3]=0x03;
                                buf_ksong[4]=0x02;//音箱本体
                                buf_ksong[5]=0x18;//音箱音乐
                                buf_ksong[24]=0x01;
                                buf_ksong[25]=0x0a;
                                buf_ksong[26]=0x01;
                                buf_ksong[28]=0x01;//播放、暂停
                                buf_ksong[30]=0xFD;
                                iAudio_sendmsg(buf_ksong,31);
                            }
                            if(my_event->value==0){
                                printf("这里发生1 0反向\n"); 
                            }
                            printf("key play pause\n");
                            break;
                        case 252:
                            if(my_event->value==0){
                            
                                char buf_presong[31]={0};
                                buf_presong[0]=0xFE;
                                buf_presong[1]=0x1f;
                                buf_presong[2]=0x01;
                                buf_presong[3]=0x03;
                                buf_presong[4]=0x02;//音箱本体
                                buf_presong[5]=0x18;//音箱音乐
                                buf_presong[24]=0x01;
                                buf_presong[25]=0x0a;
                                buf_presong[26]=0x06;
                                buf_presong[28]=0x02;//下一首
                                buf_presong[30]=0xFD;
                                iAudio_sendmsg(buf_presong,31);
                            }
                            break;
                        default:
                            break;
                    }//switch
                }//if(my_event->type==EV_KEY)
            }//if(my_event->type==EV_KEY)
        }//else
    }//while(1)
    close(fd);
}



