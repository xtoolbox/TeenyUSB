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
#include "string.h"
// Default function to get report descriptor
// some class need report descriptor, e.g. HID, user should override it
WEAK const uint8_t* tusb_get_report_descriptor(tusb_device_t* dev, tusb_setup_packet *req, uint16_t* len)
{
  /**
   // example code for get report descriptor
   uint16_t interfaceIndex = req->wIndex;
   switch(interfaceIndex){
     case 0: 
         *len = interface_0_report_descriptor_size;
         return interface_0_report_descriptor;
     case 1:
         *len = interface_1_report_descriptor_size;
         return interface_1_report_descriptor;
   }
   */
  *len = 0;
  return 0;
}

// Default class request handler
// User need override it if the class need some special request
WEAK void tusb_class_request(tusb_device_t* dev, tusb_setup_packet* setup_req)
{
  tusb_send_data(dev, 0, 0, 0);
}

// Standard get descriptor function
static void tusb_get_descriptor(tusb_device_t* dev, tusb_setup_packet *req)
{
  const uint8_t* desc = 0;
  uint16_t len = 0;
  switch (HI_BYTE(req->wValue)) {
    case USB_DESC_TYPE_DEVICE:
      desc = dev->descriptors->device;
      if(desc) len = desc[0];
      break;
    case USB_DESC_TYPE_CONFIGURATION:
      desc = dev->descriptors->config;
      if(desc)len = *((uint16_t*)desc + 1);
      break;
    case USB_DESC_TYPE_REPORT:
      desc = tusb_get_report_descriptor(dev, req, &len);
      break;
    case USB_DESC_TYPE_STRING:
    {
      uint8_t index = LO_BYTE(req->wValue);
      if(index < dev->descriptors->string_cnt){
        desc = dev->descriptors->strings[index];
        if(desc)len = desc[0];
      }
#if defined(HAS_WCID)
      else if(index == 0xee && dev->descriptors->wcid_desc){
        desc = WCID_StringDescriptor_MSOS;
        len = desc[0];
      }
#endif
      break;
    }
#if defined(USB_OTG_HS)
    // For High speed device, support the DEVICE_QUALIFIER 
    // and OTHER_SPEED_CONFIGURATION request
    case USB_DESC_TYPE_DEVICE_QUALIFIER:
    {
      static __ALIGN_BEGIN uint8_t device_qualifer_desc_buffer[USB_LEN_DEV_QUALIFIER_DESC];
      // just copy the device descriptor to qualifer descriptor
      device_qualifer_desc_buffer[0] = USB_LEN_DEV_QUALIFIER_DESC;
      device_qualifer_desc_buffer[1] = USB_DESC_TYPE_DEVICE_QUALIFIER;
      memcpy(device_qualifer_desc_buffer+2, dev->descriptors->device + 2, 6);
      // set the bNumConfigurations field
      device_qualifer_desc_buffer[8] = dev->descriptors->device[17];
      // set bReserved field to zero
      device_qualifer_desc_buffer[9] = 0;
      len = USB_LEN_DEV_QUALIFIER_DESC;
      desc = device_qualifer_desc_buffer;
      break;
    }
    case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
    {
      static __ALIGN_BEGIN uint8_t device_other_speed_desc_buffer[1024];
      desc = dev->descriptors->config;
      if(desc)len = *((uint16_t*)desc + 1);
      if(len <= sizeof(device_other_speed_desc_buffer)){
        // if config total length is less than the buffer, copy it to the buffer,
        // and set it type to OTHER_SPEED_CONFIGURATION
        memcpy(device_other_speed_desc_buffer, desc, len);
        device_other_speed_desc_buffer[1] = USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION;
        desc = device_other_speed_desc_buffer;
      }else{
        // otherwise return a fail status
        desc = 0;
      }
      break;
    }
#endif
  }
  if(desc){
    tusb_send_data(dev, 0, desc,
          req->wLength > len ? len : req->wLength);
  }else{
    STALL_EP0(dev);
  }
}

// Callback function for set address setup
#if defined(USB_OTG_FS) || defined(USB_OTG_HS)
static void tusb_otg_set_addr (tusb_device_t* dev)
{
  USB_OTG_GlobalTypeDef *USBx = GetUSB(dev);
  USBx_DEVICE->DCFG &= ~ (USB_OTG_DCFG_DAD);
  USBx_DEVICE->DCFG |= (dev->addr << 4) & USB_OTG_DCFG_DAD ;
}
#endif
#if defined(USB) 
static void tusb_fs_set_addr(tusb_device_t* dev)
{
  if (dev->addr){
    GetUSB(dev)->DADDR = (dev->addr | USB_DADDR_EF);
    dev->addr = 0;
  }
}
#endif

#if defined(HAS_WCID)
static void tusb_vendor_request(tusb_device_t* dev, tusb_setup_packet* setup_req)
{
  uint32_t len = 0;
  const uint8_t* desc = 0;
  switch (setup_req->bRequest) {
    case WCID_VENDOR_CODE:
      switch(setup_req->wIndex)
      case 4:
        desc = dev->descriptors->wcid_desc;
        len = desc[0] + (desc[1]<<8) + (desc[2]<<16) + (desc[3]<<24);
        break;
      case 5:
        desc = dev->descriptors->wcid_properties;
        len = desc[0] + (desc[1]<<8) + (desc[2]<<16) + (desc[3]<<24);
        break;
  }
  // TODO: Handle length more than 0xffff
  tusb_send_data(dev, 0, desc,
          setup_req->wLength > len ? len : setup_req->wLength);
}
#endif

