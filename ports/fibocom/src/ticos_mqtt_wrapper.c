#define OSI_LOG_TAG OSI_MAKE_LOG_TAG('M', 'Q', 'T', 'T')

#include "fibo_opencpu.h"
#include "oc_mqtt.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ticos_api.h"



BOOL run = false;
int connect_res_result = 0;
BOOL quit = true;
INT8 mqtt_client_id[128] = "";
INT8 mqtt_host[64] = "123165487546548697895646546578956456498794564654897894654";
INT8 mqtt_usr_name[128] = "";
INT8 mqtt_usr_pwd[64] = "";
UINT16 mqtt_port = 1883;


static UINT32 g_lock = 0;
UINT8 clear_session = 0;
UINT16 keepalive = 10;
UINT8 UseTls = false;
static void prvThreadEntry(void *param);




static void fibo_mqtt_connect_start(void){
    OSI_PRINTFI("mqttapi thread create start");
    while (!quit)
    {
        fibo_taskSleep(1000);
    }
    OSI_PRINTFI("mqttapi thread create");
     fibo_thread_create(prvThreadEntry, "mqtt-thread", 1024 * 16, NULL, OSI_PRIORITY_NORMAL);

}

/**
 * @brief mqtt客户端向云端推送数据的接口
 * @note  ticos sdk会调用此接口，完成数据的上传。需要用户实现此函数
 * @param topic 上报信息的topic
 * @param data 上报的数据内容
 * @param len  上报的数据长度
 * @param qos  通信质量
 * @param retain retain flag
 * @return 0 for success, other for fail.
 */
int ticos_hal_mqtt_publish(const char *topic, const char *data, int len, int qos, int retain)
{
    //fibo_mqtt_pub(INT8 *topic, UINT8 qos, BOOL retain, INT8 *message, UINT16 msglen);
    INT8 *fibo_pub_topic  = (INT8 *)topic;
    INT8 *fibo_pub_message = (INT8 *)data;
    UINT8 fibo_pub_qos = (UINT8) qos;
    BOOL fibo_pub_retain = (BOOL) retain;
    UINT16 fibo_pub_msglen = (UINT16) len;
    return fibo_mqtt_pub(fibo_pub_topic, fibo_pub_qos, fibo_pub_retain, fibo_pub_message, fibo_pub_msglen);
}

/**
 * @brief mqtt客户端订阅云端topic接口
 * @note  ticos sdk会调用此接口，完成指定topic的订阅。需要用户实现此函数
 * @param topic 需要订阅的topic
 * @param qos  通信质量
 * @return 0 for success, other for fail.
 */
int ticos_hal_mqtt_subscribe(const char *topic, int qos)
{
    INT8 *fibo_sub_topic  = (INT8 *)topic;
    UINT8 fibo_sub_qos = (UINT8) qos;
    //INT32 fibo_mqtt_sub(INT8 *topic, UINT8 qos);
    return fibo_mqtt_sub(fibo_sub_topic, fibo_sub_qos);
}



static void prvThreadEntry(void *param)
{
    OSI_LOGI(0, "application thread enter, param 0x%x", param);
    int ret = 0;
    int test = 1;
    OSI_PRINTFI("mqttapi wait network");
    reg_info_t reg_info;
    quit = false;
    run = true;

    if (g_lock == 0)
    {
        g_lock = fibo_sem_new(0);
    }
    while (test)
    {
        fibo_getRegInfo(&reg_info, 0);
        fibo_taskSleep(1000);
        OSI_PRINTFI("[%s-%d]", __FUNCTION__, __LINE__);
        if (reg_info.nStatus == 1)
        {
            test = 0;
            fibo_PDPActive(1, NULL, NULL, NULL, 0, 0, NULL);
            fibo_taskSleep(1000);
            OSI_PRINTFI("[%s-%d]", __FUNCTION__, __LINE__);
        }
    }

    fibo_taskSleep(1 * 1000);
    OSI_PRINTFI("mqtt [%s-%d]", __FUNCTION__, __LINE__);
    fibo_mqtt_set(mqtt_usr_name, mqtt_usr_pwd);
    fibo_set_ssl_chkmode(0);

    // fibo_sem_wait(g_lock);
    do
    {
        ret = fibo_mqtt_connect(mqtt_client_id, mqtt_host, mqtt_port, 1, keepalive, UseTls);
        //ret = fibo_mqtt_connect("lh-beta-client", "101.133.167.135", 1883, 1, keepalive, UseTls);
        OSI_PRINTFI("ret= %d",ret);
        fibo_taskSleep(2000);
    } while (ret < 0);

    OSI_PRINTFI("mqttapi connect finish");

    fibo_sem_wait(g_lock);
    OSI_PRINTFI("mqttapi start sub topic");
    if (connect_res_result)
    {   
        ret = ticos_mqtt_subscribe();
        //ret = fibo_mqtt_sub("topic1", 1);
        if (ret < 0)
        {
            OSI_PRINTFI("mqttapi sub failed");
        }
    }
    else
    {
        run = false;
        OSI_PRINTFI("mqttapi connect res fail");
    }

    while (run)
    {
        OSI_PRINTFI("mqttapi running");
        fibo_taskSleep(2 * 1000);
    }
    quit = true;
    OSI_PRINTFI("mqttapi thread exit");
    fibo_mqtt_connect_start();
    fibo_thread_delete();
}



