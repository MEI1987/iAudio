/*  usdk-device.c */
/* wifi home app */

#include "common.h"
#include "ugw.h"
#include "Usdk-device.h"
#include "socket.h"

ugw_ctrl_dev dev_ctrl_list[10];


static void* dev_list_callback(void *arg, context_t *context);
static void* attr_push_callback(void *arg, context_t *context);
static void* alarm_push_callback(void *arg, context_t *context);
static uint8 judge_wifi_dev(char* device_type);
static void USDK_devid_to_upmac(char* devid);
static void USDK_devid_to_listmac(char* devid);
static void Aircondition_StateUpdate(char* aircondition_devmac);
static void AirFilter_StateUpdate(char* aircondition_devmac);
static void WaterHeater_StateUpdate(char* aircondition_devmac);
static void WashingMachine_StateUpdate(char* WashingMachine_devmac);
static void *wstime_up_f(void);
extern void attr_to_cloud(char* attr[],char* attr_val[],int attr_num);
extern void Risco_alarm_to_cloud(char alarm_value1,char alarm_value2,char* attrvalue);

static void WIFI_DevState_update(uint8 devtype,char* devmac);

extern void socket_send2(void* buf);
extern char* Robot_Attr[];
int dev_num;
static  ugw_handle_t *handle;
static ugw_listener_t ugw_listener = {NULL,dev_list_callback,alarm_push_callback,attr_push_callback,NULL};

static int Ugw_init_OK = FALSE;

static uint8 USDK_socketNo = 1;

static  uint8 output_MAC[32]={0};

extern uint8 CmdNo_save;


static void USDK_send_dev_msg(void *msg , uint8 len)
{
	MSG pmsg = {0};

	pmsg.type		= MSG_USDK;
	memcpy(&pmsg.msg, msg,  len);

	socket_send2( &pmsg);
}

static void* dev_list_callback(void *arg, context_t *context){
    dev_num=context->dev_count;
	printf("dev_list_callback come in:dev_count = %d------------------------\n",context->dev_count);
	for(int i=0;i<context->dev_count;i++)
		printf("device_id:%s,ip:%s,connect_status:%d,connect_type:%d\n",context->devs[i].device_id,context->devs[i].ip,context->devs[i].connect_status,context->devs[i].connect_type);
}

static void* attr_push_callback(void *arg, context_t *context){
	
	time_t timep;
	time(&timep);
    int wif_devtype;	
	printf( "attr current time:%s \n" , ctime(&timep));
	
	for(int i=0;i<context->pair_count;i++)
		printf("name:%s,value:%s\n",context->pairs[i].name,context->pairs[i].value);
/*    context_t* ctx = ugw_new_context();

    ugw_get_devs(handle, ctx);

    for(int i=0;i<ctx->dev_count;i++){
        if(memcmp(ctx->devs[i].device_id,context->device_id,16)==0){
            printf("%s-------------%s\n",ctx->devs[i].device_id,ctx->devs[i].deviceType);

            wif_devtype=judge_wifi_dev(ctx->devs[i].type_id);

            WIFI_DevState_update(wif_devtype,context->device_id);
        }
    }
    ugw_free_context(ctx);
*/
}

static void* alarm_push_callback(void *arg, context_t *context){
	printf("alarm_push_callback come in:device_id=%s ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n",context->device_id);

	for(int i=0;i<context->pair_count;i++){
		printf("alarm name:%s,value:%s\n",context->pairs[i].name,context->pairs[i].value);
	}
}
static uint8 judge_wifi_dev(char* type_id)//MEI 2015.11.6
{
    int  wifi_device=0;

    if(memcmp(type_id,Aircon_DEV,strlen(Aircon_DEV))==0){
        wifi_device=DEV_WIFI_Aircondition;      
    }
 
    else if(memcmp(type_id,Airfil_DEV,strlen(Airfil_DEV))==0){
        wifi_device=DEV_WIFI_AirFilter;
     }

    else if(memcmp(type_id,WaterHeater_DEV,strlen(WaterHeater_DEV))==0){
        wifi_device=DEV_WIFI_WaterHeater;
    }
    else if(memcmp(type_id,WashMachine_DEV,strlen(WashMachine_DEV))==0){

        wifi_device=DEV_WIFI_Washer;
    }
    return wifi_device;

}
static void WIFI_DevState_update(uint8 devtype,char* devmac)//MEI2015.11.18
{
    switch(devtype) {
        case DEV_WIFI_Aircondition:

            Aircondition_StateUpdate(devmac);
            break;
        case DEV_WIFI_AirFilter:

             AirFilter_StateUpdate(devmac);
             break;
        case DEV_WIFI_WaterHeater:

            WaterHeater_StateUpdate(devmac);
            break;

        case DEV_WIFI_Washer:

            WashingMachine_StateUpdate(devmac);
            break;
        default:
            break;
            }
}

