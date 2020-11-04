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
 * Copyright (c) 2020 XToolBox - admin@xtoolbox.org
 * www.tusb.org
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
 
#include "teeny_usb.h"
#include "teeny_usb_util.h"
#include "string.h"
#include "tusb_platform_stm32.h"
#include "board_config.h"
#include "tusbh.h"
#include "tusbh_hid.h"


static __IO uint32_t tick_count = 0;
void tusb_delay_ms(uint32_t Delay)
{
  uint32_t tickstart = tick_count;
  uint32_t wait = Delay;
  if (wait < HAL_MAX_DELAY)
  {
    wait += (uint32_t)(1);
  }

  while ((tick_count - tickstart) < wait)
  {
  }
}

static void init_systick(void)
{
    SysTick_Config(HAL_RCC_GetHCLKFreq()/1000);
}

void SysTick_Handler(void)
{
    tick_count++;
}

tusb_host_t g_host;
tusbh_root_hub_t g_root;

static const tusbh_boot_key_class_t cls_boot_key = {
    .backend = &tusbh_boot_keyboard_backend,
    .on_key = 0, // TODO: add keyboard handler
};

static const tusbh_boot_mouse_class_t cls_boot_mouse = {
    .backend = &tusbh_boot_mouse_backend,
    .on_mouse = 0, // TODO: add mouse handler
};

static const tusbh_hid_class_t cls_hid = {
    .backend = &tusbh_hid_backend,
};

static const tusbh_class_cp class_table[] = {
    (tusbh_class_cp)&cls_boot_key,
    (tusbh_class_cp)&cls_boot_mouse,
    (tusbh_class_cp)&cls_hid,
    0,
};

int main(void)
{
    stdio_init();
    // host demo need accurate delay, use systick to do it
    init_systick();
    TUSB_LOGD("Host demo begin\n");
    tusb_mq_t* mq = tusb_mq_create();
    tusb_mq_init(mq);
    
    g_root.mq = mq;
    g_root.id = "HS";
    g_root.support_classes = class_table;
    tusbh_init(&g_host, &g_root);
    tusb_open_host(&g_host, TUSB_DEFAULT_DEV_PARAM);
    HOST_PORT_POWER_ON();
    while(1){
        tusbh_host_handler(mq);
    }
}

