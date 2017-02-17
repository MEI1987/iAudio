/*
 * =====================================================================================
 *
 *       Filename:  iAudio.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/28/2016 11:12:20 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  meishengmin (MEI), meishengmin@haier.com
 *        Company:  Haier人工智能

 * =====================================================================================
 */

#include <signal.h>
#include <net/if.h>//MEI 2015.12.4
#include "common.h"
#include "socket.h"
#include "ugw_br.h"

#include "iAudio.h"


void iAudio_light_ctrl(char* app_name,char* app_value,int v_name,int v_value);
void get_all_propery(char* op,br_dev_handle_t dev,ugw_request_handle_t request);
extern void socket_send2(void* buf,int len);

void iAudio_music_ctrl(char* app_name,char* app_value);

int set_light_brightness(int brightness);
int get_light_brightness();
int  get_light_status();
void iAudio_sendmsg(char* buf,int len);
dev_handle_t *dev;
br_callback_t cb;
pthread_t thread_register;
Bridge_handle_t *gw;
dev_base_info_handle_t info;

dev_reg_devmap* reg_iAudio_info;
unsigned char iAudio_mac_byte[6]={0};
char iAudio_mac_string[32]={0};
char iAudio_ip_string[16] = {0};
struct music_info music_buf={{0}};

int notify_light_fd;
//-------------------------------------------------------------------

void iAudio_state_cb(br_handle_t gw,
        void * cb_param,
        int status)
{
    printf("get gw status %d\n", status);
}

int iAudio_attr_read_handler(br_dev_handle_t dev,
        void * cb_param,
        ugw_request_handle_t request,
        const char * name)
{
    printf("get attr read %s, %s\n", name, (char *)cb_param);

    //TODO: save the request for rsp
    if(!name)
    {
        printf("read handler name is NULL \n");
        return -1;
    }
    printf("\n get attr read %s\n", name);

    return 0;
}

int iAudio_attr_write_handler(br_dev_handle_t dev_ctr,
        void * cb_param,
        ugw_request_handle_t request,
        const char * name,
        const char * value)
{
    printf("---------iAudio attr write handler------ %s:%s -------------\n", name, value);

    if(dev_ctr==reg_iAudio_info->registered_dev_br){
        //TODO: save the request for rsp
        char* app_name=(char*)name;
        char* app_value=(char*)value;
        for(int i=1;i<11;i++){
            if((strcmp(iAudio_attr[i],app_name)==0)&&(i<5)){
                iAudio_light_ctrl(app_name,app_value,0,0);
                break;
            }    
            else if((memcmp(iAudio_attr[i],app_name,strlen(app_name))==0)&&(i>=5)&&(i<10)){
                 iAudio_music_ctrl(app_name,app_value);
                break;
            }	
        }
        int ret=br_write_dev_attr_rsp(dev_ctr,request,0);
        if(ret!=0){
            printf("回报失败\n");
        }
    }
    else{
        printf("-----App transfer handler error\n -----");
    }
   
	printf("---------iAudio_attr_write_handler end-------------\n");
    return 0;
}

int iAudio_op_handler(br_dev_handle_t dev,
        void * cb_param,
        ugw_request_handle_t request,
        const char * op,
        const param_list_t * params)
{
    printf("get op %s, %s\n", op, (char *)cb_param);
    //TODO: save the request for rsp
    if(dev==reg_iAudio_info->registered_dev_br){
        if(strcmp(op,"getAllProperty")==0){
            param_list_t * args = br_param_list_init();
            int color=get_light_status();
            char scolor[10]={0};
            sprintf(scolor,"%02x",color);
            br_param_list_add_element(args,iAudio_attr[2],scolor);//light color

            br_param_list_add_element(args,iAudio_attr[4],"");//light brightness
            br_param_list_add_element(args,iAudio_attr[5],"");//bofang zanting
            br_param_list_add_element(args,iAudio_attr[8],"");//songName
            br_param_list_add_element(args,iAudio_attr[10],"");//totalTime
            br_dev_operation_rsp(dev,request,0,args);
            br_param_list_destroy(args);
        }
    }
    else{
        printf("-----write -----%d\n",(int)dev);
    }
   // */
    return 0;
}
void get_all_propery(char* op,br_dev_handle_t dev,ugw_request_handle_t request){
     if(strcmp(op,"getAllProperty")==0){
        param_list_t * args = br_param_list_init();
        int color=get_light_status();
        char scolor[10]={0};
        sprintf(scolor,"%02x",color);
        char sbright[10]={0};
        int bright=get_light_brightness();
        sprintf(sbright,"%d",bright);

        br_param_list_add_element(args,iAudio_attr[2],scolor);//light color
        br_param_list_add_element(args,iAudio_attr[4],sbright);//light brightness
        br_param_list_add_element(args,iAudio_attr[5],"");//bofang zanting
        br_param_list_add_element(args,iAudio_attr[8],"");//songName
        br_dev_operation_rsp(dev,request,0,args);
        br_param_list_destroy(args);
    }
}

