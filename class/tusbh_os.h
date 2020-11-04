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
 * TeenyUSB - light weight usb stack for micro controllers
 * 
 * Copyright (c) 2020 XToolBox  - admin@xtoolbox.org
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

#ifndef __TUSBH_OS_H__
#define __TUSBH_OS_H__

////////////////////////////////////////////////
// OS related APIs
////////////////////////////////////////////////

typedef struct _tusbh_message_t tusbh_message_t;

struct _tusbh_message_t
{
    uint32_t     param;
    uint32_t     len;
    void*        data;
    void (*handler)(tusbh_message_t* msg);
};

#define POST_MESSAGE(mq, func, p, d, l)  \
do{                             \
    tusbh_message_t msg = {     \
    .param = p,                 \
    .len = l,                   \
    .data = d,                  \
    .handler = func};           \
    tusbh_mq_post(mq, &msg);    \
}while(0)


/// API for message queue
typedef struct _tusbh_msg_q tusbh_msg_q_t;

tusbh_msg_q_t* tusbh_mq_create(void);

void tusbh_mq_free(tusbh_msg_q_t* mq);

int tusbh_mq_init(tusbh_msg_q_t* mq);

int tusbh_mq_post(tusbh_msg_q_t* mq, const tusbh_message_t* msg);

// return value: 1 - success, 0 
int tusbh_mq_get(tusbh_msg_q_t* mq, tusbh_message_t* msg);

/// API for event
typedef struct _tusbh_evt tusbh_evt_t;

tusbh_evt_t* tusbh_evt_create(void);

void tusbh_evt_free(tusbh_evt_t* evt);

int tusbh_evt_init(tusbh_evt_t* evt);

int tusbh_evt_set(tusbh_evt_t* evt);

int tusbh_evt_clear(tusbh_evt_t* evt);

// return 0 for success, otherwise fail
int tusbh_evt_wait(tusbh_evt_t* evt, uint32_t timeout_ms);

// API for memory manage
typedef struct _tusbh_device tusbh_device_t;

tusbh_device_t* tusbh_new_device(void);

void tusbh_free_device(tusbh_device_t* device);

// return memory must 32 bit aligned
// real allocate memory size muse be muple of 4 (USB FIFO requirement)
void* tusbh_malloc(uint32_t size);

void tusbh_free(void* p);


#endif

