/*
 * Name   :  teeny_usb_desc.c
 * Author :  admin@xtoolbox.org
 * Date   :  2019-09-17 11:21:35
 * Desc   :  This file is auto generate by the teeny_usb script tool
 *           Visit https://github.com/xtoolbox/TeenyUSB for more info
 */

/*
  Input source name:  cmsis_dap_desc.lua  
  Content type is lua script:
  ------------- lua script begin ------------
return {
  Device {
    strManufacture = "TeenyUSB",
    strProduct = "TeenyUSB HS CMSIS-DAP",
    strSerial = "TUSB123456HS",
    idVendor = 0x0d28,
    idProduct = 0x0204,
    bcdUSB = 0x200,
    prefix = "TUSB_HS_DAP",
    Config {
        Interface{
            strInterface = "CMSIS-DAP v2",
            WCID=WinUSB,
            GUID="{CDB3B5AD-293B-4663-AA36-1AAE46463776}",
            bInterfaceClass = 0xff,
            bInterfaceSubClass = 0x00,
            EndPoint(OUT(1),  Bulk, 512),
            EndPoint(IN(2), Bulk, 512),
        },
   }
 },
 Device {
    strManufacture = "TeenyUSB",
    strProduct = "TeenyUSB FS CMSIS-DAP",
    strSerial = "TUSB123456FS",
    idVendor = 0x0d28,
    idProduct = 0x0204,
    bcdUSB = 0x200,
    prefix = "TUSB_FS_DAP",
    Config {
        USB_HID{
            strInterface = "CMSIS-DAP v1",
            ReadEp = EndPoint(IN(2),  Interrupt, 64),
            WriteEp = EndPoint(OUT(1), Interrupt, 64),
            report = HID_BuildReport([[
// report descriptor for general input/output
  0x06, 0x00, 0xFF,  // Usage Page (Vendor Defined 0xFF00)
  0x09, 0x01,        // Usage (0x01)
  0xA1, 0x01,        // Collection (Application)
  0x09, 0x01,        //   Usage (0x01)
  0x15, 0x00,        //   Logical Minimum (0)
  0x26, 0xFF, 0x00,  //   Logical Maximum (255)
  0x75, 0x08,        //   Report Size (8)
  0x95, 0x40,        //   Report Count 64
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x95, 0x40,        //   Report Count 64
  0x09, 0x01,        //   Usage (0x01)
  0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x95, 0x01,        //   Report Count 1
  0x09, 0x01,        //   Usage (0x01)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0xC0               // End Collection
]]),
        }
   }
 }
}  

  ------------- lua script end   ------------
 */
#include "teeny_usb.h"
/////////////////////////////////////////////////////////////////////
//// Descriptor for device1 define begin
/////////////////////////////////////////////////////////////////////
#define  TUSB_HS_DAP_DEVICE_DESCRIPTOR_SIZE  (18)
__ALIGN_BEGIN  const uint8_t TUSB_HS_DAP_DeviceDescriptor [18] __ALIGN_END = {
    0x12,                                         /* bLength */
    USB_DEVICE_DESCRIPTOR_TYPE,                   /* bDescriptorType */
    0x00, 0x02,                                   /* bcdUSB */
    0x00,                                         /* bDeviceClass */
    0x00,                                         /* bDeviceSubClass */
    0x00,                                         /* bDeviceProtocol */
    0x40,                                         /* bMaxPacketSize */
    LOBYTE(TUSB_HS_DAP_VID), HIBYTE(TUSB_HS_DAP_VID),/* idVendor */
    LOBYTE(TUSB_HS_DAP_PID), HIBYTE(TUSB_HS_DAP_PID),/* idProduct */
    0x00, 0x01,                                   /* bcdDevice */
    0x01,                                         /* iManufacture */
    0x02,                                         /* iProduct */
    0x03,                                         /* iSerial */
    0x01,                                         /* bNumConfigurations */
};

