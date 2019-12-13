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
#include "tusbd_user.h"
#include "tusbd_hid.h"
#include "usb_key_code.h"
#include "tusbd_cdc.h"
#include "tusbd_msc.h"
#include "tusbd_cdc_rndis.h"
#include "tusbh.h"
#include <rtthread.h>
#include <rtdevice.h>
#include <stdlib.h>

tusbh_msg_q_t* dev_mq;

__ALIGN_BEGIN uint8_t cdc_buf[CDC_EP1_RX_SIZE] __ALIGN_END;

static void cdc_recv_msg(tusbh_message_t* msg)
{
    tusb_cdc_device_t* cdc = (tusb_cdc_device_t*)msg->param;
    TUSB_PRINTF("CDC got %d bytes\n", msg->len);
    tusb_set_rx_valid(cdc->dev, cdc->ep_out);
}

static int cdc_recv_data(tusb_cdc_device_t* cdc, const void* data, uint16_t len)
{
    POST_MESSAGE(dev_mq, cdc_recv_msg, (uint32_t)cdc, (void*)data, len);
    return 1;
}

static int cdc_send_done(tusb_cdc_device_t* cdc);

static int cdc_send(int argc, char* argv[]);

MSH_CMD_EXPORT(cdc_send, Send CDC data);


static const char* line_coding_to_str(const tusb_cdc_line_coding_t* line_coding)
{
    static char res[16];
    sprintf(res, "%d ", (int)line_coding->bitrate);
    switch(line_coding->parity){
        case CDC_NONE  : strcat(res,"N"); break;
        case CDC_ODD   : strcat(res,"O"); break;
        case CDC_EVEN  : strcat(res,"E"); break;
        case CDC_MARK  : strcat(res,"M"); break;
        case CDC_SPACE : strcat(res,"S"); break;
        default:         strcat(res,"-"); break;
    }
    char bits[8];
    sprintf(bits, " %d ", line_coding->databits);
    strcat(res, bits);
    switch(line_coding->stopbits){
        case 1: strcat(res,"1.5"); break;
        case 2: strcat(res,"2"); break;
        default: strcat(res,"1"); break;
    }
    return res;
}

static void cdc_linecoding_msg(tusbh_message_t* msg)
{
    tusb_cdc_device_t* cdc = (tusb_cdc_device_t*)msg->param;
    TUSB_PRINTF("CDC new line coding %s\n",
        line_coding_to_str(&cdc->line_coding));
}

void cdc_line_coding_change(tusb_cdc_device_t* cdc)
{
    POST_MESSAGE(dev_mq, cdc_linecoding_msg, (uint32_t)cdc, 0, 0);
}

tusb_cdc_device_t cdc_dev = {
  .backend = &cdc_device_backend,
  .ep_in = 1,
  .ep_out = 1,
  .ep_int = 2,
  .on_recv_data = cdc_recv_data,
  .on_send_done = cdc_send_done,
  .on_line_coding_change = cdc_line_coding_change,
  .rx_buf = cdc_buf,
  .rx_size = sizeof(cdc_buf),
};

static tusb_device_interface_t* cdc_interfaces[] = {
  (tusb_device_interface_t*)&cdc_dev, 0,   // CDC need two interfaces
};
static void cdc_init_ep(tusb_device_t* dev)
{
  CDC_TUSB_INIT(dev);
}
tusb_device_config_t cdc_device_config = {
  .if_count = sizeof(cdc_interfaces)/sizeof(cdc_interfaces[0]),
  .interfaces = &cdc_interfaces[0],
  .ep_init = cdc_init_ep,
};


static rt_device_t  msc_device = RT_NULL;
static uint32_t     msc_blk_size = 0;
static int msc_medium(int argc, char* argv[])
{
    if(argc<2){
        rt_kprintf("msc_medium <block dev name>\n");
        return 0;
    }
    rt_device_t dev = rt_device_find(argv[1]);
    if(dev == RT_NULL){
        rt_kprintf("Fail to find %s\n", argv[1]);
        return 0;
    }
    if(rt_device_open(dev, RT_DEVICE_OFLAG_RDWR) != RT_EOK){
        rt_kprintf("Fail to open %s\n", argv[1]);
        return 0;
    }
    if(msc_device != RT_NULL){
        rt_device_close(msc_device);
    }
    msc_device = dev;
    return 0;
}

MSH_CMD_EXPORT(msc_medium, set MSC block device name);

static int msc_is_ready(tusb_msc_device_t* msc, uint8_t lun)
{
    return msc_device != RT_NULL;
}

const static tusb_scsi_ops_t msc_ops = {
    .is_ready = msc_is_ready,
};