void iAudio_light_ctrl(char* app_name,char* app_value,int v_name,int v_value){
    int fd;  
    if((fd= open(IAUDIO_LIGHT_DEV, O_RDWR)) < 0){
            perror("Can not open keyboard input file\n");
    }
    int color;
    int mode;
    int bright;
    if((strcmp(iAudio_attr[1],app_name)==0)||(v_name==1)){
        if(v_name!=0){
            color=v_value;
        }
        else{
            color=atoi(app_value);
        }
        int status=0;
        if(color==0){
            status=0x0;//guanji
            write(fd,&status,sizeof(status));
        }
        else if(color==1){
            status=0xFFFFFFFF;//kaiji
            write(fd,&status,sizeof(status));
        }
    }
    if((strcmp(iAudio_attr[2],app_name)==0)||(v_name==3)){// 3 ledcolor
        if(v_name!=0){
            color=v_value;
        }
        else{
            color=atoi(app_value);
        }
        int grb;//7 colors
        switch (color){
            case COLOR_MIN:
                grb=0xFFFFFF;//居然是区分大小写的
                write(fd,&grb,sizeof(grb));
                break;
            case COLOR_RED://bgr
                grb=0x0000FF;
                write(fd,&grb,sizeof(grb));
                break;
            case COLOR_ORANGE:
                grb=0x00A5FF;
                write(fd,&grb,sizeof(grb));
                break;
            case COLOR_YELLOW:
                grb=0x00FFFF;
                write(fd,&grb,sizeof(grb));
                break;
            case COLOR_GREEN:
                grb=0x00FF00;
                write(fd,&grb,sizeof(grb));
                break;
            case COLOR_CYAN:
                grb=0xFFFF00;
                write(fd,&grb,sizeof(grb));
                break;
            case COLOR_BLUE:
                grb=0xFF0000;
                write(fd,&grb,sizeof(grb));
                break;
            case COLOR_PURPLE:
                grb=0xF020A0;
                write(fd,&grb,sizeof(grb));
                break;
            default:
                break;
        }
    }
    else if((strcmp(iAudio_attr[3],app_name)==0)||v_name==3){  //mode
        if(v_name!=0){
            mode=v_value;   
        }
        else{
            mode=atoi(app_value);
        }
        int mode_bgr=0;
        switch (mode){
            case LMODE_STANDARD:
                mode_bgr=0xFAFAFF;
                write(fd,&mode_bgr,sizeof(mode_bgr));
                break;
            case LMODE_READ:
                mode_bgr=0x8FF6FF;
                write(fd,&mode_bgr,sizeof(mode_bgr));
                break;
            case LMODE_ROMANTIC: 
                mode_bgr=0xCBC0FF;
                write(fd,&mode_bgr,sizeof(mode_bgr));
                break;
            case LMODE_SLEEP:
                mode_bgr=0x00D7FF;
                write(fd,&mode_bgr,sizeof(mode_bgr));
                break;
            default:
                break;
        }
    }
    else if((strcmp(iAudio_attr[4],app_name)==0)||v_name==2){
        //brightness
        if(v_name!=0){
            bright=v_value; 
            
            switch(bright){
                case BRIGHT_DOWN:
                {
                    int cur_bri=get_light_brightness();
                    int set_bri=0;
                    if(cur_bri<70){
                        set_bri=++cur_bri;
                    }
                    set_light_brightness(set_bri);
                    break;
                }
                case BRIGHT_UP:
                {
                    int cur_bri=get_light_brightness();
                    int set_bri=0;
                    if(cur_bri>15){
                        set_bri=--cur_bri;
                    }
                    set_light_brightness(set_bri);
                    break;
                }
                case BRIGHT_MIN:
                    set_light_brightness(70);
                    break;
                case BRIGHT_MAX:
                    set_light_brightness(15);
                    break;
                default:
                    break;
            }   
        }
        else{
            bright=atoi(app_value);
            int real_bri=70-(int)((bright/100)*55);
            set_light_brightness(real_bri);
        }
    }
    printf("----------------end --------\n");
    close(fd);
}
void iAudio_music_ctrl(char* app_name,char* app_value){
    printf("music ctrl----------\n");
    char buf[100]={0};
    buf[0]=0xFE;
    buf[1]=0xFE;
    buf[3]=0x02;
    buf[4]=0x05;
    buf[5]=0x18;
    buf[24]=0x01;
    buf[25]=0x0a;
    //Socket_data music_data={{0}};
    if((strcmp(app_name,"playMode")==0)){//播放 暂停
        buf[26]=0x01;
        buf[28]=atoi(app_value);
        buf[30]=0xFD;
        iAudio_sendmsg(buf,31);
    }
    else if((strcmp(app_name,"playControl"))==0){//上一首 下一首
        buf[26]=0x06;
        buf[28]=atoi(app_value);
        buf[30]=0xFD;
        iAudio_sendmsg(buf,31);
    }
    else if((strcmp(app_name,"muteStatus"))==0){//静音
        buf[26]=0x04; 
        buf[28]=atoi(app_value);
        buf[30]=0xFD;
        iAudio_sendmsg(buf,31);
    }
    else if(strcmp(app_name,"alarm")==0){
        buf[26]=0x07;
        //char alarm_value[256]={0};
        int alarm_len=strlen(app_value);
        buf[29]=alarm_len;
        memcpy(&buf[30],app_value,alarm_len);
        buf[30+alarm_len]=0xFD;
        iAudio_sendmsg(buf,30+alarm_len);
    }
}

