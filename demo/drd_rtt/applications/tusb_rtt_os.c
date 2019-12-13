/*       
 *         _______                    _    _  _____ ____  
 *        |__   __|                  | |  | |/ ____|  _ \ 
 *           | | ___  ___ _ __  _   _| |  | | (___ | |_) |
 *           | |/ _ \/ _ \ '_ \| | | | |  | |\___ \|  _ < 
 *           | |  __/  __/ | | | |_| | |__| |____) | |_) |
 *           |_|\___|\___|_| |_|\__, |\____/|_____/|____/ 
 *                               __/ |                    
 *                              |___/                     
 *
 * TeenyUSB - light weight usb stack for STM32 micro controllers
 * 
 * Copyright (c) 2019 XToolBox  - admin@xtoolbox.org
 *                         www.tusb.org
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

// OS port for rt-thread

#include "tusbh.h"
#include "tusbh_os.h"
#include <rtthread.h>
#include <string.h>

tusbh_msg_q_t* tusbh_mq_create(void)
{
    rt_mq_t mq = rt_mq_create("tu_mq", sizeof(tusbh_message_t), 16, RT_IPC_FLAG_FIFO);
    return (tusbh_msg_q_t*)mq;
}

void tusbh_mq_free(tusbh_msg_q_t* mq)
{
    rt_mq_delete((rt_mq_t)mq);
}

int tusbh_mq_init(tusbh_msg_q_t* mq)
{
    return 0;
}

int tusbh_mq_post(tusbh_msg_q_t* mq, const tusbh_message_t* msg)
{
    if(rt_mq_send((rt_mq_t)mq, (void*)msg, sizeof(tusbh_message_t)) == RT_EOK){
        return 0;
    }
    return -1;
}

// return value: 1 - success, 0 
int tusbh_mq_get(tusbh_msg_q_t* mq, tusbh_message_t* msg)
{
    return rt_mq_recv((rt_mq_t)mq, msg, sizeof(tusbh_message_t), RT_WAITING_FOREVER)
        == RT_EOK;
}

/// API for event

tusbh_evt_t* tusbh_evt_create(void)
{
    rt_event_t evt = rt_event_create("tu_ev", RT_IPC_FLAG_FIFO);
    return (tusbh_evt_t*)evt;
}

void tusbh_evt_free(tusbh_evt_t* evt)
{
    rt_event_delete( (rt_event_t)evt );
}

int tusbh_evt_init(tusbh_evt_t* evt)
{
    return 0;
}

int tusbh_evt_set(tusbh_evt_t* evt)
{
    rt_event_send( (rt_event_t)evt, 1);
    return 0;
}

// return 0 for success, otherwise fail
int tusbh_evt_wait(tusbh_evt_t* evt, uint32_t timeout_ms)
{
    rt_uint32_t recv;
    rt_err_t r = rt_event_recv( (rt_event_t)evt,
                        0xffffffff,
                       RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                       rt_tick_from_millisecond(timeout_ms),
                       &recv);
    if( (r == RT_EOK) && (recv & 1) ){
        return 0;
    }
    return -1;
}

int tusbh_evt_clear(tusbh_evt_t* evt)
{
    rt_event_control((rt_event_t)evt, RT_IPC_CMD_RESET, 0);
    return 0;
}

// API for memory manage
//typedef struct _tusbh_device tusbh_device_t;

tusbh_device_t* tusbh_new_device(void)
{
    tusbh_device_t* dev = (tusbh_device_t*)rt_malloc(sizeof(tusbh_device_t));
    if(dev){
        memset(dev, 0, sizeof(tusbh_device_t));
    }
    return dev;
}

void tusbh_free_device(tusbh_device_t* device)
{
    rt_free(device);
}

// return memory must 32 bit aligned
// real allocate memory size muse be muple of 4 (USB FIFO requirement)
void* tusbh_malloc(uint32_t size)
{
    return rt_malloc_align( ( (size+3) & ~3), 4);
}

void tusbh_free(void* p)
{
    rt_free_align(p);
}
