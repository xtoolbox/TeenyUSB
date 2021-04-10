/*
 * Name   :  teeny_usb_desc.h
 * Author :  admin@xtoolbox.org
 * Date   :  2021-04-10 11:29:10

 * Desc   :  This file is auto generate by the TeenyDT
 *           Visit http://dt.tusb.org for more info
 */

/*
  Input source name:  winusb30_desc.lua
  
  Content type is lua script:
  ------------- lua script begin ------------
return {
Device {
    strManufacturer = "TeenyUSB",
    strProduct = "TeenyUSB WinUSB30",
    strSerial = "TUSB123456",
    idVendor = 0x0483,
    idProduct = 0x0001,
    prefix = "BULK30",
    bcdUSB = 0x300,
    Config {
        Interface{
            WCID=WinUSB,
            GUID="{1D4B2365-4749-48EA-B38A-7C6FDDDD7E26}",
            EndPoint(IN(1),  BulkDouble, 1024, 1, 4),
            EndPoint(OUT(1), BulkDouble, 1024, 1, 4),
        },
   },
},

Device {
    strManufacturer = "TeenyUSB",
    strProduct = "TeenyUSB WinUSB20",
    strSerial = "TUSB123456",
    idVendor = 0x0483,
    idProduct = 0x0002,
    prefix = "BULK20",
    bcdUSB = 0x210,
    Config {
        Interface{
            WCID=WinUSB,
            GUID="{1D4B2365-4749-48EA-B38A-7C6FDDDD7E26}",
            EndPoint(IN(1),  BulkDouble, 512),
            EndPoint(OUT(1), BulkDouble, 512),
        },
   },
}

}


  ------------- lua script end   ------------
 */
/////////////////////////////////////////
//// Device 0
/////////////////////////////////////////
#ifndef __BULK30_TEENY_USB_DESC_H__
#define __BULK30_TEENY_USB_DESC_H__

#include "teeny_usb_device.h"
extern const tusb_descriptors_t BULK30_descriptors;
#endif

/////////////////////////////////////////
//// Device 1
/////////////////////////////////////////
#ifndef __BULK20_TEENY_USB_DESC_H__
#define __BULK20_TEENY_USB_DESC_H__

#include "teeny_usb_device.h"
extern const tusb_descriptors_t BULK20_descriptors;
#endif

