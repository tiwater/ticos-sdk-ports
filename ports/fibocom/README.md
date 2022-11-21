# ticos-sdk-ports for fibocom

Ticos-SDK-Ports提供了不同平台的接入方案，其中 fibocom提供了针对4G cat.1 广和通L610系列的Ticos Cloud平台适配和L610 OpenCPU sdk 移植方案。

# 使用说明  

## 主要依赖库  

- 广和通OpenCPU SDK,广和通官方提供;
- [ticos-sdk-for-c](https://github.com/tiwater/ticos-sdk-for-c.git) ,Ticos Cloud 协议接入SDK;
- [Ticos-sdk-ports](https://github.com/tiwater/ticos-sdk-ports.git), 其中，fibocom提供L610系列模组Ticos Cloud接入hal层实现和编译移植.

## ticos-sdk-for-c

除github外，可从 [河图](https://console.ticos.cn) `-> 产品 -> 硬件开发 -> SDK 下载`项中进行下载, 将下载的 zip 包中的文件移至你的工程开发环境。

### 主要接口说明

* API 接口: src/ticos_api.h

  - 调用提供 ticos_cloud_start() 启动云服务；
  - 连接成功后，用户需要调用 ticos_mqtt_subscribe() 函数订阅sdk相关topic用于接收云端消息；
  - 连接成功后，物模型属性发生改变时，用户可主动调用 ticos_property_report() 上报属性到云端；
  - 连接成功后，用户可主动调用 ticos_telemetry_report() 上报遥测到云端；
  - 云端下发数据时，需要调用 ticos_msg_recv() 进行解析；
  - 用户可主动调用 ticos_cloud_stop() 结束云端的连接。

### 接入Ticos Cloud

1. 在[河图](https://console.ticos.cn)中创建硬件产品，并根据产品需求定义出物模型;
   
2. 为物模型添加相应的业务处理逻辑：

   - 从 [河图](https://console.ticos.cn) `-> 产品 -> 硬件开发 -> SDK 下载`项中进行下载, 将下载的 zip 包解压缩后，将其中的文件移入用户工程中的源文件目录；
   - 或者也可按如下步骤手动操作，从而可以对物模型代码的生成过程中的步骤根据需要进行调整：
     - 要求: 已安装 python3 运行环境；
     - 将从服务端下载的物模型文件(例: thing_model.json)放到 tools/codegen 目录下；
     - 在 tools/codegen 目录下运行: python ./ticos_thingmodel_gen.py --json_file thing_model.json；
     - 成功后会在当前目录下产生 ticos_thingmodel.c 和 ticos_thingmodel.h 等文件, 将生成的文件移入用户工程中的源文件目录，或者与用户已经存在的代码进行合并；
   - 在 ticos_thingmodel.c 中填入用户的业务逻辑。_send 后缀的函数为设备端向云端发送物模型对应属性/遥测时回调的接口，函数应返回该属性/遥测的值，通常是从物理设备获取到对应的值后返回，由 SDK 将该值上传至云端；_recv 后缀的函数为设备端接收到云下发的属性/命令时调用的接口，函数的参数即为接收到的值，用户根据业务需求对该值进行处理；
` ticos/fibocom/examples/03_cloud/中提供接入例程的物模型文件`.

3. 提供对应硬件平台的 MQTT client 实现，使 SDK 可接入云端服务器，可参考 examples/Ticos_Hub_ESP32/ticos_mqtt_wrapper.cpp 相应的接口实现:

   - 提供 ticos_hal_mqtt_start() 函数，能启动平台相关的 MQTT client 客户端连接到 Ticos Cloud；
   - 提供 ticos_hal_mqtt_publish() 函数，将数据上报到云端；
   - 提供 ticos_hal_mqtt_subscribe() 函数，订阅mqtt相关的主题
   - 提供 ticos_hal_mqtt_stop() 函数，停止平台相关的 MQTT client 服务
   - MQTT在接收到数据后，需要调用sdk中的 ticos_msg_recv() 函数进行数据的处理；
   - 根据河图中的产品定义信息，为 MQTT 连接提供产品 ID、设备 ID、设备密钥这三组值，在调用ticos_cloud_start() 时传入此三元组信息。  
`针对L610系列模组的移植适配,参见 fibocom/src/ticos_mqtt_wrapper.c`.

### 编译和移植

    1. 在L610系列模组的OpenCPU SDK根目录下新建ticos目录，同时下载ticos-sdk-for-c源文件以及ticos-sdk-ports/ports/fibocom，目录如下：  
    - /ticos/fibocom/ 针对ticos cloud的接口硬件hal实现；
    - /ticos/ticos-sdk-for-c/ ticos cloud快捷接入core。
    2. ticos-sdk-for-c api hal层实现，参见 /ticos/fibocom/ticos_mqtt_wrapper.c
    3. 物模型导入，将接入ticos cloud第二步中生成的ticos_thingmodel.c 和 ticos_thingmodel.h 等文件 移入用户工程主目录,参见 /ticos/fibocom/examples/03_cloud/ticos_thingmodel.c以及 /ticos/fibocom/examples/03_cloud/ticos_thingmodel.h
    4. 编译适配，将对应示例下的CMakeLists.txt 复制到 L610系列模组的OpenCPU SDK根目录下，例：
       cp .../ticos/fibocom/examples/03_cloud/CMakeLists.txt .../CMakeLists.txt
    5. void *appimg_enter(void *param)是open SDK的唯一允许的入口函数，存放用户具体业务逻辑代码，参见 /ticos/fibocom/examples/03_cloud/ticos_main.c
    6. 编译
    在openCpu sdk下依次执行以下编译命令：
    - . tools/core_launch.sh
    - cout
    - cmake ../.. -G Ninja
    - ninja

```
$ . tools/core_launch.sh
$ cout
$ cmake ../.. -G Ninja
-- The C compiler identification is GNU 7.2.1
-- The CXX compiler identification is GNU 7.2.1
-- The ASM compiler identification is GNU
-- Found assembler: /mnt/hgfs/L610-CN-62-_96.63.01/core_sdk_16000.1077.00.96.63.01/prebuilts/linux/gcc-arm-none-eabi/bin/arm-none-eabi-gcc
-- Configuring done
-- Generating done
-- Build files have been written to: /mnt/hgfs/L610-CN-62-_96.63.01/core_sdk_16000.1077.00.96.63.01/out/appimage_debug
$ ninja
[11/11] Generating hex/cloud_ticos.pac
```
    7.烧录
    参见《FIBOCOM L610 系列 OpenCPU编程指南_V1.0.2》