int msc_get_cap(tusb_msc_device_t* msc, uint8_t lun, uint32_t *block_num, uint32_t *block_size)
{
    // TODO: bind medium here
    if(msc_device != RT_NULL){
        struct rt_device_blk_geometry geometry;
        rt_memset(&geometry, 0, sizeof(geometry));
        rt_device_control(msc_device, RT_DEVICE_CTRL_BLK_GETGEOME, &geometry);
        *block_num = geometry.sector_count;
        *block_size = geometry.bytes_per_sector;
        msc_blk_size = geometry.bytes_per_sector;
        return 0;
    }
    return -1;
}

int msc_block_read(tusb_msc_device_t* msc, uint8_t lun, uint8_t *buf, uint32_t block_addr, uint16_t block_len)
{
    if(msc_device != RT_NULL){
        if(rt_device_read(msc_device, block_addr, buf, block_len) == block_len){
            return block_len * msc_blk_size;
        }
    }
    return -1;
}

int msc_block_write(tusb_msc_device_t* msc, uint8_t lun, const uint8_t *buf, uint32_t block_addr, uint16_t block_len)
{
    if(msc_device != RT_NULL){
        if(rt_device_write(msc_device, block_addr, buf, block_len) == block_len){
            return block_len * msc_blk_size;
        }
    }
    return -1;
}

static void process_msc_msg(tusbh_message_t* msg)
{
    tusb_msc_device_loop((tusb_msc_device_t*)msg->data);
}

int msc_recv_data(tusb_msc_device_t* msc, const void* data, uint16_t len)
{
    POST_MESSAGE(dev_mq, process_msc_msg, 0, msc, 0);
    return 1; // return 1 will block the recv buffer
}

int msc_send_done(tusb_msc_device_t* msc)
{
    POST_MESSAGE(dev_mq, process_msc_msg, 0, msc, 0);
    return 0;
}

tusb_msc_device_t msc_dev = {
  .backend = &msc_device_backend,
  .ep_in = 1,
  .ep_out = 2,
  .max_lun = 0, // 1 logic unit
  .on_recv_data = msc_recv_data,
  .on_send_done = msc_send_done,
  .get_cap = msc_get_cap,
  .block_read = msc_block_read,
  .block_write = msc_block_write,
  .scsi_ops = &msc_ops,
};

static tusb_device_interface_t* msc_interfaces[] = {
  (tusb_device_interface_t*)&msc_dev,
};
static void msc_init_ep(tusb_device_t* dev)
{
  MSC_TUSB_INIT(dev);
}
tusb_device_config_t msc_device_config = {
  .if_count = sizeof(msc_interfaces)/sizeof(msc_interfaces[0]),
  .interfaces = &msc_interfaces[0],
  .ep_init = msc_init_ep,
};


static void set_report_msg(tusbh_message_t* msg)
{
    TUSB_PRINTF("HID set report type: %d, len %d\n",
        (int)msg->param, (int)msg->len);
}

int on_kb_report(tusb_hid_device_t* hid, uint8_t reportType, uint8_t reportID, const void* data, uint16_t len)
{
    POST_MESSAGE(dev_mq, set_report_msg, reportType, (void*)data, len);
    return 0;
}

tusb_hid_device_t kb_dev = {
  .backend = &hid_device_backend,
  .ep_in = 1,
  .on_set_report = on_kb_report,
  .report_desc = KB_ReportDescriptor_if0,
  .report_desc_size = KB_REPORT_DESCRIPTOR_SIZE_IF0,
};

static tusb_device_interface_t* kb_interfaces[] = {
  (tusb_device_interface_t*)&kb_dev,
};
static void kb_init_ep(tusb_device_t* dev)
{
  KB_TUSB_INIT(dev);
}
tusb_device_config_t kb_device_config = {
  .if_count = sizeof(kb_interfaces)/sizeof(kb_interfaces[0]),
  .interfaces = &kb_interfaces[0],
  .ep_init = kb_init_ep,
};

tusb_hid_device_t mouse_dev = {
  .backend = &hid_device_backend,
  .ep_in = 1,
  .report_desc = MOUSE_ReportDescriptor_if0,
  .report_desc_size = MOUSE_REPORT_DESCRIPTOR_SIZE_IF0,
};

static tusb_device_interface_t* mouse_interfaces[] = {
  (tusb_device_interface_t*)&mouse_dev,
};
static void mouse_init_ep(tusb_device_t* dev)
{
  MOUSE_TUSB_INIT(dev);
}
tusb_device_config_t mouse_device_config = {
  .if_count = sizeof(mouse_interfaces)/sizeof(mouse_interfaces[0]),
  .interfaces = &mouse_interfaces[0],
  .ep_init = mouse_init_ep,
};


