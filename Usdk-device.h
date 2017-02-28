#ifndef _USDK_DEVICE_H_
#define _USDK_DEVICE_H_


#define WIFI_CONFIG_FILE_PATH       "/data/misc/wifi/wpa_supplicant.conf"
#define WIFI_DEV_LIST_SV_PATH 		 "/data/misc/smarthome/config/ugw_dev_list"


//char AIRCON_TANK_DEV[10] = {"02003001"};//deviceType被弃用
//char AIRCON_SPLIT_DEV[10] = {"02002001"};
//char AIRFILlTER_DEV[10] = {"21001001"};
//char WATER_HEATER_DEV[10] = {"06000001"};
//char ROLLER_WASHINGMA_DEV[10]={"05002001"};
char *Aircon_DEV="00000000000000008080000000041410";
char *Airfil_DEV="";
char *WaterHeater_DEV="111c120024000810060200418003270000000000000000000000000000000000";
char *WashMachine_DEV="";
char* CookerHood_DEV="111c120024000810090302318001030000000000000000000000000000000000";//燃气罩
char* Ignition_DEV="11e9994e2d7607441d0100000000000000000000000000000000000000000000";
char *Airbox_DEV="111c120024000810330200118999990000000000000000000000000000000000"; //空气魔方

//#define AIR_FILTER_DEV	 21001001
//#define AIR_BOX_DEV		1400d001
//#define AIDEHYDE_DEV	 1400d002
//#define AIRCUBE_DEV		33001001
//#define SMARTSOCKET_DEV 14006001


typedef struct {
	char devType[10];
	char devID[32];
}ugw_ctrl_dev;

char *Attr_val[] ={//ctrl set
                "0", "1", "2", "3", "4", "5",
                //Temperature

                "6", "7", "8", "9", "10", "11", "12", "13", "14", "15",
                // AirconAttr Temperature set 16-30
                "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26","27","28","29", "30",        
                "31", "32", "33", "34", "35", "36", "37", "38", "39", "40",
                "41", "42", "43", "44", "45", "46", "47", "48", "49", "50",
                "51", "52", "53", "54", "55", "56", "57", "58", "59", "60",
                "61", "62", "63", "64", "65", "66", "67", "68", "69", "70",
                "71", "72", "73", "74", "75", "76", "77", "78", "79", "80",
                "81", "82", "83", "84", "85", "86", "87", "88", "89", "90",
                "91", "92", "93", "94", "95", "96", "97", "98", "99", "100",};


    
//--------------------------------Airconditionor---------------------------------------------

char *AirconAttr[] = {"0",
			"202001",//guanji
            "202002",//kaiji
            "202003",//guanbi xinfeng
            "202004", //dakai xinfeng
            "202005",//guanbi jiankang
 "202006",//dakai jiankang
 "202007",//guanbi dian jia re
 "202008",//dakai dianjiare
 "202009",//guanbi jiashi
 "20200a",//dakai jiashi
 "20200j",//guanbi dianzi suo
 "20200k",//dakai dianzi suo 
 "20200l",//guanbi zi qing sao 
 "20200m",//dakai zi qingsao
 "20200n",//蜂鸣器响（云适应 老空调使用）
 "20200o",//睡眠曲线（老空调使用）
 "20200D",//设定模式
 "20200E",//设定温度
 "20200F",//设定风速
 "20200G",//
 "20200H",//设定感人模式（默认关闭感人功能）
 "20200I",//设定上下摆风
 "20200J",//设定左右摆风
					};
/*
char *AirconAttr_val[] ={"0", "1", "2", "3", "4", "5",
					     "6","7","8","9","10", "11", "12", "13", "14", "15",
					     "16","17","18","19","20", "21", "22", "23", "24", "25",
					     "26","27","28","29", "30"};
 */

//airconditionor attr
typedef enum{
	AIRCON_ATTR_MIN = 0,
	AIRCON_indoorTemp = 1,
	AIRCON_indoorHumi = 2,
	AIRCON_status =3,  // 0--off, 1--on
	AIRCON_operation = 4, // 1--auto, 2--cool, 3--heat, 4--wind, 5--humi
	AIRCON_targetTemp = 5,
	AIRCON_outdoorTemp = 6,
	AIRCON_windSpeed = 7, // 1--low, 2--med, 3--high, 4--auto
	AIRCON_windDVertical = 8, // 1--stationary, 2--swing
	AIRCON_windDHorizontal = 9, // 1--stationary, 2--swing
	AIRCON_airQuality = 10, // 1--perfect, 2--better, 3--normal, 4--bad
	AIRCON_electricHeating = 11, // 0--off, 1--on
	AIRCON_freshAir = 12, // 0--off, 1--on
	AIRCON_anion = 13, // 0--off, 1--on
	AIRCON_eLock = 14, // 0--off, 1--on
	AIRCON_selfCleaning = 15, // 0--off, 1--on
	AIRCON_cloudConrtol = 16, // 0--off, 1--on
	AIRCON_setSleepCurve = 17, // 0--off, 1--on
	AIRCON_human_Sensing = 18, // 0--off, 1--on
	AIRCON_pm25Val = 19,
	// alarm
	AIRCON_alarmCancel = 20,
	AIRCON_humiditySensorErr =21,
	AIRCON_tempSensorErr =22,
	AIRCON_powerProtection =23,
	//autoctrl
	AIRCON_AutoCtrl_Temp = 25,  // 0:-Temp, 1: +Temp
	AIRCON_AutoCtrl_Wind = 27,  //  0: -speed, 1: +speed
	AIRCON_ATTR_MAX,
}Dev_AirCon_Attr;