void iAudio_sendmsg(char* buf,int len){
    MSG mymsg={0};
    mymsg.type=MSG_SOCKET;
    memcpy(&mymsg.msg,buf,len);
    socket_send2(&mymsg,len);
}

void iAudio_music_report(int attr,int min,int val,char* music_info){
    param_list_t * music_args = br_param_list_init();
    char s_val[10]={0};
    sprintf(s_val,"%d",val);
    if(attr==1){//playMode播放暂停
        br_param_list_add_element(music_args,"playMode",s_val);
    }
    else if(attr==5){//
        br_param_list_add_element(music_args,"songName",music_info); 
    }
    else if(attr==4){
        br_param_list_add_element(music_args,"muteStatus",s_val);
    }

    if((br_dev_status_report(reg_iAudio_info->registered_dev_br,music_args))!=0){
        printf("ugw report music attr false!\n");
    }
    printf("music report success! \n");
    br_param_list_destroy(music_args);
}

int  get_light_status(){
    int fd=open(IAUDIO_LIGHT_DEV,O_RDWR);
    if(fd<=0){
       printf("Can not open light file\n"); 
    }

    int light_0x;
    read(fd,&light_0x,sizeof(light_0x));
    
    return light_0x;
    close(fd);
}
int set_light_brightness(int brightness){

    int fd=open(IAUDIO_LIGHT_BRIGHTNESS,O_RDONLY);
    unsigned long args[3];
    args[0] = 0; //必须为0；
    args[1] = brightness;//// 此处亮度值位15--70， 15为最亮，70为最暗
    args[2] = 0;//必须为0;
    int ret=ioctl(fd,DISP_CMD_LCD_SET_BRIGHTNESS,args);
    
    close(fd);
    return ret;

}
int get_light_brightness(){
     
    int fd=open(IAUDIO_LIGHT_BRIGHTNESS,O_RDONLY);
    unsigned long args[3];
    ioctl(fd,DISP_CMD_LCD_GET_BRIGHTNESS,args);
    int bright=(int)args[1]; 
    close(fd);

    return bright;
}
void iAudio_light_report(){
    printf("start test light\n");    
    int fd=open(IAUDIO_LIGHT_DEV,O_RDWR);
    if(fd <= 0){
        printf("Can not open keyboard input file\n");
    }

    //char buf[128]={0};
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(fd , &rfds);
    int readrgb;
    while (1) {
        int ret = select(fd +1, &rfds, NULL, NULL, NULL);
        if (ret < 0)
            continue;
        if (FD_ISSET(fd , &rfds)) {
           int readn = read(fd, &readrgb, sizeof(int));
            if (readn <= 0) {
                printf("uart read error %d\n", readn);
                continue;
            }

            if(readrgb!=0){
                 printf("LIGHT fashengle bianhua :%02x \n",readrgb);
            }
        }
    }
}
void notify_light_init(){
    if((notify_light_fd= open(IAUDIO_NOTIFY_LIGHT, O_RDWR)) < 0){
        perror("fail to open");
    }
    return;

}
void notify_light_ctrl(int cmd){
    write(notify_light_fd,&cmd,sizeof(cmd));
    return;
}
void notify_light_close(){
    close(notify_light_fd);
}
//----------------------------------------------------------------------------------
dev_base_info_handle_t dev_br_handle(char* dev_mac,char* typeid){
	
	dev_base_info_handle_t info;
	printf(" Now creat a dev_br_handle device mac: %s\n",dev_mac);
	info = br_malloc_dbi(dev_mac,typeid,
		"MqttUGW", "0.0.0", "IAUDIO", "0.0.0", 0);
	if (!info){
		printf("br_malloc_dbi error\n");
	}
	
	return info;
}



