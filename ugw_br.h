/**
 * @file ugw_br.h
 *
 * @brief  第三方设备接入API
 *
 * @date 2015年10月28日
 * @author  zj
 * @defgroup Global 全局定义
 * @defgroup Param 参数处理
 * @defgroup Debug 调试信息
 * @defgroup Bridge 桥处理
 * @defgroup Device 设备相关信息
 */

#ifndef _UGW_BR_H_
#define _UGW_BR_H_

/**
 * @typedef void * _handle_t
 * typedef _handle_t
 * @ingroup Global
 * */
typedef void * _handle_t;

/**
 * @def br_handle_t
 * 桥句柄
 * @ingroup Bridge
 * */
#define br_handle_t _handle_t
/**
 * @def br_dev_handle_t
 * 桥设备句柄
 * @ingroup Device
 * */
#define br_dev_handle_t _handle_t
/**
 * @def ugw_request_handle_t
 * 用于应答uGW操作相关的句柄
 * @ingroup Device
 * */
#define ugw_request_handle_t _handle_t
/**
 * @def dev_base_info_handle_t
 * 设备基本信息句柄
 * @ingroup Device
 * */
#define dev_base_info_handle_t _handle_t
/**
 * @def dev_base_info_handle_t
 * 设备发现信息句柄
 * @ingroup Device
 * */
#define ugw_dev_dis_handle _handle_t
/**
 * @def dev_base_info_handle_t
 * 桥设备发现信息句柄
 * @ingroup Bridge
 * */
#define ugw_br_dis_handle _handle_t

#define BR_ERROR_PARAM  (-5)   //br 错误的参数

/**
 * @struct param_t ugw_br.h "ugw_br.h"
 * @brief 属性与报警信息内容结构
 * name 属性或报警名
 * value 属性或报警值
 * @ingroup Param
 * */
typedef struct {
    char *name;
    char *value;
}param_t;

/**
 * @struct dev_handle_t
 * @brief 设备句柄与设备发现句柄结构
 * br_dev_hle 设备句柄
 * br_dev_dis_hle 设备发现句柄
 * @ingroup Param
 * */
typedef struct dev_handle_t
{
    br_dev_handle_t br_dev_hle;
    ugw_dev_dis_handle br_dev_dis_hle;
}dev_handle_t;

/**
 * @struct Bridge_handle_t
 * @brief 桥实例句柄与桥实例发现句柄结构
 * br_hle 桥实例句柄
 * br_dis_hle 桥实例发现句柄
 * @ingroup Param
 * */
typedef struct Bridge_handle_t
{
    br_handle_t br_hle;
    ugw_br_dis_handle  br_dis_hle;
}Bridge_handle_t;

/**
 * @struct param_list ugw_br.h "ugw_br.h"
 * @brief 参数列表
 * @ingroup Param
 * */
struct param_list;
/**
 * @typedef struct param_list param_list_t
 * typedef param_list_t
 * @ingroup Param
 * */
typedef struct param_list param_list_t;

/**
 * @def BR_LEVEL_DEBUG
 * @ingroup Debug
 * */
#define BR_LEVEL_DEBUG 0x01
/**
 * @def BR_LEVEL_INFO
 * @ingroup Debug
 * */
#define BR_LEVEL_INFO 0x02
/**
 * @def BR_LEVEL_WARNING
 * @ingroup Debug
 * */
#define BR_LEVEL_WARNING 0x04
/**
 * @def BR_LEVEL_ERROR
 * @ingroup Debug
 * */
#define BR_LEVEL_ERROR 0x08
/**
 * @def BR_LEVEL_NODEBUG
 * @ingroup Debug
 * */
#define BR_LEVEL_NODEBUG 0x10

/**
 * @brief 打印信息回调函数
 * @param[in] format 打印格式
 * @return 
 * @ingroup Debug
 * */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化一个参数列表
 * @return 返回一个列表指针
 * @ingroup Param
 * */
param_list_t *br_param_list_init(void);

/**
 * @brief 销毁一个参数列表
 * @param[in] list 参数列表指针
 * @ingroup Param
 * */
void br_param_list_destroy(param_list_t *list);

/**
 * @brief 向链表中添加一个元素
 * @param[in] list 链表头
 * @param[in] name 元素名
 * @param[in] value 元素值
 * @return[in] 成功返回元素结构体
 * @ingroup Param
 * */
const param_t *br_param_list_add_element(
        param_list_t *list, const char *name, const char *value);

/**
 * @brief 获取链表元素个数
 * @param[in] list 链表头
 * @return 返回链元素个数
 * @ingroup Param
 * */