/*
int ugw_get_same_type_devs(uint8 dev){
	 int devNum = 0 ;
	context_t* context = ugw_new_context();
	 int ret = ugw_get_devs(handle, context);
	

	if( context->dev_count == 0){
       printf("ugw_get_devs_list :ret ==%d,count:%d\n",ret,context->dev_count);
		printf("ugw_get_devs_list :no dev\n");
		return 0;
	}

	devNum = 0;
	for(int i=0;i<context->dev_count;i++){
		printf("device_id:%s,ip:%s,connect_status:%d,connect_type:%d,device_type:%s---type_id:%s\n",
			context->devs[i].device_id,context->devs[i].ip,context->devs[i].connect_status,
			context->devs[i].connect_type, context->devs[i].deviceType,context->devs[i].type_id);

		switch(dev){
			case DEV_WIFI_Aircondition:
				if((memcmp(context->devs[i].deviceType , AIRCON_TANK_DEV ,10) == 0)||
				   (memcmp(context->devs[i].deviceType , AIRCON_SPLIT_DEV, 10)==0)){
					memcpy( dev_ctrl_list[devNum].devType, context->devs[i].deviceType, 10);
					memcpy(dev_ctrl_list[devNum].devID, context->devs[i].device_id, 32);
					devNum++;
				}
				
				break;

			case DEV_WIFI_AirFilter:
				if(memcmp(context->devs[i].deviceType , AIRFILlTER_DEV ,10) == 0){
					memcpy( dev_ctrl_list[devNum].devType, context->devs[i].deviceType, 10);
					memcpy(dev_ctrl_list[devNum].devID, context->devs[i].device_id, 32);
					devNum++;
				}
				
				break;

			case DEV_WIFI_WaterHeater:
				if(memcmp(context->devs[i].deviceType , WATER_HEATER_DEV ,10) == 0){
					memcpy( dev_ctrl_list[devNum].devType, context->devs[i].deviceType, 10);
					memcpy(dev_ctrl_list[devNum].devID, context->devs[i].device_id, 32);
					devNum++;
				}
				
				break;
			case DEV_WIFI_Washer://MEI 2016.1.26
				if(memcmp(context->devs[i].deviceType , ROLLER_WASHINGMA_DEV,10) == 0){
					memcpy( dev_ctrl_list[devNum].devType, context->devs[i].deviceType, 10);
                    
					memcpy(dev_ctrl_list[devNum].devID, context->devs[i].device_id, 32);
					devNum++;
				}
		}
	}

	ugw_free_context(context);

	return devNum;
}
*/

static uint8 ConvertCharToHex(char ch)
{
    if ((ch >= '0')&&(ch <= '9'))

    {
        return (ch-'0');
    }

    else if ((ch >= 'A')&&(ch <= 'Z'))

    {
        return ((ch-'A')+0x0A);
    }

    else if ((ch >= 'a')&&(ch <= 'z'))

    {
        return ((ch-'a')+0x0A);
    }
}

static void USDK_devid_to_upmac(char* devid){

	char mac[32]={0};
	int i=0;
	int j=0;
	char output[32]={0};

	while(*devid!='\0'){
		if(j>3){
			mac[i]=*devid;
			i++;
		}
		devid++;
		j++;
	}
        for(i=0; i<(strlen(mac)/2); i++){
             output_MAC[i] = (ConvertCharToHex(mac[i*2])<<4);
       	     output_MAC[i] += ConvertCharToHex(mac[i*2+1]);
            
            printf("0x%x ",  output_MAC[i]);
        }
	printf("\n");
}


static void USDK_devid_to_listmac(char* devid){

	uint8 mac[32]={0};
	int i=0;

	while(*devid!='\0'){
		mac[i++]=*devid++;	
	}

    for(i=0; i<(strlen(mac)/2); i++){

         output_MAC[i] = (ConvertCharToHex(mac[i*2])<<4);
         output_MAC[i] += ConvertCharToHex(mac[i*2+1]);
        
        printf("0x%x ",  output_MAC[i]);
    //sprintf(buf,"0x%x",buf+output[i]);
    }
	printf("\n");
}


static void USDK_msg_up(void *data, uint8 len,uint8* mac_id)
{
	uint8 buf [100]={0};
	uint8 up_length=len;

	memcpy(buf,data,len);

	buf[3]=0x03;
	buf[4]=0x01;
	for(int i=0;i<10;i++){
		buf[8+i] = *mac_id++;
		//mac_id++;
	}
	USDK_send_dev_msg(buf , up_length);

}

