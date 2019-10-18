/*
 * Name   :  teeny_usb_desc.c
 * Author :  admin@xtoolbox.org
 * Date   :  2019-10-13 10:30:05
 * Desc   :  This file is auto generate by the teeny_usb script tool
 *           Visit https://github.com/xtoolbox/TeenyUSB for more info
 */

/*
  Input source name:  composite_desc.lua  
  Content type is lua script:
  ------------- lua script begin ------------
return Device {
    strManufacture = "TeenyUSB",
    strProduct = "TeenyUSB Composite DEMO",
    strSerial = "TUSB123456",
    idVendor = 0x0483,
    idProduct = 0x0011,
    prefix = "COMP",
    Config {
        USB_HID{
            ReadEp = EndPoint(IN(2),  Interrupt, 16),
            WriteEp = EndPoint(OUT(2), Interrupt, 16),
            report = HID_InOut(16),
        },
        CDC_ACM{
            EndPoint(IN(8),  Interrupt, 16),
            EndPoint(IN(1), BulkDouble, 32),
            EndPoint(OUT(1),  BulkDouble, 32),
        },
        Interface{
            WCID=WinUSB,
            strInterface = "TeenyUSB WinUSB",
            GUID="{1D4B2365-4749-48EA-B38A-7C6FDDDD7E26}",
            EndPoint(IN(3),  BulkDouble, 32),
            EndPoint(OUT(3), BulkDouble, 32),
        },
        Interface{
            bInterfaceClass = 0x08,        -- MSC
            bInterfaceSubClass = 0x06,     -- SCSI
            bInterfaceProtocol = 0x50,     -- BOT
            EndPoint(IN(4),  BulkDouble, 64),
            EndPoint(OUT(4), BulkDouble, 64),
        },
   }
}  

  ------------- lua script end   ------------
 */
#include "teeny_usb.h"
#define  COMP_DEVICE_DESCRIPTOR_SIZE  (18)
__ALIGN_BEGIN  const uint8_t COMP_DeviceDescriptor [18] __ALIGN_END = {
    0x12,                                         /* bLength */
    USB_DEVICE_DESCRIPTOR_TYPE,                   /* bDescriptorType */
    0x00, 0x02,                                   /* bcdUSB */
    0xef,                                         /* bDeviceClass */
    0x02,                                         /* bDeviceSubClass */
    0x01,                                         /* bDeviceProtocol */
    0x40,                                         /* bMaxPacketSize */
    LOBYTE(COMP_VID), HIBYTE(COMP_VID),           /* idVendor */
    LOBYTE(COMP_PID), HIBYTE(COMP_PID),           /* idProduct */
    0x00, 0x01,                                   /* bcdDevice */
    0x01,                                         /* iManufacture */
    0x02,                                         /* iProduct */
    0x03,                                         /* iSerial */
    0x01,                                         /* bNumConfigurations */
};

// Configs 
#define COMP_REPORT_DESCRIPTOR_SIZE_IF0 24
WEAK __ALIGN_BEGIN const uint8_t COMP_ReportDescriptor_if0[COMP_REPORT_DESCRIPTOR_SIZE_IF0] __ALIGN_END = {

        // report descriptor for general input/output
        0x06, 0x00, 0xFF,  // Usage Page (Vendor Defined 0xFF00)
        0x09, 0x01,        // Usage (0x01)
        0xA1, 0x01,        // Collection (Application)
        0x09, 0x02,        //   Usage (0x02)
        0x15, 0x00,        //   Logical Minimum (0)
        0x25, 0xFF,        //   Logical Maximum (255)
        0x75, 0x08,        //   Report Size (8)
        0x95, 0x10,        //   Report Count (16)
        0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
        0x09, 0x03,        //   Usage (0x03)
        0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
        0xC0               // End Collection
    
};

