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

#include "tusbh.h"
#include <stdarg.h>
#include "string.h"
#include <stdlib.h>

#ifndef TUSB_HAS_OS

#define  TUSBH_MSG_Q_LENGTH        16
struct _tusbh_msg_q{
    uint16_t  rd_index;
    uint16_t  wr_index;
    tusbh_message_t queue[TUSBH_MSG_Q_LENGTH];
};

static tusbh_msg_q_t _default_mq;



#define MQ_SIZE    4
static tusbh_msg_q_t mq_pool[MQ_SIZE];
static uint8_t mq_used[MQ_SIZE];



tusbh_msg_q_t* tusbh_mq_create()
{
    for(int i=0;i<MQ_SIZE;i++){
        if(!mq_used[i]){
            mq_used[i] = 1;
            return &mq_pool[i];
        }
    }
    TUSB_OS_INFO("Error: no free mq space\n");
    return 0;
}

void tusbh_mq_free(tusbh_msg_q_t* mq)
{
    for(int i=0;i<MQ_SIZE;i++){
        if(mq == &mq_pool[i]){
            if(!mq_used[i]){
                TUSB_OS_INFO("Error: mq not in use\n");            
            }
            mq_used[i] = 0;
        }
    }
    TUSB_OS_INFO("Error: mq memory out bound\n");
}


int tusbh_mq_init(tusbh_msg_q_t* mq)
{
    if(!mq) mq = &_default_mq;
    memset(mq, 0, sizeof(tusbh_msg_q_t));
    return 0;
}

#define MQ_MASK (TUSBH_MSG_Q_LENGTH-1)

static inline int tusbh_mq_full(tusbh_msg_q_t* mq){
    if(mq->wr_index == mq->rd_index)return 0;
    if( (mq->wr_index & MQ_MASK) == (mq->rd_index & MQ_MASK) ) return 1;
    return 0;
}

int tusbh_mq_post(tusbh_msg_q_t* mq, const tusbh_message_t* msg)
{
    if(!mq) mq = &_default_mq;
    if(! tusbh_mq_full(mq) ){
        memcpy(&mq->queue[mq->wr_index & MQ_MASK ], msg, sizeof(tusbh_message_t));
        mq->wr_index++;
        return 1;
    }
    return 0;
}

int tusbh_mq_get(tusbh_msg_q_t* mq, tusbh_message_t* msg)
{
    if(!mq) mq = &_default_mq;
    if(mq->rd_index != mq->wr_index){
        memcpy(msg, &mq->queue[mq->rd_index & MQ_MASK ], sizeof(tusbh_message_t));
        mq->rd_index++;
        return 1;
    }
    return 0;
}


#define MAX_DEVICE_COUNT   16
static tusbh_device_t device_pool[MAX_DEVICE_COUNT];
static uint8_t dev_used[MAX_DEVICE_COUNT] = {0};

tusbh_device_t* tusbh_new_device()
{
    for(int i=0;i<MAX_DEVICE_COUNT;i++){
        if(!dev_used[i]){
            dev_used[i] = 1;
            return &device_pool[i];
        }
    }
    TUSB_OS_INFO("Error: no free device space\n");
    return 0;
}

void tusbh_free_device(tusbh_device_t* device)
{
    for(int i=0;i<MAX_DEVICE_COUNT;i++){
        if(device == &device_pool[i]){
            if(!dev_used[i]){
                TUSB_OS_INFO("Error: device not in use\n");
            }
            dev_used[i] = 0;
            return;
        }
    }
    TUSB_OS_INFO("Error: device memory out bound\n");
}


struct _tusbh_evt
{
    uint8_t event;
};

static tusbh_evt_t  event_pool[16];
static uint8_t event_used[16] = {0};

tusbh_evt_t* tusbh_evt_create()
{
    for(int i=0;i<sizeof(event_used);i++){
        if(!event_used[i]){
            event_used[i] = 1;
            tusbh_evt_init(&event_pool[i]);
            return &event_pool[i];
        }
    }
    TUSB_OS_INFO("Error: no free event space\n");
    return 0;
}

void tusbh_evt_free(tusbh_evt_t* evt)
{
    for(int i=0;i<sizeof(event_used);i++){
        if(&event_pool[i] == evt){
            if(!event_used[i]){
                TUSB_OS_INFO("Error: event not in use\n");
            }
            event_used[i] = 0;
            return;
        }
    }
    TUSB_OS_INFO("Error: Event memory out bound\n");
}

int tusbh_evt_init(tusbh_evt_t* evt)
{
    evt->event = 0;
    return 0;
}

int tusbh_evt_set(tusbh_evt_t* evt)
{
    evt->event = 1;
    return 0;
}


int tusbh_evt_clear(tusbh_evt_t* evt)
{
    evt->event = 0;
    return 0;
}

int tusbh_evt_wait(tusbh_evt_t* evt, uint32_t timeout_ms)
{
    while(timeout_ms>0){
        if(evt->event){
            evt->event = 0;
            return 0;
        }
        if(timeout_ms != 0xffffffff){
            timeout_ms--;
        }
        tusb_delay_ms(1);
    }
    return -1;
}

static int mem_used;
static int mem_max;
void* tusbh_malloc(uint32_t size)
{
    size = (size + 3) & (~3);
    mem_used+=size;
    if(mem_max < mem_used){
        mem_max = mem_used;
    }
    void* r = malloc(size+8);
    TUSB_ASSERT( (r != 0) && (((uint32_t)r) & 3) == 0 );
    uint32_t* p = (uint32_t*)r;
    *p = size;
    *(p + (size/4) + 1) = 0xdeadbeef;
    //TUSB_OS_INFO("Allocate %p %d\n", p, size);
    return (void*)(p+1);
}

void tusbh_free(void* ptr)
{
    TUSB_ASSERT(ptr != 0);
    uint32_t* p = (uint32_t*)ptr;
    p = p - 1;
    uint32_t size = *p;
    mem_used -= size;
    TUSB_ASSERT(*(p+(size/4)+1) == 0xdeadbeef);
    //TUSB_OS_INFO("Free %p %d\n", p, size);
    free(p);
}

void show_memory(void)
{
    printf("  Memory used %d, max %d\n", mem_used, mem_max);
    int used;
    used = 0;
    for(int i=0;i<sizeof(mq_used);i++){
        if(mq_used[i]){
            used++;
        }
    }
    printf("  MQ used %d\n", used);
    used = 0;
    for(int i=0;i<sizeof(dev_used);i++){
        if(dev_used[i]){
            used++;
        }
    }
    printf("  Device used %d\n", used);
    used = 0;
    for(int i=0;i<sizeof(event_used);i++){
        if(event_used[i]){
            used++;
        }
    }
    printf("  Event used %d\n", used);
}

#endif  // TUSB_HAS_OS
