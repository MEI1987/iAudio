#ifndef _SOCKET_H_
#define _SOCKET_H_


#define SERVPORT 9769//3333
#define CLIENTPORT  9770
#define BACKLOG 10
//#define MAXDATASIZE 100
#define SOCKET_ADDR "127.0.0.1"
//#define SOCKET_ADDR "192.168.220.130"

#define SOCKET_HEAD_0xFE  0xFE
#define SOCKET_END_0xFD    0xFD


typedef enum{
	SOCKET_CMD_UNDEF = 0,
	SOCKET_CMD_ASK = 0x01,
	SOCKET_CMD_CTRL = 0x02,
	SOCKET_CMD_REPORT = 0x03,
	SOCKET_UGW_NETWORK = 0x04,

	SOCKET_CMD_MAX ,	
}Socket_Cmd;


typedef enum{
	DEV_TYPE_UNDEF = 0,
	DEV_TYPE_UGW = 0x01,
	DEV_TYPE_IAUDIO = 0x05,
	DEV_TYPE_MAX,	
}GateWay_Dev_Type;

//dev 
typedef enum{
	
	DEV_Aircondition = 0x10, // wifi
	DEV_WIFI_Aircondition = DEV_Aircondition,
	DEV_WIFI_AirFilter = 0x11,
	DEV_WIFI_WaterHeater = 0x12,
	DEV_WIFI_Washer = 0x13, // yx 2015.10.08
	DEV_WIFI_IceBox = 0x14,
	DEV_WIFI_CookerHood = 0x15,
	DEV_WIFI_iRobot = 0x16,
	DEV_AudioLight = 0x17,
	DEV_AudioMusic = 0x18,
	DEV_MAX ,

}GateWay_Dev;


 typedef struct{
 	uint8 header[2]; ////byte0~1
	uint8 cmdNo;  //// byte2
	uint8 cmd; //Socket_Cmd ////byte3 
	uint8 devType; // GateWay_Dev_Type ////byte4
	uint8 dev;	// GateWay_Dev ////byte5
	uint8 place; ////byte6
	uint8 devNo;  ////byte7
	uint8 devMAC[10];////byte8~17
	uint8 data[6]; ////byte18~23
	uint8 devAttrNo; ////byte24
	uint8 devAttr[2]; // DEV_attr ////byte25~26
	uint8 statusVal[2]; ////byte27~28
	uint8 devName_len; ////byte29
	uint8 devName[256]; ////byte30~ // 
}Socket_data;


typedef struct{
	uint8 devAttr;
	uint8 statusVal[2];
}Dev_Attr;


extern void socket_recv_pthread(void);
extern void socket_send_thread(void);


#endif
