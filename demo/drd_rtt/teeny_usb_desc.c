/*
 * Name   :  teeny_usb_desc.c
 * Author :  admin@xtoolbox.org
 * Date   :  2019-12-10 17:48:38

 * Desc   :  This file is auto generate by the TeenyDT
 *           Visit http://dt.tusb.org for more info
 */

/*
  Input source name:  drd_desc.lua
  
  Content type is lua script:
  ------------- lua script begin ------------
return {

Device {
    strManufacturer = "TeenyUSB",
    strProduct = "TeenyUSB CDC DEMO",
    strSerial = "TUSB123456",
    idVendor = 0x0483,
    idProduct = 0x0012,
    prefix = "CDC",
    Config {
        CDC_ACM{
            EndPoint(IN(2),  Interrupt, 16),
            EndPoint(IN(1), BulkDouble, 64),
            EndPoint(OUT(1),  BulkDouble, 64),
        }
   }
},

Device {
    strManufacturer = "TeenyUSB",
    strProduct = "TeenyUSB MSC DEMO",
    strSerial = "TUSB123456",
    idVendor = 0x0483,
    idProduct = 0x0013,
    prefix = "MSC",
    Config {
        Interface{
            bInterfaceClass = 0x08,        -- MSC
            bInterfaceSubClass = 0x06,     -- SCSI
            bInterfaceProtocol = 0x50,     -- BOT
            EndPoint(IN(1),  BulkDouble, 64),
            EndPoint(OUT(2), BulkDouble, 64),
        },
   }
},

Device {
    strManufacturer = "TeenyUSB",
    strProduct = "TeenyUSB Boot Keyboard DEMO",
    strSerial = "TUSB123456",
    idVendor = 0x0483,
    idProduct = 0x0014,
    prefix = "KB",
    Config {
        USB_HID{
            isBoot = true, -- same as bInterfaceSubClass = 1
            isKey = true,  -- same as bInterfaceProtocol = 1
            report = HID_BootKeyBoard(),
            EndPoint(IN(1),  Interrupt, 8),
       }
   }
},

Device {
    strManufacturer = "TeenyUSB",
    strProduct = "TeenyUSB Boot Mouse DEMO",
    strSerial = "TUSB123456",
    idVendor = 0x0483,
    idProduct = 0x0015,
    prefix = "MOUSE",
    Config {
        USB_HID{
            isBoot = true,   -- same as bInterfaceSubClass = 1
            isMouse = true,  -- same as bInterfaceProtocol = 2
            extDesc = HID_BootMouse(), -- same as report = xxx
            EndPoint(IN(1),  Interrupt, 8),
       }
   }
},

Device {
    strManufacturer = "TeenyUSB",
    strProduct = "TeenyUSB WinUSB DEMO",
    strSerial = "TUSB123456",
    idVendor = 0x0483,
    idProduct = 0x0016,
    prefix = "WINUSB",
    Config {
        Interface{
            extDesc = WinUSB("{1D4B2365-4749-48EA-B38A-7C6FDDDD7E26}"),
            strInterface = "TeenyUSB WinUSB RTT",
            EndPoint(IN(1),  BulkDouble, 64),
            EndPoint(OUT(1), BulkDouble, 64),
        },
   }
},

Device {
    strManufacturer = "TeenyUSB",
    strProduct = "TeenyUSB RNDIS DEMO",
    strSerial = "TUSB123456",
    idVendor = 0x0483,
    idProduct = 0x0017,
    prefix = "RNDIS",
    Config {
        CDC_ACM{
            bInterfaceProtocol = 0xff,
            extDesc = RNDIS("5162001"),
            EndPoint(IN(3),  Interrupt, 8),
            EndPoint(IN(1), BulkDouble, 64),
            EndPoint(OUT(1),  BulkDouble, 64),
        },
   }
}

}

  ------------- lua script end   ------------
 */
/////////////////////////////////////////
//// Device 0
/////////////////////////////////////////
#include "teeny_usb.h"

#define  CDC_DEVICE_DESCRIPTOR_SIZE  (18)
__ALIGN_BEGIN  const uint8_t CDC_DeviceDescriptor [18] __ALIGN_END = {
  ///////////////////////////////////////
  /// device descriptor
  ///////////////////////////////////////
  0x12,                                             /* bLength */
  USB_DEVICE_DESCRIPTOR_TYPE,                       /* bDescriptorType */
  0x00, 0x02,                                       /* bcdUSB */
  0xef,                                             /* bDeviceClass */
  0x02,                                             /* bDeviceSubClass */
  0x01,                                             /* bDeviceProtocol */
  0x40,                                             /* bMaxPacketSize */
  0x83, 0x04,                                       /* idVendor */
  0x12, 0x00,                                       /* idProduct */
  0x00, 0x01,                                       /* bcdDevice */
  0x01,                                             /* iManufacturer */
  0x02,                                             /* iProduct */
  0x03,                                             /* iSerial */
  0x01,                                             /* bNumConfigurations */
};
#define  CDC_CONFIG_DESCRIPTOR_SIZE  (75)
__ALIGN_BEGIN  const uint8_t CDC_ConfigDescriptor [75] __ALIGN_END = {
  ///////////////////////////////////////
  /// config descriptor
  ///////////////////////////////////////
  0x09,                                             /* bLength */
  USB_CONFIGURATION_DESCRIPTOR_TYPE,                /* bDescriptorType */
  0x4b, 0x00,                                       /* wTotalLength */
  0x02,                                             /* bNumInterfaces */
  0x01,                                             /* bConfigurationValue */
  0x00,                                             /* iConfiguration */
  0x80,                                             /* bmAttributes */
  0x64,                                             /* bMaxPower */
  
  ///////////////////////////////////////
  /// interface association descriptor
  ///////////////////////////////////////
  0x08,                                             /* bLength */
  USB_IAD_DESCRIPTOR_TYPE,                          /* bDescriptorType */
  0x00,                                             /* bFirstInterface */
  0x02,                                             /* bInterfaceCount */
  0x02,                                             /* bFunctionClass */
  0x02,                                             /* bFunctionSubClass */
  0x01,                                             /* bFunctionProtocol */
  0x00,                                             /* iFunction */
  
  ///////////////////////////////////////
  /// interface descriptor
  ///////////////////////////////////////
  0x09,                                             /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,                    /* bDescriptorType */
  0x00,                                             /* bInterfaceNumber */
  0x00,                                             /* bAlternateSetting */
  0x01,                                             /* bNumEndpoints */
  0x02,                                             /* bInterfaceClass */
  0x02,                                             /* bInterfaceSubClass */
  0x01,                                             /* bInterfaceProtocol */
  0x00,                                             /* iInterface */
  
  ///////////////////////////////////////
  /// cdc acm header descriptor
  ///////////////////////////////////////
  0x05,                                             /* bLength */
  0x24,                                             /* bDescriptorType */
  0x00,                                             /* bDescriptorSubtype */
  0x10, 0x01,                                       /* bcdCDC */
  
  ///////////////////////////////////////
  /// cdc acm call management descriptor
  ///////////////////////////////////////
  0x05,                                             /* bLength */
  0x24,                                             /* bDescriptorType */
  0x01,                                             /* bDescriptorSubtype */
  0x00,                                             /* bmCapabilities */
  0x01,                                             /* bDataInterface */
  
  ///////////////////////////////////////
  /// cdc acm descriptor
  ///////////////////////////////////////
  0x04,                                             /* bLength */
  0x24,                                             /* bDescriptorType */
  0x02,                                             /* bDescriptorSubtype */
  0x02,                                             /* bmCapabilities */
  
  ///////////////////////////////////////
  /// cdc acm union descriptor
  ///////////////////////////////////////
  0x05,                                             /* bLength */
  0x24,                                             /* bDescriptorType */
  0x06,                                             /* bDescriptorSubtype */
  0x00,                                             /* bMasterInterface */
  0x01,                                             /* bSlaveInterface0 */
  
  ///////////////////////////////////////
  /// endpoint descriptor
  ///////////////////////////////////////
  0x07,                                             /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,                     /* bDescriptorType */
  0x82,                                             /* bEndpointAddress */
  0x03,                                             /* bmAttributes */
  0x10, 0x00,                                       /* wMaxPacketSize */
  0x01,                                             /* bInterval */
  
  ///////////////////////////////////////
  /// interface descriptor
  ///////////////////////////////////////
  0x09,                                             /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,                    /* bDescriptorType */
  0x01,                                             /* bInterfaceNumber */
  0x00,                                             /* bAlternateSetting */
  0x02,                                             /* bNumEndpoints */
  0x0a,                                             /* bInterfaceClass */
  0x00,                                             /* bInterfaceSubClass */
  0x00,                                             /* bInterfaceProtocol */
  0x00,                                             /* iInterface */
  
  ///////////////////////////////////////
  /// endpoint descriptor
  ///////////////////////////////////////
  0x07,                                             /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,                     /* bDescriptorType */
  0x81,                                             /* bEndpointAddress */
  0x02,                                             /* bmAttributes */
  0x40, 0x00,                                       /* wMaxPacketSize */
  0x01,                                             /* bInterval */
  
  ///////////////////////////////////////
  /// endpoint descriptor
  ///////////////////////////////////////
  0x07,                                             /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,                     /* bDescriptorType */
  0x01,                                             /* bEndpointAddress */
  0x02,                                             /* bmAttributes */
  0x40, 0x00,                                       /* wMaxPacketSize */
  0x01,                                             /* bInterval */
};
#define  CDC_STRING_DESCRIPTOR0_STR   "\x09\x04"
#define  CDC_STRING_DESCRIPTOR0_SIZE  (4)
WEAK __ALIGN_BEGIN  const uint8_t CDC_StringDescriptor0 [4] __ALIGN_END = {
  0x04,                                         /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                   /* bDescriptorType */
  0x09, 0x04,                                   /* wLangID0 */
};
#define  CDC_STRING_DESCRIPTOR1_STR   "TeenyUSB"
#define  CDC_STRING_DESCRIPTOR1_SIZE   (18)
WEAK __ALIGN_BEGIN  const uint8_t CDC_StringDescriptor1 [18] __ALIGN_END = {
  0x12,                                             /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                       /* bDescriptorType */
  'T', 0x00,                                        /* wcChar0 */
  'e', 0x00,                                        /* wcChar1 */
  'e', 0x00,                                        /* wcChar2 */
  'n', 0x00,                                        /* wcChar3 */
  'y', 0x00,                                        /* wcChar4 */
  'U', 0x00,                                        /* wcChar5 */
  'S', 0x00,                                        /* wcChar6 */
  'B', 0x00,                                        /* wcChar7 */
};
#define  CDC_STRING_DESCRIPTOR2_STR   "TeenyUSB CDC DEMO"
#define  CDC_STRING_DESCRIPTOR2_SIZE   (36)
WEAK __ALIGN_BEGIN  const uint8_t CDC_StringDescriptor2 [36] __ALIGN_END = {
  0x24,                                             /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                       /* bDescriptorType */
  'T', 0x00,                                        /* wcChar0 */
  'e', 0x00,                                        /* wcChar1 */
  'e', 0x00,                                        /* wcChar2 */
  'n', 0x00,                                        /* wcChar3 */
  'y', 0x00,                                        /* wcChar4 */
  'U', 0x00,                                        /* wcChar5 */
  'S', 0x00,                                        /* wcChar6 */
  'B', 0x00,                                        /* wcChar7 */
  ' ', 0x00,                                        /* wcChar8 */
  'C', 0x00,                                        /* wcChar9 */
  'D', 0x00,                                        /* wcChar10 */
  'C', 0x00,                                        /* wcChar11 */
  ' ', 0x00,                                        /* wcChar12 */
  'D', 0x00,                                        /* wcChar13 */
  'E', 0x00,                                        /* wcChar14 */
  'M', 0x00,                                        /* wcChar15 */
  'O', 0x00,                                        /* wcChar16 */
};
#define  CDC_STRING_DESCRIPTOR3_STR   "TUSB123456"
#define  CDC_STRING_DESCRIPTOR3_SIZE   (22)
WEAK __ALIGN_BEGIN  const uint8_t CDC_StringDescriptor3 [22] __ALIGN_END = {
  0x16,                                             /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                       /* bDescriptorType */
  'T', 0x00,                                        /* wcChar0 */
  'U', 0x00,                                        /* wcChar1 */
  'S', 0x00,                                        /* wcChar2 */
  'B', 0x00,                                        /* wcChar3 */
  '1', 0x00,                                        /* wcChar4 */
  '2', 0x00,                                        /* wcChar5 */
  '3', 0x00,                                        /* wcChar6 */
  '4', 0x00,                                        /* wcChar7 */
  '5', 0x00,                                        /* wcChar8 */
  '6', 0x00,                                        /* wcChar9 */
};
#define CDC_STRING_COUNT    (4)
const uint8_t* const CDC_StringDescriptors[4] = {
  CDC_StringDescriptor0,
  CDC_StringDescriptor1,
  CDC_StringDescriptor2,
  CDC_StringDescriptor3,
};


