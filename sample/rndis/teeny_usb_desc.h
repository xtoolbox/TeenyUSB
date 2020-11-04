/*
 * Name   :  teeny_usb_desc.h
 * Author :  admin@xtoolbox.org
 * Date   :  2020-08-20 15:05:12

 * Desc   :  This file is auto generate by the TeenyDT
 *           Visit http://dt.tusb.org for more info
 */

/*
  Input source name:  rndis_desc.lua
  
  Content type is lua script:
  ------------- lua script begin ------------
return Device {
    strManufacturer = "TeenyUSB",
    strProduct = "TeenyUSB RNDIS DEMO",
    strSerial = "TUSB123456",
    idVendor = 0x0483,
    idProduct = 0x0031,
    prefix = "RNDIS",
    Config {
        CDC_ACM{
            bInterfaceProtocol = 0xff,
            extDesc = RNDIS("5162001"),
            EndPoint(IN(2),  Interrupt, 8),
            EndPoint(IN(1), BulkDouble, 64),
            EndPoint(OUT(1),  BulkDouble, 64),
        },
   }
}

  ------------- lua script end   ------------
 */
#ifndef __RNDIS_TEENY_USB_DESC_H__
#define __RNDIS_TEENY_USB_DESC_H__

#include "teeny_usb_device.h"
extern const tusb_descriptors_t RNDIS_descriptors;
#endif