/****************************Airconditon***********************************/
static int Airconditon_ctrl(uint8 Attr, uint8 Val)
{
	uint8 devAttr = Attr;
	uint8 statusVal = Val;
    char name[10]={0};
    char value[10]={0};
	int ret;
	context_t *ctx = ugw_new_context();

    ugw_get_devs(handle, ctx);

    for(int i=0;i<ctx->dev_count;i++){
       if(strcmp(ctx->devs[i].type_id,Aircon_DEV)==0){
       // if(memcmp(ctx->devs[i].type_id,Aircon_DEV,strlen(Aircon_DEV))==0){
            if(devAttr==0x19)
            {	
                if ((ret = ugw_get_attr(handle, ctx, ctx->devs[i].device_id,"602001")) != 0){
                    printf("Airconditon_ctrl err =%d",ret);
                    printf("%s CMD_ASK(TEMP) error!\n",ctx->devs[i].device_id);
                    ret = FALSE;
                }
                else{
                        int ti;
                        for(ti=0; ti<=30; ti ++){
                            if(strcmp(ctx->value, Attr_val[ti]) == 0){
                                if(statusVal){
                                     if( ti == 30)
                                        statusVal = 30;
                                     else
                                        statusVal = ti+1;
                                }
                                else{
                                     if(ti == 0)
                                        statusVal = 0;
                                     else
                                        statusVal = ti-1;
                                }
                                break;
                            }
                        }
                    strcpy(name,"20200E");
                    sprintf(value,"%d",statusVal); 
                }
                            /*    
                        case AIRCON_windSpeed:{
                            int wi;
                            for(wi = 1;wi <=4 ;wi++){
                                if(strcmp(ctx->value, Attr_val[wi]) == 0){
                                    //printf("wi =%d\n", wi);
                                    if(statusVal){
                                         if(wi >= 3)
                                            statusVal = 3;
                                         else
                                            statusVal = wi+1;
                                    }
                                    else{
                                         if((wi==1) ||(wi==4))
                                            statusVal = 1;
                                         else
                                            statusVal = wi-1;
                                    }
                                    break;
                                }
                            }																					    
                        }
                        break;*/
                    }
                    
            if(devAttr==3){//kaiguanji懒得弄了，直接写把
                if(statusVal==0){
                    strcpy(name,"202001");
                    strcpy(value,"202001");
                    printf("---------%s--------%s\n",name,value);
                }
                else if(statusVal==1){
                    strcpy(name,"202002");
                    strcpy(value,"202002");
                }
            }
            else if(devAttr==4){//yunxing moshi 
                strcpy(name,"20200D");
                if(statusVal==1){//auto
                    strcpy(value,"302000");
                }
                else if(statusVal==2){//cool
                    strcpy(value,"302001");
                }
                else if(statusVal==3){//heat
                    strcpy(value,"302004");
                }
                else if(statusVal==4){//wind
                    strcpy(value,"302006");
                }
                else if(statusVal==5){//chushi
                    strcpy(value,"302002");
                }
            }
            else if(devAttr==5){//wendu
                strcpy(name,"20200E");
                sprintf(value,"%d",statusVal); 
            }
            else if(devAttr==7){//fengsu
                strcpy(name,"20200F");
                if(statusVal==4){//auto
                    strcpy(value,"302005");
                }
                else if(statusVal==1){//low
                    strcpy(value,"302003");
                }
                else if(statusVal==2){//mid
                    strcpy(value,"302002");
                }
                else if(statusVal==3){//gao
                    strcpy(value,"302001");
                }
            }
            else if(devAttr==0x08){//xinfeng
                strcpy(name,"20200I");
                if(statusVal==0){
                    strcpy(value,"302000");
                }
                else if(statusVal==1){
                    strcpy(value,"302008");
                }
            }
            else if(devAttr==0x09){//xinfeng
                strcpy(name,"20200J");
                if(statusVal==0){
                    strcpy(value,"302000");
                }
                else if(statusVal==1){
                    strcpy(value,"302007");
                }
            }
            if(devAttr==0x0C){//xinfeng
                if(statusVal==0){
                    strcpy(name,"202003");
                    strcpy(value,"202003");
                }
                else if(statusVal==1){
                    strcpy(name,"202004");
                    strcpy(value,"202004");
                }
            }
        	if ((ret = ugw_set_attr(handle, ctx, ctx->devs[i].device_id,name,value)) != 0){
                printf("set (%s) failed; ret=%d\n", AirconAttr[devAttr], ret);
                printf("%s CMD_CTRL(%s):%s error!\n",ctx->devs[i].device_id,AirconAttr[devAttr],Attr_val[statusVal]);
                ret = FALSE;
            }
            else{
                ret = TRUE;
            }
        }
    }
	ugw_free_context(ctx);
	
	return ret;
}
static int Airconditon_AskState(int dev,uint8 Attr, uint8 *Val)
{
	int ret;

	context_t *ctx = ugw_new_context();
	if ((ret = ugw_get_attr(handle, ctx, dev_ctrl_list[dev].devID,  AirconAttr[Attr])) != 0){
		printf("Airconditon_AskState err =%d",ret);
		printf("%s CMD_ASK(%s) error!\n",dev_ctrl_list[dev].devID,AirconAttr[Attr]);
		ret = FALSE;
   	}
	else{				
		*Val =(uint8)atoi(ctx->value);//value<-->last
		ret = TRUE;
	}
	ugw_free_context(ctx);

	return ret;
}