#define  COMP_CONFIG_DESCRIPTOR_SIZE  (153)
__ALIGN_BEGIN const uint8_t COMP_ConfigDescriptor [153] __ALIGN_END = {
    0x09,                                         /* bLength */
    USB_CONFIGURATION_DESCRIPTOR_TYPE,            /* bDescriptorType */
    0x99, 0x00,                                   /* wTotalLength */
    0x05,                                         /* bNumInterfaces */
    0x01,                                         /* bConfigurationValue */
    0x00,                                         /* iConfiguration */
    0x80,                                         /* bmAttributes */
    0x64,                                         /* bMaxPower */
    /* Interface descriptor, len: 32*/
    0x09,                                         /* bLength */
    USB_INTERFACE_DESCRIPTOR_TYPE,                /* bDescriptorType */
    0x00,                                         /* bInterfaceNumber */
    0x00,                                         /* bAlternateSetting */
    0x02,                                         /* bNumEndpoints */
    0x03,                                         /* bInterfaceClass */
    0x00,                                         /* bInterfaceSubClass */
    0x00,                                         /* bInterfaceProtocol */
    0x00,                                         /* iInterface */
    /* HID descriptor  */
    0x09,                                         /* bLength */
    0x21,                                         /* bDescriptorType */
    0x11, 0x01,                                   /* bcdHID */
    0x00,                                         /* bCountryCode */
    0x01,                                         /* bNumDescriptors */
    0x22,                                         /* bDescriptorType1 */
    0x18, 0x00,                                   /* wDescriptorLength1 */
    /* EndPoint descriptor */
    0x07,                                         /* bLength */
    USB_ENDPOINT_DESCRIPTOR_TYPE,                 /* bDescriptorType */
    0x82,                                         /* bEndpointAddress */
    0x03,                                         /* bmAttributes */
    0x10, 0x00,                                   /* wMaxPacketSize */
    0x01,                                         /* bInterval */
    /* EndPoint descriptor */
    0x07,                                         /* bLength */
    USB_ENDPOINT_DESCRIPTOR_TYPE,                 /* bDescriptorType */
    0x02,                                         /* bEndpointAddress */
    0x03,                                         /* bmAttributes */
    0x10, 0x00,                                   /* wMaxPacketSize */
    0x01,                                         /* bInterval */
  /* IAD descriptor */
    0x08,                                         /* bLength */
    USB_IAD_DESCRIPTOR_TYPE,                      /* bDescriptorType */
    0x01,                                         /* bFirstInterface */
    0x02,                                         /* bInterfaceCount */
    0x02,                                         /* bFunctionClass */
    0x02,                                         /* bFunctionSubClass */
    0x01,                                         /* bFunctionProtocol */
    0x00,                                         /* iFunction */
    /* Interface descriptor, len: 35*/
    0x09,                                         /* bLength */
    USB_INTERFACE_DESCRIPTOR_TYPE,                /* bDescriptorType */
    0x01,                                         /* bInterfaceNumber */
    0x00,                                         /* bAlternateSetting */
    0x01,                                         /* bNumEndpoints */
    0x02,                                         /* bInterfaceClass */
    0x02,                                         /* bInterfaceSubClass */
    0x01,                                         /* bInterfaceProtocol */
    0x00,                                         /* iInterface */
    /* Function descriptor Header */
    0x05,                                         /* bLength */
    0x24,                                         /* bDescriptorType */
    0x00,                                         /* bDescriptorSubtype */
    0x10, 0x01,                                   /* bcdCDC */
    /* Function descriptor Call Management */
    0x05,                                         /* bLength */
    0x24,                                         /* bDescriptorType */
    0x01,                                         /* bDescriptorSubtype */
    0x00,                                         /* bmCapabilities */
    0x01,                                         /* bDataInterface */
    /* Function descriptor ACM (Abstract Control Management) */
    0x04,                                         /* bLength */
    0x24,                                         /* bDescriptorType */
    0x02,                                         /* bDescriptorSubtype */
    0x02,                                         /* bmCapabilities */
    /* Function descriptor Union */
    0x05,                                         /* bLength */
    0x24,                                         /* bDescriptorType */
    0x06,                                         /* bDescriptorSubtype */
    0x00,                                         /* bMasterInterface */
    0x01,                                         /* bSlaveInterface0 */
    /* EndPoint descriptor */
    0x07,                                         /* bLength */
    USB_ENDPOINT_DESCRIPTOR_TYPE,                 /* bDescriptorType */
    0x88,                                         /* bEndpointAddress */
    0x03,                                         /* bmAttributes */
    0x10, 0x00,                                   /* wMaxPacketSize */
    0x01,                                         /* bInterval */
    /* Interface descriptor, len: 23*/
    0x09,                                         /* bLength */
    USB_INTERFACE_DESCRIPTOR_TYPE,                /* bDescriptorType */
    0x02,                                         /* bInterfaceNumber */
    0x00,                                         /* bAlternateSetting */
    0x02,                                         /* bNumEndpoints */
    0x0a,                                         /* bInterfaceClass */
    0x00,                                         /* bInterfaceSubClass */
    0x00,                                         /* bInterfaceProtocol */
    0x00,                                         /* iInterface */
    /* EndPoint descriptor */
    0x07,                                         /* bLength */
    USB_ENDPOINT_DESCRIPTOR_TYPE,                 /* bDescriptorType */
    0x81,                                         /* bEndpointAddress */
    0x02,                                         /* bmAttributes */
    0x20, 0x00,                                   /* wMaxPacketSize */
    0x01,                                         /* bInterval */
    /* EndPoint descriptor */
    0x07,                                         /* bLength */
    USB_ENDPOINT_DESCRIPTOR_TYPE,                 /* bDescriptorType */
    0x01,                                         /* bEndpointAddress */
    0x02,                                         /* bmAttributes */
    0x20, 0x00,                                   /* wMaxPacketSize */
    0x01,                                         /* bInterval */
    /* Interface descriptor, len: 23*/
    0x09,                                         /* bLength */
    USB_INTERFACE_DESCRIPTOR_TYPE,                /* bDescriptorType */
    0x03,                                         /* bInterfaceNumber */
    0x00,                                         /* bAlternateSetting */
    0x02,                                         /* bNumEndpoints */
    0xff,                                         /* bInterfaceClass */
    0xff,                                         /* bInterfaceSubClass */
    0x00,                                         /* bInterfaceProtocol */
    0x04,                                         /* iInterface */
    /* EndPoint descriptor */
    0x07,                                         /* bLength */
    USB_ENDPOINT_DESCRIPTOR_TYPE,                 /* bDescriptorType */
    0x83,                                         /* bEndpointAddress */
    0x02,                                         /* bmAttributes */
    0x20, 0x00,                                   /* wMaxPacketSize */
    0x01,                                         /* bInterval */
    /* EndPoint descriptor */
    0x07,                                         /* bLength */
    USB_ENDPOINT_DESCRIPTOR_TYPE,                 /* bDescriptorType */
    0x03,                                         /* bEndpointAddress */
    0x02,                                         /* bmAttributes */
    0x20, 0x00,                                   /* wMaxPacketSize */
    0x01,                                         /* bInterval */
    /* Interface descriptor, len: 23*/
    0x09,                                         /* bLength */
    USB_INTERFACE_DESCRIPTOR_TYPE,                /* bDescriptorType */
    0x04,                                         /* bInterfaceNumber */
    0x00,                                         /* bAlternateSetting */
    0x02,                                         /* bNumEndpoints */
    0x08,                                         /* bInterfaceClass */
    0x06,                                         /* bInterfaceSubClass */
    0x50,                                         /* bInterfaceProtocol */
    0x00,                                         /* iInterface */
    /* EndPoint descriptor */
    0x07,                                         /* bLength */
    USB_ENDPOINT_DESCRIPTOR_TYPE,                 /* bDescriptorType */
    0x84,                                         /* bEndpointAddress */
    0x02,                                         /* bmAttributes */
    0x40, 0x00,                                   /* wMaxPacketSize */
    0x01,                                         /* bInterval */
    /* EndPoint descriptor */
    0x07,                                         /* bLength */
    USB_ENDPOINT_DESCRIPTOR_TYPE,                 /* bDescriptorType */
    0x04,                                         /* bEndpointAddress */
    0x02,                                         /* bmAttributes */
    0x40, 0x00,                                   /* wMaxPacketSize */
    0x01,                                         /* bInterval */
};