CDC_TXEP_MAX_SIZE
CDC_RXEP_MAX_SIZE
//  Device descriptors
const tusb_descriptors CDC_descriptors = {
  .device = CDC_DeviceDescriptor,
  .config = CDC_ConfigDescriptor,
  .strings = CDC_StringDescriptors,
  .string_cnt = CDC_STRING_COUNT,
#if defined(HAS_WCID)
#if defined(CDC_WCID_DESCRIPTOR_SIZE)
  .wcid_desc = CDC_WCIDDescriptor,
#else
  .wcid_desc = 0,
#endif // CDC_WCID_DESCRIPTOR_SIZE)

#if defined(CDC_WCID_PROPERTIES_SIZE)
  .wcid_properties = CDC_WCIDProperties,
#else
  .wcid_properties = 0,
#endif // CDC_WCID_PROPERTIES_SIZE

#endif // HAS_WCID

#if defined(HAS_WCID_20)
#if defined(CDC_WCID_BOS_SIZE)
  .wcid_bos = CDC_WCIDBOS,
#else
  .wcid_bos = 0,  
#endif // CDC_WCID_BOS_SIZE)

#if defined(CDC_WCID_DESC_SET_SIZE)
  .wcid_desc_set = CDC_WCIDDescriptorSet,
#else
  .wcid_desc_set = 0,  
#endif // CDC_WCID_DESC_SET_SIZE


#endif // HAS_WCID_20
};
/////////////////////////////////////////
//// Device 1
/////////////////////////////////////////
#include "teeny_usb.h"

#define  MSC_DEVICE_DESCRIPTOR_SIZE  (18)
__ALIGN_BEGIN  const uint8_t MSC_DeviceDescriptor [18] __ALIGN_END = {
  ///////////////////////////////////////
  /// device descriptor
  ///////////////////////////////////////
  0x12,                                             /* bLength */
  USB_DEVICE_DESCRIPTOR_TYPE,                       /* bDescriptorType */
  0x00, 0x02,                                       /* bcdUSB */
  0x00,                                             /* bDeviceClass */
  0x00,                                             /* bDeviceSubClass */
  0x00,                                             /* bDeviceProtocol */
  0x40,                                             /* bMaxPacketSize */
  0x83, 0x04,                                       /* idVendor */
  0x13, 0x00,                                       /* idProduct */
  0x00, 0x01,                                       /* bcdDevice */
  0x01,                                             /* iManufacturer */
  0x02,                                             /* iProduct */
  0x03,                                             /* iSerial */
  0x01,                                             /* bNumConfigurations */
};
#define  MSC_CONFIG_DESCRIPTOR_SIZE  (32)
__ALIGN_BEGIN  const uint8_t MSC_ConfigDescriptor [32] __ALIGN_END = {
  ///////////////////////////////////////
  /// config descriptor
  ///////////////////////////////////////
  0x09,                                             /* bLength */
  USB_CONFIGURATION_DESCRIPTOR_TYPE,                /* bDescriptorType */
  0x20, 0x00,                                       /* wTotalLength */
  0x01,                                             /* bNumInterfaces */
  0x01,                                             /* bConfigurationValue */
  0x00,                                             /* iConfiguration */
  0x80,                                             /* bmAttributes */
  0x64,                                             /* bMaxPower */
  
  ///////////////////////////////////////
  /// interface descriptor
  ///////////////////////////////////////
  0x09,                                             /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,                    /* bDescriptorType */
  0x00,                                             /* bInterfaceNumber */
  0x00,                                             /* bAlternateSetting */
  0x02,                                             /* bNumEndpoints */
  0x08,                                             /* bInterfaceClass */
  0x06,                                             /* bInterfaceSubClass */
  0x50,                                             /* bInterfaceProtocol */
  0x00,                                             /* iInterface */
  
  ///////////////////////////////////////
  /// endpoint descriptor
  ///////////////////////////////////////
  0x07,                                             /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,                     /* bDescriptorType */
  0x81,                                             /* bEndpointAddress */
  0x02,                                             /* bmAttributes */
  0x40, 0x00,                                       /* wMaxPacketSize */
  0x01,                                             /* bInterval */
  
  ///////////////////////////////////////
  /// endpoint descriptor
  ///////////////////////////////////////
  0x07,                                             /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,                     /* bDescriptorType */
  0x02,                                             /* bEndpointAddress */
  0x02,                                             /* bmAttributes */
  0x40, 0x00,                                       /* wMaxPacketSize */
  0x01,                                             /* bInterval */
};
#define  MSC_STRING_DESCRIPTOR0_STR   "\x09\x04"
#define  MSC_STRING_DESCRIPTOR0_SIZE  (4)
WEAK __ALIGN_BEGIN  const uint8_t MSC_StringDescriptor0 [4] __ALIGN_END = {
  0x04,                                         /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                   /* bDescriptorType */
  0x09, 0x04,                                   /* wLangID0 */
};
#define  MSC_STRING_DESCRIPTOR1_STR   "TeenyUSB"
#define  MSC_STRING_DESCRIPTOR1_SIZE   (18)
WEAK __ALIGN_BEGIN  const uint8_t MSC_StringDescriptor1 [18] __ALIGN_END = {
  0x12,                                             /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                       /* bDescriptorType */
  'T', 0x00,                                        /* wcChar0 */
  'e', 0x00,                                        /* wcChar1 */
  'e', 0x00,                                        /* wcChar2 */
  'n', 0x00,                                        /* wcChar3 */
  'y', 0x00,                                        /* wcChar4 */
  'U', 0x00,                                        /* wcChar5 */
  'S', 0x00,                                        /* wcChar6 */
  'B', 0x00,                                        /* wcChar7 */
};
#define  MSC_STRING_DESCRIPTOR2_STR   "TeenyUSB MSC DEMO"
#define  MSC_STRING_DESCRIPTOR2_SIZE   (36)
WEAK __ALIGN_BEGIN  const uint8_t MSC_StringDescriptor2 [36] __ALIGN_END = {
  0x24,                                             /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                       /* bDescriptorType */
  'T', 0x00,                                        /* wcChar0 */
  'e', 0x00,                                        /* wcChar1 */
  'e', 0x00,                                        /* wcChar2 */
  'n', 0x00,                                        /* wcChar3 */
  'y', 0x00,                                        /* wcChar4 */
  'U', 0x00,                                        /* wcChar5 */
  'S', 0x00,                                        /* wcChar6 */
  'B', 0x00,                                        /* wcChar7 */
  ' ', 0x00,                                        /* wcChar8 */
  'M', 0x00,                                        /* wcChar9 */
  'S', 0x00,                                        /* wcChar10 */
  'C', 0x00,                                        /* wcChar11 */
  ' ', 0x00,                                        /* wcChar12 */
  'D', 0x00,                                        /* wcChar13 */
  'E', 0x00,                                        /* wcChar14 */
  'M', 0x00,                                        /* wcChar15 */
  'O', 0x00,                                        /* wcChar16 */
};
#define  MSC_STRING_DESCRIPTOR3_STR   "TUSB123456"
#define  MSC_STRING_DESCRIPTOR3_SIZE   (22)
WEAK __ALIGN_BEGIN  const uint8_t MSC_StringDescriptor3 [22] __ALIGN_END = {
  0x16,                                             /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                       /* bDescriptorType */
  'T', 0x00,                                        /* wcChar0 */
  'U', 0x00,                                        /* wcChar1 */
  'S', 0x00,                                        /* wcChar2 */
  'B', 0x00,                                        /* wcChar3 */
  '1', 0x00,                                        /* wcChar4 */
  '2', 0x00,                                        /* wcChar5 */
  '3', 0x00,                                        /* wcChar6 */
  '4', 0x00,                                        /* wcChar7 */
  '5', 0x00,                                        /* wcChar8 */
  '6', 0x00,                                        /* wcChar9 */
};
#define MSC_STRING_COUNT    (4)
const uint8_t* const MSC_StringDescriptors[4] = {
  MSC_StringDescriptor0,
  MSC_StringDescriptor1,
  MSC_StringDescriptor2,
  MSC_StringDescriptor3,
};