// Configs 
#define  TUSB_HS_DAP_CONFIG_DESCRIPTOR_SIZE  (32)
__ALIGN_BEGIN const uint8_t TUSB_HS_DAP_ConfigDescriptor [32] __ALIGN_END = {
    0x09,                                         /* bLength */
    USB_CONFIGURATION_DESCRIPTOR_TYPE,            /* bDescriptorType */
    0x20, 0x00,                                   /* wTotalLength */
    0x01,                                         /* bNumInterfaces */
    0x01,                                         /* bConfigurationValue */
    0x00,                                         /* iConfiguration */
    0x80,                                         /* bmAttributes */
    0x64,                                         /* bMaxPower */
    /* Interface descriptor, len: 23*/
    0x09,                                         /* bLength */
    USB_INTERFACE_DESCRIPTOR_TYPE,                /* bDescriptorType */
    0x00,                                         /* bInterfaceNumber */
    0x00,                                         /* bAlternateSetting */
    0x02,                                         /* bNumEndpoints */
    0xff,                                         /* bInterfaceClass */
    0x00,                                         /* bInterfaceSubClass */
    0x00,                                         /* bInterfaceProtocol */
    0x04,                                         /* iInterface */
    /* EndPoint descriptor */
    0x07,                                         /* bLength */
    USB_ENDPOINT_DESCRIPTOR_TYPE,                 /* bDescriptorType */
    0x01,                                         /* bEndpointAddress */
    0x02,                                         /* bmAttributes */
    0x00, 0x02,                                   /* wMaxPacketSize */
    0x01,                                         /* bInterval */
    /* EndPoint descriptor */
    0x07,                                         /* bLength */
    USB_ENDPOINT_DESCRIPTOR_TYPE,                 /* bDescriptorType */
    0x82,                                         /* bEndpointAddress */
    0x02,                                         /* bmAttributes */
    0x00, 0x02,                                   /* wMaxPacketSize */
    0x01,                                         /* bInterval */
};

// Power status
#define TUSB_HS_DAP_DEV_STATUS0      (0)
#define TUSB_HS_DAP_DEV_STATUS1      (0)
#define TUSB_HS_DAP_DEV_STATUS    ((TUSB_HS_DAP_DEV_STATUS0) |(TUSB_HS_DAP_DEV_STATUS1) )

TUSB_HS_DAP_TXEP_MAX_SIZE
TUSB_HS_DAP_RXEP_MAX_SIZE

