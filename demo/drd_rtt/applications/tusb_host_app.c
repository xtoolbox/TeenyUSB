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
#include "usb_key_code.h"
#include <rtthread.h>
#include "string.h"
#include <dfs_ramfs.h>
#include <dfs_fs.h>
#include <dfs_posix.h>

static int process_key(tusbh_ep_info_t* ep, const uint8_t* key);
static int msc_ff_mount(tusbh_interface_t* interface, int max_lun, const tusbh_block_info_t* blocks);
static int msc_ff_unmount(tusbh_interface_t* interface);

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

static const tusbh_msc_class_t cls_msc_bot = {
    .backend = &tusbh_msc_bot_backend,
    .mount = msc_ff_mount,
    .unmount = msc_ff_unmount,
};

static const tusbh_cdc_acm_class_t cls_cdc_acm = {
    .backend = &tusbh_cdc_acm_backend,
};

int v_eth_host_unmount(tusbh_interface_t* itf);
int v_eth_host_response(tusbh_interface_t* itf, rndis_generic_msg_t* msg);

static const tusbh_cdc_rndis_class_t cls_cdc_rndis = {
    .backend = &tusbh_cdc_rndis_backend,
    .unmount = v_eth_host_unmount,
    .on_response = v_eth_host_response,
};

typedef void(*traverse_action_t)(tusbh_interface_t* itf, void* data);

static void usb_traverse(tusbh_device_t* dev, tusbh_class_reg_t cls,  traverse_action_t action,void* data)
{
    if(!dev)return;
    for(int i=0;i<dev->interface_num;i++){
        tusbh_interface_t* itf = &dev->interfaces[i];
        if(itf->cls == cls){
            action(itf, data);
        }
    }
    for(int i=0;i<TUSBH_MAX_CHILD;i++){
        tusbh_device_t* child = dev->children[i];
        if(child){
            usb_traverse(child, cls, action, data);
        }
    }
}

// supported host classes
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
static void set_led(tusbh_interface_t* interface, void* data)
{
    tusbh_hid_info_t* info = tusbh_get_info(interface, tusbh_hid_info_t);
    tusbh_set_keyboard_led(info->ep_in, (uint8_t)((uint32_t)data) );
}

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
        uint32_t t = key_leds;
        usb_traverse(ep_device(ep), (tusbh_class_reg_t)&cls_boot_key, set_led, (void*)t);
    }
    return 0;
}


typedef struct _rt_msc_dev
{
    tusbh_interface_t* interface;
    int lun;
    struct rt_semaphore lock;
    uint32_t block_size;
    uint32_t block_count;
    char     name[16];
    int      id;
}rt_msc_dev_t;

static uint32_t msc_mask = 0;
static void free_msc_id(int id)
{
    uint32_t f = 0;
    if(id<32){
        f = 1ul<<id;
    }
    msc_mask &= ~f;
}

static int get_msc_id()
{
    uint32_t f = 1;
    int i;
    for(i=0;i<32;i++){
        if( (msc_mask & f) == 0){
            msc_mask |= f;
            return i;
        }
    }
    return -1;
}

#define get_msc(dev)  ((rt_msc_dev_t*)((dev)->user_data))
/* RT-Thread Device Driver Interface */
static rt_err_t rt_msc_init(rt_device_t dev)
{
	return RT_EOK;
}

static rt_err_t rt_msc_open(rt_device_t dev, rt_uint16_t oflag)
{
	return RT_EOK;
}

static rt_err_t rt_msc_close(rt_device_t dev)
{
	return RT_EOK;
}

static rt_size_t rt_msc_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
    rt_msc_dev_t*  msc = get_msc(dev);
    rt_sem_take(&msc->lock, RT_WAITING_FOREVER);
    int res = tusbh_msc_block_read(msc->interface, msc->lun, pos, size, buffer);
    rt_sem_release(&msc->lock);
    if(res<0)return 0;
	return size;
}

static rt_size_t rt_msc_write (rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
    rt_msc_dev_t*  msc = get_msc(dev);
    rt_sem_take(&msc->lock, RT_WAITING_FOREVER);
    int res = tusbh_msc_block_write(msc->interface, msc->lun, pos, size, (void*)buffer);
    rt_sem_release(&msc->lock);
    if(res<0)return 0;
    return size;
}

