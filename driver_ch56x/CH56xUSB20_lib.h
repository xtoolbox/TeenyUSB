#ifndef USB20_CH56X_USB20_LIB_H_
#define USB20_CH56X_USB20_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "CH56x_common.h"
#include "CH56x_usb20.h"


static inline void USB20_Device_Setaddress( UINT32 address )
{
    USBHS->USB_CONTROL &= 0x00ffffff;
    USBHS->USB_CONTROL |= (UINT32)address<<24; // SET ADDRESS
}


#ifdef __cplusplus
}
#endif

#endif