MSC_TXEP_MAX_SIZE
MSC_RXEP_MAX_SIZE
//  Device descriptors
const tusb_descriptors MSC_descriptors = {
  .device = MSC_DeviceDescriptor,
  .config = MSC_ConfigDescriptor,
  .strings = MSC_StringDescriptors,
  .string_cnt = MSC_STRING_COUNT,
#if defined(HAS_WCID)
#if defined(MSC_WCID_DESCRIPTOR_SIZE)
  .wcid_desc = MSC_WCIDDescriptor,
#else
  .wcid_desc = 0,
#endif // MSC_WCID_DESCRIPTOR_SIZE)

#if defined(MSC_WCID_PROPERTIES_SIZE)
  .wcid_properties = MSC_WCIDProperties,
#else
  .wcid_properties = 0,
#endif // MSC_WCID_PROPERTIES_SIZE

#endif // HAS_WCID

#if defined(HAS_WCID_20)
#if defined(MSC_WCID_BOS_SIZE)
  .wcid_bos = MSC_WCIDBOS,
#else
  .wcid_bos = 0,  
#endif // MSC_WCID_BOS_SIZE)

#if defined(MSC_WCID_DESC_SET_SIZE)
  .wcid_desc_set = MSC_WCIDDescriptorSet,
#else
  .wcid_desc_set = 0,  
#endif // MSC_WCID_DESC_SET_SIZE


#endif // HAS_WCID_20
};
/////////////////////////////////////////
//// Device 2
/////////////////////////////////////////
#include "teeny_usb.h"