static rt_err_t rt_msc_control(rt_device_t dev, int cmd, void *args)
{
    RT_ASSERT(dev != RT_NULL);
    rt_msc_dev_t*  msc = get_msc(dev);
    if (cmd == RT_DEVICE_CTRL_BLK_GETGEOME)
    {
        struct rt_device_blk_geometry *geometry;

        geometry = (struct rt_device_blk_geometry *)args;
        if (geometry == RT_NULL) return -RT_ERROR;

        geometry->bytes_per_sector = msc->block_size;
        geometry->block_size = msc->block_size;
		geometry->sector_count = msc->block_count;
    }

	return RT_EOK;
}

typedef struct _msc_node
{
    rt_list_t         node;
    struct rt_device  device;
    rt_msc_dev_t      msc;
}msc_node_t;

static rt_list_t msc_head;

int msc_ff_mount(tusbh_interface_t* interface, int max_lun, const tusbh_block_info_t* blocks)
{
    for(int i=0;i<=max_lun;i++){
        msc_node_t* node = (msc_node_t*)rt_malloc(sizeof(msc_node_t));
        if(node != RT_NULL){
            node->device.type  = RT_Device_Class_Block;
            node->device.init 	= rt_msc_init;
            node->device.open 	= rt_msc_open;
            node->device.close = rt_msc_close;
            node->device.read 	= rt_msc_read;
            node->device.write = rt_msc_write;
            node->device.control = rt_msc_control;
            node->msc.block_size = blocks[i].block_size;
            node->msc.block_count = blocks[i].block_count;
            node->msc.id = get_msc_id();
            node->msc.interface = interface;
            node->msc.lun = i;
            rt_sem_init(&node->msc.lock, node->msc.name, 1, RT_IPC_FLAG_FIFO);
            rt_sprintf(node->msc.name, "udisk%d", node->msc.id);
            node->device.user_data = &node->msc;
            rt_err_t err = rt_device_register(&node->device, node->msc.name, RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_REMOVABLE);
            if(err == RT_EOK){
                char dir[16];
                rt_sprintf(dir, "/%s", node->msc.name);
                mkdir(dir, 0);
                if(dfs_mount(node->msc.name, dir, "elm", 0, 0) == 0){
                    const scsi_inquiry_std_response_t* inquiry = &blocks[i].inquiry;
                    TUSB_PRINTF("Mount %s, (%8.8s - %16.16s - %4.4s lun=%d )\n",
                    dir, inquiry->vid, inquiry->pid, inquiry->product_version, i);
                    rt_list_insert_after(&msc_head, &node->node);
                    continue;
                }else{
                    TUSB_PRINTF("Mount device fail, %s\n",node->msc.name);
                }
            }else{
                TUSB_PRINTF("register device fail, %s\n",node->msc.name);
            }
            free_msc_id(node->msc.id);
            rt_free(node);
        }
    }
    return 0;
}

int msc_ff_unmount(tusbh_interface_t* interface)
{
    for(rt_list_t* pos = msc_head.next; pos != &msc_head;){
        msc_node_t* node = rt_list_entry(pos, msc_node_t, node);
        if(node->msc.interface == interface){
            char dir[16];
            rt_sprintf(dir, "/%s", node->msc.name);
            TUSB_PRINTF("Unmount %s\n", dir);
            dfs_unmount(dir);
            rmdir(dir);
            rt_device_unregister( &node->device);
            rt_sem_detach(&node->msc.lock);
            free_msc_id(node->msc.id);
            rt_list_t* t = pos;
            pos = pos->next;
            rt_list_remove(t);
            rt_free(node);
        }else{
            pos = pos->next;
        }
    }
    return 0;
}

tusbh_msg_q_t* host_mq;
static tusb_host_t* host_fs;
static tusb_host_t* host_hs;
static tusbh_root_hub_t root_fs;
static tusbh_root_hub_t root_hs;

static void usb_host_thread_entry(void* arg)
{
    TUSB_PRINTF("Host thread runnig\n");
    while(1){
        tusbh_msg_loop(host_mq);
    }
}

