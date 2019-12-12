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
#include "tusbh_msc.h"
#include "tusbh_cdc_acm.h"
#include "tusbh_cdc_rndis.h"
#include "string.h"
#include "usb_key_code.h"

#define TEENYUSB_LOGO \
" _______                    _    _  _____ ____  \n"           \
"|__   __|                  | |  | |/ ____|  _ \\ \n"          \
"   | | ___  ___ _ __  _   _| |  | | (___ | |_) |\n"           \
"   | |/ _ \\/ _ \\ '_ \\| | | | |  | |\\___ \\|  _ < \n"      \
"   | |  __/  __/ | | | |_| | |__| |____) | |_) |\n"           \
"   |_|\\___|\\___|_| |_|\\__, |\\____/|_____/|____/ \n"       \
"                       __/ |                    \n"           \
"                      |___/                     \n"

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

int msc_ff_mount(tusbh_interface_t* interface, int max_lun, const tusbh_block_info_t* blocks);
int msc_ff_unmount(tusbh_interface_t* interface);

static const tusbh_msc_class_t cls_msc_bot = {
    .backend = &tusbh_msc_bot_backend,
    .mount = msc_ff_mount,
    .unmount = msc_ff_unmount,
};

static const tusbh_cdc_acm_class_t cls_cdc_acm = {
    .backend = &tusbh_cdc_acm_backend,
};

static const tusbh_cdc_rndis_class_t cls_cdc_rndis = {
    .backend = &tusbh_cdc_rndis_backend,
};

static const tusbh_class_reg_t class_table[] = {
    (tusbh_class_reg_t)&cls_boot_key,
    (tusbh_class_reg_t)&cls_boot_mouse,
    (tusbh_class_reg_t)&cls_hub,
    (tusbh_class_reg_t)&cls_msc_bot,
    (tusbh_class_reg_t)&cls_cdc_acm,
    (tusbh_class_reg_t)&cls_cdc_rndis,
    (tusbh_class_reg_t)&cls_hid,
    (tusbh_class_reg_t)&cls_vendor,
    0,
};


static tusb_host_t* fs;
static tusb_host_t* hs;

void show_memory(char* argv[], int argc);
void cmd_lsusb(char* argv[], int argc)
{
    if(fs)ls_usb(fs);
    if(hs)ls_usb(hs);
}

void cmd_ls(char* argv[], int argc);
void cmd_mkdir(char* argv[], int argc);
void cmd_mv(char* argv[], int argc);
void cmd_cat(char* argv[], int argc);
void cmd_cp(char* argv[], int argc);
void cmd_rm(char* argv[], int argc);
void cmd_append(char* argv[], int argc);

static tusbh_interface_t* find_cdc(tusbh_device_t* dev)
{
    if(!dev)return 0;
    for(int i=0;i<dev->interface_num;i++){
        tusbh_interface_t* itf = &dev->interfaces[i];
        if(itf->cls ==  (tusbh_class_reg_t)&cls_cdc_acm){
            return itf;
        }
    }
    for(int i=0;i<TUSBH_MAX_CHILD;i++){
        tusbh_device_t* child = dev->children[i];
        if(child){
            return find_cdc(child);
        }
    }
    return 0;
}

__ALIGN_BEGIN static uint8_t test_cdc_data[64] __ALIGN_END = {
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
    0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
};


void cmd_cdc_test(char* argv[], int argc)
{
    tusbh_interface_t* cdc = 0;
    if(fs){
        cdc = find_cdc(  ((tusbh_root_hub_t*)fs->user_data)->children[0]);
    }
    
    if(!cdc && hs){
        cdc = find_cdc(  ((tusbh_root_hub_t*)hs->user_data)->children[0]);
    }
    
    if(!cdc){
        printf("No CDC interface attached\n");
    }
    
    int r = tusbh_cdc_send_data(cdc, test_cdc_data, 32, 2000);
    printf("Send data result = %d\n", r);
    if(r<0)return;
    r = tusbh_cdc_recv_data(cdc, test_cdc_data+32, 32, 2000);
    printf("Recv data result = %d\n", r);
    for(int i=0;i<r;i++){
        printf("%02x ", test_cdc_data[32+i]);
    }
    printf("\n");
}


typedef struct _cli
{
    const char* cmd;
    const char* desc;
    void (*action)(char* argv[], int argc);
}cli_t;



const cli_t commands[] = {
    {"lsusb",     "list usb devices",    cmd_lsusb    },
    {"showmem",   "show memory usage",   show_memory  },
    {"ls",        "ls <dir>   list dir", cmd_ls       },
    {"mkdir",     "mkdir <dir>  create dir", cmd_mkdir},
    {"mv",        "mv <oldfile> <newfile> move file", cmd_mv},
    {"cat",       "cat <file> display file", cmd_cat},
    {"cp",        "cp <source> <dest> copy file", cmd_cp},
    {"rm",        "rm <target> remove file or path", cmd_rm},
    {"append",    "append <file> <data> [data data ...] add data to file", cmd_append},
    {"testcdc",   "testcdc   test cdc loopback interface", cmd_cdc_test},
};

static void process_command(char* cmd)
{
#define MAX_ARGC 8
    char * argv[MAX_ARGC];
    for(int i=0;i< sizeof(commands)/sizeof(commands[0]); i++){
        const cli_t* c = &commands[i];
        if(strstr(cmd, c->cmd) == cmd && c->action){
            int argc = 1;
            argv[0] = cmd;
            while(*cmd){
                if(*cmd == ' '){
                    while(*cmd && *cmd == ' '){
                        *cmd = 0;
                        cmd++;
                    }
                    if(*cmd){
                        if(argc<MAX_ARGC){
                            argv[argc] = cmd;
                            argc++;
                        }
                        cmd++;
                    }
                }else{
                    cmd++;
                }
            }
            c->action(argv, argc);
            return;
        }
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
            printf("\n");
            for(int i=0;i< sizeof(commands)/sizeof(commands[0]); i++){
                const cli_t* c = &commands[i];
                printf("%-10s - %s\n", c->cmd, c->desc?c->desc:"");
            }
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
    printf("\n" TEENYUSB_LOGO PROMPT);
    
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