__ALIGN_BEGIN uint8_t user_buf[WINUSB_EP1_RX_SIZE*4] __ALIGN_END;

static void user_recv_msg(tusbh_message_t* msg)
{
    tusb_user_device_t* raw = (tusb_user_device_t*)msg->param;
    TUSB_PRINTF("WinUSB got %d bytes\n", msg->len);
    tusb_set_rx_valid(raw->dev, raw->ep_out);
}

static int user_recv_data(tusb_user_device_t* raw, const void* data, uint16_t len)
{
    POST_MESSAGE(dev_mq, user_recv_msg, (uint32_t)raw, (void*)data, len);
    return 1;
}

static int user_send_done(tusb_user_device_t* raw);

static int winusb_send(int argc, char* argv[]);

MSH_CMD_EXPORT(winusb_send, Send WinUSB data);

tusb_user_device_t winusb_dev = {
  .backend = &user_device_backend,
  .ep_in = 1,
  .ep_out = 1,
  .on_recv_data = user_recv_data,
  .on_send_done = user_send_done,
  .rx_buf = user_buf,
  .rx_size = sizeof(user_buf),
};

static tusb_device_interface_t* winusb_interfaces[] = {
  (tusb_device_interface_t*)&winusb_dev,
};
static void winusb_init_ep(tusb_device_t* dev)
{
  WINUSB_TUSB_INIT(dev);
}

tusb_device_config_t winusb_device_config = {
  .if_count = sizeof(winusb_interfaces)/sizeof(winusb_interfaces[0]),
  .interfaces = &winusb_interfaces[0],
  .ep_init = winusb_init_ep,
};

extern tusb_device_config_t rndis_device_config;


tusb_device_config_t*  config_fs;
tusb_device_config_t*  config_hs;
static tusb_device_t* dev_fs;
static tusb_device_t* dev_hs;
static int fs_on;
static int hs_on;

__ALIGN_BEGIN  uint8_t test_buf[64] __ALIGN_END;
static struct rt_completion cdc_comp;
static int cdc_send_done(tusb_cdc_device_t* cdc)
{
    rt_completion_done(&cdc_comp);
    return 0;
}

static int cdc_send(int argc, char* argv[])
{
    if(argc<2){
        rt_kprintf("cdc_send <data>\n");
        return 0;
    }
    if(config_fs == &cdc_device_config || config_hs == &cdc_device_config){
        rt_completion_init(&cdc_comp);
        int len = strlen(argv[1]);
        if(len > sizeof(test_buf)) len = sizeof(test_buf);
        memcpy(test_buf, argv[1], len);
        if(tusb_cdc_device_send(&cdc_dev, test_buf, len) != 0){
            rt_kprintf("CDC send data fail\n");
            return 0;
        }
        if(rt_completion_wait(&cdc_comp, rt_tick_from_millisecond(500)) != RT_EOK){
            rt_kprintf("cdc send data timeout\n");
            tusb_cancel_send(cdc_dev.dev, cdc_dev.ep_in);
        }
    }else{
        rt_kprintf("cdc not setup\n");
    }
    return 0;
}

static struct rt_completion user_comp;
static int user_send_done(tusb_user_device_t* user)
{
    rt_completion_done(&user_comp);
    return 0;
}

static int winusb_send(int argc, char* argv[])
{
    if(argc<2){
        rt_kprintf("winusb_send <data>\n");
        return 0;
    }
    if(config_fs == &winusb_device_config || config_hs == &winusb_device_config){
        rt_completion_init(&user_comp);
        int len = strlen(argv[1]);
        if(len > sizeof(test_buf)) len = sizeof(test_buf);
        memcpy(test_buf, argv[1], len);
        if(tusb_user_device_send(&winusb_dev, test_buf, len) != 0){
            rt_kprintf("WinUSB send data fail\n");
            return 0;
        }
        if(rt_completion_wait(&user_comp, rt_tick_from_millisecond(500)) != RT_EOK){
            rt_kprintf("WinUSB send data timeout\n");
            tusb_cancel_send(winusb_dev.dev, winusb_dev.ep_in);
        }
    }else{
        rt_kprintf("WinUSB not setup\n");
    }
    return 0;
}


static int mouse_move(int argc, char* argv[])
{
    if(argc<3){
        rt_kprintf("mouse_move dx dy\n");
        return 0;
    }
    if(config_fs == &mouse_device_config || config_hs == &mouse_device_config){
        test_buf[0] = 0;
        ((int8_t*)test_buf)[1] = atoi(argv[1]);
        ((int8_t*)test_buf)[2] = atoi(argv[2]);
        tusb_hid_device_send(&mouse_dev, test_buf, 3);
    }else{
        rt_kprintf("Mouse not setup\n");
    }
    return 0;
}

