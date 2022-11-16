# Ticos_Cloud使用方式:  
### 前置条件:对于杰理的SDK有初步的了解,参见:[https://github.com/Jieli-Tech/fw-AC63_BT_SDK](https://github.com/Jieli-Tech/fw-AC63_BT_SDK)   
##### 1.将Ticos_Cloud文件夹放入杰理SDK的\apps\common\third_party_profile\目录下.使用code::blocks打开工程.  
  
##### 2.通过右键工程Add files recursively将Ticos_Cloud文件夹加入工程.默认会将整个文件夹加入,需要再手动将hal中的esp32移除(因为目前ticos_cloud使用的是腾讯连连,所以需要将Tecent_LL从工程中移除,否则会编译出错.)  

##### 3.右键工程 build-options的search dirctories选项下添加头文件.包含ticos_cloud路径下的文件.需要添加以及删除的头文件如下所示:   
	
	//add
	..\..\..\common\third_party_profile\Ticos_Cloud\src
	..\..\..\common\third_party_profile\Ticos_Cloud\ticos-llsync-port\data_template 
	..\..\..\common\third_party_profile\Ticos_Cloud\ticos-llsync-port\hal\jieli 
	..\..\..\common\third_party_profile\Ticos_Cloud\ticos-llsync-port\inc
	..\..\..\common\third_party_profile\Ticos_Cloud\ticos-llsync-port\src\core 
	..\..\..\common\third_party_profile\Ticos_Cloud\ticos-llsync-port\src\utils
	
	//del
	..\..\..\..\apps\common\third_party_profile\Tecent_LL\include  
	..\..\..\..\apps\common\third_party_profile\Tecent_LL\tecent_ll_demo
	

##### 4.在app_config.h文件中根据添加example的规则,增加如下宏定义.

	#define CONFIG_APP_TICOS_CLOUD            1 //TICOS云 

	#elif CONFIG_APP_TICOS_CLOUD
	//配置双模同名字，同地址
	#define DOUBLE_BT_SAME_NAME                0 //同名字
	
	//蓝牙配置
	#define CONFIG_BT_GATT_COMMON_ENABLE       1 //配置使用gatt公共模块
	#define CONFIG_BT_SM_SUPPORT_ENABLE        0 //配置是否支持加密
	#define CONFIG_BT_GATT_CLIENT_NUM          0 //配置主机client个数(app not support,应用不支持使能)
	#define CONFIG_BT_GATT_SERVER_NUM          1 //配置从机server个数,max is 1
	#define CONFIG_BT_GATT_CONNECTION_NUM      (CONFIG_BT_GATT_SERVER_NUM + CONFIG_BT_GATT_CLIENT_NUM) //配置连接个数
##### 5.app_mian.c中做如下改动即可  

	#elif CONFIG_APP_CONN_24G
	    it.name = "conn_24g";
	    it.action = ACTION_CONN_24G_MAIN;
	#elif CONFIG_APP_TICOS_CLOUD
	#else
	    while (1) {
	        printf("no app!!!");
	    }
	#endif
	
	    log_info("run app>>> %s", it.name);
	    log_info("%s,%s", __DATE__, __TIME__);
	
	    ticos_cloud_start();
	    //start_app(&it);


