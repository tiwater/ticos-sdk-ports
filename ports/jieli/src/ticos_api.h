// Copyright (c) Tiwater Technology Ltd. All rights reserved.
// SPDX-License-Identifier: MIT

#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
/**
 * @brief 启动 ticos 云服务.
 * @return 0 for success, other is error
 */
int ticos_cloud_start(char *product_id, char *device_name, char *psk);

/**
 * @brief 停止 ticos 云服务.
 * @return void
 */
void ticos_cloud_stop();


#ifdef __cplusplus
}
#endif