static void host_power_msg(tusbh_message_t* msg)
{
    int id = msg->param;
    int is_on = msg->len;
#if defined(USB_CORE_ID_FS)
    if(id == USB_CORE_ID_FS){
        if(is_on){
            HOST_PORT_POWER_ON_FS();
            tusb_host_init(host_fs, &root_fs);
            tusb_open_host(host_fs);
        }else{
            HOST_PORT_POWER_OFF_FS();
            tusb_close_host(host_fs);
        }
    }
#endif
    
#if defined(USB_CORE_ID_HS)
    if(id == USB_CORE_ID_HS){
        if(is_on){
            HOST_PORT_POWER_ON_HS();
            tusb_host_init(host_hs, &root_hs);
            tusb_open_host(host_hs);
        }else{
            HOST_PORT_POWER_OFF_HS();
            tusb_close_host(host_hs);
        }
    }
#endif
}

void host_power(int id, int is_on)
{
    tusbh_message_t msg;
    msg.param = id;
    msg.len = is_on;
    host_power_msg(&msg);
    //POST_MESSAGE(host_mq, host_power_msg, id, 0, is_on);
}

void device_mode(int id, int is_on);

static void change_usb_mode(int id, uint8_t is_host)
{
#if defined(USB_CORE_ID_FS)
    if(id == USB_CORE_ID_FS){
        if(is_host){
            TUSB_PRINTF("Change FS to host mode\n");
            device_mode(id, 0);
            host_power(id, 1);
        }else{
            TUSB_PRINTF("Change FS to device mode\n");
            host_power(id, 0);
            device_mode(id, 1);
        }
    }
#endif
#if defined(USB_CORE_ID_HS)
    if(id == USB_CORE_ID_HS){
        if(is_host){
            TUSB_PRINTF("Change HS to host mode\n");
            device_mode(id, 0);
            host_power(id, 1);
        }else{
            TUSB_PRINTF("Change HS to device mode\n");
            host_power(id, 0);
            device_mode(id, 1);
        }
    }
#endif
}

static void usb_id_check_thread_entry(void* arg)
{
    TUSB_PRINTF("USB ID check thread runnig\n");
#define USB_DEBOUNCE_TIEM  5
  rt_uint8_t fs_host_cnt = 0;
  rt_uint8_t fs_device_cnt = 0;
  rt_uint8_t hs_host_cnt = 0;
  rt_uint8_t hs_device_cnt = 0;
  while (1){
#if defined(USB_CORE_ID_FS)
    if((GET_HOST_PORT_ID_FS()) == 0){
      fs_device_cnt = 0;
      if(fs_host_cnt<=USB_DEBOUNCE_TIEM){
        fs_host_cnt++;
        if(fs_host_cnt>USB_DEBOUNCE_TIEM){
            change_usb_mode(USB_CORE_ID_FS, 1);
        }
      }
    }else{
      fs_host_cnt = 0;
      if(GET_HOST_PORT_ID_FS()){
        if(fs_device_cnt<=USB_DEBOUNCE_TIEM){
          fs_device_cnt++;
          if(fs_device_cnt>USB_DEBOUNCE_TIEM){
            change_usb_mode(USB_CORE_ID_FS, 0);
          }
        }
      }else{
        fs_device_cnt = 0;
      }
    }
#endif
#if defined(USB_CORE_ID_HS)
    if((GET_HOST_PORT_ID_HS()) == 0){
      hs_device_cnt = 0;
      if(hs_host_cnt<=USB_DEBOUNCE_TIEM){
        hs_host_cnt++;
        if(hs_host_cnt>USB_DEBOUNCE_TIEM){
          change_usb_mode(USB_CORE_ID_HS, 1);
        }
      }
    }else{
      hs_host_cnt = 0;
      if(GET_HOST_PORT_ID_HS()){
        if(hs_device_cnt<=USB_DEBOUNCE_TIEM){
          hs_device_cnt++;
          if(hs_device_cnt>USB_DEBOUNCE_TIEM){
            change_usb_mode(USB_CORE_ID_HS, 0);
          }
        }
      }else{
        hs_device_cnt = 0;
      }
    }
#endif
    rt_thread_mdelay(50);
  }
}

static struct rt_semaphore root_sem;
static uint8_t root_mem[64*1024];
#define ROOT_BLK   512
static rt_err_t rt_root_init(rt_device_t dev)
{
    rt_sem_init(&root_sem, "root_sem", 1, RT_IPC_FLAG_FIFO);
	return RT_EOK;
}

