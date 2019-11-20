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

#include "teeny_usb.h"
#include "tusbh.h"
#include "tusbh_vendor.h"
#include "tusbh_hub.h"
#include "tusbh_hid.h"
#include "string.h"
#include "usb_key_code.h"

extern uint32_t SystemCoreClock;

static __IO uint32_t tick_ms;
void SysTick_Handler(void)
{
  tick_ms++;
}

// host need accurate delay
void tusb_delay_ms(uint32_t ms)
{
  uint32_t t = tick_ms;
  while( tick_ms - t < ms);
}

static int process_key(tusbh_ep_info_t* ep, const uint8_t* key);

static tusbh_root_hub_t root_fs;
static tusbh_root_hub_t root_hs;
static const tusbh_boot_key_class_t cls_boot_key = {
    .backend = &tusbh_boot_keyboard_backend,
    .on_key = process_key
};

static const tusbh_boot_mouse_class_t cls_boot_mouse = {
    .backend = &tusbh_boot_mouse_backend,
    // .on_mouse = process_mouse
};

static const tusbh_hid_class_t cls_hid = {
    .backend = &tusbh_hid_backend,
    //.on_recv_data = process_hid_recv,
    //.on_send_done = process_hid_sent,
};

static const tusbh_hub_class_t cls_hub = {
    .backend = &tusbh_hub_backend,
};

static const tusbh_vendor_class_t cls_vendor = {
    .backend = &tusbh_vendor_backend,
    //.transfer_done = process_vendor_xfer_done
};

static const tusbh_class_reg_t class_table[] = {
    (tusbh_class_reg_t)&cls_boot_key,
    (tusbh_class_reg_t)&cls_boot_mouse,
    (tusbh_class_reg_t)&cls_hub,
    (tusbh_class_reg_t)&cls_hid,
    (tusbh_class_reg_t)&cls_vendor,
    0,
};


static tusb_host_t* fs;
static tusb_host_t* hs;

void show_memory(void);
static void process_command(const char* cmd)
{
    if(strstr(cmd, "lsusb") == cmd){
        if(fs)ls_usb(fs);
        if(hs)ls_usb(hs);
        return;
    }else if(strstr(cmd, "showmem") == cmd){
        show_memory();
        return;
    }
    printf("Unkown command: %s\n", cmd);
}

#define PROMPT  "TeenyUSB > "

static char cmd_buf[256];
static int cmd_len;
static void command_loop(void)
{
    int ch = getchar();
    if(ch > 0){
        if(ch == '\r'){
            cmd_buf[cmd_len] = 0;
            if(cmd_buf[0]){
                process_command(cmd_buf);
            }
            cmd_len = 0;
            printf(PROMPT);
        }else if(ch == '\t'){
            printf("\nlsusb showmem\n");
            printf(PROMPT);
            cmd_len = 0;
        }else{
            if(cmd_len < sizeof(cmd_buf)-1){
                cmd_buf[cmd_len] = ch;
                cmd_len++;
            }
        }
    }
}

int main()
{
    setbuf (stdout, NULL);
    setvbuf (stdout, NULL, _IONBF, BUFSIZ);
    stdio_init();
  
    SystemCoreClockUpdate();
    
    SysTick_Config(SystemCoreClock/1000);
  
    printf("Start USB host demo\n" PROMPT);
    
    tusbh_msg_q_t* mq = tusbh_mq_create();
    tusbh_mq_init(mq);
    
#if defined(USB_CORE_ID_FS)
    fs = tusb_get_host(USB_CORE_ID_FS);
    HOST_PORT_POWER_ON_FS();
    root_fs.mq = mq;
    root_fs.id = "FS";
    root_fs.support_classes = class_table;
    tusb_host_init(fs, &root_fs);
    tusb_open_host(fs);
#else
    (void)root_fs;
    fs = 0;
#endif    

#if defined(USB_CORE_ID_HS)
    hs = tusb_get_host(USB_CORE_ID_HS);    
    HOST_PORT_POWER_ON_HS();
    root_hs.mq = mq;
    root_hs.id = "HS";
    root_hs.support_classes = class_table;
    tusb_host_init(hs, &root_hs);
    tusb_open_host(hs);
#else
    (void)root_hs;
    hs = 0;
#endif

    cmd_len = 0;
    while(1){
        command_loop();
        // there is only one message q for every thing
        tusbh_msg_loop(mq);
    }
}