// Power status
#define COMP_DEV_STATUS0      (0)
#define COMP_DEV_STATUS1      (0)
#define COMP_DEV_STATUS    ((COMP_DEV_STATUS0) |(COMP_DEV_STATUS1) )

COMP_TXEP_MAX_SIZE
COMP_RXEP_MAX_SIZE

// Strings 
#define COMP_STRING_DESCRIPTOR0_STR          "\x09\x04"
#define COMP_STRING_DESCRIPTOR0_SIZE          (4)
WEAK __ALIGN_BEGIN const uint8_t  COMP_StringDescriptor0 [4] __ALIGN_END = {
    0x04,                                         /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,                   /* bDescriptorType */
    0x09, 0x04,                                   /* wLangID0 */
};
#define COMP_STRING_DESCRIPTOR1_STR          "TeenyUSB"
#define COMP_STRING_DESCRIPTOR1_SIZE          (18)
WEAK __ALIGN_BEGIN const uint8_t  COMP_StringDescriptor1 [18] __ALIGN_END = {
    0x12,                                         /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,                   /* bDescriptorType */
    'T', 0x00,                                    /* wcChar0 */
    'e', 0x00,                                    /* wcChar1 */
    'e', 0x00,                                    /* wcChar2 */
    'n', 0x00,                                    /* wcChar3 */
    'y', 0x00,                                    /* wcChar4 */
    'U', 0x00,                                    /* wcChar5 */
    'S', 0x00,                                    /* wcChar6 */
    'B', 0x00,                                    /* wcChar7 */
};
#define COMP_STRING_DESCRIPTOR2_STR          "TeenyUSB Composite DEMO"
#define COMP_STRING_DESCRIPTOR2_SIZE          (48)
WEAK __ALIGN_BEGIN const uint8_t  COMP_StringDescriptor2 [48] __ALIGN_END = {
    0x30,                                         /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,                   /* bDescriptorType */
    'T', 0x00,                                    /* wcChar0 */
    'e', 0x00,                                    /* wcChar1 */
    'e', 0x00,                                    /* wcChar2 */
    'n', 0x00,                                    /* wcChar3 */
    'y', 0x00,                                    /* wcChar4 */
    'U', 0x00,                                    /* wcChar5 */
    'S', 0x00,                                    /* wcChar6 */
    'B', 0x00,                                    /* wcChar7 */
    ' ', 0x00,                                    /* wcChar8 */
    'C', 0x00,                                    /* wcChar9 */
    'o', 0x00,                                    /* wcChar10 */
    'm', 0x00,                                    /* wcChar11 */
    'p', 0x00,                                    /* wcChar12 */
    'o', 0x00,                                    /* wcChar13 */
    's', 0x00,                                    /* wcChar14 */
    'i', 0x00,                                    /* wcChar15 */
    't', 0x00,                                    /* wcChar16 */
    'e', 0x00,                                    /* wcChar17 */
    ' ', 0x00,                                    /* wcChar18 */
    'D', 0x00,                                    /* wcChar19 */
    'E', 0x00,                                    /* wcChar20 */
    'M', 0x00,                                    /* wcChar21 */
    'O', 0x00,                                    /* wcChar22 */
};
#define COMP_STRING_DESCRIPTOR3_STR          "TUSB123456"
#define COMP_STRING_DESCRIPTOR3_SIZE          (22)
WEAK __ALIGN_BEGIN const uint8_t  COMP_StringDescriptor3 [22] __ALIGN_END = {
    0x16,                                         /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,                   /* bDescriptorType */
    'T', 0x00,                                    /* wcChar0 */
    'U', 0x00,                                    /* wcChar1 */
    'S', 0x00,                                    /* wcChar2 */
    'B', 0x00,                                    /* wcChar3 */
    '1', 0x00,                                    /* wcChar4 */
    '2', 0x00,                                    /* wcChar5 */
    '3', 0x00,                                    /* wcChar6 */
    '4', 0x00,                                    /* wcChar7 */
    '5', 0x00,                                    /* wcChar8 */
    '6', 0x00,                                    /* wcChar9 */
};
#define COMP_STRING_DESCRIPTOR4_STR          "TeenyUSB WinUSB"
#define COMP_STRING_DESCRIPTOR4_SIZE          (32)
WEAK __ALIGN_BEGIN const uint8_t  COMP_StringDescriptor4 [32] __ALIGN_END = {
    0x20,                                         /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,                   /* bDescriptorType */
    'T', 0x00,                                    /* wcChar0 */
    'e', 0x00,                                    /* wcChar1 */
    'e', 0x00,                                    /* wcChar2 */
    'n', 0x00,                                    /* wcChar3 */
    'y', 0x00,                                    /* wcChar4 */
    'U', 0x00,                                    /* wcChar5 */
    'S', 0x00,                                    /* wcChar6 */
    'B', 0x00,                                    /* wcChar7 */
    ' ', 0x00,                                    /* wcChar8 */
    'W', 0x00,                                    /* wcChar9 */
    'i', 0x00,                                    /* wcChar10 */
    'n', 0x00,                                    /* wcChar11 */
    'U', 0x00,                                    /* wcChar12 */
    'S', 0x00,                                    /* wcChar13 */
    'B', 0x00,                                    /* wcChar14 */
};
const uint8_t* const COMP_StringDescriptors[COMP_STRING_COUNT] = {
COMP_StringDescriptor0,
COMP_StringDescriptor1,
COMP_StringDescriptor2,
COMP_StringDescriptor3,
COMP_StringDescriptor4,
};



