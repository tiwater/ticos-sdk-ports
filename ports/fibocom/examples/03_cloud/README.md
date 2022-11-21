# ticos cloud 接入移植示例

本示例提供L610系列模组物模型接入ticos clous平台的能力以及ticos-sdk-for-c的硬件适配hal层实现。

# 移植
移植和环境搭建参见[README.md](README.md)。

## 物模型
 示例提供的物模型：
 ```
 [
    {
        "@id": "dtmi:tiwater:BOB45WX7H4;1",
        "@type": "Interface",
        "@context": "dtmi:dtdl:context;2",
        "contents": [
            {
                "@id": "dtmi:tiwater:BOB45WX7H4:switch;1",
                "name": "switch",
                "unit": "None",
                "@type": "Property",
                "schema": "boolean",
                "modeName": {
                    "zh": "主模型"
                },
                "trueName": {
                    "zh": "开"
                },
                "writable": true,
                "falseName": {
                    "zh": "关"
                },
                "description": {
                    "zh": ""
                },
                "displayName": {
                    "zh": "开关"
                }
            },
            {
                "@id": "dtmi:tiwater:BOB45WX7H4:light;1",
                "name": "light",
                "step": 1,
                "unit": "None",
                "@type": "Property",
                "schema": "integer",
                "maxValue": 100,
                "minValue": 1,
                "modeName": {
                    "zh": "主模型"
                },
                "writable": true,
                "description": {
                    "zh": ""
                },
                "displayName": {
                    "zh": "量度"
                }
            },
            {
                "@id": "dtmi:tiwater:BOB45WX7H4:DebugInfo;1",
                "name": "DebugInfo",
                "unit": "None",
                "@type": "Property",
                "schema": "string",
                "modeName": {
                    "zh": "主模型"
                },
                "writable": true,
                "description": {
                    "zh": ""
                },
                "displayName": {
                    "zh": "调试信息"
                }
            },
            {
                "@id": "dtmi:tiwater:BOB45WX7H4:testCmd;1",
                "name": "testCmd",
                "unit": "None",
                "@type": "Command",
                "durable": false,
                "request": {
                    "name": "request",
                    "@type": "CommandPayload",
                    "schema": {
                        "@type": "Object",
                        "fields": [
                            {
                                "name": "param1",
                                "schema": "boolean",
                                "trueName": {
                                    "zh": "True"
                                },
                                "falseName": {
                                    "zh": "False"
                                },
                                "displayName": {
                                    "zh": "param1"
                                }
                            },
                            {
                                "name": "param2",
                                "step": 1,
                                "schema": "float",
                                "maxValue": 10,
                                "minValue": 0,
                                "displayName": {
                                    "zh": "param2"
                                },
                                "decimalPlaces": 4
                            },
                            {
                                "name": "param3",
                                "schema": "string",
                                "displayName": {
                                    "zh": "param3"
                                }
                            },
                            {
                                "@id": "dtmi:tiwater:BOB45WX7H4:testCmd:schema:param4;1",
                                "name": "param4",
                                "@type": "Enum",
                                "enumValues": [
                                    {
                                        "name": "iv0",
                                        "enumValue": 0,
                                        "displayName": {
                                            "zh": "v0"
                                        }
                                    },
                                    {
                                        "name": "iv1",
                                        "enumValue": 1,
                                        "displayName": {
                                            "zh": "v1"
                                        }
                                    },
                                    {
                                        "name": "iv2",
                                        "enumValue": 2,
                                        "displayName": {
                                            "zh": "v2"
                                        }
                                    }
                                ],
                                "displayName": {
                                    "zh": "param4"
                                },
                                "valueSchema": "integer"
                            }
                        ],
                        "displayName": {
                            "zh": "Object"
                        }
                    },
                    "displayName": {
                        "zh": "请求"
                    }
                },
                "displayName": {
                    "zh": "testCmd"
                },
                "modeName": {
                    "zh": "主模型"
                }
            }
        ],
        "displayName": {
            "en": "彩灯"
        }
    }
]
 ```