#define  KB_DEVICE_DESCRIPTOR_SIZE  (18)
__ALIGN_BEGIN  const uint8_t KB_DeviceDescriptor [18] __ALIGN_END = {
  ///////////////////////////////////////
  /// device descriptor
  ///////////////////////////////////////
  0x12,                                             /* bLength */
  USB_DEVICE_DESCRIPTOR_TYPE,                       /* bDescriptorType */
  0x00, 0x02,                                       /* bcdUSB */
  0x00,                                             /* bDeviceClass */
  0x00,                                             /* bDeviceSubClass */
  0x00,                                             /* bDeviceProtocol */
  0x40,                                             /* bMaxPacketSize */
  0x83, 0x04,                                       /* idVendor */
  0x14, 0x00,                                       /* idProduct */
  0x00, 0x01,                                       /* bcdDevice */
  0x01,                                             /* iManufacturer */
  0x02,                                             /* iProduct */
  0x03,                                             /* iSerial */
  0x01,                                             /* bNumConfigurations */
};
#define  KB_CONFIG_DESCRIPTOR_SIZE  (34)
__ALIGN_BEGIN  const uint8_t KB_ConfigDescriptor [34] __ALIGN_END = {
  ///////////////////////////////////////
  /// config descriptor
  ///////////////////////////////////////
  0x09,                                             /* bLength */
  USB_CONFIGURATION_DESCRIPTOR_TYPE,                /* bDescriptorType */
  0x22, 0x00,                                       /* wTotalLength */
  0x01,                                             /* bNumInterfaces */
  0x01,                                             /* bConfigurationValue */
  0x00,                                             /* iConfiguration */
  0x80,                                             /* bmAttributes */
  0x64,                                             /* bMaxPower */
  
  ///////////////////////////////////////
  /// interface descriptor
  ///////////////////////////////////////
  0x09,                                             /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,                    /* bDescriptorType */
  0x00,                                             /* bInterfaceNumber */
  0x00,                                             /* bAlternateSetting */
  0x01,                                             /* bNumEndpoints */
  0x03,                                             /* bInterfaceClass */
  0x01,                                             /* bInterfaceSubClass */
  0x01,                                             /* bInterfaceProtocol */
  0x00,                                             /* iInterface */
  
  ///////////////////////////////////////
  /// hid descriptor
  ///////////////////////////////////////
  0x09,                                             /* bLength */
  0x21,                                             /* bDescriptorType */
  0x11, 0x01,                                       /* bcdHID */
  0x00,                                             /* bCountryCode */
  0x01,                                             /* bNumDescriptors */
  0x22,                                             /* bDescriptorType1 */
  0x3f, 0x00,                                       /* wDescriptorLength1 */
  
  ///////////////////////////////////////
  /// endpoint descriptor
  ///////////////////////////////////////
  0x07,                                             /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,                     /* bDescriptorType */
  0x81,                                             /* bEndpointAddress */
  0x03,                                             /* bmAttributes */
  0x08, 0x00,                                       /* wMaxPacketSize */
  0x01,                                             /* bInterval */
};
#define  KB_STRING_DESCRIPTOR0_STR   "\x09\x04"
#define  KB_STRING_DESCRIPTOR0_SIZE  (4)
WEAK __ALIGN_BEGIN  const uint8_t KB_StringDescriptor0 [4] __ALIGN_END = {
  0x04,                                         /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                   /* bDescriptorType */
  0x09, 0x04,                                   /* wLangID0 */
};
#define  KB_STRING_DESCRIPTOR1_STR   "TeenyUSB"
#define  KB_STRING_DESCRIPTOR1_SIZE   (18)
WEAK __ALIGN_BEGIN  const uint8_t KB_StringDescriptor1 [18] __ALIGN_END = {
  0x12,                                             /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                       /* bDescriptorType */
  'T', 0x00,                                        /* wcChar0 */
  'e', 0x00,                                        /* wcChar1 */
  'e', 0x00,                                        /* wcChar2 */
  'n', 0x00,                                        /* wcChar3 */
  'y', 0x00,                                        /* wcChar4 */
  'U', 0x00,                                        /* wcChar5 */
  'S', 0x00,                                        /* wcChar6 */
  'B', 0x00,                                        /* wcChar7 */
};
#define  KB_STRING_DESCRIPTOR2_STR   "TeenyUSB Boot Keyboard DEMO"
#define  KB_STRING_DESCRIPTOR2_SIZE   (56)
WEAK __ALIGN_BEGIN  const uint8_t KB_StringDescriptor2 [56] __ALIGN_END = {
  0x38,                                             /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                       /* bDescriptorType */
  'T', 0x00,                                        /* wcChar0 */
  'e', 0x00,                                        /* wcChar1 */
  'e', 0x00,                                        /* wcChar2 */
  'n', 0x00,                                        /* wcChar3 */
  'y', 0x00,                                        /* wcChar4 */
  'U', 0x00,                                        /* wcChar5 */
  'S', 0x00,                                        /* wcChar6 */
  'B', 0x00,                                        /* wcChar7 */
  ' ', 0x00,                                        /* wcChar8 */
  'B', 0x00,                                        /* wcChar9 */
  'o', 0x00,                                        /* wcChar10 */
  'o', 0x00,                                        /* wcChar11 */
  't', 0x00,                                        /* wcChar12 */
  ' ', 0x00,                                        /* wcChar13 */
  'K', 0x00,                                        /* wcChar14 */
  'e', 0x00,                                        /* wcChar15 */
  'y', 0x00,                                        /* wcChar16 */
  'b', 0x00,                                        /* wcChar17 */
  'o', 0x00,                                        /* wcChar18 */
  'a', 0x00,                                        /* wcChar19 */
  'r', 0x00,                                        /* wcChar20 */
  'd', 0x00,                                        /* wcChar21 */
  ' ', 0x00,                                        /* wcChar22 */
  'D', 0x00,                                        /* wcChar23 */
  'E', 0x00,                                        /* wcChar24 */
  'M', 0x00,                                        /* wcChar25 */
  'O', 0x00,                                        /* wcChar26 */
};
#define  KB_STRING_DESCRIPTOR3_STR   "TUSB123456"
#define  KB_STRING_DESCRIPTOR3_SIZE   (22)
WEAK __ALIGN_BEGIN  const uint8_t KB_StringDescriptor3 [22] __ALIGN_END = {
  0x16,                                             /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                       /* bDescriptorType */
  'T', 0x00,                                        /* wcChar0 */
  'U', 0x00,                                        /* wcChar1 */
  'S', 0x00,                                        /* wcChar2 */
  'B', 0x00,                                        /* wcChar3 */
  '1', 0x00,                                        /* wcChar4 */
  '2', 0x00,                                        /* wcChar5 */
  '3', 0x00,                                        /* wcChar6 */
  '4', 0x00,                                        /* wcChar7 */
  '5', 0x00,                                        /* wcChar8 */
  '6', 0x00,                                        /* wcChar9 */
};
#define KB_STRING_COUNT    (4)
const uint8_t* const KB_StringDescriptors[4] = {
  KB_StringDescriptor0,
  KB_StringDescriptor1,
  KB_StringDescriptor2,
  KB_StringDescriptor3,
};
#define KB_REPORT_DESCRIPTOR_SIZE_IF0  (63)
WEAK __ALIGN_BEGIN const uint8_t KB_ReportDescriptor_if0[KB_REPORT_DESCRIPTOR_SIZE_IF0] __ALIGN_END = {
        //  Boot keyboad report descriptor
        0x05, 0x01,     // USAGE_PAGE (Generic Desktop)
        0x09, 0x06,     // USAGE (Keyboard)
        0xa1, 0x01,     // COLLECTION (Application)
        0x05, 0x07,     //   USAGE_PAGE (Keyboard)
        0x19, 0xe0,     //   USAGE_MINIMUM (Keyboard LeftControl)
        0x29, 0xe7,     //   USAGE_MAXIMUM (Keyboard Right GUI)
        0x15, 0x00,     //   LOGICAL_MINIMUM (0)
        0x25, 0x01,     //   LOGICAL_MAXIMUM (1)
        0x75, 0x01,     //   REPORT_SIZE (1)
        0x95, 0x08,     //   REPORT_COUNT (8)
        0x81, 0x02,     //   INPUT (Data,Var,Abs)
        0x95, 0x01,     //   REPORT_COUNT (1)
        0x75, 0x08,     //   REPORT_SIZE (8)
        0x81, 0x03,     //   INPUT (Cnst,Var,Abs)
        0x95, 0x05,     //   REPORT_COUNT (5)
        0x75, 0x01,     //   REPORT_SIZE (1)
        0x05, 0x08,     //   USAGE_PAGE (LEDs)
        0x19, 0x01,     //   USAGE_MINIMUM (Num Lock)
        0x29, 0x05,     //   USAGE_MAXIMUM (Kana)
        0x91, 0x02,     //   OUTPUT (Data,Var,Abs)
        0x95, 0x01,     //   REPORT_COUNT (1)
        0x75, 0x03,     //   REPORT_SIZE (3)
        0x91, 0x03,     //   OUTPUT (Cnst,Var,Abs)
        0x95, 0x06,     //   REPORT_COUNT (6)
        0x75, 0x08,     //   REPORT_SIZE (8)
        0x15, 0x00,     //   LOGICAL_MINIMUM (0)
        0x25, 0x65,     //   LOGICAL_MAXIMUM (101)
        0x05, 0x07,     //   USAGE_PAGE (Keyboard)
        0x19, 0x00,     //   USAGE_MINIMUM (Reserved (no event indicated))
        0x29, 0x65,     //   USAGE_MAXIMUM (Keyboard Application)
        0x81, 0x00,     //   INPUT (Data,Ary,Abs)
        0xc0            // END_COLLECTION
    };



KB_TXEP_MAX_SIZE
KB_RXEP_MAX_SIZE
//  Device descriptors
const tusb_descriptors KB_descriptors = {
  .device = KB_DeviceDescriptor,
  .config = KB_ConfigDescriptor,
  .strings = KB_StringDescriptors,
  .string_cnt = KB_STRING_COUNT,
#if defined(HAS_WCID)
#if defined(KB_WCID_DESCRIPTOR_SIZE)
  .wcid_desc = KB_WCIDDescriptor,
#else
  .wcid_desc = 0,
#endif // KB_WCID_DESCRIPTOR_SIZE)

#if defined(KB_WCID_PROPERTIES_SIZE)
  .wcid_properties = KB_WCIDProperties,
#else
  .wcid_properties = 0,
#endif // KB_WCID_PROPERTIES_SIZE

#endif // HAS_WCID

#if defined(HAS_WCID_20)
#if defined(KB_WCID_BOS_SIZE)
  .wcid_bos = KB_WCIDBOS,
#else
  .wcid_bos = 0,  
#endif // KB_WCID_BOS_SIZE)

#if defined(KB_WCID_DESC_SET_SIZE)
  .wcid_desc_set = KB_WCIDDescriptorSet,
#else
  .wcid_desc_set = 0,  
#endif // KB_WCID_DESC_SET_SIZE


#endif // HAS_WCID_20
};
/////////////////////////////////////////
//// Device 3
/////////////////////////////////////////
#include "teeny_usb.h"