int br_param_list_len(const param_list_t *list);

/**
 * @brief 获取元素信息
 * @param[in] list 链表头
 * @param[in] index 位置索引
 * @return 成功返回元素信息
 * @ingroup Param
 * */
const param_t * br_param_list_get_element(const param_list_t *list, int index);

/**
 * @brief 删除链表中一个元素
 * @param[in] list 链表头
 * @param[in] index 位置索引
 * @ingroup Param
 * */
void br_param_list_remove_element(param_list_t *list, int index);

/**
 * @brief 清空链表
 * @param[in] list 链表头
 * @ingroup Param
 * */
void br_param_list_clear(param_list_t *list);

/**
 * @brief 申请一个设备句柄
 * MUST NOT NULL: dev_id, productID, swType, swVers, hwType, hwVers
 * @param[in] dev_id 设备ID
 * @param[in] productID 产品ID
 * @param[in] swType MqttWifi, MqttGprs
 * @param[in] swVers 0.0.0 ~ 1000.1000.1000
 * @param[in] hwType QKWifi, HTGprs
 * @param[in] hwVers 0.0.0 ~ 1000.1000.1000
 * @param[in] upgrade 0, 1

 * @return 返回一个设备基本信息的句柄，必须使用br_free_dbi进行释放
 * @ingroup Device
 */
dev_base_info_handle_t br_malloc_dbi(
        const char *dev_id,
        const char *uPlusID,
        const char *swType,
        const char *swVers,
        const char *hwType,     
        const char *hwVers,
        const unsigned char upgrade
     );

/**
 * @def br_malloc_dbi_simple(dev_id,productID,swType,swVers,hwType,hwVers,upgrade)
 * 只使用必要信息生成设备基本信息句柄
 * @ingroup Device
 * */
#define br_malloc_dbi_simple(dev_id,uPlusID,swType,swVers,hwType,hwVers,upgrade) \
    br_malloc_dbi(dev_id,uPlusID,swType,swVers,hwType,hwVers,upgrade)

/**
 * @brief 释放设备基本信息句柄
 * @param[in] dbi 设备基本信息句柄
 * @ingroup Device
 * */
void br_free_dbi(dev_base_info_handle_t dbi);

/**
 * @brief 桥运行状态回调，当状态不为0时，调用其它接口可能返回失败
 * @param[in] br 桥实例句柄
 * @param[in] cb_param 创建桥时，为本回调传入的参数
 * @param[in] status 桥状态: 0 正常运行，<0 运行异常
 * @ingroup Bridge
 */
typedef void (*br_state_cb)(br_handle_t br, void *cb_param, int status);

/**
 * @brief 创建一个桥实例，完成该实例与uGW通信的相关初始化
 * @param[in] cb 桥运行状态回调
 * @param[in] domain 桥实例运行设备IP信息
 * @param[in] port 桥实例的端口号，此端口不能被占用，否则接口返回NULL
 * @param[in] cb_param 为桥运行状态回调传入的参数
 * @param[in] basic_dev_info 运行桥的设备的信息
 * @return[in] Bridge_handle_t: 创建的桥实例结构体句柄， NULL: 初始化失败
 * @ingroup Bridge
 */
Bridge_handle_t *br_creat_instance(br_state_cb cb,void *cb_param, dev_base_info_handle_t basic_dev_info);

/**
 * @brief 删除桥实例
 * @param[in] br 要删除的桥实例结构体句柄
 * @ingroup Bridge
 */
void br_delete_instance(Bridge_handle_t *br);

/**
 * @brief 读取设备属性信息回调
 * 使用br_read_dev_attr_rsp进行应答
 * @param[in] dev 设备实例句柄
 * @param[in] cb_param 用户自定义回调参数
 * @param[in] request 请求消息的句柄
 * @param[in] name 设备属性名称
 * @return 0: 成功， <0: 失败
 * @ingroup Device
 */
typedef int (*br_read_dev_attr)(br_dev_handle_t dev, void *cb_param,
        ugw_request_handle_t request, const char *name);

/**
 * @brief 设置设备属性信息回调
 * @param[in] dev 设备实例句柄
 * @param[in] cb_param 用户自定义回调参数
 * @param[in] request 请求消息的句柄
 * @param[in] name 设备属性名称
 * @param[in] value 设备属性值
 * @return 0: 成功， <0: 失败
 * @ingroup Device
 */
typedef int (*br_write_dev_attr)(br_dev_handle_t dev, void *cb_param,
        ugw_request_handle_t request, const char *name, const char *value);