static rt_err_t rt_root_open(rt_device_t dev, rt_uint16_t oflag){ return RT_EOK; }
static rt_err_t rt_root_close(rt_device_t dev) { return RT_EOK; }
static rt_size_t rt_root_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
    rt_sem_take(&root_sem, RT_WAITING_FOREVER);
    memcpy(buffer, root_mem + (ROOT_BLK*pos), size*ROOT_BLK);
    rt_sem_release(&root_sem);
	return size;
}
static rt_size_t rt_root_write (rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
    rt_sem_take(&root_sem, RT_WAITING_FOREVER);
    memcpy(root_mem + (ROOT_BLK*pos), buffer, size*ROOT_BLK);
    rt_sem_release(&root_sem);
    return size;
}

static rt_err_t rt_root_control(rt_device_t dev, int cmd, void *args)
{
    RT_ASSERT(dev != RT_NULL);
    if (cmd == RT_DEVICE_CTRL_BLK_GETGEOME)
    {
        struct rt_device_blk_geometry *geometry;
        geometry = (struct rt_device_blk_geometry *)args;
        if (geometry == RT_NULL) return -RT_ERROR;
        geometry->bytes_per_sector = ROOT_BLK;
        geometry->block_size = ROOT_BLK;
		geometry->sector_count = sizeof(root_mem)/ROOT_BLK;
    }
	return RT_EOK;
}

static struct rt_device root_dev = {
    .type    = RT_Device_Class_Block,
    .init 	 = rt_root_init,
    .open 	 = rt_root_open,
    .close   = rt_root_close,
    .read 	 = rt_root_read,
    .write   = rt_root_write,
    .control = rt_root_control,
};


static int host_thread_init(void)
{
    rt_thread_t tid;
    host_mq = tusbh_mq_create();
    
    rt_list_init(&msc_head);
    
    rt_err_t err = rt_device_register(&root_dev, "root_dev", RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_REMOVABLE);
    if(err == RT_EOK){
        
        if(dfs_mkfs("elm", "root_dev") == 0){        
            if(dfs_mount("root_dev", "/", "elm", 0, 0) != 0){
                rt_kprintf("Mount root dev fail!\n");
            }
        }else{
            rt_kprintf("mkfs root dev fail!\n");
        }
    }else{
        rt_kprintf("Register root dev fail!\n");
    }
    
    RT_ASSERT(host_mq != RT_NULL);
    
#if defined(USB_CORE_ID_FS)
    host_fs = tusb_get_host(USB_CORE_ID_FS);
    root_fs.mq = host_mq;
    root_fs.id = "FS";
    root_fs.support_classes = class_table;
    tusb_host_init(host_fs, &root_fs);
    HOST_PORT_POWER_OFF_FS();
    SETUP_HOST_PORT_ID_FS();
#else
    (void)root_fs;
    host_fs = RT_NULL;
#endif    

#if defined(USB_CORE_ID_HS)
    host_hs = tusb_get_host(USB_CORE_ID_HS);    
    HOST_PORT_POWER_ON_HS();
    root_hs.mq = host_mq;
    root_hs.id = "HS";
    root_hs.support_classes = class_table;
    tusb_host_init(host_hs, &root_hs);
    HOST_PORT_POWER_OFF_HS();
    SETUP_HOST_PORT_ID_HS();
#else
    (void)root_hs;
    host_hs = RT_NULL;
#endif

    tid = rt_thread_create("U_id", usb_id_check_thread_entry, RT_NULL, 1024, 25, 5);
    RT_ASSERT(tid != RT_NULL);
    rt_thread_startup(tid);
    
    tid = rt_thread_create("U_host", usb_host_thread_entry, RT_NULL, 2048, 2, 5);
    RT_ASSERT(tid != RT_NULL);
    rt_thread_startup(tid);
    
    return 0;
}

void ls_usbdev(void);

long lsusb(void)
{
  if(host_fs)ls_usb(host_fs);
  if(host_hs)ls_usb(host_hs);
    
  ls_usbdev();
  return 0;
}

MSH_CMD_EXPORT(lsusb, list usb device in system);

INIT_APP_EXPORT(host_thread_init);