#define  MOUSE_DEVICE_DESCRIPTOR_SIZE  (18)
__ALIGN_BEGIN  const uint8_t MOUSE_DeviceDescriptor [18] __ALIGN_END = {
  ///////////////////////////////////////
  /// device descriptor
  ///////////////////////////////////////
  0x12,                                             /* bLength */
  USB_DEVICE_DESCRIPTOR_TYPE,                       /* bDescriptorType */
  0x00, 0x02,                                       /* bcdUSB */
  0x00,                                             /* bDeviceClass */
  0x00,                                             /* bDeviceSubClass */
  0x00,                                             /* bDeviceProtocol */
  0x40,                                             /* bMaxPacketSize */
  0x83, 0x04,                                       /* idVendor */
  0x15, 0x00,                                       /* idProduct */
  0x00, 0x01,                                       /* bcdDevice */
  0x01,                                             /* iManufacturer */
  0x02,                                             /* iProduct */
  0x03,                                             /* iSerial */
  0x01,                                             /* bNumConfigurations */
};
#define  MOUSE_CONFIG_DESCRIPTOR_SIZE  (34)
__ALIGN_BEGIN  const uint8_t MOUSE_ConfigDescriptor [34] __ALIGN_END = {
  ///////////////////////////////////////
  /// config descriptor
  ///////////////////////////////////////
  0x09,                                             /* bLength */
  USB_CONFIGURATION_DESCRIPTOR_TYPE,                /* bDescriptorType */
  0x22, 0x00,                                       /* wTotalLength */
  0x01,                                             /* bNumInterfaces */
  0x01,                                             /* bConfigurationValue */
  0x00,                                             /* iConfiguration */
  0x80,                                             /* bmAttributes */
  0x64,                                             /* bMaxPower */
  
  ///////////////////////////////////////
  /// interface descriptor
  ///////////////////////////////////////
  0x09,                                             /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,                    /* bDescriptorType */
  0x00,                                             /* bInterfaceNumber */
  0x00,                                             /* bAlternateSetting */
  0x01,                                             /* bNumEndpoints */
  0x03,                                             /* bInterfaceClass */
  0x01,                                             /* bInterfaceSubClass */
  0x02,                                             /* bInterfaceProtocol */
  0x00,                                             /* iInterface */
  
  ///////////////////////////////////////
  /// hid descriptor
  ///////////////////////////////////////
  0x09,                                             /* bLength */
  0x21,                                             /* bDescriptorType */
  0x11, 0x01,                                       /* bcdHID */
  0x00,                                             /* bCountryCode */
  0x01,                                             /* bNumDescriptors */
  0x22,                                             /* bDescriptorType1 */
  0x32, 0x00,                                       /* wDescriptorLength1 */
  
  ///////////////////////////////////////
  /// endpoint descriptor
  ///////////////////////////////////////
  0x07,                                             /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,                     /* bDescriptorType */
  0x81,                                             /* bEndpointAddress */
  0x03,                                             /* bmAttributes */
  0x08, 0x00,                                       /* wMaxPacketSize */
  0x01,                                             /* bInterval */
};
#define  MOUSE_STRING_DESCRIPTOR0_STR   "\x09\x04"
#define  MOUSE_STRING_DESCRIPTOR0_SIZE  (4)
WEAK __ALIGN_BEGIN  const uint8_t MOUSE_StringDescriptor0 [4] __ALIGN_END = {
  0x04,                                         /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                   /* bDescriptorType */
  0x09, 0x04,                                   /* wLangID0 */
};
#define  MOUSE_STRING_DESCRIPTOR1_STR   "TeenyUSB"
#define  MOUSE_STRING_DESCRIPTOR1_SIZE   (18)
WEAK __ALIGN_BEGIN  const uint8_t MOUSE_StringDescriptor1 [18] __ALIGN_END = {
  0x12,                                             /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                       /* bDescriptorType */
  'T', 0x00,                                        /* wcChar0 */
  'e', 0x00,                                        /* wcChar1 */
  'e', 0x00,                                        /* wcChar2 */
  'n', 0x00,                                        /* wcChar3 */
  'y', 0x00,                                        /* wcChar4 */
  'U', 0x00,                                        /* wcChar5 */
  'S', 0x00,                                        /* wcChar6 */
  'B', 0x00,                                        /* wcChar7 */
};
#define  MOUSE_STRING_DESCRIPTOR2_STR   "TeenyUSB Boot Mouse DEMO"
#define  MOUSE_STRING_DESCRIPTOR2_SIZE   (50)
WEAK __ALIGN_BEGIN  const uint8_t MOUSE_StringDescriptor2 [50] __ALIGN_END = {
  0x32,                                             /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                       /* bDescriptorType */
  'T', 0x00,                                        /* wcChar0 */
  'e', 0x00,                                        /* wcChar1 */
  'e', 0x00,                                        /* wcChar2 */
  'n', 0x00,                                        /* wcChar3 */
  'y', 0x00,                                        /* wcChar4 */
  'U', 0x00,                                        /* wcChar5 */
  'S', 0x00,                                        /* wcChar6 */
  'B', 0x00,                                        /* wcChar7 */
  ' ', 0x00,                                        /* wcChar8 */
  'B', 0x00,                                        /* wcChar9 */
  'o', 0x00,                                        /* wcChar10 */
  'o', 0x00,                                        /* wcChar11 */
  't', 0x00,                                        /* wcChar12 */
  ' ', 0x00,                                        /* wcChar13 */
  'M', 0x00,                                        /* wcChar14 */
  'o', 0x00,                                        /* wcChar15 */
  'u', 0x00,                                        /* wcChar16 */
  's', 0x00,                                        /* wcChar17 */
  'e', 0x00,                                        /* wcChar18 */
  ' ', 0x00,                                        /* wcChar19 */
  'D', 0x00,                                        /* wcChar20 */
  'E', 0x00,                                        /* wcChar21 */
  'M', 0x00,                                        /* wcChar22 */
  'O', 0x00,                                        /* wcChar23 */
};
#define  MOUSE_STRING_DESCRIPTOR3_STR   "TUSB123456"
#define  MOUSE_STRING_DESCRIPTOR3_SIZE   (22)
WEAK __ALIGN_BEGIN  const uint8_t MOUSE_StringDescriptor3 [22] __ALIGN_END = {
  0x16,                                             /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                       /* bDescriptorType */
  'T', 0x00,                                        /* wcChar0 */
  'U', 0x00,                                        /* wcChar1 */
  'S', 0x00,                                        /* wcChar2 */
  'B', 0x00,                                        /* wcChar3 */
  '1', 0x00,                                        /* wcChar4 */
  '2', 0x00,                                        /* wcChar5 */
  '3', 0x00,                                        /* wcChar6 */
  '4', 0x00,                                        /* wcChar7 */
  '5', 0x00,                                        /* wcChar8 */
  '6', 0x00,                                        /* wcChar9 */
};
#define MOUSE_STRING_COUNT    (4)
const uint8_t* const MOUSE_StringDescriptors[4] = {
  MOUSE_StringDescriptor0,
  MOUSE_StringDescriptor1,
  MOUSE_StringDescriptor2,
  MOUSE_StringDescriptor3,
};
#define MOUSE_REPORT_DESCRIPTOR_SIZE_IF0  (50)
WEAK __ALIGN_BEGIN const uint8_t MOUSE_ReportDescriptor_if0[MOUSE_REPORT_DESCRIPTOR_SIZE_IF0] __ALIGN_END = {
        //  Boot Mouse report descriptor
        0x05, 0x01,     // USAGE_PAGE (Generic Desktop)
        0x09, 0x02,     // USAGE (Mouse)
        0xa1, 0x01,     // COLLECTION (Application)
        0x09, 0x01,     //   USAGE (Pointer)
        0xa1, 0x00,     //   COLLECTION (Physical)
        0x05, 0x09,     //     USAGE_PAGE (Button)
        0x19, 0x01,     //     USAGE_MINIMUM (Button 1)
        0x29, 0x03,     //     USAGE_MAXIMUM (Button 3)
        0x15, 0x00,     //     LOGICAL_MINIMUM (0)
        0x25, 0x01,     //     LOGICAL_MAXIMUM (1)
        0x95, 0x03,     //     REPORT_COUNT (3)
        0x75, 0x01,     //     REPORT_SIZE (1)
        0x81, 0x02,     //     INPUT (Data,Var,Abs)
        0x95, 0x01,     //     REPORT_COUNT (1)
        0x75, 0x05,     //     REPORT_SIZE (5)
        0x81, 0x03,     //     INPUT (Cnst,Var,Abs)
        0x05, 0x01,     //     USAGE_PAGE (Generic Desktop)
        0x09, 0x30,     //     USAGE (X)
        0x09, 0x31,     //     USAGE (Y)
        0x15, 0x81,     //     LOGICAL_MINIMUM (-127)
        0x25, 0x7f,     //     LOGICAL_MAXIMUM (127)
        0x75, 0x08,     //     REPORT_SIZE (8)
        0x95, 0x02,     //     REPORT_COUNT (2)
        0x81, 0x06,     //     INPUT (Data,Var,Rel)
        0xc0,           //   END_COLLECTION
        0xc0            // END_COLLECTION
    };



MOUSE_TXEP_MAX_SIZE
MOUSE_RXEP_MAX_SIZE
//  Device descriptors
const tusb_descriptors MOUSE_descriptors = {
  .device = MOUSE_DeviceDescriptor,
  .config = MOUSE_ConfigDescriptor,
  .strings = MOUSE_StringDescriptors,
  .string_cnt = MOUSE_STRING_COUNT,
#if defined(HAS_WCID)
#if defined(MOUSE_WCID_DESCRIPTOR_SIZE)
  .wcid_desc = MOUSE_WCIDDescriptor,
#else
  .wcid_desc = 0,
#endif // MOUSE_WCID_DESCRIPTOR_SIZE)

#if defined(MOUSE_WCID_PROPERTIES_SIZE)
  .wcid_properties = MOUSE_WCIDProperties,
#else
  .wcid_properties = 0,
#endif // MOUSE_WCID_PROPERTIES_SIZE

#endif // HAS_WCID

#if defined(HAS_WCID_20)
#if defined(MOUSE_WCID_BOS_SIZE)
  .wcid_bos = MOUSE_WCIDBOS,
#else
  .wcid_bos = 0,  
#endif // MOUSE_WCID_BOS_SIZE)

#if defined(MOUSE_WCID_DESC_SET_SIZE)
  .wcid_desc_set = MOUSE_WCIDDescriptorSet,
#else
  .wcid_desc_set = 0,  
#endif // MOUSE_WCID_DESC_SET_SIZE


#endif // HAS_WCID_20
};
/////////////////////////////////////////
//// Device 4
/////////////////////////////////////////
#include "teeny_usb.h"

