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
#include "string.h"

#define HID_REQ_SET_PROTOCOL          0x0B
#define HID_REQ_GET_PROTOCOL          0x03

#define HID_REQ_SET_IDLE              0x0A
#define HID_REQ_GET_IDLE              0x02

#define HID_REQ_SET_REPORT            0x09
#define HID_REQ_GET_REPORT            0x01


__ALIGN_BEGIN static uint8_t hid_cmd[64] __ALIGN_END;
// make sure data is 32bit aligned
__ALIGN_BEGIN static uint32_t cmd_buff __ALIGN_END;
static uint8_t USBD_HID_IdleState;
static uint8_t USBD_HID_Protocol;
void HID_DataoutRequest(tusb_device_t* dev)
{
  uint8_t reportID = LO_BYTE(dev->setup.wValue);
  (void)reportID;
  // got output report
}

int tusb_class_request(tusb_device_t* dev, tusb_setup_packet* setup_req)
{
  // Get interface report descriptor
  if( (setup_req->bmRequestType & USB_REQ_RECIPIENT_MASK) == USB_REQ_RECIPIENT_INTERFACE ){
    if(setup_req->bRequest == USB_REQ_GET_DESCRIPTOR){
      if(HI_BYTE(setup_req->wValue) == USB_DESC_TYPE_REPORT){
        uint16_t ifn = setup_req->wIndex;
        if(ifn < 1){
          uint16_t len = TUSB_FS_DAP_REPORT_DESCRIPTOR_SIZE_IF0;
          const uint8_t* desc = TUSB_FS_DAP_ReportDescriptor_if0;
          len = setup_req->wLength > len ? len : setup_req->wLength;
#if defined(DESCRIPTOR_BUFFER_SIZE) && DESCRIPTOR_BUFFER_SIZE > 0
          if(dev->desc_buffer != desc){
            memcpy(dev->desc_buffer, desc, len);
            desc = dev->desc_buffer;
          }
#endif
          tusb_control_send(dev, desc, len);
          return 1;
        }
      }
    }
  }
  // handle class request
  if( (setup_req->bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_CLASS ){
    switch(setup_req->bRequest){
      case HID_REQ_SET_IDLE:
        USBD_HID_IdleState = HI_BYTE(setup_req->wValue);
        tusb_send_status(dev);
        return 1;
      case HID_REQ_GET_IDLE:
        cmd_buff = USBD_HID_IdleState;
        tusb_control_send(dev, &cmd_buff, 1);
        return 1;
      case HID_REQ_SET_PROTOCOL:
        USBD_HID_Protocol = LO_BYTE(setup_req->wValue);
        tusb_send_status(dev);
        return 1;
      case HID_REQ_GET_PROTOCOL:
        cmd_buff = USBD_HID_Protocol;
        tusb_control_send(dev,  &cmd_buff, 1);
        return 1;
      case HID_REQ_SET_REPORT:
        tusb_set_recv_buffer(dev, 0, hid_cmd, setup_req->wLength);
        dev->ep0_rx_done = HID_DataoutRequest;
        return 1;
      case HID_REQ_GET_REPORT:
      {
        uint8_t reportID = LO_BYTE(dev->setup.wValue);
        (void)reportID;
        tusb_control_send(dev,  &hid_cmd, setup_req->wLength);
        return 1;
      }
      default:
        return 0;
    }
  }
  return 0;
}