/**
 * @brief 启动平台相关的mqtt服务
 * @note  用户需要根据平台实现此函数，提供一个mqtt的客户端。ticos sdk会调用此接口连接到云端
 */
int ticos_hal_mqtt_start(const char *url, int port, const char *client_id, const char *user_name, const char *passwd)
{
    run = false;
    connect_res_result = 0;
    memset(mqtt_host,0,sizeof(mqtt_host));
    memset(mqtt_client_id,0,sizeof(mqtt_client_id));
    memset(mqtt_usr_name,0,sizeof(mqtt_usr_name));
    memset(mqtt_usr_pwd,0,sizeof(mqtt_usr_pwd));
    

    memcpy(mqtt_host, url ,strlen(url));
    mqtt_port = port;
    memcpy(mqtt_client_id, client_id, strlen(client_id));
    memcpy(mqtt_usr_name, user_name, strlen(user_name));
    memcpy(mqtt_usr_pwd, passwd, strlen(passwd));
    OSI_PRINTFI("%s,%d,%s,%s,%s",mqtt_host,mqtt_port,mqtt_client_id,mqtt_usr_name,mqtt_usr_pwd);
    fibo_mqtt_connect_start();
    return 0;
}

/**
 * @brief 停止平台相关的mqtt服务
 * @note  该函数停止mqtt客户端与云端的连接, 需要用户根据平台实现。ticos sdk停止时会调用此接口
 */
void ticos_hal_mqtt_stop()
{
    fibo_mqtt_close();
}



void user_signal_process(GAPP_SIGNAL_ID_T sig, va_list arg)
{
    OSI_PRINTFI("mqttapi RECV SIGNAL:%d", sig);
    char *topic;
    int8_t qos;
    char *message;
    uint32_t len;
    int ret = 1;
    switch (sig)
    {
    case GAPP_SIG_CONNECT_RSP:
        connect_res_result = ret = va_arg(arg, int);
        OSI_PRINTFI("mqttapi connect :%s", ret ? "ok" : "fail");
        fibo_sem_signal(g_lock);
        break;
    case GAPP_SIG_CLOSE_RSP:
        ret = va_arg(arg, int);
        OSI_PRINTFI("mqttapi close :%s", ret ? "ok" : "fail");
        fibo_mqtt_connect_start();
        break;
    case GAPP_SIG_SUB_RSP:
        ret = va_arg(arg, int);
        OSI_PRINTFI("mqttapi sub :%s", ret ? "ok" : "fail");
        fibo_sem_signal(g_lock);
        break;
    case GAPP_SIG_UNSUB_RSP:
        ret = va_arg(arg, int);
        OSI_PRINTFI("mqttapi unsub :%s", ret ? "ok" : "fail");
        fibo_sem_signal(g_lock);
        break;
    case GAPP_SIG_PUB_RSP:
        ret = va_arg(arg, int);
        OSI_PRINTFI("mqttapi pub :%s", ret ? "ok" : "fail");
        fibo_sem_signal(g_lock);
        break;
    case GAPP_SIG_INCOMING_DATA_RSP:
        //gapp_dispatch(GAPP_SIG_INCOMING_DATA_RSP, pub_msg->topic, pub_msg->qos, pub_msg->message, pub_msg->recv_len);
        topic = va_arg(arg, char *);
        qos = va_arg(arg, int);
        message = va_arg(arg, char *);
        len = va_arg(arg, uint32_t);
        ticos_msg_recv(topic, message, (int) len);
        OSI_PRINTFI("mqttapi recv message :topic:%s, qos=%d message=%s len=%d", topic, qos, message, len);
        break;
    default:
        break;
    }
}