#if defined(HAS_WCID)

// Define WCID os string descriptor 
#ifndef WCID_MSOS_STRING
#define WCID_MSOS_STRING
#define WCID_STRING_DESCRIPTOR_MSOS_STR          "MSFT100"
#define WCID_STRING_DESCRIPTOR_MSOS_SIZE          (18)
WEAK __ALIGN_BEGIN const uint8_t  WCID_StringDescriptor_MSOS [18] __ALIGN_END = {
    0x12,                                         /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,                   /* bDescriptorType */
    'M', 0x00,                                    /* wcChar0 */
    'S', 0x00,                                    /* wcChar1 */
    'F', 0x00,                                    /* wcChar2 */
    'T', 0x00,                                    /* wcChar3 */
    '1', 0x00,                                    /* wcChar4 */
    '0', 0x00,                                    /* wcChar5 */
    '0', 0x00,                                    /* wcChar6 */
    WCID_VENDOR_CODE,                             /* bVendorCode */
    0x00,                                         /* bReserved */
};
#endif

#define  COMP_WCID_DESCRIPTOR_SIZE  (40)
WEAK __ALIGN_BEGIN const uint8_t COMP_WCIDDescriptor [40] __ALIGN_END = {
    0x28, 0x00, 0x00, 0x00,                       /* dwLength */
    0x00, 0x01,                                   /* bcdVersion */
    0x04, 0x00,                                   /* wIndex */
    0x01,                                         /* bCount */
    0,0,0,0,0,0,0,                                /* Reserved */
    /* WCID Function  */
    0x03,                                         /* bFirstInterfaceNumber */
    0x01,                                         /* bReserved */
    /* CID */
    'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00, 
    /* sub CID */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0,0,0,0,0,0,                                  /* Reserved */
};


