/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-17     ZYH          first implementation
 */
#include "drv_usart.h"
#include "board.h"
#include <rtdevice.h>
#include <rthw.h>
#include <rtthread.h>
/* STM32 uart driver */

static int uart1;
static struct rt_serial_device serial1;

static rt_err_t drv_configure(struct rt_serial_device *serial,
                              struct serial_configure *cfg)
{
    // always 115200 N 8 1
    return RT_EOK;
}

static rt_err_t drv_control(struct rt_serial_device *serial,
                            int cmd, void *arg)
{
    return RT_EOK;
}

static int drv_putc(struct rt_serial_device *serial, char c)
{
    stdout_sendchar(c);
    return 1;
}


#define STDIN_LEN   256
#define STDIN_MASK  (STDIN_LEN-1)
static char stdin_buf[STDIN_LEN];
static uint32_t stdin_wr;
static uint32_t stdin_rd;

void stdin_recvchar(int ch)
{
    
    if(  (stdin_wr != stdin_rd)
       && ( (stdin_wr & STDIN_MASK) == (stdin_rd & STDIN_MASK) ) )
    {
        // buffer full
    }
    stdin_buf[  STDIN_MASK & stdin_wr] = (char)ch;
    stdin_wr++;
    
    rt_interrupt_enter();
    rt_hw_serial_isr(&serial1, RT_SERIAL_EVENT_RX_IND);
    rt_interrupt_leave();
}

int stdin_getchar(void)
{
    if(stdin_wr == stdin_rd){
        return -1;
    }
    int ch = stdin_buf[  STDIN_MASK & stdin_rd];
    stdin_rd++;
    return ch;
}

static int drv_getc(struct rt_serial_device *serial)
{
    return stdin_getchar();
}

static const struct rt_uart_ops drv_uart_ops =
{
    drv_configure,
    drv_control,
    drv_putc,
    drv_getc,
};

int hw_usart_init(void)
{
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
    stdio_init();
    serial1.ops    = &drv_uart_ops;
    serial1.config = config;
    rt_hw_serial_register(&serial1, "uart1",
        RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
        &uart1);
    return 0;
}

INIT_BOARD_EXPORT(hw_usart_init);
