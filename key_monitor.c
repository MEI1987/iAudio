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

extern void iAudio_light_ctrl(char* app_name,char* app_value,int v_name,int v_value);
void key_monitor(){
    printf("key monitor init \n");
    int fd;
    if((fd = open("/dev/input/event2", O_RDWR)) < 0){
        perror("Can not open keyboard input file\n");
    
    }
       
    char buf[128]={0};
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(fd , &rfds);
    
    time_t press_t;
    long up_t[10]={0};
    long up_d[10]={0};
    int count=0;
    int sn_count=0;
    int spre_count=0;
    long light_open_t=0;
    long light_ctrl_t=0;//brightness mode time
    int net_count=0;
    long song_n[10]={0};
    long song_pre[10]={0};
    int re_count=0;
    long play_p[10]={0};
    while (1) {
        int ret = select(fd +1, &rfds, NULL, NULL, NULL);
        if (ret < 0)
            continue;
        if (FD_ISSET(fd , &rfds)) {
           int readn = read(fd, buf, sizeof(struct input_event));
            if (readn <= 0) {
                printf("uart read error %d\n", readn);
                continue;
            }
            struct input_event *my_event=(struct input_event*)buf;
            if(my_event->type==EV_KEY){
                 printf("This is a key:%d %d\n",my_event->code,my_event->value);
                 time(&press_t); 
                 up_t[count]=press_t;//voice+ count
                 up_d[count]=press_t;
                 printf("current time :%ld------------\n",press_t);

                switch(my_event->code){
                    case 114:
                        printf("voice down\n");
                        if(my_event->value==0){
                            printf("current time :%ld-------------\n",press_t);
                            if((up_d[count]-up_d[count-1])>2){
                                printf("close Light\n");
                                iAudio_light_ctrl("","",1,0);
                            }
                            count=0;
                        }
                        break;
                    case 115:
                        //voiceup
                        printf("voice up\n");
                        if(my_event->value==0){
                            printf("current time :%d-------------\n",(int)press_t);
                            if((up_t[count]-up_t[count-1])>1){
                                printf("open Light\n");
                                light_open_t=press_t;//flag light open time
                                iAudio_light_ctrl("","",1,1);//必须指向一个有效的对象
                            }
                            count=0;
                        }
                        break;
                    case 163:
                        sn_count++;//被绕晕了，数据相减，最好别用0
                        song_n[sn_count]=press_t;
                        if((press_t-light_open_t)<5){
                            printf("Light brightness ---\n");
                            light_open_t=press_t;//5s内操作时间延迟5s
                        }

                        if(my_event->value==0){
                            int intevl_n=song_n[sn_count]-song_n[sn_count-1];
                            int intevl_p=song_pre[spre_count]-song_pre[spre_count-1];
                            if((intevl_n>3)&&(intevl_p>3)){
                                printf("start network config\n"); 
                                sn_count=0;//松手后就编程了0的原因？ 
                            }
                            else if((intevl_n>3)&&(intevl_p<3)){
                                printf("开始增加亮度\n");
                                light_ctrl_t=press_t; 
                            }
                            else if(press_t-light_ctrl_t<5){//5s内调节亮度`
                                printf("tiaojie liangdu \n"); 
                                light_ctrl_t=press_t;
                            }
                            else{
                                
                                printf("next Song normal Mode\n");
                            
                            }
                        }
                        break;
                    case 164:
                        play_p[re_count]=press_t;
                        if(my_event->value==0){
                            if((play_p[re_count]-play_p[re_count-1])>5){
                                system("reboot");
                                re_count=0;
                            }
                        }
                    
                        printf("key play pause\n");
                        break;
                    case 165:
                        spre_count++;//方后面时，对应的pre song 会是负数,因为你这边按住，那边松开，最大值达不到sn_count，一减就是负的了
                        song_pre[spre_count]=press_t;
                        if((press_t-light_open_t)<5){
                            printf("Light brightness +++\n");
                            light_open_t=press_t;//5s内操作时间延迟5s
                        }
                        if(my_event->value==0){
                            int intevl_n2=song_n[sn_count]-song_n[sn_count-1];
                            int intevl_p2=song_pre[spre_count]-song_pre[spre_count-1];
                            if(((song_n[sn_count]-song_n[sn_count-1])>3)&&((song_pre[spre_count]-song_pre[spre_count-1])>3)){
                            // if ((song_pre[spre_count]-song_pre[spre_count-1])>3){
                                printf("start network config\n");
                                spre_count=0; 
                            }
                            else if(press_t-light_ctrl_t<5){
                                printf("开始降低亮度\n");
                                light_ctrl_t=press_t; 
                            }
                            else{
                                printf("key previous song\n");//brightness ++
                            }
                        }
                        break;
                    default:
                        break;
                }
                count++;
            }
        }
    }
    close(fd);
}