static void Aircondition_StateUpdate(char* aircondition_devmac){

	int ret;
	uint8 atr_val[19]={0};
	uint8 air_upmac[10]={0};
	context_t *ctx = ugw_new_context();

	for(int Attr=1;Attr<20;Attr++){	
		if ((ret = ugw_get_attr(handle, ctx, aircondition_devmac,  AirconAttr[Attr])) != 0){
			printf("Airconditon_UpDate err =%d\n",ret);
			printf("%s ATTR_UPDATE(%s) error!\n",aircondition_devmac,AirconAttr[Attr]);
	   	}
		else{	
			atr_val[Attr-1]=atoi(ctx->value);
		}
	}
	//attr info to socket
	uint8 buf_air[100]={0};
	buf_air[0]=SOCKET_HEAD_0xFE;
	buf_air[1]=0x1f;
	buf_air[2]=0x01;
	buf_air[5]=0x10;
	buf_air[24] = 0x01;
	buf_air[25]=0x02;//on-off
	buf_air[26]=0x03;//on-off
	buf_air[27]=0x00;
	buf_air[28]=atr_val[3-1];
	buf_air[31]=SOCKET_END_0xFD;
	USDK_devid_to_upmac(aircondition_devmac);
	USDK_msg_up(buf_air, 31,output_MAC);
	buf_air[26]=0x01;//indoortemp
	buf_air[28]=atr_val[1-1];
	usleep(500);
	USDK_msg_up(buf_air, 31,output_MAC);
	buf_air[26]=0x04;//mode
	buf_air[28]=atr_val[4-1];
	usleep(500);
	USDK_msg_up(buf_air, 31,output_MAC);
	buf_air[26]=0x07;//windspeed
	buf_air[28]=atr_val[7-1];
	usleep(500);
	USDK_msg_up(buf_air, 31,output_MAC);
	
	//attr info upto link list
	USDK_devid_to_listmac(aircondition_devmac);

	memcpy(air_upmac,output_MAC,10);

	ugw_free_context(ctx);
}

/****************************AirFilter***********************************/

static int AirFilter_ctrl(uint8 Attr, uint8 Val){
	uint8 devAttr = Attr;
	uint8 statusVal = Val;
	int ret;
						
	context_t *ctx = ugw_new_context();
    for(int i=0;i<ctx->dev_count;i++){
        if(memcmp(ctx->devs[i].type_id,Airfil_DEV,strlen(Airfil_DEV))==0){
            
            if ((ret = ugw_set_attr(handle, ctx,ctx->devs[i].device_id, 
            AirFilterAttr[devAttr], Attr_val[statusVal])) != 0){
            printf("AirFilter_ctrl failed; ret=%d\n", ret);
            printf("%s CMD_CTRL(%s) error!\n",ctx->devs[i].device_id,AirFilterAttr[devAttr]);
            ret = FALSE;
            }
            else{
                ret = TRUE;
            }
               
        }
    }

	ugw_free_context(ctx);
	return ret;
}

static int AirFilter_AskState(int dev,uint8 Attr, uint8 *Val){
	int ret;

	context_t *ctx = ugw_new_context();
	if ((ret = ugw_get_attr(handle, ctx, dev_ctrl_list[dev].devID,  AirFilterAttr[Attr])) != 0){
		printf("Airconditon_AskState err =%d",ret);
		printf("%s CMD_ASK(%s) error!\n",dev_ctrl_list[dev].devID,AirFilterAttr[Attr]);
		ret = FALSE;
   	}
	else{				
		*Val =(uint8)atoi(ctx->value);//value<-->last
		ret = TRUE;
	}
	ugw_free_context(ctx);

	return ret;
}


static void AirFilter_StateUpdate(char* airfilter_devmac){

	int ret;
	uint8 atr_val[12]={0};
	uint8 filter_upmac[10]={0};
	context_t *ctx = ugw_new_context();

	for(int Attr=1;Attr<13;Attr++){	
		if ((ret = ugw_get_attr(handle, ctx, airfilter_devmac,  AirFilterAttr[Attr])) != 0){
			printf("AirFilter__UpDate err =%d\n",ret);
			printf("%s ATTR_UPDATE(%s) error!\n",airfilter_devmac,AirFilterAttr[Attr]);
	   	}
		else{	
			atr_val[Attr-1]=atoi(ctx->value);
		}
	}

    uint8 buf_airf[100]={0};
	buf_airf[0]=SOCKET_HEAD_0xFE;
	buf_airf[1]=0x1f;
	buf_airf[2]=0x01;
	buf_airf[5]=0x11;
	buf_airf[24] = 0x01;
	buf_airf[25]=0x03;//
	buf_airf[26]=0x09;//pm2.5
	buf_airf[27]=0x00;
	buf_airf[28]=atr_val[9-1];
	buf_airf[31]=SOCKET_END_0xFD;
	USDK_devid_to_upmac(airfilter_devmac);
	USDK_msg_up(buf_airf, 31,output_MAC);
	//attr info upto link list
	USDK_devid_to_listmac(airfilter_devmac);

	memcpy(filter_upmac,output_MAC,10);

	ugw_free_context(ctx);
}