char* get_iAudio_mac()//MEI 2015.12.4
{
	struct ifreq ifreqw;
	int sock = 0;

	//sock = socket(AF_INET,SOCK_STREAM,0);
	sock = socket(AF_INET,SOCK_DGRAM,0);//SOCK_DGRAM
	if(sock < 0){
		perror("get mac error sock\n");
	}

	strcpy(ifreqw.ifr_name,"wlan0");
	if(ioctl(sock,SIOCGIFHWADDR,&ifreqw) < 0){
		perror("get mac error ioctl\n");
	}

	memcpy(iAudio_mac_byte,ifreqw.ifr_hwaddr.sa_data,6);
	sprintf(iAudio_mac_string,"%02X%02X%02X%02X%02X%02X", \
		iAudio_mac_byte[0],iAudio_mac_byte[1],iAudio_mac_byte[2],iAudio_mac_byte[3],iAudio_mac_byte[4],iAudio_mac_byte[5]);
	printf("Dev mac :%s\n",iAudio_mac_string);

	return iAudio_mac_string;
}

char* get_hostIP(char* nwcard_name)//MEI change 2016年07月01日 11时16分48秒
{
    int sock_get_ip;

    struct   sockaddr_in *sin;
    struct   ifreq ifr_ip;   

    if ((sock_get_ip=socket(AF_INET, SOCK_STREAM, 0)) == -1){
         printf("socket create failse...GetLocalIp!/n");
         return NULL;
    }
   
    memset(&ifr_ip, 0, sizeof(ifr_ip));   
    strncpy(ifr_ip.ifr_name, nwcard_name, sizeof(ifr_ip.ifr_name) - 1);   
 
    if( ioctl( sock_get_ip, SIOCGIFADDR, &ifr_ip) < 0 )   {   
         return NULL;   
    }     
    sin = (struct sockaddr_in *)&ifr_ip.ifr_addr;   
    
    printf("%s\n",inet_ntoa(sin->sin_addr));    
    close( sock_get_ip );
    
    return inet_ntoa(sin->sin_addr);       
}
//--------------------------------
void iAudio_init()//self register
{
	//char cur_ip[32]={0};
    //	signal(SIGPIPE, SIG_IGN);
//	br_debug_start(printf, 2);

	get_iAudio_mac();

    char* cur_ip=get_hostIP("wlan0");
    if(cur_ip==NULL)
       strcpy(cur_ip,"100.100.100.100");
	
	info = dev_br_handle(iAudio_mac_string,IAUDIO_TYPE);
	
	if (!info){
		printf("cae_malloc_dbi_simple fail \n");
	}

	gw = br_creat_instance(iAudio_state_cb,cur_ip, info);
	if (!gw){
		printf("cae_cloud_create_devGW fail \n");
	}

	cb.read_cb = iAudio_attr_read_handler;
	cb.write_cb = iAudio_attr_write_handler;
	cb.op_cb = iAudio_op_handler;

	dev = br_add_dev(gw, info, &cb,NULL);//add main
	if (NULL==dev){
		printf("cae_cloud_add_dev_simple fail:br_add_dev error \n");
	}
    printf("Creat iAudio success\n");
    
    reg_iAudio_info=(dev_reg_devmap*)malloc(sizeof(dev_reg_devmap));
	reg_iAudio_info->registered_dev_br=dev->br_dev_hle;
	memcpy(reg_iAudio_info->registered_devid,iAudio_mac_string,32);
	reg_iAudio_info->devtype=DEV_IAUDIO;
	
}

//-------------------iAudio Socket ----------
void iAudio_unpack(void* msg,int msg_len){
   // int socket_len=msg_len;
    Socket_data* iAudio_soc=(Socket_data*)msg;//test buyong memcpy
    char music_info[256]={0};
    printf("iAudio unpack-------------\n");
    switch (iAudio_soc->dev){
        case DEV_AudioLight:
            iAudio_light_ctrl(NULL,NULL,iAudio_soc->devAttr[1],iAudio_soc->statusVal[1]);
            break;
        case DEV_AudioMusic:
            if(iAudio_soc->devName_len!=0){
                memcpy(music_info,&iAudio_soc->devName[0],iAudio_soc->devName_len);
            }
            
            iAudio_music_report(iAudio_soc->devAttr[1],iAudio_soc->statusVal[0],iAudio_soc->statusVal[1],music_info);
            break;
        default:
            break;
    }
}