//--------------------------------AirFilter---------------------------------------------


char *AirFilterAttr[] = {"0",
					"indoorTemperature",
					"indoorHumidity",
					"onOffStatus",
					"operationMode",
					"windDirection",
					"windSpeed",
					"airQuality",
					"filterReplaceAlarmExist",
					"pm2p5Value",
					"ch2oValue",
					"vocValue",
					"co2Value",
					// alarm
					"alarmCancel",
					"humiditySensorErr",
					"tempSensorErr",
					"airQualitySensorErr",
					"vocSensorErr",
					"pm2p5SensorErr",
					"indoorFanErr",};

//--------------------------------Water Heater---------------------------------------------

char *WaterHeaterAttr[] = {"0",
						"onOffStatus",
						"time",
						"targetTemperature",
						"currentTemperature",
						"volume",
						"heatingStatus",
						"outTemperature",
						// MEI2016.1.18
						"profiles",
						"resnStatus",
						"resnMode",
						"resn1Time", 
						"resn1Temperature", 
						"resn2Time" ,
						"resn2Temperature", 
						//alarm
						"alarmCancel",
						"dryHeatingAlarm",
						"leakageAlarm",
						"powerCommunicationErr",
						"tVCommunicationErr",
						"setTemperatureErr",
						"middleTempSensorErr",
						"upTempSensorErr",
						"downTempSensorErr",
						"middleUpTempSensorErr",
						"middleDownTempSensorErr",
						"relayErr",
						"inTempSensorErr",
						"tVColdWaterSensorErr",
						"tVHotWaterSensorErr",
						"stepMotorErr",
						"tankMaintenanceErr",
						"heaterMaintenanceErr",
						};


//-------------------------------WashingMachine----------------------------------------------

char* WashingMachineAttr[]={"0",
							"onOffStatus",
							"runningMode",
							"childLockStatus",
							"laundryCycle",
							"dryMode",
							"dirtyLevel",
							"stainType",
							"spinSpeed",
							"spinTime",
							"rinseTimes",
							"washWaterLevel",
							"washTime",
							"autoDetergentStatus",
							"autoSoftenerStatus",
							"autoDisinfectantStatus",
							"resnStatus",
							"finishReminderStatus",
							"finishShutdownStatus",
							"purifiedWashStatus",
							"echoStatus",
							"voiceStatus",
							"powerfulStatus",
							"anionStatus",
							"actualClothingWeight",
							"remainingHour",
							"remainingMinute",
							"totalWashCycle",
							"currentWashCycle",
							"waterUsed",
							"totalWaterUsed",
							"cyclePhase",
							"doorStatus",
							"doorLockStatus",
							"laundryCycleStatus",
							"remoteControlStatus",
							"hintMsg",
							//ALARM
							"alarmCancel",
							"anErr",
							"voltageHighAlarm",
							"voltageLowAlarm",
							"overcurrentAlarm",
							"launchFailed",
							"dryingRelayErr",
							"heaterErr",
							"tempSensorErr",
							"inAirSensorErr",
							"condenserSensorErr",
							"waterLevelSensorErr",
							"doorLockFail",
							"doorUnlockFail",
							"weightErr",
							"overweightAlarm",
							"inWaterErr",
							"drainErr",
							"waterOverHighAlarm",
							"waterOverLowAlarm",
							"dryHeatingAlarm",
							"washFinishAlarm",
							"waterHeatingBreak",
							"hallSensorErr",
							"speedErr",
							"ddMotorNTCErr",
							"motorStatorHighTempProtection",
							"currentSensingCircuitErr",
							"ipmErr",
							"phaseBreakProtection",
							"noPowerOnSignal",
							"fcBoardCommErr",
							"driverCommErr",
							"displayBoardCommErr",
							"busCommErr",
							"dryDriverCommErr",
							"purificationReductionWarning",
							"purificationReductionAlarm",
							"purificationDriverCommErr",
							"mainRelayAdhesion",
							"dryWaterUnusualAlarm",
							"waterUnusualAlarm",
							"spinUnbalance",
							"mainEEPROMErr",
							"hAlarm"		};
char* CookerHood_attr[]={"0",
    "209001",//开关机
    "209002",//照明开关
    "209003",//延迟开关
    "209004",//风速设置
};
char* CookerHood_val[]={"0",
    "209000",//开关机
    "209001",
    "309000",//照明/延迟开关，风速关/柔速
    "309001",
    "309002", //中高速
    "309003"
};

char* waterheaterTem_val[]={
"3180Uz","3180UA","3180UB","3180UC","3180UD","3180UE","3180UF","3180UG","3180UH","3180UI",
"3180UJ","3180UK","3180UL","3180UM","3180UN","3180UO","3180UP","3180UQ","3180UR","3180US",
"3180UT","3180UU","3180UV","3180UW","3180UX","3180UY","3180UZ","3180u1","3180u2","3180u3",
"3180u4","3180u5","3180u6","3180u7","3180u8","3180u9"
};//35-70


//------------------------------------------------------------------------
extern int USDK_device_init(void);
extern void USDK_device_txloop(void);
extern void USDK_device_rxloop(void);


#endif