#define  WINUSB_DEVICE_DESCRIPTOR_SIZE  (18)
__ALIGN_BEGIN  const uint8_t WINUSB_DeviceDescriptor [18] __ALIGN_END = {
  ///////////////////////////////////////
  /// device descriptor
  ///////////////////////////////////////
  0x12,                                             /* bLength */
  USB_DEVICE_DESCRIPTOR_TYPE,                       /* bDescriptorType */
  0x00, 0x02,                                       /* bcdUSB */
  0x00,                                             /* bDeviceClass */
  0x00,                                             /* bDeviceSubClass */
  0x00,                                             /* bDeviceProtocol */
  0x40,                                             /* bMaxPacketSize */
  0x83, 0x04,                                       /* idVendor */
  0x16, 0x00,                                       /* idProduct */
  0x00, 0x01,                                       /* bcdDevice */
  0x01,                                             /* iManufacturer */
  0x02,                                             /* iProduct */
  0x03,                                             /* iSerial */
  0x01,                                             /* bNumConfigurations */
};
#define  WINUSB_CONFIG_DESCRIPTOR_SIZE  (32)
__ALIGN_BEGIN  const uint8_t WINUSB_ConfigDescriptor [32] __ALIGN_END = {
  ///////////////////////////////////////
  /// config descriptor
  ///////////////////////////////////////
  0x09,                                             /* bLength */
  USB_CONFIGURATION_DESCRIPTOR_TYPE,                /* bDescriptorType */
  0x20, 0x00,                                       /* wTotalLength */
  0x01,                                             /* bNumInterfaces */
  0x01,                                             /* bConfigurationValue */
  0x00,                                             /* iConfiguration */
  0x80,                                             /* bmAttributes */
  0x64,                                             /* bMaxPower */
  
  ///////////////////////////////////////
  /// interface descriptor
  ///////////////////////////////////////
  0x09,                                             /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,                    /* bDescriptorType */
  0x00,                                             /* bInterfaceNumber */
  0x00,                                             /* bAlternateSetting */
  0x02,                                             /* bNumEndpoints */
  0xff,                                             /* bInterfaceClass */
  0xff,                                             /* bInterfaceSubClass */
  0x00,                                             /* bInterfaceProtocol */
  0x04,                                             /* iInterface */
  
  ///////////////////////////////////////
  /// endpoint descriptor
  ///////////////////////////////////////
  0x07,                                             /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,                     /* bDescriptorType */
  0x81,                                             /* bEndpointAddress */
  0x02,                                             /* bmAttributes */
  0x40, 0x00,                                       /* wMaxPacketSize */
  0x01,                                             /* bInterval */
  
  ///////////////////////////////////////
  /// endpoint descriptor
  ///////////////////////////////////////
  0x07,                                             /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,                     /* bDescriptorType */
  0x01,                                             /* bEndpointAddress */
  0x02,                                             /* bmAttributes */
  0x40, 0x00,                                       /* wMaxPacketSize */
  0x01,                                             /* bInterval */
};
#define  WINUSB_STRING_DESCRIPTOR0_STR   "\x09\x04"
#define  WINUSB_STRING_DESCRIPTOR0_SIZE  (4)
WEAK __ALIGN_BEGIN  const uint8_t WINUSB_StringDescriptor0 [4] __ALIGN_END = {
  0x04,                                         /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                   /* bDescriptorType */
  0x09, 0x04,                                   /* wLangID0 */
};
#define  WINUSB_STRING_DESCRIPTOR1_STR   "TeenyUSB"
#define  WINUSB_STRING_DESCRIPTOR1_SIZE   (18)
WEAK __ALIGN_BEGIN  const uint8_t WINUSB_StringDescriptor1 [18] __ALIGN_END = {
  0x12,                                             /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                       /* bDescriptorType */
  'T', 0x00,                                        /* wcChar0 */
  'e', 0x00,                                        /* wcChar1 */
  'e', 0x00,                                        /* wcChar2 */
  'n', 0x00,                                        /* wcChar3 */
  'y', 0x00,                                        /* wcChar4 */
  'U', 0x00,                                        /* wcChar5 */
  'S', 0x00,                                        /* wcChar6 */
  'B', 0x00,                                        /* wcChar7 */
};
#define  WINUSB_STRING_DESCRIPTOR2_STR   "TeenyUSB WinUSB DEMO"
#define  WINUSB_STRING_DESCRIPTOR2_SIZE   (42)
WEAK __ALIGN_BEGIN  const uint8_t WINUSB_StringDescriptor2 [42] __ALIGN_END = {
  0x2a,                                             /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                       /* bDescriptorType */
  'T', 0x00,                                        /* wcChar0 */
  'e', 0x00,                                        /* wcChar1 */
  'e', 0x00,                                        /* wcChar2 */
  'n', 0x00,                                        /* wcChar3 */
  'y', 0x00,                                        /* wcChar4 */
  'U', 0x00,                                        /* wcChar5 */
  'S', 0x00,                                        /* wcChar6 */
  'B', 0x00,                                        /* wcChar7 */
  ' ', 0x00,                                        /* wcChar8 */
  'W', 0x00,                                        /* wcChar9 */
  'i', 0x00,                                        /* wcChar10 */
  'n', 0x00,                                        /* wcChar11 */
  'U', 0x00,                                        /* wcChar12 */
  'S', 0x00,                                        /* wcChar13 */
  'B', 0x00,                                        /* wcChar14 */
  ' ', 0x00,                                        /* wcChar15 */
  'D', 0x00,                                        /* wcChar16 */
  'E', 0x00,                                        /* wcChar17 */
  'M', 0x00,                                        /* wcChar18 */
  'O', 0x00,                                        /* wcChar19 */
};
#define  WINUSB_STRING_DESCRIPTOR3_STR   "TUSB123456"
#define  WINUSB_STRING_DESCRIPTOR3_SIZE   (22)
WEAK __ALIGN_BEGIN  const uint8_t WINUSB_StringDescriptor3 [22] __ALIGN_END = {
  0x16,                                             /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                       /* bDescriptorType */
  'T', 0x00,                                        /* wcChar0 */
  'U', 0x00,                                        /* wcChar1 */
  'S', 0x00,                                        /* wcChar2 */
  'B', 0x00,                                        /* wcChar3 */
  '1', 0x00,                                        /* wcChar4 */
  '2', 0x00,                                        /* wcChar5 */
  '3', 0x00,                                        /* wcChar6 */
  '4', 0x00,                                        /* wcChar7 */
  '5', 0x00,                                        /* wcChar8 */
  '6', 0x00,                                        /* wcChar9 */
};
#define  WINUSB_STRING_DESCRIPTOR4_STR   "TeenyUSB WinUSB RTT"
#define  WINUSB_STRING_DESCRIPTOR4_SIZE   (40)
WEAK __ALIGN_BEGIN  const uint8_t WINUSB_StringDescriptor4 [40] __ALIGN_END = {
  0x28,                                             /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                       /* bDescriptorType */
  'T', 0x00,                                        /* wcChar0 */
  'e', 0x00,                                        /* wcChar1 */
  'e', 0x00,                                        /* wcChar2 */
  'n', 0x00,                                        /* wcChar3 */
  'y', 0x00,                                        /* wcChar4 */
  'U', 0x00,                                        /* wcChar5 */
  'S', 0x00,                                        /* wcChar6 */
  'B', 0x00,                                        /* wcChar7 */
  ' ', 0x00,                                        /* wcChar8 */
  'W', 0x00,                                        /* wcChar9 */
  'i', 0x00,                                        /* wcChar10 */
  'n', 0x00,                                        /* wcChar11 */
  'U', 0x00,                                        /* wcChar12 */
  'S', 0x00,                                        /* wcChar13 */
  'B', 0x00,                                        /* wcChar14 */
  ' ', 0x00,                                        /* wcChar15 */
  'R', 0x00,                                        /* wcChar16 */
  'T', 0x00,                                        /* wcChar17 */
  'T', 0x00,                                        /* wcChar18 */
};
#define WINUSB_STRING_COUNT    (5)
const uint8_t* const WINUSB_StringDescriptors[5] = {
  WINUSB_StringDescriptor0,
  WINUSB_StringDescriptor1,
  WINUSB_StringDescriptor2,
  WINUSB_StringDescriptor3,
  WINUSB_StringDescriptor4,
};

//////////////////////////////////////////////////////
///// WCID descripors
//////////////////////////////////////////////////////
#if defined(HAS_WCID)

// Define WCID os string descriptor 
#ifndef WCID_MSOS_STRING
#define WCID_MSOS_STRING
#define WCID_STRING_DESCRIPTOR_MSOS_STR          "MSFT100"
#define WCID_STRING_DESCRIPTOR_MSOS_SIZE          (18)
WEAK __ALIGN_BEGIN const uint8_t  WCID_StringDescriptor_MSOS [18] __ALIGN_END = {
  ///////////////////////////////////////
  /// MS OS string descriptor
  ///////////////////////////////////////
  0x12,                                             /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                       /* bDescriptorType */
  /* MSFT100 */
  'M', 0x00, 'S', 0x00, 'F', 0x00, 'T', 0x00,       /* wcChar_7 */
  '1', 0x00, '0', 0x00, '0', 0x00,                  /* wcChar_7 */
  WCID_VENDOR_CODE,                                 /* bVendorCode */
  0x00,                                             /* bReserved */
};

