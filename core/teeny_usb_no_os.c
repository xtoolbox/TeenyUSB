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

#include "stdint.h"
#include "string.h"
#include "stdlib.h"
#include "teeny_usb_util.h"
#include "teeny_usb_osal.h"


#ifndef TUSB_MQ_T
// a simple MQ without OS support


#define  TUSBH_MSG_Q_LENGTH        16
struct _tusb_mq{
    uint16_t  rd_index;
    uint16_t  wr_index;
    tusb_msg_t queue[TUSBH_MSG_Q_LENGTH];
};

#define MQ_SIZE    4
static tusb_mq_t mq_pool[MQ_SIZE];
static uint8_t mq_used[MQ_SIZE];

tusb_mq_t* tusb_mq_create()
{
    for(int i=0;i<MQ_SIZE;i++){
        if(!mq_used[i]){
            mq_used[i] = 1;
            return &mq_pool[i];
        }
    }
    TUSB_LOGE("Error: no free mq space\n");
    return 0;
}

void tusb_mq_free(tusb_mq_t* mq)
{
    for(int i=0;i<MQ_SIZE;i++){
        if(mq == &mq_pool[i]){
            if(!mq_used[i]){
                TUSB_LOGE("Error: mq not in use\n");            
            }
            mq_used[i] = 0;
        }
    }
    TUSB_LOGE("Error: mq memory out bound\n");
}


int tusb_mq_init(tusb_mq_t* mq)
{
    memset(mq, 0, sizeof(tusb_mq_t));
    return 0;
}

#define MQ_MASK (TUSBH_MSG_Q_LENGTH-1)

static inline int tusb_mq_full(tusb_mq_t* mq){
    if(mq->wr_index == mq->rd_index)return 0;
    if( (mq->wr_index & MQ_MASK) == (mq->rd_index & MQ_MASK) ) return 1;
    return 0;
}

int tusb_mq_post(tusb_mq_t* mq, const tusb_msg_t* msg)
{
    if(! tusb_mq_full(mq) ){
        memcpy(&mq->queue[mq->wr_index & MQ_MASK ], msg, sizeof(tusb_msg_t));
        mq->wr_index++;
        return 1;
    }
    return 0;
}

int tusb_mq_get(tusb_mq_t* mq, tusb_msg_t* msg)
{
    if(mq->rd_index != mq->wr_index){
        memcpy(msg, &mq->queue[mq->rd_index & MQ_MASK ], sizeof(tusb_msg_t));
        mq->rd_index++;
        return 1;
    }
    return 0;
}

#endif // #ifndef TUSB_MQ_T


#ifndef TUSB_MALLOC
void* tusb_malloc(int size)
{
    return malloc(size);
}
#endif // #ifndef TUSB_MALLOC

#ifndef TUSB_FREE
void tusb_free(void* p)
{
    free(p);
}
#endif // #ifndef TUSB_FREE




#ifndef TUSB_EV_T
struct _tusb_ev
{
    uint8_t event;
};
#endif


#ifndef TUSB_EV_CREATE
tusb_ev_t* tusb_ev_create(void)
{
    tusb_ev_t* ev = (tusb_ev_t*)tusb_malloc(sizeof(tusb_ev_t));
    ev->event = 0;
    return ev;
}
#endif

#ifndef TUSB_EV_DELETE
void tusb_ev_delete(tusb_ev_t* ev)
{
    tusb_free(ev);
}
#endif

#ifndef TUSB_EV_SET
void tusb_ev_set(tusb_ev_t* ev)
{
    if(ev){
        ev->event = 1;
    }
}
#endif

#ifndef TUSB_EV_WAIT
int tusb_ev_wait(tusb_ev_t* ev, uint32_t timeout_in_ms)
{
    if(!ev)return -1;
    while(timeout_in_ms>0){
        if(ev->event){
            ev->event = 0;
            return 0;
        }
        if(timeout_in_ms != 0xffffffff){
            timeout_in_ms--;
        }
        tusb_delay_ms(1);
    }
    return -1;
}
#endif