static int mouse_down(int argc, char* argv[])
{
    if(config_fs == &mouse_device_config || config_hs == &mouse_device_config){
        test_buf[0] = 1;
        test_buf[1] = 0;
        test_buf[2] = 0;
        tusb_hid_device_send(&mouse_dev, test_buf, 3);
        rt_thread_delay(rt_tick_from_millisecond(500));
        test_buf[0] = 0;
        tusb_hid_device_send(&mouse_dev, test_buf, 3);
    }else{
        rt_kprintf("Mouse not setup\n");
    }
    return 0;
}

static int key(int argc, char* argv[])
{
    if(argc<2){
        rt_kprintf("key <key value>\n");
        return 0;
    }
    uint8_t keyc = KEY_DOT;
    if(argv[1][0]>= 'A' && argv[1][0]<= 'Z'){
        keyc = KEY_A + argv[1][0] - 'A';
    }
    if(argv[1][0]>= 'a' && argv[1][0]<= 'z'){
        keyc = KEY_A + argv[1][0] - 'a';
    }
    if(config_fs == &kb_device_config || config_hs == &kb_device_config){
        memset(test_buf, 0, 8);
        test_buf[2] = keyc;
        tusb_hid_device_send(&kb_dev, test_buf, 8);
        rt_thread_delay(rt_tick_from_millisecond(500));
        test_buf[2] = 0;
        tusb_hid_device_send(&kb_dev, test_buf, 8);
    }else{
        rt_kprintf("Keyboard not setup\n");
    }
    return 0;
}

MSH_CMD_EXPORT(mouse_move, Simulate move mouse);
MSH_CMD_EXPORT(mouse_down, Simulate left click mouse);
MSH_CMD_EXPORT(key, simulate key down);

static const char* dev_name(tusb_device_config_t* cfg)
{
    if(cfg == &cdc_device_config){
        return "CDC ACM";
    }else if(cfg == &msc_device_config){
        return "MSC";
    }else if(cfg == &kb_device_config){
        return "Keyboard";
    }else if(cfg == &mouse_device_config){
        return "Mouse";
    }else if(cfg == &winusb_device_config){
        return "WinUSB";
    }else if(cfg == &rndis_device_config){
        return "RNDIS";
    }
    return "Unknown";
}

void ls_usbdev(void)
{
#if defined(USB_CORE_ID_FS)
    TUSB_PRINTF("FS device mode is %s\n", dev_name(config_fs));
#endif
#if defined(USB_CORE_ID_HS)
    TUSB_PRINTF("HS device mode is %s\n", dev_name(config_hs));
#endif
}

void device_change(int id, int is_on, tusb_device_config_t* new_config)
{
#if defined(USB_CORE_ID_FS)
    if(id == USB_CORE_ID_FS){
        if(new_config == config_hs){
            TUSB_PRINTF("%s device is used by HS device core\n", dev_name(new_config));
            return;
        }
        if(is_on != fs_on){
            if(is_on){
                TUSB_PRINTF("Open USB FS core %s device\n", dev_name(new_config));
                config_fs = new_config;
                tusb_set_device_config(dev_fs, config_fs);
                tusb_open_device(dev_fs);
            }else{
                TUSB_PRINTF("Close USB FS core %s device\n", dev_name(new_config));
                tusb_close_device(dev_fs);
            }
            fs_on = is_on;
        }else{
            if(!is_on){
                config_fs = new_config;
            }
        }
        if(new_config != config_fs && fs_on){
            TUSB_PRINTF("Change USB FS core %s device\n", dev_name(new_config));
            config_fs = new_config;
            tusb_close_device(dev_fs);
            tusb_delay_ms(500);
            tusb_set_device_config(dev_fs, config_fs);
            tusb_open_device(dev_fs);
        }
    }
#endif
    
#if defined(USB_CORE_ID_HS)
    if(id == USB_CORE_ID_HS){
        if(new_config == config_fs){
            TUSB_PRINTF("%s device is used by FS device core\n", dev_name(new_config));
            return;
        }
        if(is_on != hs_on){
            if(is_on){
                TUSB_PRINTF("Open USB HS core %s device\n", dev_name(new_config));
                config_hs = new_config;
                tusb_set_device_config(dev_hs, config_hs);
                tusb_open_device(dev_hs);
            }else{
                TUSB_PRINTF("Close USB HS core %s device\n", dev_name(new_config));
                tusb_close_device(dev_hs);
            }
            hs_on = is_on;
        }else{
            if(!is_on){
                config_hs = new_config;
            }
        }
        if(new_config != config_hs && hs_on){
            TUSB_PRINTF("Change USB HS core to %s device\n", dev_name(new_config));
            config_hs = new_config;
            tusb_close_device(dev_hs);
            tusb_delay_ms(400);
            tusb_set_device_config(dev_hs, config_hs);
            tusb_open_device(dev_hs);
        }
    }
#endif
}

