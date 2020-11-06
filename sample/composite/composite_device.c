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

#include "teeny_usb.h"
#include "teeny_usb_desc.h"
#include "teeny_usb_util.h"
#include "string.h"
#include "tusb_platform_stm32.h"
#include "board_config.h"

#include "tusbd_user.h"
#include "tusbd_hid.h"
#include "tusbd_cdc.h"
#include "tusbd_msc.h"

#define USER_RX_EP_SIZE   32
#define CDC_RX_EP_SIZE    32
#define HID_RX_EP_SIZE    16
extern const uint8_t COMP_ReportDescriptor_if0[];
#define HID_REPORT_DESC         COMP_ReportDescriptor_if0
#define HID_REPORT_DESC_SIZE    24

// allocate more buffer for better performance
__ALIGN_BEGIN uint8_t user_buf[USER_RX_EP_SIZE*4] __ALIGN_END;

int user_recv_data(tusb_user_device_t* raw, const void* data, uint16_t len);
int user_send_done(tusb_user_device_t* raw, const void* data, uint16_t len);

tusb_user_device_t user_dev = {
  .backend = &user_device_backend,
  .ep_in = 3,
  .ep_out = 3,
  .on_recv_data = user_recv_data,
  .on_send_done = user_send_done,
  .rx_buf = user_buf,
  .rx_size = sizeof(user_buf),
};

// The HID recv buffer size must equal to the out report size
__ALIGN_BEGIN uint8_t hid_buf[HID_RX_EP_SIZE] __ALIGN_END;
int hid_recv_data(tusb_hid_device_t* hid, const void* data, uint16_t len);
int hid_send_done(tusb_hid_device_t* hid, const void* data, uint16_t len);

tusb_hid_device_t hid_dev = {
  .backend = &hid_device_backend,
  .ep_in = 2,
  .ep_out = 2,
  .on_recv_data = hid_recv_data,
  .on_send_done = hid_send_done,
  .rx_buf = hid_buf,
  .rx_size = sizeof(hid_buf),
  .report_desc = HID_REPORT_DESC,
  .report_desc_size = HID_REPORT_DESC_SIZE,
};

// The CDC recv buffer size should equal to the out endpoint size
// or we will need a timeout to flush the recv buffer
__ALIGN_BEGIN uint8_t cdc_buf[CDC_RX_EP_SIZE] __ALIGN_END;

int cdc_recv_data(tusb_cdc_device_t* cdc, const void* data, uint16_t len);
int cdc_send_done(tusb_cdc_device_t* cdc, const void* data, uint16_t len);
void cdc_line_coding_change(tusb_cdc_device_t* cdc);

tusb_cdc_device_t cdc_dev = {
  .backend = &cdc_device_backend,
  .ep_in = 1,
  .ep_out = 1,
  .ep_int = 8,
  .on_recv_data = cdc_recv_data,
  .on_send_done = cdc_send_done,
  .on_line_coding_change = cdc_line_coding_change,
  .rx_buf = cdc_buf,
  .rx_size = sizeof(cdc_buf),
};


int msc_get_cap(tusb_msc_device_t* msc, uint8_t lun, uint32_t *block_num, uint32_t *block_size);
int msc_block_read(tusb_msc_device_t* msc, uint8_t lun, uint8_t *buf, uint32_t block_addr, uint16_t block_len);
int msc_block_write(tusb_msc_device_t* msc, uint8_t lun, const uint8_t *buf, uint32_t block_addr, uint16_t block_len);

tusb_msc_device_t msc_dev = {
  .backend = &msc_device_backend,
  .ep_in = 4,
  .ep_out = 4,
  .max_lun = 0, // 1 logic unit
  .get_cap = msc_get_cap,
  .block_read = msc_block_read,
  .block_write = msc_block_write,
};

// make sure the interface order is same in "composite_desc.lua"
static tusb_device_interface_t* device_interfaces[] = {
  (tusb_device_interface_t*)&hid_dev,
  (tusb_device_interface_t*)&cdc_dev, 0,   // CDC need two interfaces
  (tusb_device_interface_t*)&user_dev,
  (tusb_device_interface_t*)&msc_dev,
};

tusb_device_config_t device_config = {
  .if_count = sizeof(device_interfaces)/sizeof(device_interfaces[0]),
  .interfaces = &device_interfaces[0],
};

void tusb_delay_ms(uint32_t ms)
{
  uint32_t i,j;
  for(i=0;i<ms;++i)
    for(j=0;j<200;++j);
}

static int user_len = 0;
int user_recv_data(tusb_user_device_t* raw, const void* data, uint16_t len)
{
  user_len = (int)len;
  return 1; // return 1 means the recv buffer is busy
}

int user_send_done(tusb_user_device_t* raw, const void* data, uint16_t len)
{
  tusb_set_rx_valid(raw->dev, raw->ep_out);
  return 0;
}

static int hid_len = 0;
int hid_recv_data(tusb_hid_device_t* hid, const void* data, uint16_t len)
{
  hid_len = (int)len;
  return 1; // return 1 means the recv buffer is busy
}

int hid_send_done(tusb_hid_device_t* hid, const void* data, uint16_t len)
{
  tusb_set_rx_valid(hid->dev, hid->ep_out);
  return 0;
}