#define  COMP__IF3_WCID_PROPERTIES_SIZE  (142)
WEAK __ALIGN_BEGIN const uint8_t COMP__IF3_WCIDProperties [142] __ALIGN_END = {
    0x8e, 0x00, 0x00, 0x00,                       /* dwLength */
    0x00, 0x01,                                   /* bcdVersion */
    0x05, 0x00,                                   /* wIndex */
    0x01, 0x00,                                   /* wCount */
    /*WCID property field */
  /* DeviceInterfaceGUID = 
       {1D4B2365-4749-48EA-B38A-7C6FDDDD7E26}  */
    0x84, 0x00, 0x00, 0x00,                       /* dwSize */
    0x01, 0x00, 0x00, 0x00,                       /* dwPropertyDataType */
    0x28, 0x00,                                   /* wPropertyNameLength */
    'D', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00, 
    'c', 0x00, 'e', 0x00, 'I', 0x00, 'n', 0x00, 
    't', 0x00, 'e', 0x00, 'r', 0x00, 'f', 0x00, 
    'a', 0x00, 'c', 0x00, 'e', 0x00, 'G', 0x00, 
    'U', 0x00, 'I', 0x00, 'D', 0x00, 0x00, 0x00, 
    
    0x4e, 0x00, 0x00, 0x00,                       /* dwPropertyDataLength */
    '{', 0x00, '1', 0x00, 'D', 0x00, '4', 0x00, 
    'B', 0x00, '2', 0x00, '3', 0x00, '6', 0x00, 
    '5', 0x00, '-', 0x00, '4', 0x00, '7', 0x00, 
    '4', 0x00, '9', 0x00, '-', 0x00, '4', 0x00, 
    '8', 0x00, 'E', 0x00, 'A', 0x00, '-', 0x00, 
    'B', 0x00, '3', 0x00, '8', 0x00, 'A', 0x00, 
    '-', 0x00, '7', 0x00, 'C', 0x00, '6', 0x00, 
    'F', 0x00, 'D', 0x00, 'D', 0x00, 'D', 0x00, 
    'D', 0x00, '7', 0x00, 'E', 0x00, '2', 0x00, 
    '6', 0x00, '}', 0x00, 0x00, 0x00, 
};


