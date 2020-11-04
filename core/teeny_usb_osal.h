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

#ifndef __TEENY_USB_OSAL_H__
#define __TEENY_USB_OSAL_H__

#include "teeny_usb_config.h"

typedef struct _tusb_msg{
    void*    context;
    uint32_t param1;
    uint32_t param2;
    void (*handler)(struct _tusb_msg*);
}tusb_msg_t;


#ifdef TUSB_MQ_T
#define tusb_mq_t  TUSB_MQ_T
#else
typedef struct _tusb_mq tusb_mq_t;
#endif

tusb_mq_t* tusb_mq_create(void);

void tusb_mq_free(tusb_mq_t* mq);

int tusb_mq_init(tusb_mq_t* mq);

int tusb_mq_post(tusb_mq_t* mq, const tusb_msg_t* msg);

int tusb_mq_get(tusb_mq_t* mq, tusb_msg_t* msg);


#ifdef TUSB_MALLOC
#define tusb_malloc  TUSB_MALLOC
#else
void* tusb_malloc(int size);
#endif

#ifdef TUSB_FREE
#define tusb_free  TUSB_FREE
#else
void tusb_free(void* p);
#endif


#ifdef TUSB_EV_T
#define tusb_ev_t  TUSB_EV_T
#else
typedef struct _tusb_ev tusb_ev_t;
#endif

#ifdef TUSB_EV_CREATE
#define tusb_ev_create TUSB_EV_CREATE
#else
tusb_ev_t* tusb_ev_create(void);
#endif

#ifdef TUSB_EV_DELETE
#define tusb_ev_delete TUSB_EV_DELETE
#else
void tusb_ev_delete(tusb_ev_t* ev);
#endif

#ifdef TUSB_EV_SET
#define tusb_ev_set TUSB_EV_SET
#else
void tusb_ev_set(tusb_ev_t* ev);
#endif

#ifdef TUSB_EV_WAIT
#define tusb_ev_wait TUSB_EV_WAIT
#else
int tusb_ev_wait(tusb_ev_t* ev, uint32_t timeout_in_ms);
#endif




#endif