/****************************WaterHeater***********************************/
static int WaterHeater_ctrl(int attr, int val){
	int ret;
    char name[10]={0};
    char value[10]={0};
						
	context_t *ctx = ugw_new_context();

	for(int i=0;i<ctx->dev_count;i++){
	/*	printf("device_id:%s,ip:%s,connect_status:%d,connect_type:%d,device_type:%s\n",
			context->devs[i].device_id,context->devs[i].ip,context->devs[i].connect_status,
			context->devs[i].connect_type, context->devs[i].deviceType);*/
        if(strcmp(ctx->devs[i].type_id,WaterHeater_DEV)==0){
            if(attr==0x01){
                strcpy(name,"218001");//kaiguanji
                if(val==0){
                strcpy(value,"318000");
                }
                else if(val==1){
                strcpy(value,"318001");
                }
            }
            else if(attr==0x03){
                strcpy(name,"218002");//shezhi temp
                for(int num=35;num<71;num++){
                    if(num==val){
                        strcpy(value,waterheaterTem_val[num-35]);
                    }
                }
            }
            else if(attr==0x08){//qingjing moshi
                strcpy(name,"21800e");
                if(val==0x00){
                    strcpy(value,"318000");
                }
                else if(val==0x01){
                    strcpy(value,"318001");
                }                
                else if(val==0x02){
                    strcpy(value,"318002");
                }
            }
        ret = ugw_set_attr(handle, ctx, ctx->devs[i].device_id,name,value);
             if(ret!=0){
                printf("Water Heater  ctrl failed; ret=%d\n", ret);
             }

        }
    }

	ugw_free_context(ctx);
	return ret;
}
static void WaterHeater_group_ctrl(int val_num, char* group_cmd,pair_t * pairvalue){
	int ret;
	context_t *ctx = ugw_new_context();
    for(int i=0;i<ctx->dev_count;i++){
        if(memcmp(ctx->devs[i].type_id,Airfil_DEV,strlen(WaterHeater_DEV))==0){
            if ((ret = ugw_exe_ops(handle, ctx,ctx->devs[i].device_id, 
            group_cmd, val_num, pairvalue, NULL))  != 40016){
            printf("WaterHeater %s CMD_CTRL set group ctrl(%s) failed; ret=%d\n",ctx->devs[i].device_id,group_cmd, ret);
            }
            else{
                printf("WaterHeater group_cmd(%s) ctrl success!\n",group_cmd);
            }		
        }
    }
			
	ugw_free_context(ctx);
}

static int WaterHeater_AskState(int dev,uint8 Attr, uint8 *Val){
	int ret;

	context_t *ctx = ugw_new_context();
	if ((ret = ugw_get_attr(handle, ctx, dev_ctrl_list[dev].devID,  WaterHeaterAttr[Attr])) != 0){
		printf("Airconditon_AskState err =%d",ret);
		printf("%s CMD_ASK(%s) error!\n",dev_ctrl_list[dev].devID,WaterHeaterAttr[Attr]);
		ret = FALSE;
   	}
	else{				
		*Val =(uint8)atoi(ctx->value);//value<-->last
		ret = TRUE;
	}
	ugw_free_context(ctx);

	return ret;
}


static void WaterHeater_StateUpdate(char* WaterHeater_devmac){

	int ret;
	uint8 atr_val[7]={0};
	context_t *ctx = ugw_new_context();

	for(int Attr=1;Attr<8;Attr++){	
		if ((ret = ugw_get_attr(handle, ctx, WaterHeater_devmac,  WaterHeaterAttr[Attr])) != 0){
			printf("WaterHeater_UpDate err =%d\n",ret);
			printf("%s ATTR_UPDATE(%s) error!\n",WaterHeater_devmac,WaterHeaterAttr[Attr]);
	   	}
		else{	
			atr_val[Attr-1]=atoi(ctx->value);
		}
	}
//attr info to socket
	uint8 buf_wh[100]={0};
	buf_wh[0]=SOCKET_HEAD_0xFE;
	buf_wh[1]=0x1f;
	buf_wh[2]=0x01;
	buf_wh[5]=0x12;
	buf_wh[24] = 0x01;
	buf_wh[25]=0x04;//on-off
	buf_wh[26]=0x01;//on-off
	buf_wh[27]=0x00;
	buf_wh[28]=atr_val[1-1];
	buf_wh[31]=SOCKET_END_0xFD; //其实写成错了，但是并不影响01/12/2017 05:26:13 PM
	USDK_devid_to_upmac(WaterHeater_devmac);
	USDK_msg_up(buf_wh, 31,output_MAC);
	buf_wh[25]=0x04;
	buf_wh[26]=0x03;//targetTemp
	buf_wh[28]=atr_val[3-1];
	usleep(500);
	USDK_msg_up(buf_wh, 31,output_MAC);
	buf_wh[25]=0x04;
	buf_wh[26]=0x04;//currentTemp
	buf_wh[28]=atr_val[4-1];
	usleep(500);
	USDK_msg_up(buf_wh, 31,output_MAC);
//attr info upto link list
	USDK_devid_to_listmac(WaterHeater_devmac);

	ugw_free_context(ctx);

}