#define COMP_WCID_PROPERTIES_SIZE (6)
WEAK __ALIGN_BEGIN const desc_t COMP_WCIDProperties[ COMP_WCID_PROPERTIES_SIZE ] __ALIGN_END = {
0,    // No WCID in Interface 0
0,    // No WCID in Interface 1
0,    // No WCID in Interface 2
COMP__IF3_WCIDProperties,
0,    // No WCID in Interface 4

};

#endif

//  Device descriptors
const tusb_descriptors COMP_descriptors = {
  .device = COMP_DeviceDescriptor,
  .config = COMP_ConfigDescriptor,
  .strings = COMP_StringDescriptors,
  .string_cnt = COMP_STRING_COUNT,
#if defined(HAS_WCID)
#if defined(COMP_WCID_DESCRIPTOR_SIZE)
  .wcid_desc = COMP_WCIDDescriptor,
#else
  .wcid_desc = 0,  
#endif // COMP_WCID_DESCRIPTOR_SIZE)

#if defined(COMP_WCID_PROPERTIES_SIZE)
  .wcid_properties = COMP_WCIDProperties,
#else
  .wcid_properties = 0,  
#endif // COMP_WCID_PROPERTIES_SIZE

#endif // HAS_WCID

#if defined(HAS_WCID_20)
#if defined(COMP_WCID_BOS_SIZE)
  .wcid_bos = COMP_WCIDBOS,
#else
  .wcid_bos = 0,  
#endif // COMP_WCID_BOS_SIZE)

#if defined(COMP_WCID_DESC_SET_SIZE)
  .wcid_desc_set = COMP_WCIDDescriptorSet,
#else
  .wcid_desc_set = 0,  
#endif // COMP_WCID_DESC_SET_SIZE


#endif // HAS_WCID_20
};


