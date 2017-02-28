#ifndef _IAUDIO_H_
#define _IAUDIO_H_

#define IAUDIO_TYPE "2014228030d3cf2ca101b3cefb9ca016295dc3f1c175b4b50c8cff8663a384c0"

#define IAUDIO_LIGHT_DEV "/dev/light_ctr"
#define IAUDIO_MUSIC_DEV ""
#define IAUDIO_NOTIFY_LIGHT "/dev/rgb-leds"
#define IAUDIO_LIGHT_BRIGHTNESS "/dev/disp"
#define GET_ARRAY_LEN(name) sizeof(name)/sizeof(name[0])
#define DEV_IAUDIO 0xA0

#define DISP_CMD_LCD_SET_BRIGHTNESS 0x102
#define DISP_CMD_LCD_GET_BRIGHTNESS 0x103  

typedef struct _dev_reg_devmap{
	char registered_devid[34];
	br_dev_handle_t registered_dev_br;
	int devtype;
}dev_reg_devmap;
/*
struct music_info{

    char basic_info[10];
    char song_info[256];
};
*/
typedef enum{
    COLOR_MIN=0x0,
    COLOR_RED=0x01,
    COLOR_ORANGE=0x02,
    COLOR_YELLOW=0X03,
    COLOR_GREEN=0x04,
    COLOR_CYAN=0x05,
    COLOR_BLUE=0x06,
    COLOR_PURPLE=0x07,
}color;
typedef enum{
    LMODE_MIN=0x0,
    LMODE_STANDARD=0x1,
    LMODE_READ=0x02,
    LMODE_ROMANTIC=0x03,
    //LMODE_SLEEP=0x04,
    LMODE_NINGJING=0x04,
}light_mode;

typedef enum{
    BRIGHT_UP=0x0,
    BRIGHT_DOWN=0x1,
    BRIGHT_MIN=0x2,
    BRIGHT_MAX=0x3,
}bright_mode;

typedef struct{
    char buffer[100];
    int len;
}ssoc;




//这里之前换行时，有一个null，导致了少一个字符串，最后是null，导致错误
char* iAudio_attr[]={"0",
    "onOffStatus","ledColor","ledMode","ledBrightness",//1-4 light  
    "playMode","loopStatus","volume","muteStatus","songName","playControl","totalTime",//5-11 music
    "musicTitle","songInfo",
};

#endif
