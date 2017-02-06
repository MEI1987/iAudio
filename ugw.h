#ifndef UGW_H
#define UGW_H_

//错误码
#define UGW_OK							0
#define UGW_INVALID_PARAM				40001//u
#define UGW_CMD_TIME_OUT				40005
#define UGW_UNKNOWN_ERROR				40016
#define UGW_INVALID_CONTENT				40025
#define UGW_BUSY						40026
#define UGW_INVALID_DEV					40027
#define UGW_ATTR_READONLY				40028
#define UGW_SERVER_DISCONNECT           40029
#define UGW_VERSION_CHECK_ERROR         40030


#define OUT
#define IN
#define IO

//设备结构体，包含设备基本信息和状态
typedef struct{
	char        device_id[33];
	char        ip[16];
	int         connect_status;//0:offline 1:online
	int         connect_type;
	char		deviceType[10];//设备型号
	char		type_id[65];//设备类型唯一标识码
	char		dev_name[65];
	char		model_number[65];
	char		model_name[65];
	char		model_label[65];
	char		model_description[65];
	char		manufacturer_name[65];
	char		manufacturer_fullname[65];
	char		manufacturer_shortname[65];
	char		manufacturer_label[65];
	char		manufacturer_description[65];
	char		firmwareVersion[10];
	char		hardwareVersion[16];	
	char		softwareVersion[16];
}devinfo_t;

//控制参数/操作状态/报警信息，一个长度不限制的键值对
typedef struct{
	char *name; //属性名
	char *value;//属性值
}pair_t;

//上下文结构体，用于传入传出参数
typedef struct{
	char 	  device_id[33];	//设备ID
	int		  pair_count;	//返回的操作状态/报警个数
	int		  dev_count;		//设备个数
	pair_t    *pairs;		//返回操作状态/报警列表
  	devinfo_t *devs;	//返回设备列表
	char	  *value;		//属性值/设备能力集/版本号
	int sys_info;//0:客户端服务开启,1:客户端与服务端未连接,2客户端与服务端版本不匹配
}context_t;

//ugw_handle
typedef void ugw_handle_t;

//listener函数定义
typedef void * (*listener_handle_f)(void *arg, context_t *context);

//ugw监听器
typedef struct{
	listener_handle_f sys_listener;
	listener_handle_f dev_list_listener;//变化设备列表在context->devs中返回
	listener_handle_f  alarm_listener;//报警在context->pairs中返回
	listener_handle_f  attr_listener;//属性在context->pairs中返回
	listener_handle_f  msg_listener;//属性在context->pairs中返回
	void        	*sys_arg;
	void        	*dev_list_arg;
	void            *attr_arg;
	void            *alarm_arg;
	void            *msg_arg;
}ugw_listener_t;

//启动UGW
ugw_handle_t *ugw_init(ugw_listener_t *listener);

//释放资源
int ugw_destroy(ugw_handle_t *handle);

//交换数据接口
context_t* ugw_new_context();

//释放传出参数
void ugw_free_context(context_t *ctx);

//获取设备列表
int ugw_get_devs(IN ugw_handle_t *handle, OUT context_t* ctx);//ctx->devs返回
//属性设置接口,设备的操作状态通过异步方式返回
int ugw_set_attr(IN ugw_handle_t *handle, context_t* ctx, IN char* device_id, IN char* name, IN char* value);
//属性获取接口
int ugw_get_attr(IN ugw_handle_t *handle, OUT context_t* ctx, IN char* device_id, IN char* name);//ctx->value返回
//非属性操作接口//当为组命令操作时,不定参部分参数列表[group_name,group_count,name,value];当为非组命令操作时,不定参参数列表为空。
int ugw_exe_ops(IN ugw_handle_t *handle, OUT context_t* ctx, IN char* device_id, IN char* operation,...);//... 支持pair_t数组传入，返回值待定
//配置smartconfig接口
int ugw_smartconfig(IN ugw_handle_t *handle, IN char *ssid, IN char *pwd, IN char *networkcard);
//取消smartconfig配置接口
int ugw_cancel_smartconfig(IN ugw_handle_t *handle);
//获取client和server的程序版本和通信协议版本
int ugw_get_all_version(ugw_handle_t *handle, context_t *ctx);
#endif