#endif // WCID_MSOS_STRING
#define  WINUSB_IF0_WCID_PROPERTIES_SIZE  (142)
WEAK __ALIGN_BEGIN const uint8_t WINUSB_IF0_WCIDProperties [142] __ALIGN_END = {
  ///////////////////////////////////////
  /// WCID property descriptor
  ///////////////////////////////////////
  0x8e, 0x00, 0x00, 0x00,                           /* dwLength */
  0x00, 0x01,                                       /* bcdVersion */
  0x05, 0x00,                                       /* wIndex */
  0x01, 0x00,                                       /* wCount */
  
  ///////////////////////////////////////
  /// registry propter descriptor
  ///////////////////////////////////////
  0x84, 0x00, 0x00, 0x00,                           /* dwSize */
  0x01, 0x00, 0x00, 0x00,                           /* dwPropertyDataType */
  0x28, 0x00,                                       /* wPropertyNameLength */
  /* DeviceInterfaceGUID */
  'D', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00,       /* wcName_20 */
  'c', 0x00, 'e', 0x00, 'I', 0x00, 'n', 0x00,       /* wcName_20 */
  't', 0x00, 'e', 0x00, 'r', 0x00, 'f', 0x00,       /* wcName_20 */
  'a', 0x00, 'c', 0x00, 'e', 0x00, 'G', 0x00,       /* wcName_20 */
  'U', 0x00, 'I', 0x00, 'D', 0x00, 0x00, 0x00,      /* wcName_20 */
  0x4e, 0x00, 0x00, 0x00,                           /* dwPropertyDataLength */
  /* {1D4B2365-4749-48EA-B38A-7C6FDDDD7E26} */
  '{', 0x00, '1', 0x00, 'D', 0x00, '4', 0x00,       /* wcData_39 */
  'B', 0x00, '2', 0x00, '3', 0x00, '6', 0x00,       /* wcData_39 */
  '5', 0x00, '-', 0x00, '4', 0x00, '7', 0x00,       /* wcData_39 */
  '4', 0x00, '9', 0x00, '-', 0x00, '4', 0x00,       /* wcData_39 */
  '8', 0x00, 'E', 0x00, 'A', 0x00, '-', 0x00,       /* wcData_39 */
  'B', 0x00, '3', 0x00, '8', 0x00, 'A', 0x00,       /* wcData_39 */
  '-', 0x00, '7', 0x00, 'C', 0x00, '6', 0x00,       /* wcData_39 */
  'F', 0x00, 'D', 0x00, 'D', 0x00, 'D', 0x00,       /* wcData_39 */
  'D', 0x00, '7', 0x00, 'E', 0x00, '2', 0x00,       /* wcData_39 */
  '6', 0x00, '}', 0x00, 0x00, 0x00,                 /* wcData_39 */
};
#define  WINUSB_WCID_DESCRIPTOR_SIZE  (40)
WEAK __ALIGN_BEGIN const uint8_t WINUSB_WCIDDescriptor [40] __ALIGN_END = {
  ///////////////////////////////////////
  /// WCID descriptor
  ///////////////////////////////////////
  0x28, 0x00, 0x00, 0x00,                           /* dwLength */
  0x00, 0x01,                                       /* bcdVersion */
  0x04, 0x00,                                       /* wIndex */
  0x01,                                             /* bCount */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,         /* bReserved_7 */
  
  ///////////////////////////////////////
  /// WCID function descriptor
  ///////////////////////////////////////
  0x00,                                             /* bFirstInterfaceNumber */
  0x01,                                             /* bReserved */
  /* WINUSB */
  'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00,         /* cCID_8 */
  /*  */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* cSubCID_8 */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,               /* bReserved_6 */
};
#define WINUSB_WCID_PROPERTIES_SIZE (1)
WEAK __ALIGN_BEGIN const desc_t WINUSB_WCIDProperties[ WINUSB_WCID_PROPERTIES_SIZE ] __ALIGN_END = {
  WINUSB_IF0_WCIDProperties,
};
#endif    // #if defined(HAS_WCID)


WINUSB_TXEP_MAX_SIZE
WINUSB_RXEP_MAX_SIZE
//  Device descriptors
const tusb_descriptors WINUSB_descriptors = {
  .device = WINUSB_DeviceDescriptor,
  .config = WINUSB_ConfigDescriptor,
  .strings = WINUSB_StringDescriptors,
  .string_cnt = WINUSB_STRING_COUNT,
#if defined(HAS_WCID)
#if defined(WINUSB_WCID_DESCRIPTOR_SIZE)
  .wcid_desc = WINUSB_WCIDDescriptor,
#else
  .wcid_desc = 0,
#endif // WINUSB_WCID_DESCRIPTOR_SIZE)

#if defined(WINUSB_WCID_PROPERTIES_SIZE)
  .wcid_properties = WINUSB_WCIDProperties,
#else
  .wcid_properties = 0,
#endif // WINUSB_WCID_PROPERTIES_SIZE

#endif // HAS_WCID

#if defined(HAS_WCID_20)
#if defined(WINUSB_WCID_BOS_SIZE)
  .wcid_bos = WINUSB_WCIDBOS,
#else
  .wcid_bos = 0,  
#endif // WINUSB_WCID_BOS_SIZE)

#if defined(WINUSB_WCID_DESC_SET_SIZE)
  .wcid_desc_set = WINUSB_WCIDDescriptorSet,
#else
  .wcid_desc_set = 0,  
#endif // WINUSB_WCID_DESC_SET_SIZE


#endif // HAS_WCID_20
};
/////////////////////////////////////////
//// Device 5
/////////////////////////////////////////
#include "teeny_usb.h"

