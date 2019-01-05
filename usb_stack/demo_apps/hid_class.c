/*
 * teeny usb - light weight usb stack for STM32 micro controllers
 * 
 * Copyright (c) 2018 XToolBox  - admin@xtoolbox.org
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


#define HID_REQ_SET_PROTOCOL          0x0B
#define HID_REQ_GET_PROTOCOL          0x03

#define HID_REQ_SET_IDLE              0x0A
#define HID_REQ_GET_IDLE              0x02

#define HID_REQ_SET_REPORT            0x09
#define HID_REQ_GET_REPORT            0x01


__ALIGN_BEGIN static uint8_t hid_cmd[16] __ALIGN_END;
static uint8_t USBD_HID_IdleState;
static uint8_t USBD_HID_Protocol;
void HID_DataoutRequest(tusb_device_t* dev)
{
  uint8_t reportID = LO_BYTE(dev->setup.wValue);
  (void)reportID;
  // got output report
}

void tusb_class_request(tusb_device_t* dev, tusb_setup_packet* setup_req)
{
  if(setup_req->wLength > 0){
    if (setup_req->bmRequestType & 0x80){
    }else{
      tusb_set_recv_buffer(dev, 0, hid_cmd, setup_req->wLength);
      dev->ep0_rx_done = HID_DataoutRequest;
    }
  }else{
    switch(setup_req->bRequest){
      case HID_REQ_SET_IDLE:
        USBD_HID_IdleState = HI_BYTE(setup_req->wValue);
        break;
      case HID_REQ_GET_IDLE:
        tusb_send_data(dev, 0, &USBD_HID_IdleState, 1);
        return;
      case HID_REQ_SET_PROTOCOL:
        USBD_HID_Protocol = LO_BYTE(setup_req->wValue);
        break;
      case HID_REQ_GET_PROTOCOL:
        tusb_send_data(dev, 0, &USBD_HID_Protocol, 1);
        return;
      default:
        break;
    }
  }
  tusb_send_data(dev, 0, 0, 0);
}