/****************************WASHINGMACHINE**********************************MEI2016.1.15*/
static int WashingMachine_ctrl(uint8 Attr, uint8 Val){
	uint8 devAttr = Attr;
	uint8 statusVal = Val;
	int ret;
						
	context_t *ctx = ugw_new_context();
    for(int i=0;i<ctx->dev_count;i++){
        if(memcmp(ctx->devs[i].type_id,Airfil_DEV,strlen(WaterHeater_DEV))==0){
            if ((ret = ugw_set_attr(handle, ctx, ctx->devs[i].device_id, 
            WashingMachineAttr[devAttr], Attr_val[statusVal])) != 0){
            printf("WasherMachine_ctrl failed; ret=%d\n", ret);
            printf("%s CMD_CTRL(%s) error!\n",ctx->devs[i].device_id,WashingMachineAttr[devAttr]);
            ret = FALSE;
            }
            else{
                ret = TRUE;
            }
        }
  }

	ugw_free_context(ctx);

	return ret;
}
static int CookerHood_ctrl(int attr,int val){
	int ret;
    char name[10]={0};
    char value[10]={0};
						
	context_t *ctx = ugw_new_context();

	for(int i=0;i<ctx->dev_count;i++){
	/*	printf("device_id:%s,ip:%s,connect_status:%d,connect_type:%d,device_type:%s\n",
			context->devs[i].device_id,context->devs[i].ip,context->devs[i].connect_status,
			context->devs[i].connect_type, context->devs[i].deviceType);*/
        if(strcmp(ctx->devs[i].type_id,CookerHood_DEV)==0){
            if(attr==0x01){
                strcmp(name,"209001");//open close
                if(val==0){
                    strcpy(value,"209000");
                }
                else if(val==1){
                    strcpy(value,"209001");
                }
            }
            else if(attr=0x02){
                strcmp(name,"209002");//light open close
                if(val==0){
                    strcpy(value,"309000");
                }
                else if(val==1){
                    strcpy(value,"309001");
                }
            }
            else if(attr=0x03){
                strcmp(name,"209004");//windspeed open close
                if(val==0){
                    strcpy(value,"309000");
                }
                else if(val==1){
                    strcpy(value,"309001");
                }
                else if(val==2){
                    strcpy(value,"309002");
                }
                else if(val==3){
                    strcpy(value,"309003");
                } 
            }
            if(attr==0x04){
                strcmp(name,"209003");//delay open close
                if(val==0){
                    strcpy(value,"309000");
                }
                else if(val==1){
                    strcpy(value,"309001");
                }
            }
             ret = ugw_set_attr(handle, ctx, ctx->devs[i].device_id,name,value);
             if(ret!=0){
                printf("Cooker Hood ctrl failed; ret=%d\n", ret);
             }
        }
    }
	ugw_free_context(ctx);

	return ret;
}
/*  
static int WashingMachine_AskState(int dev,uint8 Attr, uint8 *Val){

	int ret;

	context_t *ctx = ugw_new_context();
	if ((ret = ugw_get_attr(handle, ctx, dev_ctrl_list[dev].devID,  WaterHeaterAttr[Attr])) != 0){
		printf("WashingMachine_AskState err =%d",ret);
		printf("%s CMD_ASK(%s) error!\n",dev_ctrl_list[dev].devID,WaterHeaterAttr[Attr]);
		ret = FALSE;
   	}
	else{				
		*Val =(uint8)atoi(ctx->value);//value<-->last
		ret = TRUE;
	}
	ugw_free_context(ctx);

	return ret;
}*/
 void *wstime_up_f(void){
	
    while(1){
        int time_value[2]={0};
        char buf_timeup[50]={0};
        int run_state=0;
        context_t* context = ugw_new_context();
        int ret = ugw_get_devs(handle, context);
        
        for(int i=0;i<context->dev_count;i++){
        
            if(DEV_WIFI_Washer==judge_wifi_dev(context->devs[i].deviceType)){
                if((ret = ugw_get_attr(handle, context, context->devs[i].device_id,  WashingMachineAttr[2])) != 0){
                    printf("%s WS_TIME_UPDATE(%s) error!\n",context->devs[i].device_id,WashingMachineAttr[0x19]);
                }
                if((run_state=atoi(context->value))==1){
                    if ((ret = ugw_get_attr(handle, context, context->devs[i].device_id,  WashingMachineAttr[0x19])) != 0){
                        printf("%s WS_TIME_UPDATE(%s) error!\n",context->devs[i].device_id,WashingMachineAttr[0x19]);
		   				}
						time_value[0]=atoi(context->value);
						if ((ret = ugw_get_attr(handle, context, context->devs[i].device_id,  WashingMachineAttr[0x1A])) != 0){
							printf("%s WS_TIME_UPDATE(%s) error!\n",context->devs[i].device_id,WashingMachineAttr[0x1A]);
			   			}
						time_value[1]=atoi(context->value);
						
						buf_timeup[0]=SOCKET_HEAD_0xFE;
						buf_timeup[1]=0x1f;
						buf_timeup[2]=0x01;
						buf_timeup[5]=0x13;
						buf_timeup[24] = 0x01;
						buf_timeup[25]=0x05;
						buf_timeup[26]=0x19;//hour
						buf_timeup[27]=0x00;
						buf_timeup[28]=time_value[0];
						buf_timeup[31]=SOCKET_END_0xFD;
						printf(" WS_TIME--------------%s\n",context->value);
						USDK_devid_to_upmac(context->devs[i].device_id);
						USDK_msg_up(buf_timeup, 31,output_MAC);
						usleep(500);
						buf_timeup[26]=0x1A;//minute
						buf_timeup[28]=time_value[1];
						buf_timeup[31]=SOCKET_END_0xFD;
						USDK_devid_to_upmac(context->devs[i].device_id);
						USDK_msg_up(buf_timeup, 31,output_MAC);

					}
						
				}

			}
			 ugw_free_context(context);
			sleep(60);
		}
    return;
}
static void WashingMachine_StateUpdate(char* WashingMachine_devmac) {

	int ret;
	uint8 atr_val[36]={0};
	
	context_t *ctx = ugw_new_context();

	for(int Attr=1;Attr<37;Attr++){	
		if ((ret = ugw_get_attr(handle, ctx, WashingMachine_devmac,  WashingMachineAttr[Attr])) != 0){
			printf("WashingMachineAttr_UpDate err =%d\n",ret);
			printf("%s ATTR_UPDATE(%s) error!\n",WashingMachine_devmac,WashingMachineAttr[Attr]);
	   	}
		else{	
			atr_val[Attr-1]=atoi(ctx->value);
		}
	}
	uint8 buf_wm[100]={0};
	buf_wm[0]=SOCKET_HEAD_0xFE;
	buf_wm[1]=0x1f;
	buf_wm[2]=0x01;
	buf_wm[5]=0x13;
	buf_wm[24] = 0x01;
	buf_wm[25]=0x05;//on-off
	buf_wm[26]=0x01;//on-off
	buf_wm[27]=0x00;
	buf_wm[28]=atr_val[1-1];
	buf_wm[31]=SOCKET_END_0xFD;
	USDK_devid_to_upmac(WashingMachine_devmac);
	USDK_msg_up(buf_wm, 31,output_MAC);
	usleep(500);
	buf_wm[26]=0x20;//tonggai
	buf_wm[28]=atr_val[32-1];
	USDK_msg_up(buf_wm, 31,output_MAC);
	buf_wm[26]=0x1F;//current state
	buf_wm[28]=atr_val[31-1];
	usleep(500);
	USDK_msg_up(buf_wm, 31,output_MAC);
	buf_wm[26]=0x24;//current state
	buf_wm[28]=atr_val[36-1];
	usleep(500);
	USDK_msg_up(buf_wm, 31,output_MAC);

//attr info upto link list
	USDK_devid_to_listmac(WashingMachine_devmac);

	ugw_free_context(ctx);
}