#define  RNDIS_DEVICE_DESCRIPTOR_SIZE  (18)
__ALIGN_BEGIN  const uint8_t RNDIS_DeviceDescriptor [18] __ALIGN_END = {
  ///////////////////////////////////////
  /// device descriptor
  ///////////////////////////////////////
  0x12,                                             /* bLength */
  USB_DEVICE_DESCRIPTOR_TYPE,                       /* bDescriptorType */
  0x00, 0x02,                                       /* bcdUSB */
  0xef,                                             /* bDeviceClass */
  0x02,                                             /* bDeviceSubClass */
  0x01,                                             /* bDeviceProtocol */
  0x40,                                             /* bMaxPacketSize */
  0x83, 0x04,                                       /* idVendor */
  0x17, 0x00,                                       /* idProduct */
  0x00, 0x01,                                       /* bcdDevice */
  0x01,                                             /* iManufacturer */
  0x02,                                             /* iProduct */
  0x03,                                             /* iSerial */
  0x01,                                             /* bNumConfigurations */
};
#define  RNDIS_CONFIG_DESCRIPTOR_SIZE  (75)
__ALIGN_BEGIN  const uint8_t RNDIS_ConfigDescriptor [75] __ALIGN_END = {
  ///////////////////////////////////////
  /// config descriptor
  ///////////////////////////////////////
  0x09,                                             /* bLength */
  USB_CONFIGURATION_DESCRIPTOR_TYPE,                /* bDescriptorType */
  0x4b, 0x00,                                       /* wTotalLength */
  0x02,                                             /* bNumInterfaces */
  0x01,                                             /* bConfigurationValue */
  0x00,                                             /* iConfiguration */
  0x80,                                             /* bmAttributes */
  0x64,                                             /* bMaxPower */
  
  ///////////////////////////////////////
  /// interface association descriptor
  ///////////////////////////////////////
  0x08,                                             /* bLength */
  USB_IAD_DESCRIPTOR_TYPE,                          /* bDescriptorType */
  0x00,                                             /* bFirstInterface */
  0x02,                                             /* bInterfaceCount */
  0x02,                                             /* bFunctionClass */
  0x02,                                             /* bFunctionSubClass */
  0xff,                                             /* bFunctionProtocol */
  0x00,                                             /* iFunction */
  
  ///////////////////////////////////////
  /// interface descriptor
  ///////////////////////////////////////
  0x09,                                             /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,                    /* bDescriptorType */
  0x00,                                             /* bInterfaceNumber */
  0x00,                                             /* bAlternateSetting */
  0x01,                                             /* bNumEndpoints */
  0x02,                                             /* bInterfaceClass */
  0x02,                                             /* bInterfaceSubClass */
  0xff,                                             /* bInterfaceProtocol */
  0x00,                                             /* iInterface */
  
  ///////////////////////////////////////
  /// cdc acm header descriptor
  ///////////////////////////////////////
  0x05,                                             /* bLength */
  0x24,                                             /* bDescriptorType */
  0x00,                                             /* bDescriptorSubtype */
  0x10, 0x01,                                       /* bcdCDC */
  
  ///////////////////////////////////////
  /// cdc acm call management descriptor
  ///////////////////////////////////////
  0x05,                                             /* bLength */
  0x24,                                             /* bDescriptorType */
  0x01,                                             /* bDescriptorSubtype */
  0x00,                                             /* bmCapabilities */
  0x01,                                             /* bDataInterface */
  
  ///////////////////////////////////////
  /// cdc acm descriptor
  ///////////////////////////////////////
  0x04,                                             /* bLength */
  0x24,                                             /* bDescriptorType */
  0x02,                                             /* bDescriptorSubtype */
  0x02,                                             /* bmCapabilities */
  
  ///////////////////////////////////////
  /// cdc acm union descriptor
  ///////////////////////////////////////
  0x05,                                             /* bLength */
  0x24,                                             /* bDescriptorType */
  0x06,                                             /* bDescriptorSubtype */
  0x00,                                             /* bMasterInterface */
  0x01,                                             /* bSlaveInterface0 */
  
  ///////////////////////////////////////
  /// endpoint descriptor
  ///////////////////////////////////////
  0x07,                                             /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,                     /* bDescriptorType */
  0x83,                                             /* bEndpointAddress */
  0x03,                                             /* bmAttributes */
  0x08, 0x00,                                       /* wMaxPacketSize */
  0x01,                                             /* bInterval */
  
  ///////////////////////////////////////
  /// interface descriptor
  ///////////////////////////////////////
  0x09,                                             /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,                    /* bDescriptorType */
  0x01,                                             /* bInterfaceNumber */
  0x00,                                             /* bAlternateSetting */
  0x02,                                             /* bNumEndpoints */
  0x0a,                                             /* bInterfaceClass */
  0x00,                                             /* bInterfaceSubClass */
  0x00,                                             /* bInterfaceProtocol */
  0x00,                                             /* iInterface */
  
  ///////////////////////////////////////
  /// endpoint descriptor
  ///////////////////////////////////////
  0x07,                                             /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,                     /* bDescriptorType */
  0x81,                                             /* bEndpointAddress */
  0x02,                                             /* bmAttributes */
  0x40, 0x00,                                       /* wMaxPacketSize */
  0x01,                                             /* bInterval */
  
  ///////////////////////////////////////
  /// endpoint descriptor
  ///////////////////////////////////////
  0x07,                                             /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,                     /* bDescriptorType */
  0x01,                                             /* bEndpointAddress */
  0x02,                                             /* bmAttributes */
  0x40, 0x00,                                       /* wMaxPacketSize */
  0x01,                                             /* bInterval */
};
#define  RNDIS_STRING_DESCRIPTOR0_STR   "\x09\x04"
#define  RNDIS_STRING_DESCRIPTOR0_SIZE  (4)
WEAK __ALIGN_BEGIN  const uint8_t RNDIS_StringDescriptor0 [4] __ALIGN_END = {
  0x04,                                         /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                   /* bDescriptorType */
  0x09, 0x04,                                   /* wLangID0 */
};
#define  RNDIS_STRING_DESCRIPTOR1_STR   "TeenyUSB"
#define  RNDIS_STRING_DESCRIPTOR1_SIZE   (18)
WEAK __ALIGN_BEGIN  const uint8_t RNDIS_StringDescriptor1 [18] __ALIGN_END = {
  0x12,                                             /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                       /* bDescriptorType */
  'T', 0x00,                                        /* wcChar0 */
  'e', 0x00,                                        /* wcChar1 */
  'e', 0x00,                                        /* wcChar2 */
  'n', 0x00,                                        /* wcChar3 */
  'y', 0x00,                                        /* wcChar4 */
  'U', 0x00,                                        /* wcChar5 */
  'S', 0x00,                                        /* wcChar6 */
  'B', 0x00,                                        /* wcChar7 */
};
#define  RNDIS_STRING_DESCRIPTOR2_STR   "TeenyUSB RNDIS DEMO"
#define  RNDIS_STRING_DESCRIPTOR2_SIZE   (40)
WEAK __ALIGN_BEGIN  const uint8_t RNDIS_StringDescriptor2 [40] __ALIGN_END = {
  0x28,                                             /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                       /* bDescriptorType */
  'T', 0x00,                                        /* wcChar0 */
  'e', 0x00,                                        /* wcChar1 */
  'e', 0x00,                                        /* wcChar2 */
  'n', 0x00,                                        /* wcChar3 */
  'y', 0x00,                                        /* wcChar4 */
  'U', 0x00,                                        /* wcChar5 */
  'S', 0x00,                                        /* wcChar6 */
  'B', 0x00,                                        /* wcChar7 */
  ' ', 0x00,                                        /* wcChar8 */
  'R', 0x00,                                        /* wcChar9 */
  'N', 0x00,                                        /* wcChar10 */
  'D', 0x00,                                        /* wcChar11 */
  'I', 0x00,                                        /* wcChar12 */
  'S', 0x00,                                        /* wcChar13 */
  ' ', 0x00,                                        /* wcChar14 */
  'D', 0x00,                                        /* wcChar15 */
  'E', 0x00,                                        /* wcChar16 */
  'M', 0x00,                                        /* wcChar17 */
  'O', 0x00,                                        /* wcChar18 */
};
#define  RNDIS_STRING_DESCRIPTOR3_STR   "TUSB123456"
#define  RNDIS_STRING_DESCRIPTOR3_SIZE   (22)
WEAK __ALIGN_BEGIN  const uint8_t RNDIS_StringDescriptor3 [22] __ALIGN_END = {
  0x16,                                             /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                       /* bDescriptorType */
  'T', 0x00,                                        /* wcChar0 */
  'U', 0x00,                                        /* wcChar1 */
  'S', 0x00,                                        /* wcChar2 */
  'B', 0x00,                                        /* wcChar3 */
  '1', 0x00,                                        /* wcChar4 */
  '2', 0x00,                                        /* wcChar5 */
  '3', 0x00,                                        /* wcChar6 */
  '4', 0x00,                                        /* wcChar7 */
  '5', 0x00,                                        /* wcChar8 */
  '6', 0x00,                                        /* wcChar9 */
};
#define RNDIS_STRING_COUNT    (4)
const uint8_t* const RNDIS_StringDescriptors[4] = {
  RNDIS_StringDescriptor0,
  RNDIS_StringDescriptor1,
  RNDIS_StringDescriptor2,
  RNDIS_StringDescriptor3,
};

//////////////////////////////////////////////////////
///// WCID descripors
//////////////////////////////////////////////////////
#if defined(HAS_WCID)

// Define WCID os string descriptor 
#ifndef WCID_MSOS_STRING
#define WCID_MSOS_STRING
#define WCID_STRING_DESCRIPTOR_MSOS_STR          "MSFT100"
#define WCID_STRING_DESCRIPTOR_MSOS_SIZE          (18)
WEAK __ALIGN_BEGIN const uint8_t  WCID_StringDescriptor_MSOS [18] __ALIGN_END = {
  ///////////////////////////////////////
  /// MS OS string descriptor
  ///////////////////////////////////////
  0x12,                                             /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,                       /* bDescriptorType */
  /* MSFT100 */
  'M', 0x00, 'S', 0x00, 'F', 0x00, 'T', 0x00,       /* wcChar_7 */
  '1', 0x00, '0', 0x00, '0', 0x00,                  /* wcChar_7 */
  WCID_VENDOR_CODE,                                 /* bVendorCode */
  0x00,                                             /* bReserved */
};

#endif // WCID_MSOS_STRING
#define  RNDIS_WCID_DESCRIPTOR_SIZE  (40)
WEAK __ALIGN_BEGIN const uint8_t RNDIS_WCIDDescriptor [40] __ALIGN_END = {
  ///////////////////////////////////////
  /// WCID descriptor
  ///////////////////////////////////////
  0x28, 0x00, 0x00, 0x00,                           /* dwLength */
  0x00, 0x01,                                       /* bcdVersion */
  0x04, 0x00,                                       /* wIndex */
  0x01,                                             /* bCount */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,         /* bReserved_7 */
  
  ///////////////////////////////////////
  /// WCID function descriptor
  ///////////////////////////////////////
  0x00,                                             /* bFirstInterfaceNumber */
  0x01,                                             /* bReserved */
  /* RNDIS */
  'R', 'N', 'D', 'I', 'S', 0x00, 0x00, 0x00,        /* cCID_8 */
  /* 5162001 */
  '5', '1', '6', '2', '0', '0', '1', 0x00,          /* cSubCID_8 */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,               /* bReserved_6 */
};
#define RNDIS_WCID_PROPERTIES_SIZE (2)
WEAK __ALIGN_BEGIN const desc_t RNDIS_WCIDProperties[ RNDIS_WCID_PROPERTIES_SIZE ] __ALIGN_END = {
  0,    // No WCID in Interface 0
  0,    // No WCID in Interface 1
};
#endif    // #if defined(HAS_WCID)


RNDIS_TXEP_MAX_SIZE
RNDIS_RXEP_MAX_SIZE
//  Device descriptors
const tusb_descriptors RNDIS_descriptors = {
  .device = RNDIS_DeviceDescriptor,
  .config = RNDIS_ConfigDescriptor,
  .strings = RNDIS_StringDescriptors,
  .string_cnt = RNDIS_STRING_COUNT,
#if defined(HAS_WCID)
#if defined(RNDIS_WCID_DESCRIPTOR_SIZE)
  .wcid_desc = RNDIS_WCIDDescriptor,
#else
  .wcid_desc = 0,
#endif // RNDIS_WCID_DESCRIPTOR_SIZE)

#if defined(RNDIS_WCID_PROPERTIES_SIZE)
  .wcid_properties = RNDIS_WCIDProperties,
#else
  .wcid_properties = 0,
#endif // RNDIS_WCID_PROPERTIES_SIZE

#endif // HAS_WCID

#if defined(HAS_WCID_20)
#if defined(RNDIS_WCID_BOS_SIZE)
  .wcid_bos = RNDIS_WCIDBOS,
#else
  .wcid_bos = 0,  
#endif // RNDIS_WCID_BOS_SIZE)

#if defined(RNDIS_WCID_DESC_SET_SIZE)
  .wcid_desc_set = RNDIS_WCIDDescriptorSet,
#else
  .wcid_desc_set = 0,  
#endif // RNDIS_WCID_DESC_SET_SIZE


#endif // HAS_WCID_20
};