/**
 * @brief 设备操作处理回调
 * @param[in] dev 设备实例句柄
 * @param[in] cb_param 用户自定义回调参数
 * @param[in] request 请求消息的句柄
 * @param[in] params 设备操作参数信息
 * @return 0: 成功， <0: 失败
 * @ingroup Device
 */
typedef int (*br_dev_operation)(br_dev_handle_t dev, void *cb_param,
        ugw_request_handle_t request, const char *op, const param_list_t *list);

/**
 * @struct br_callback_t ugw_br.h "ugw_br.h"
 * @brief 用于添加设备时注册设备操作相关的回调
 * @ingroup Device
 * */
typedef struct {
    br_read_dev_attr read_cb; ///读信息回调
    br_write_dev_attr write_cb; ///设置信息回调
    br_dev_operation op_cb; ///操作回调
}br_callback_t;
/**
 * @brief 向桥实例中添加设备
 * @param[in] br 桥实例结构体句柄
 * @param[in] info 设备基本信息句柄
 * @param[in] cb 设备回调操作信息
 * @param[in] cb_param 用户自定义回调参数
 * @return br_dev_handle_t: 设备实例结构体的句柄, NULL:失败
 * @ingroup Device
 */
dev_handle_t *br_add_dev(Bridge_handle_t *br, const dev_base_info_handle_t info, br_callback_t *cb, void *cb_param);

/**
 * @brief 通过设备的句柄获取设备的ID
 * @param[in] dev 设备句柄
 * @param[out] id 获取到的设备ID信息，用户在使用完成后调用free接口进行释放
 * @return 成功返回0，其它失败
 * @ingroup Device
 * */
int br_get_dev_id_by_handle(br_dev_handle_t dev, char **id);

/**
 * @brief 获取设备所在的桥实例句柄
 * @param[in] dev 设备实例句柄
 * @return br_handle_t: 桥实例句柄
 * @ingroup Device
 */
 br_handle_t get_br_instance_by_dev(br_dev_handle_t dev);

/**
 * @brief 从桥实例中移除一个设备，销毁该设备的资源
 * @param[in] dev 设备实例句柄结构体
 * @return 0: 成功， <0: 失败 
 * @ingroup Device
 */
int br_remove_dev(dev_handle_t *dev);

/**
 * @brief 设备属性读操作回应
 * @param[in] dev 设备实例句柄
 * @param[in] request 请求消息的句柄
 * @param[in] result 执行结果
 * @param[in] value 设备属性值
 * @return 0: 成功， <0: 失败
 * @ingroup Device
 */
int br_read_dev_attr_rsp(
        br_dev_handle_t dev, ugw_request_handle_t request, int result, const char *value);

/**
 * @brief 设备属性写操作回应
 * @param[in] dev 设备实例句柄
 * @param[in] request 请求消息的句柄
 * @param[in] result 写操作的执行结果
 * @return 0: 成功， <0: 失败
 * @ingroup Device
 */
int br_write_dev_attr_rsp(br_dev_handle_t dev, ugw_request_handle_t request, const int result);

/**
 * @brief 设备操作回应
 * @param[in] dev 设备实例句柄
 * @param[in] request 请求消息的句柄
 * @param[in] result 操作的执行结果
 * @param[in] params 操作返回值
 * @return 0: 成功， <0: 失败
 * @ingroup Device
 */
int br_dev_operation_rsp(br_dev_handle_t dev, 
        ugw_request_handle_t request, int result, const param_list_t *params);

/**
 * @brief 设备的状态上报
 * @param[in] dev 设备实例句柄
 * @param[in] attrs 设备状态数据
 * @return 0: 成功， <0: 失败
 * @ingroup Device
 */
int br_dev_status_report(br_dev_handle_t dev, const param_list_t *attrs);

/**
 * @brief 设备的报警上报
 * @param[in] dev 设备实例句柄
 * @param[in] alarms 设备报警数据
 * @return 0: 成功， <0: 失败
 * @ingroup Device
 */
int br_dev_alarm_report(br_dev_handle_t dev, const param_list_t *alarms);

/**
 * @brief 设备的大数据上报
 * @param[in] dev 设备实例句柄
 * @param[in] bigdata 设备大数据数据
 * @return 0: 成功， <0: 失败
 * @ingroup Device
 */
int br_dev_bigdata_report(br_dev_handle_t dev,const char *type,const char *data,int len);
int br_dev_push_message(br_dev_handle_t dev,const param_list_t *message);


#ifdef __cplusplus
}
#endif

#endif
