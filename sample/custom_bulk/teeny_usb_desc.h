/*
 * Name   :  teeny_usb_desc.h
 * Author :  admin@xtoolbox.org
 * Date   :  2020-08-15 11:05:38

 * Desc   :  This file is auto generate by the TeenyDT
 *           Visit http://dt.tusb.org for more info
 */

/*
  Input source name:  custom_bulk_desc.lua
  
  Content type is lua script:
  ------------- lua script begin ------------
return Device {
    strManufacturer = "TeenyUSB",
    strProduct = "TeenyUSB Custom Bulk",
    strSerial = "TUSB123456",
    idVendor = 0x0483,
    idProduct = 0x0001,
    prefix = "BULK",
    Config {
        Interface{
            WCID=WinUSB,
            GUID="{1D4B2365-4749-48EA-B38A-7C6FDDDD7E26}",
            EndPoint(IN(1),  BulkDouble, 64),
            EndPoint(OUT(2), BulkDouble, 64),
        },
   },
}

  ------------- lua script end   ------------
 */
#ifndef __BULK_TEENY_USB_DESC_H__
#define __BULK_TEENY_USB_DESC_H__

#include "teeny_usb_device.h"
extern const tusb_descriptors_t BULK_descriptors;
#endif