void device_mode(int id, int is_on)
{
#if defined(USB_CORE_ID_FS)
    if(id == USB_CORE_ID_FS){
        device_change(id, is_on, config_fs);
    }
#endif
#if defined(USB_CORE_ID_HS)
    if(id == USB_CORE_ID_HS){
        device_change(id, is_on, config_hs);
    }
#endif
}


static void usb_device_thread_entry(void* arg)
{
    TUSB_PRINTF("Device thread runnig\n");
    while(1){
        tusbh_msg_loop(dev_mq);
    }
}

static int device_thread_init(void)
{
    rt_thread_t tid;
    dev_mq = tusbh_mq_create();
    RT_ASSERT(dev_mq != RT_NULL);
#if defined(USB_CORE_ID_FS)
    config_fs = &kb_device_config;
    dev_fs = tusb_get_device(USB_CORE_ID_FS);
#endif

#if defined(USB_CORE_ID_HS)
    config_hs = &mouse_device_config;
    dev_hs = tusb_get_device(USB_CORE_ID_HS);
#endif
    
    tid = rt_thread_create("U_dev", usb_device_thread_entry, RT_NULL, 1024, 3, 5);
    RT_ASSERT(tid != RT_NULL);
    rt_thread_startup(tid);
    
    return 0;
}

#if defined(USB_CORE_ID_FS)

long usbfs_cdc(void)   {    device_change(USB_CORE_ID_FS, fs_on, &cdc_device_config);    return 0; }
long usbfs_msc(void)   {    device_change(USB_CORE_ID_FS, fs_on, &msc_device_config);    return 0; }
long usbfs_kb(void)    {    device_change(USB_CORE_ID_FS, fs_on, &kb_device_config);     return 0; }
long usbfs_mouse(void) {    device_change(USB_CORE_ID_FS, fs_on, &mouse_device_config);  return 0; }
long usbfs_winusb(void){    device_change(USB_CORE_ID_FS, fs_on, &winusb_device_config); return 0; }
long usbfs_rndis(void){     device_change(USB_CORE_ID_FS, fs_on, &rndis_device_config);  return 0; }

MSH_CMD_EXPORT(usbfs_cdc, set USB FS to CDC device);
MSH_CMD_EXPORT(usbfs_msc, set USB FS to MSC device);
MSH_CMD_EXPORT(usbfs_kb, set USB FS to keyboard device);
MSH_CMD_EXPORT(usbfs_mouse, set USB FS to mouse device);
MSH_CMD_EXPORT(usbfs_winusb, set USB FS to WinUSB device);
MSH_CMD_EXPORT(usbfs_rndis, set USB FS to RNDIS device);
#endif

#if defined(USB_CORE_ID_HS)

long usbhs_cdc(void)   {    device_change(USB_CORE_ID_HS, hs_on, &cdc_device_config);    return 0; }
long usbhs_msc(void)   {    device_change(USB_CORE_ID_HS, hs_on, &msc_device_config);    return 0; }
long usbhs_kb(void)    {    device_change(USB_CORE_ID_HS, hs_on, &kb_device_config);     return 0; }
long usbhs_mouse(void) {    device_change(USB_CORE_ID_HS, hs_on, &mouse_device_config);  return 0; }
long usbhs_winusb(void){    device_change(USB_CORE_ID_HS, hs_on, &winusb_device_config); return 0; }
long usbhs_rndis(void){     device_change(USB_CORE_ID_HS, hs_on, &rndis_device_config);  return 0; }

MSH_CMD_EXPORT(usbhs_cdc, set USB HS to CDC device);
MSH_CMD_EXPORT(usbhs_msc, set USB HS to MSC device);
MSH_CMD_EXPORT(usbhs_kb, set USB HS to keyboard device);
MSH_CMD_EXPORT(usbhs_mouse, set USB HS to mouse device);
MSH_CMD_EXPORT(usbhs_winusb, set USB HS to WinUSB device);
MSH_CMD_EXPORT(usbhs_rndis, set USB HS to RNDIS device);
#endif

INIT_APP_EXPORT(device_thread_init);