void USDK_unpacket(void *data, uint8 len){
	uint8 buf[100] = {0};
	uint8 Socket_len = len;
	Socket_data Socket = {{0}};
	int ret;
    int i;
	memcpy(buf, data, len);

	if((buf[24] == 1)||(buf[24] == 0))
		memcpy(&Socket , buf, len);
	
	if(buf[24] > 1)
		memcpy(&Socket , buf, 25);//24-25

	USDK_socketNo = Socket.cmdNo;
	switch(Socket.cmd){
		case SOCKET_CMD_CTRL:{
        
			if(Socket.devAttrNo == 1){
				switch(Socket.dev){
					case DEV_WIFI_Aircondition:
							ret = Airconditon_ctrl( Socket.devAttr[1],Socket.statusVal[1]);
						/*	if(ret){
								buf[26] = Socket.devAttr[1];
								buf[28] = Socket.statusVal[1];
								USDK_devid_to_upmac(dev_ctrl_list[i].devID);//暂未修改`
								USDK_msg_up(buf, Socket_len,output_MAC);
							}	
                            */
						break;

					case DEV_WIFI_AirFilter:
							ret = AirFilter_ctrl( Socket.devAttr[1],Socket.statusVal[1]);
						/*	if(ret){
								buf[26] = Socket.devAttr[1];
								buf[28] = Socket.statusVal[1];
								USDK_devid_to_upmac(dev_ctrl_list[i].devID);
								USDK_msg_up(buf, Socket_len,output_MAC);
							}	
                            */
						break;

					case DEV_WIFI_WaterHeater:
							if(Socket.devAttr[1]!=11){
								ret = WaterHeater_ctrl( Socket.devAttr[1],Socket.statusVal[1]);
							/*	if(ret){
									buf[26] = Socket.devAttr[1];
									buf[28] = Socket.statusVal[1];
									USDK_devid_to_upmac(dev_ctrl_list[i].devID);
									USDK_msg_up(buf, Socket_len,output_MAC);
								}
                                */
							}
							else{
								char group_opt[32] = "grSetResn1";//yuyue1
								pair_t pair[3]={{0}};
								char name_buf[10]={0};
								WaterHeater_ctrl(9,1);//resnstatus
								usleep(1000);
								WaterHeater_ctrl(10,1);//resnmode
								
								pair[0].name=WaterHeaterAttr[Socket.devAttr[1]];
								sprintf(name_buf,"%02d%02d--",Socket.statusVal[0],Socket.statusVal[1]);
								printf("||||||||||%s||||||||\n",name_buf);
								pair[0].value=name_buf;
								pair[1].name=WaterHeaterAttr[12];
								pair[1].value="45";
								pair[2].name="cycleMode";
								pair[2].value="0";
								WaterHeater_group_ctrl(3, group_opt, pair);//resnmode1 ctrl
							}
						break;
					case DEV_WIFI_Washer:
						for(i=0; i < dev_num; i++){
							ret = WashingMachine_ctrl( Socket.devAttr[1],Socket.statusVal[1]);
						/*	if(ret){
								buf[26] = Socket.devAttr[1];
								buf[28] = Socket.statusVal[1];
								USDK_devid_to_upmac(dev_ctrl_list[i].devID);
								USDK_msg_up(buf, Socket_len,output_MAC);
							}	
                            */
						}
                    case DEV_WIFI_CookerHood:
                        ret=CookerHood_ctrl(Socket.devAttr[1],Socket.statusVal[1]);

						break;
					default:
						break;
				}
			}
			break;
		}
	
		case SOCKET_CMD_ASK:{
			int i;
			
			if(dev_num==0){
				printf(" no ctrl dev(%x) \n" , Socket.dev);
				return;
			}
			if(Socket.devAttrNo == 1){

				switch(Socket.dev){
					case DEV_WIFI_Aircondition:
						for(i=0; i < dev_num; i++){
							ret = Airconditon_AskState(i, Socket.devAttr[1], &Socket.statusVal[1]);
							if(ret){
								buf[28] = Socket.statusVal[1];
								USDK_devid_to_upmac(dev_ctrl_list[i].devID);
								USDK_msg_up(buf, Socket_len,output_MAC);
							}				
						}
						break;

					case DEV_WIFI_AirFilter:
						for(i=0; i < dev_num; i++){
							ret = AirFilter_AskState(i, Socket.devAttr[1], &Socket.statusVal[1]);
							if(ret){
								buf[28] = Socket.statusVal[1];
								USDK_devid_to_upmac(dev_ctrl_list[i].devID);
								USDK_msg_up(buf, Socket_len,output_MAC);
							}				
						}
						break;
					case DEV_WIFI_WaterHeater:
						for(i=0; i < dev_num; i++){
							ret = WaterHeater_AskState(i, Socket.devAttr[1], &Socket.statusVal[1]);
							if(ret){
								buf[28] = Socket.statusVal[1];
								USDK_devid_to_upmac(dev_ctrl_list[i].devID);
								USDK_msg_up(buf, Socket_len,output_MAC);
							}				
						}
						break;
					case DEV_WIFI_Washer:
						for(i=0;i<dev_num;i++){
							ret = WaterHeater_AskState(i, Socket.devAttr[1], &Socket.statusVal[1]);
							if(ret){
								buf[28] = Socket.statusVal[1];
								USDK_devid_to_upmac(dev_ctrl_list[i].devID);
								USDK_msg_up(buf, Socket_len,output_MAC);
							}			
						}
					default:
						break;
				}
			}
		}
		default:
			break;	
	}
}
void USDK_device_txloop(void){

	int ret;
	MSG usdk_msg ;

	if (handle <= 0) {
		//printf("ERR: rx devfd <= 0\n");
		return;
	}
	int res_ws;
	pthread_t thread_ws;

	res_ws=pthread_create(&thread_ws,NULL,(void*)wstime_up_f,NULL);
	if(res_ws!=0){
		perror("Thread_wstime_up creation failed");
	}

	while(1){
        sleep(100);
    }
		ugw_destroy(handle);
		pthread_exit(0);
}
int USDK_device_init(void)
{

	handle = ugw_init(&ugw_listener);
   
	if((handle<0)||(handle==NULL)){
		printf("%d ugw init failed\n", __LINE__);
		Ugw_init_OK = FALSE;
		return FALSE;
	}
	else{
		Ugw_init_OK = TRUE;
	}
	//usleep(1000);
	return TRUE;
}

