/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */

#define OSI_LOG_TAG OSI_MAKE_LOG_TAG('T', 'I', 'C', 'O', 'S', 'A', 'T')

#include "fibo_opencpu.h"
#include "oc_uart.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
int port = 0;

void uart_recv(hal_uart_port_t uart_port, UINT8 *data, UINT16 len, void *arg)
{
    //OSI_PRINTFI("uartapi recv uart_port=%d len=%d, data=%s", uart_port, len, (char *)data);
    /*
    from uart to vir-uart-at
    */
    uint8_t *cmd = date;
    fibo_at_send(cmd, strlen((const char *)cmd));
}

static void at_res_callback(UINT8 *buf, UINT16 len)
{
    fibo_hal_uart_put(port, buf, len);
    //OSI_PRINTFI("FIBO <--%s", buf);
}

static void prvThreadEntry(void *param)
{
   
    for (;;)
    {
        ;
    }
    fibo_hal_uart_deinit(port);
    fibo_thread_delete();
}
void ticos_AT_uart_init(void){

    OSI_LOGI(0, "at uart init");

    hal_uart_config_t drvcfg = {0};
    drvcfg.baud = 115200;
    drvcfg.parity = HAL_UART_NO_PARITY;
    drvcfg.data_bits = HAL_UART_DATA_BITS_8;
    drvcfg.stop_bits = HAL_UART_STOP_BITS_1;
    drvcfg.rx_buf_size = UART_RX_BUF_SIZE;
    drvcfg.tx_buf_size = UART_TX_BUF_SIZE;

    
    fibo_hal_uart_init(port, &drvcfg, uart_recv, NULL);

}
static FIBO_CALLBACK_T user_callback = {
    .at_resp = at_res_callback};

void* appimg_enter(void *param)
{
    OSI_LOGI(0, "application image enter, param 0x%x", param);
    ticos_AT_uart_init();
    fibo_thread_create(prvThreadEntry, "mythread", 1024, NULL, OSI_PRIORITY_NORMAL);
    return (void *)&user_callback;
}

void appimg_exit(void)
{
    OSI_LOGI(0, "application image exit");
}