static int cdc_len = 0;
int cdc_recv_data(tusb_cdc_device_t* cdc, const void* data, uint16_t len)
{
  cdc_len = (int)len;
  return 1; // return 1 means the recv buffer is busy
}

int cdc_send_done(tusb_cdc_device_t* cdc, const void* data, uint16_t len)
{
  tusb_set_rx_valid(cdc->dev, cdc->ep_out);
  return 0;
}

void cdc_line_coding_change(tusb_cdc_device_t* cdc)
{
    //cdc->line_coding.bitrate
    //cdc->line_coding.parity
    //cdc->line_coding.databits
    //cdc->line_coding.stopbits
    TUSB_LOGD("App Todo: set real line coding\n");
}

static tusb_device_t g_dev;
void stdio_init(void);

int main(void)
{
  stdio_init();
  TUSB_LOGD("Composite device begin\n");
  SetDescriptor(&g_dev, &COMP_descriptors);
  tusb_set_device_config(&g_dev, &device_config);
  tusb_open_device(&g_dev, TUSB_DEFAULT_DEV_PARAM);
  while(!g_dev.config){
      // wait device ready
  }
  // Device configured, ready to work
  while(1){
    if(user_len){
      for(int i=0;i<user_len;i++){
        user_buf[i]+=1;
      }
      tusb_user_device_send(&user_dev, user_buf, user_len);
      user_len = 0;
    }

    if(hid_len){
      for(int i=0;i<hid_len;i++){
        hid_buf[i]+=2;
      }
      tusb_hid_device_send(&hid_dev, hid_buf, hid_len);
      hid_len = 0;
    }

    if(cdc_len){
      for(int i=0;i<cdc_len;i++){
        cdc_buf[i]+=3;
      }
      tusb_cdc_device_send(&cdc_dev, cdc_buf, cdc_len);
      cdc_len = 0;
    }

    tusb_msc_device_loop(&msc_dev);
  }
}

#if  defined(STM32F723xx) || defined(STM32F767xx) || defined(STM32F407xx) || defined(STM32H743xx)
#define BLOCK_SIZE   512
// the stack is start at RAM end in GCC linker script, reserve the last 2 blocks
#if defined(STM32F723xx)
#define BLOCK_COUNT  ((256-64-2)*2)
#elif defined(STM32F767xx)
#define BLOCK_COUNT  ((512-64-2)*2)
#elif defined(STM32H743xx)
#define START_ADDR   (uint8_t*)(0x24000000)
#define BLOCK_COUNT  ((512-64-2)*2)
#else
#define BLOCK_COUNT  ((128-64-2)*2)
#endif

#ifndef START_ADDR
#define START_ADDR   (uint8_t*)(0x20000000ul + 64*1024ul)
#endif

int msc_get_cap(tusb_msc_device_t* msc, uint8_t lun, uint32_t *block_num, uint32_t *block_size)
{
  *block_num = BLOCK_COUNT;
  *block_size = BLOCK_SIZE;
  return 0;
}

int msc_block_read(tusb_msc_device_t* msc, uint8_t lun, uint8_t *buf, uint32_t block_addr, uint16_t block_len)
{
  uint32_t len = block_len*BLOCK_SIZE;
  uint32_t offset = block_addr*BLOCK_SIZE;
  memcpy(buf, START_ADDR + offset, len);
  return (int)len;
}

int msc_block_write(tusb_msc_device_t* msc, uint8_t lun, const uint8_t *buf, uint32_t block_addr, uint16_t block_len)
{
  uint32_t len = block_len*BLOCK_SIZE;
  uint32_t offset = block_addr*BLOCK_SIZE;
  memcpy(START_ADDR + offset, buf, len);
  return (int)len;
}
#else

#if defined(FLASH_SIZE)
#define START_ADDR   (const uint8_t*)(0x08000000ul + 20*1024ul)
#define BLOCK_SIZE FLASH_PAGE_SIZE
#define BLOCK_COUNT  ((FLASH_SIZE - 20*1024ul) / FLASH_PAGE_SIZE)
int msc_get_cap(tusb_msc_device_t* msc, uint8_t lun, uint32_t *block_num, uint32_t *block_size) {
  *block_size = BLOCK_SIZE;
  *block_num =  BLOCK_COUNT;
  return 0;
}

int msc_block_read(tusb_msc_device_t* msc, uint8_t lun, uint8_t *buf, uint32_t block_addr, uint16_t block_len)
{
  uint32_t len = block_len*BLOCK_SIZE;
  memcpy(buf, START_ADDR+block_addr*BLOCK_SIZE, len);
  return len;
}

int msc_block_write(tusb_msc_device_t* msc, uint8_t lun, const uint8_t *buf, uint32_t block_addr, uint16_t block_len)
{
  uint32_t len = block_len*BLOCK_SIZE;
  flash_write( (uint32_t)START_ADDR+block_addr*BLOCK_SIZE, buf, len);
  return len;
}

#else
#warning MSC storage not set
#endif

#endif

#if MSC_DATA_PACKET_LENGTH < BLOCK_SIZE
#error MSC data packet buffer is small than the block size
#endif