// Standard request process function
static void tusb_standard_request(tusb_device_t* dev, tusb_setup_packet* setup_req)
{
  // TODO: need process the interface and end point standard request
  /** for example:
   switch(setup_req->bmRequestType & USB_REQ_RECIPIENT_MASK ){
    USB_REQ_RECIPIENT_DEVICE:
    // standard device request
    break;
    USB_REQ_RECIPIENT_INTERFACE:
      // standard interface request
      uint16_t interface_index  = setup_req->wIndex;
      if(setup_req->bRequest  == USB_REQ_CLEAR_FEATURE){
      }else if(setup_req->bRequest  == USB_REQ_SET_FEATURE){
      }
    break;
    USB_REQ_RECIPIENT_ENDPOINT:
      // standard end point request
      uint8_t ep_addr  = LO_BYTE(setup_req->wIndex);
      if(setup_req->bRequest  == USB_REQ_CLEAR_FEATURE){
          // ep clear feature
      }else if(setup_req->bRequest  == USB_REQ_SET_FEATURE){
          // ep set feature
      }
    break;
   }
   */
  switch (setup_req->bRequest) {
  case USB_REQ_SET_ADDRESS:
    dev->addr = LO_BYTE(setup_req->wValue);
#if defined(USB_OTG_FS) || defined(USB_OTG_HS)
    tusb_otg_set_addr(dev);
#else
    dev->ep0_tx_done = tusb_fs_set_addr;
#endif
    tusb_send_data(dev, 0, 0, 0);
    break;
  
  case USB_REQ_GET_DESCRIPTOR:
    tusb_get_descriptor(dev, setup_req);
    break;
  
  case USB_REQ_GET_STATUS:
    tusb_send_data(dev, 0, (uint16_t *) &dev->status, 2);
    break;
  
  // Only support one configuration, so just save and return the config value
  case USB_REQ_GET_CONFIGURATION:
    tusb_send_data(dev, 0, (uint16_t *) &dev->config, 1);
    break;
  
  case USB_REQ_SET_CONFIGURATION:
    dev->config = LO_BYTE(setup_req->wValue);
    tusb_send_data(dev, 0, 0, 0);
    break;
  // Only support one alt setting, so just save and return the alt value
  case USB_REQ_GET_INTERFACE:
    tusb_send_data(dev, 0, &dev->alt_cfg, 1);
    break;
  case USB_REQ_SET_INTERFACE:
    dev->alt_cfg = LO_BYTE(setup_req->wValue);
    tusb_send_data(dev, 0, 0, 0);
    break;
  case USB_REQ_SET_FEATURE:
    if(setup_req->wValue == USB_FEATURE_REMOTE_WAKEUP){
      dev->remote_wakeup = 1;
      tusb_send_data(dev, 0, 0, 0);
      break;
    }
    // otherwise fall to default
  case USB_REQ_CLEAR_FEATURE:
    if(setup_req->wValue == USB_FEATURE_REMOTE_WAKEUP){
      dev->remote_wakeup = 0;
      tusb_send_data(dev, 0, 0, 0);
      break;
    }
    // otherwise fall to default
  default:
    // Error condition, stall ep0
    STALL_EP0(dev);
    break;
  }
}

// forward declare for some internal function
void tusb_recv_data(tusb_device_t* dev, uint8_t EPn);
void tusb_send_data_done(tusb_device_t* dev, uint8_t EPn);
uint32_t tusb_read_ep0(tusb_device_t* dev, void* buf);
void tusb_read_data(tusb_device_t* dev, void* buf, uint32_t len);

// Setup packet handler
void tusb_setup_handler(tusb_device_t* dev)
{
  tusb_setup_packet *setup_req = &dev->setup;
  if( (setup_req->bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_CLASS){
    tusb_class_request(dev, setup_req);
#if defined(HAS_WCID)
  }else if((setup_req->bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_VENDOR){
    tusb_vendor_request(dev, setup_req);
#endif
  }else{
    tusb_standard_request(dev, setup_req);
  }
}

// initial the ep recv buffer
int tusb_set_recv_buffer(tusb_device_t* dev, uint8_t EPn, void* data, uint16_t len)
{
  tusb_ep_data* ep = &dev->Ep[EPn];
  ep->rx_buf = (uint8_t*)data;
  ep->rx_size = len;
  ep->rx_count = 0;
  return 0;
}

// Public callback function override by user
// default data transmit done callback
WEAK void tusb_on_tx_done(tusb_device_t* dev, uint8_t EPn)
{}

// default data received  callback, return 0 will continue receive data in end point rx buffer
// return other value will cause the OUT ep NAX, user can active the OUT ep by call tusb_set_rx_valid
WEAK int tusb_on_rx_done(tusb_device_t* dev, uint8_t EPn, const void* data, uint16_t len)
{ return 0; }

WEAK void tusb_reconfig(tusb_device_t* dev)
{
}