#define MOD_CTRL      (0x01 | 0x10)
#define MOD_SHIFT     (0x02 | 0x20)
#define MOD_ALT       (0x04 | 0x40)
#define MOD_WIN       (0x08 | 0x80)

#define LED_NUM_LOCK    1
#define LED_CAPS_LOCK   2
#define LED_SCROLL_LOCK 4


static uint8_t key_leds;
static const char knum[] = "1234567890";
static const char ksign[] = "!@#$%^&*()";
static const char tabA[] = "\t -=[]\\#;'`,./";
static const char tabB[] = "\t _+{}|~:\"~<>?";
// route the key event to stdin
static int process_key(tusbh_ep_info_t* ep, const uint8_t* keys)
{
    uint8_t modify = keys[0];
    uint8_t key = keys[2];
    uint8_t last_leds = key_leds;
    if(key >= KEY_A && key <= KEY_Z){
        char ch = 'A' + key - KEY_A;
        if( (!!(modify & MOD_SHIFT)) == (!!(key_leds & LED_CAPS_LOCK)) ){
            ch += 'a'-'A';
        }
        stdin_recvchar(ch);
    }else if(key >= KEY_1 && key <= KEY_0){
        if(modify & MOD_SHIFT){
            stdin_recvchar(ksign[key - KEY_1]);
        }else{
            stdin_recvchar(knum[key - KEY_1]);
        }
    }else if(key >= KEY_TAB && key <= KEY_SLASH){
        if(modify & MOD_SHIFT){
            stdin_recvchar(tabB[key - KEY_TAB]);
        }else{
            stdin_recvchar(tabA[key - KEY_TAB]);
        }
    }else if(key == KEY_ENTER){
        stdin_recvchar('\r');
    }else if(key == KEY_CAPSLOCK){
        key_leds ^= LED_CAPS_LOCK;
    }else if(key == KEY_NUMLOCK){
        key_leds ^= LED_NUM_LOCK;
    }else if(key == KEY_SCROLLLOCK){
        key_leds ^= LED_SCROLL_LOCK;
    }
    
    if(key_leds != last_leds){
        tusbh_set_keyboard_led(ep, key_leds);
    }
    return 0;
}




#ifdef DEBUG

#define LOG_SIZE   1024
static int hc_log_index;
static channel_state_t hc_log_buf[LOG_SIZE];
static USB_OTG_HostChannelTypeDef hc_info;
static USB_OTG_HostChannelTypeDef* hc_reg;
static tusb_hc_data_t* hc_data;
static uint8_t hc_no;
void hc_log_begin(tusb_host_t* host, uint8_t hc_num)
{
    USB_OTG_GlobalTypeDef *USBx = GetUSB(host);
    USB_OTG_HostChannelTypeDef* HC = USBx_HC(hc_num);
    tusb_hc_data_t* hc = &host->hc[hc_num];
    hc_reg = HC;
    hc_data = hc;
    hc_no = hc_num;
    hc_info.HCCHAR   = HC->HCCHAR;
    hc_info.HCSPLT   = HC->HCSPLT;
    hc_info.HCINTMSK = HC->HCINTMSK;
    hc_info.HCTSIZ   = HC->HCTSIZ;
    hc_info.HCDMA    = HC->HCDMA;
    hc_log_index = 0;
}

void hc_log_data(tusb_host_t* host, uint8_t hc_num, uint32_t data)
{
    if(hc_log_index<LOG_SIZE){
        hc_log_buf[hc_log_index] = (channel_state_t)data;
        hc_log_index++;
    }
}

void hc_log_end(tusb_host_t* host, uint8_t hc_num)
{
}

#endif