// Strings 
#define TUSB_HS_DAP_STRING_DESCRIPTOR0_STR          "\x09\x04"
#define TUSB_HS_DAP_STRING_DESCRIPTOR0_SIZE          (4)
WEAK __ALIGN_BEGIN const uint8_t  TUSB_HS_DAP_StringDescriptor0 [4] __ALIGN_END = {
    0x04,                                         /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,                   /* bDescriptorType */
    0x09, 0x04,                                   /* wLangID0 */
};
#define TUSB_HS_DAP_STRING_DESCRIPTOR1_STR          "TeenyUSB"
#define TUSB_HS_DAP_STRING_DESCRIPTOR1_SIZE          (18)
WEAK __ALIGN_BEGIN const uint8_t  TUSB_HS_DAP_StringDescriptor1 [18] __ALIGN_END = {
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
#define TUSB_HS_DAP_STRING_DESCRIPTOR2_STR          "TeenyUSB HS CMSIS-DAP"
#define TUSB_HS_DAP_STRING_DESCRIPTOR2_SIZE          (44)
WEAK __ALIGN_BEGIN const uint8_t  TUSB_HS_DAP_StringDescriptor2 [44] __ALIGN_END = {
    0x2c,                                         /* bLength */
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
    'H', 0x00,                                    /* wcChar9 */
    'S', 0x00,                                    /* wcChar10 */
    ' ', 0x00,                                    /* wcChar11 */
    'C', 0x00,                                    /* wcChar12 */
    'M', 0x00,                                    /* wcChar13 */
    'S', 0x00,                                    /* wcChar14 */
    'I', 0x00,                                    /* wcChar15 */
    'S', 0x00,                                    /* wcChar16 */
    '-', 0x00,                                    /* wcChar17 */
    'D', 0x00,                                    /* wcChar18 */
    'A', 0x00,                                    /* wcChar19 */
    'P', 0x00,                                    /* wcChar20 */
};
#define TUSB_HS_DAP_STRING_DESCRIPTOR3_STR          "TUSB123456HS"
#define TUSB_HS_DAP_STRING_DESCRIPTOR3_SIZE          (26)
WEAK __ALIGN_BEGIN const uint8_t  TUSB_HS_DAP_StringDescriptor3 [26] __ALIGN_END = {
    0x1a,                                         /* bLength */
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
    'H', 0x00,                                    /* wcChar10 */
    'S', 0x00,                                    /* wcChar11 */
};
#define TUSB_HS_DAP_STRING_DESCRIPTOR4_STR          "CMSIS-DAP v2"
#define TUSB_HS_DAP_STRING_DESCRIPTOR4_SIZE          (26)
WEAK __ALIGN_BEGIN const uint8_t  TUSB_HS_DAP_StringDescriptor4 [26] __ALIGN_END = {
    0x1a,                                         /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,                   /* bDescriptorType */
    'C', 0x00,                                    /* wcChar0 */
    'M', 0x00,                                    /* wcChar1 */
    'S', 0x00,                                    /* wcChar2 */
    'I', 0x00,                                    /* wcChar3 */
    'S', 0x00,                                    /* wcChar4 */
    '-', 0x00,                                    /* wcChar5 */
    'D', 0x00,                                    /* wcChar6 */
    'A', 0x00,                                    /* wcChar7 */
    'P', 0x00,                                    /* wcChar8 */
    ' ', 0x00,                                    /* wcChar9 */
    'v', 0x00,                                    /* wcChar10 */
    '2', 0x00,                                    /* wcChar11 */
};
const uint8_t* const TUSB_HS_DAP_StringDescriptors[TUSB_HS_DAP_STRING_COUNT] = {
TUSB_HS_DAP_StringDescriptor0,
TUSB_HS_DAP_StringDescriptor1,
TUSB_HS_DAP_StringDescriptor2,
TUSB_HS_DAP_StringDescriptor3,
TUSB_HS_DAP_StringDescriptor4,
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

#define  TUSB_HS_DAP_WCID_DESCRIPTOR_SIZE  (40)
WEAK __ALIGN_BEGIN const uint8_t TUSB_HS_DAP_WCIDDescriptor [40] __ALIGN_END = {
    0x28, 0x00, 0x00, 0x00,                       /* dwLength */
    0x00, 0x01,                                   /* bcdVersion */
    0x04, 0x00,                                   /* wIndex */
    0x01,                                         /* bCount */
    0,0,0,0,0,0,0,                                /* Reserved */
    /* WCID Function  */
    0x00,                                         /* bFirstInterfaceNumber */
    0x01,                                         /* bReserved */
    /* CID */
    'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00, 
    /* sub CID */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0,0,0,0,0,0,                                  /* Reserved */
};


#define  TUSB_HS_DAP__IF0_WCID_PROPERTIES_SIZE  (142)
WEAK __ALIGN_BEGIN const uint8_t TUSB_HS_DAP__IF0_WCIDProperties [142] __ALIGN_END = {
    0x8e, 0x00, 0x00, 0x00,                       /* dwLength */
    0x00, 0x01,                                   /* bcdVersion */
    0x05, 0x00,                                   /* wIndex */
    0x01, 0x00,                                   /* wCount */
    /*WCID property field */
  /* DeviceInterfaceGUID = 
       {CDB3B5AD-293B-4663-AA36-1AAE46463776}  */
    0x84, 0x00, 0x00, 0x00,                       /* dwSize */
    0x01, 0x00, 0x00, 0x00,                       /* dwPropertyDataType */
    0x28, 0x00,                                   /* wPropertyNameLength */
    'D', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00, 
    'c', 0x00, 'e', 0x00, 'I', 0x00, 'n', 0x00, 
    't', 0x00, 'e', 0x00, 'r', 0x00, 'f', 0x00, 
    'a', 0x00, 'c', 0x00, 'e', 0x00, 'G', 0x00, 
    'U', 0x00, 'I', 0x00, 'D', 0x00, 0x00, 0x00, 
    
    0x4e, 0x00, 0x00, 0x00,                       /* dwPropertyDataLength */
    '{', 0x00, 'C', 0x00, 'D', 0x00, 'B', 0x00, 
    '3', 0x00, 'B', 0x00, '5', 0x00, 'A', 0x00, 
    'D', 0x00, '-', 0x00, '2', 0x00, '9', 0x00, 
    '3', 0x00, 'B', 0x00, '-', 0x00, '4', 0x00, 
    '6', 0x00, '6', 0x00, '3', 0x00, '-', 0x00, 
    'A', 0x00, 'A', 0x00, '3', 0x00, '6', 0x00, 
    '-', 0x00, '1', 0x00, 'A', 0x00, 'A', 0x00, 
    'E', 0x00, '4', 0x00, '6', 0x00, '4', 0x00, 
    '6', 0x00, '3', 0x00, '7', 0x00, '7', 0x00, 
    '6', 0x00, '}', 0x00, 0x00, 0x00, 
};


#define TUSB_HS_DAP_WCID_PROPERTIES_SIZE (1)
WEAK __ALIGN_BEGIN const desc_t TUSB_HS_DAP_WCIDProperties[ TUSB_HS_DAP_WCID_PROPERTIES_SIZE ] __ALIGN_END = {
TUSB_HS_DAP__IF0_WCIDProperties,

};

#endif

//  Device descriptors
const tusb_descriptors TUSB_HS_DAP_descriptors = {
  .device = TUSB_HS_DAP_DeviceDescriptor,
  .config = TUSB_HS_DAP_ConfigDescriptor,
  .strings = TUSB_HS_DAP_StringDescriptors,
  .string_cnt = TUSB_HS_DAP_STRING_COUNT,
#if defined(HAS_WCID)
#if defined(TUSB_HS_DAP_WCID_DESCRIPTOR_SIZE)
  .wcid_desc = TUSB_HS_DAP_WCIDDescriptor,
#else
  .wcid_desc = 0,  
#endif // TUSB_HS_DAP_WCID_DESCRIPTOR_SIZE)

#if defined(TUSB_HS_DAP_WCID_PROPERTIES_SIZE)
  .wcid_properties = TUSB_HS_DAP_WCIDProperties,
#else
  .wcid_properties = 0,  
#endif // TUSB_HS_DAP_WCID_PROPERTIES_SIZE

#endif // HAS_WCID

#if defined(HAS_WCID_20)
#if defined(TUSB_HS_DAP_WCID_BOS_SIZE)
  .wcid_bos = TUSB_HS_DAP_WCIDBOS,
#else
  .wcid_bos = 0,  
#endif // TUSB_HS_DAP_WCID_BOS_SIZE)

#if defined(TUSB_HS_DAP_WCID_DESC_SET_SIZE)
  .wcid_desc_set = TUSB_HS_DAP_WCIDDescriptorSet,
#else
  .wcid_desc_set = 0,  
#endif // TUSB_HS_DAP_WCID_DESC_SET_SIZE


#endif // HAS_WCID_20
};
/////////////////////////////////////////////////////////////////////
//// Descriptor for device1  define end
/////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////
//// Descriptor for device2 define begin
/////////////////////////////////////////////////////////////////////
#define  TUSB_FS_DAP_DEVICE_DESCRIPTOR_SIZE  (18)
__ALIGN_BEGIN  const uint8_t TUSB_FS_DAP_DeviceDescriptor [18] __ALIGN_END = {
    0x12,                                         /* bLength */
    USB_DEVICE_DESCRIPTOR_TYPE,                   /* bDescriptorType */
    0x00, 0x02,                                   /* bcdUSB */
    0x00,                                         /* bDeviceClass */
    0x00,                                         /* bDeviceSubClass */
    0x00,                                         /* bDeviceProtocol */
    0x40,                                         /* bMaxPacketSize */
    LOBYTE(TUSB_FS_DAP_VID), HIBYTE(TUSB_FS_DAP_VID),/* idVendor */
    LOBYTE(TUSB_FS_DAP_PID), HIBYTE(TUSB_FS_DAP_PID),/* idProduct */
    0x00, 0x01,                                   /* bcdDevice */
    0x01,                                         /* iManufacture */
    0x02,                                         /* iProduct */
    0x03,                                         /* iSerial */
    0x01,                                         /* bNumConfigurations */
};

// Configs 
#define TUSB_FS_DAP_REPORT_DESCRIPTOR_SIZE_IF0 33
WEAK __ALIGN_BEGIN const uint8_t TUSB_FS_DAP_ReportDescriptor_if0[TUSB_FS_DAP_REPORT_DESCRIPTOR_SIZE_IF0] __ALIGN_END = {

// report descriptor for general input/output
  0x06, 0x00, 0xFF,  // Usage Page (Vendor Defined 0xFF00)
  0x09, 0x01,        // Usage (0x01)
  0xA1, 0x01,        // Collection (Application)
  0x09, 0x01,        //   Usage (0x01)
  0x15, 0x00,        //   Logical Minimum (0)
  0x26, 0xFF, 0x00,  //   Logical Maximum (255)
  0x75, 0x08,        //   Report Size (8)
  0x95, 0x40,        //   Report Count 64
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x95, 0x40,        //   Report Count 64
  0x09, 0x01,        //   Usage (0x01)
  0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x95, 0x01,        //   Report Count 1
  0x09, 0x01,        //   Usage (0x01)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0xC0               // End Collection

};

#define  TUSB_FS_DAP_CONFIG_DESCRIPTOR_SIZE  (41)
__ALIGN_BEGIN const uint8_t TUSB_FS_DAP_ConfigDescriptor [41] __ALIGN_END = {
    0x09,                                         /* bLength */
    USB_CONFIGURATION_DESCRIPTOR_TYPE,            /* bDescriptorType */
    0x29, 0x00,                                   /* wTotalLength */
    0x01,                                         /* bNumInterfaces */
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
    0x21, 0x00,                                   /* wDescriptorLength1 */
    /* EndPoint descriptor */
    0x07,                                         /* bLength */
    USB_ENDPOINT_DESCRIPTOR_TYPE,                 /* bDescriptorType */
    0x82,                                         /* bEndpointAddress */
    0x03,                                         /* bmAttributes */
    0x40, 0x00,                                   /* wMaxPacketSize */
    0x01,                                         /* bInterval */
    /* EndPoint descriptor */
    0x07,                                         /* bLength */
    USB_ENDPOINT_DESCRIPTOR_TYPE,                 /* bDescriptorType */
    0x01,                                         /* bEndpointAddress */
    0x03,                                         /* bmAttributes */
    0x40, 0x00,                                   /* wMaxPacketSize */
    0x01,                                         /* bInterval */
};

// Power status
#define TUSB_FS_DAP_DEV_STATUS0      (0)
#define TUSB_FS_DAP_DEV_STATUS1      (0)
#define TUSB_FS_DAP_DEV_STATUS    ((TUSB_FS_DAP_DEV_STATUS0) |(TUSB_FS_DAP_DEV_STATUS1) )

TUSB_FS_DAP_TXEP_MAX_SIZE
TUSB_FS_DAP_RXEP_MAX_SIZE

// Strings 
#define TUSB_FS_DAP_STRING_DESCRIPTOR0_STR          "\x09\x04"
#define TUSB_FS_DAP_STRING_DESCRIPTOR0_SIZE          (4)
WEAK __ALIGN_BEGIN const uint8_t  TUSB_FS_DAP_StringDescriptor0 [4] __ALIGN_END = {
    0x04,                                         /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,                   /* bDescriptorType */
    0x09, 0x04,                                   /* wLangID0 */
};
#define TUSB_FS_DAP_STRING_DESCRIPTOR1_STR          "TeenyUSB"
#define TUSB_FS_DAP_STRING_DESCRIPTOR1_SIZE          (18)
WEAK __ALIGN_BEGIN const uint8_t  TUSB_FS_DAP_StringDescriptor1 [18] __ALIGN_END = {
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
#define TUSB_FS_DAP_STRING_DESCRIPTOR2_STR          "TeenyUSB FS CMSIS-DAP"
#define TUSB_FS_DAP_STRING_DESCRIPTOR2_SIZE          (44)
WEAK __ALIGN_BEGIN const uint8_t  TUSB_FS_DAP_StringDescriptor2 [44] __ALIGN_END = {
    0x2c,                                         /* bLength */
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
    'F', 0x00,                                    /* wcChar9 */
    'S', 0x00,                                    /* wcChar10 */
    ' ', 0x00,                                    /* wcChar11 */
    'C', 0x00,                                    /* wcChar12 */
    'M', 0x00,                                    /* wcChar13 */
    'S', 0x00,                                    /* wcChar14 */
    'I', 0x00,                                    /* wcChar15 */
    'S', 0x00,                                    /* wcChar16 */
    '-', 0x00,                                    /* wcChar17 */
    'D', 0x00,                                    /* wcChar18 */
    'A', 0x00,                                    /* wcChar19 */
    'P', 0x00,                                    /* wcChar20 */
};
#define TUSB_FS_DAP_STRING_DESCRIPTOR3_STR          "TUSB123456FS"
#define TUSB_FS_DAP_STRING_DESCRIPTOR3_SIZE          (26)
WEAK __ALIGN_BEGIN const uint8_t  TUSB_FS_DAP_StringDescriptor3 [26] __ALIGN_END = {
    0x1a,                                         /* bLength */
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
    'F', 0x00,                                    /* wcChar10 */
    'S', 0x00,                                    /* wcChar11 */
};
const uint8_t* const TUSB_FS_DAP_StringDescriptors[TUSB_FS_DAP_STRING_COUNT] = {
TUSB_FS_DAP_StringDescriptor0,
TUSB_FS_DAP_StringDescriptor1,
TUSB_FS_DAP_StringDescriptor2,
TUSB_FS_DAP_StringDescriptor3,
};

//  Device descriptors
const tusb_descriptors TUSB_FS_DAP_descriptors = {
  .device = TUSB_FS_DAP_DeviceDescriptor,
  .config = TUSB_FS_DAP_ConfigDescriptor,
  .strings = TUSB_FS_DAP_StringDescriptors,
  .string_cnt = TUSB_FS_DAP_STRING_COUNT,
#if defined(HAS_WCID)
#if defined(TUSB_FS_DAP_WCID_DESCRIPTOR_SIZE)
  .wcid_desc = TUSB_FS_DAP_WCIDDescriptor,
#else
  .wcid_desc = 0,  
#endif // TUSB_FS_DAP_WCID_DESCRIPTOR_SIZE)

#if defined(TUSB_FS_DAP_WCID_PROPERTIES_SIZE)
  .wcid_properties = TUSB_FS_DAP_WCIDProperties,
#else
  .wcid_properties = 0,  
#endif // TUSB_FS_DAP_WCID_PROPERTIES_SIZE

#endif // HAS_WCID

#if defined(HAS_WCID_20)
#if defined(TUSB_FS_DAP_WCID_BOS_SIZE)
  .wcid_bos = TUSB_FS_DAP_WCIDBOS,
#else
  .wcid_bos = 0,  
#endif // TUSB_FS_DAP_WCID_BOS_SIZE)

#if defined(TUSB_FS_DAP_WCID_DESC_SET_SIZE)
  .wcid_desc_set = TUSB_FS_DAP_WCIDDescriptorSet,
#else
  .wcid_desc_set = 0,  
#endif // TUSB_FS_DAP_WCID_DESC_SET_SIZE


#endif // HAS_WCID_20
};
/////////////////////////////////////////////////////////////////////
//// Descriptor for device2  define end
/////////////////////////////////////////////////////////////////////





