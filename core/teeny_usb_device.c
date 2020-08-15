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
 * TeenyUSB - light weight usb stack for micro controllers
 * 
 * Copyright (c) 2020 XToolBox  - admin@xtoolbox.org
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

#include "teeny_usb_device.h"
#include "teeny_usb_device_driver.h"
#include "teeny_usb_util.h"
#include "string.h"

#ifndef TUSB_WCID_VENDOR_CODE
#define TUSB_WCID_VENDOR_CODE 0x17
#endif

static void tusb_setup_handler(tusb_device_t *dev);

int tusb_device_ep_xfer_done(tusb_device_driver_t *drv, uint8_t EPn, const void *data, int len, uint8_t isSetup)
{
    tusb_device_t *dev = (tusb_device_t *)tusb_dev_drv_get_context(drv);
    if (dev)
    {
        if (EPn == 0x00)
        {
            if (isSetup)
            {
                // endpoint 0, setup data out
                memcpy(&dev->setup, data, len);
                const tusb_setup_packet *s = &dev->setup;
                TUSB_LOGD("Setup data, bReq:0x%02x bmReq:0x%02x wVal:0x%04x wIdx:0x%04x wLen:0x%04x\n",
                          s->bRequest, s->bmRequestType, s->wValue, s->wIndex, s->wLength);
                tusb_setup_handler(dev);
            }else if (dev->ep0_rx_done)
            {
                dev->ep0_rx_done(dev, data, len);
                dev->ep0_rx_done = NULL;
            }
        }
        else if (EPn == 0x80)
        {
            if (dev->ep0_tx_done)
            {
                dev->ep0_tx_done(dev, data, len);
                dev->ep0_tx_done = NULL;
            }
            // prepare receive status
            tusb_set_recv_buffer(dev, 0, NULL, 0);
            tusb_set_rx_valid(dev, 0);
        }
        else if (EPn & 0x80)
        {
            tusb_on_tx_done(dev, EPn & 0x7f, data, len);
        }
        else
        {
            return tusb_on_rx_done(dev, EPn, data, len);
        }
    }
    else
    {
        TUSB_LOGE("Driver's device not set\n");
    }
    return 0;
}

void tusb_device_reset(tusb_device_driver_t *drv)
{
    tusb_device_t *dev = (tusb_device_t *)tusb_dev_drv_get_context(drv);
    if (dev)
    {
        // backup driver, user data, descriptors
        tusb_device_driver_t *dev_drv = dev->dev_drv;
        void *user_data = dev->user_data;
        const tusb_descriptors_t *descriptors = dev->descriptors;
        memset(dev, 0, sizeof(tusb_device_t));
        TUSB_LOGD("Device reset\n");
        // restore driver, user data, descriptors
        dev->dev_drv = dev_drv;
        dev->user_data = user_data;
        dev->descriptors = descriptors;
        if (dev->descriptors->device)
        {
            // setup endpoint 0
            tusb_ep_config epc[2];
            epc[0].addr = 0x80;
            epc[0].attribute = USBD_EP_TYPE_CTRL;
            epc[0].mps = dev->descriptors->device[7];
            epc[1].addr = 0x00;
            epc[1].attribute = USBD_EP_TYPE_CTRL;
            epc[1].mps = dev->descriptors->device[7];
            tusb_dev_drv_setup_endpoint(drv, epc, 2, 1);
        }
        else
        {
            TUSB_LOGE("Device descriptor not set\n");
        }
    }
    else
    {
        TUSB_LOGE("Driver's device not set\n");
    }
}

// Default class request handler
// User need override it if the class need some special request
WEAK int tusb_class_request(tusb_device_t *dev, tusb_setup_packet *setup_req)
{
    return 0;
}

WEAK void tusb_reconfig(tusb_device_t *dev)
{
    TUSB_LOGD("Device configured\n");
}

#if defined(TUSB_SUPPORT_WCID)
static __ALIGN_BEGIN const uint8_t tusb_wcid_msos_string_descriptor[18] __ALIGN_END = {
    ///////////////////////////////////////
    /// MS OS string descriptor
    ///////////////////////////////////////
    0x12,                       /* bLength */
    USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */
    /* MSFT100 */
    'M', 0x00, 'S', 0x00, 'F', 0x00, 'T', 0x00, /* wcChar_7 */
    '1', 0x00, '0', 0x00, '0', 0x00,            /* wcChar_7 */
    TUSB_WCID_VENDOR_CODE,                      /* bVendorCode */
    0x00,                                       /* bReserved */
};
#endif //#if defined(TUSB_SUPPORT_WCID)

// Standard get descriptor function
static int tusb_get_descriptor(tusb_device_t *dev, tusb_setup_packet *req)
{
    const uint8_t *desc = 0;
    uint16_t len = 0;
    switch (HI_BYTE(req->wValue))
    {
    case USB_DESC_TYPE_DEVICE:
        TUSB_LOGD("Get device descriptor\n");
        desc = dev->descriptors->device;
        if (desc)
            len = desc[0];
        break;
    case USB_DESC_TYPE_CONFIGURATION:
    {
        uint8_t index = LO_BYTE(req->wValue);
        TUSB_LOGD("Get config descriptor, %d\n", index);
        if (index < dev->descriptors->config_count)
        {
            desc = dev->descriptors->configs[index];
            if (desc)
                len = *((uint16_t *)desc + 1);
        }
        else
        {
            TUSB_LOGE("Device config descriptor index out of range, %d\n", index);
        }
    }
    break;
    case USB_DESC_TYPE_STRING:
    {
        uint8_t index = LO_BYTE(req->wValue);
        TUSB_LOGD("Get string descriptor, %d\n", index);
        if (index < dev->descriptors->string_cnt)
        {
            desc = dev->descriptors->strings[index];
            if (desc)
                len = desc[0];
        }
#if defined(TUSB_SUPPORT_WCID)
        else if (index == 0xee && dev->descriptors->extra && dev->descriptors->extra->wcid_desc)
        {
            desc = tusb_wcid_msos_string_descriptor;
            len = desc[0];
        }
#else
        else if (index == 0xee)
        {
            TUSB_LOGE("WCID descriptors is not support, define TUSB_SUPPORT_WCID to enable it\n");
        }
#endif //#if defined(TUSB_SUPPORT_WCID)
        break;
    }
    case USB_DESC_TYPE_BOS:
#if defined(TUSB_SUPPORT_WCID)
    {
        TUSB_LOGD("Get BOS descriptor\n");
        if (dev->descriptors && dev->descriptors->extra && dev->descriptors->extra->wcid_bos)
        {
            desc = dev->descriptors->extra->wcid_bos;
            if (desc)
                len = *((uint16_t *)desc + 1);
        }
        else
        {
            TUSB_LOGW("WCID2 BOS not set\n");
        }
        break;
    }
#else
    {
        TUSB_LOGE("BOS WCID descriptor is not support, define TUSB_SUPPORT_WCID to enable it\n");
    }
#endif //#if defined(TUSB_SUPPORT_WCID)

    // For High speed device, support the DEVICE_QUALIFIER
    // and OTHER_SPEED_CONFIGURATION request
    case USB_DESC_TYPE_DEVICE_QUALIFIER:
    {
        TUSB_LOGD("Get device qualifier descriptor\n");
#if defined(DESCRIPTOR_BUFFER_SIZE) && DESCRIPTOR_BUFFER_SIZE > 0

        if (8 > DESCRIPTOR_BUFFER_SIZE)
        {
            TUSB_LOGE("Descriptor buffer size too small for qualifier desc, require %d, current %d\n", 8, DESCRIPTOR_BUFFER_SIZE);
            return 0;
        }
        dev->desc_buffer[0] = USB_LEN_DEV_QUALIFIER_DESC;
        dev->desc_buffer[1] = USB_DESC_TYPE_DEVICE_QUALIFIER;
        memcpy(dev->desc_buffer + 2, dev->descriptors->device + 2, 6);
        // set the bNumConfigurations field
        dev->desc_buffer[8] = dev->descriptors->device[17];
        // set bReserved field to zero
        dev->desc_buffer[9] = 0;
        len = USB_LEN_DEV_QUALIFIER_DESC;
        desc = dev->desc_buffer;
#else
        TUSB_LOGE("Device qualifier descriptor is not support, define DESCRIPTOR_BUFFER_SIZE to enable it\n");
        desc = 0;
#endif

        break;
    }
    case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
    {
        TUSB_LOGD("Get other speed descriptor\n");
#if defined(DESCRIPTOR_BUFFER_SIZE) && DESCRIPTOR_BUFFER_SIZE > 0

        uint8_t index = LO_BYTE(req->wValue);
        TUSB_LOGD("Get config descriptor, %d\n", index);
        if (index < dev->descriptors->config_count)
        {
            desc = dev->descriptors->configs[index];
        }
        else
        {
            TUSB_LOGE("Other speed config index out of range %d\n", index);
            return 0;
        }
        if (desc)
            len = *((uint16_t *)desc + 1);
        if (len > DESCRIPTOR_BUFFER_SIZE)
        {
            TUSB_LOGE("Descriptor buffer size too small for other speed desc, require %d, current %d\n", len, DESCRIPTOR_BUFFER_SIZE);
            return 0;
        }

        memcpy(dev->desc_buffer, desc, len);
        dev->desc_buffer[1] = USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION;
        desc = dev->desc_buffer;
#else
        TUSB_LOGE("Other speed descriptor is not support, define DESCRIPTOR_BUFFER_SIZE to enable it\n");
        desc = 0;
#endif
        break;
    }
    }
    if (desc)
    {
        len = req->wLength > len ? len : req->wLength;
#if defined(DESCRIPTOR_BUFFER_SIZE) && DESCRIPTOR_BUFFER_SIZE > 0
        if (len > DESCRIPTOR_BUFFER_SIZE)
        {
            TUSB_LOGE("Descriptor buffer size too small, require %d, current %d\n", len, DESCRIPTOR_BUFFER_SIZE);
            return 0;
        }
        if (dev->desc_buffer != desc)
        {
            memcpy(dev->desc_buffer, desc, len);
            desc = dev->desc_buffer;
        }
#endif
        tusb_control_send(dev, desc, len);
        return 1;
    }
    else
    {
        return 0;
    }
}

static int tusb_vendor_request(tusb_device_t *dev, tusb_setup_packet *setup_req)
{
#if defined(TUSB_SUPPORT_WCID)
    uint32_t len = 0;
    const uint8_t *desc = 0;
    if (setup_req->bRequest == TUSB_WCID_VENDOR_CODE)
    {
        switch (setup_req->wIndex)
        {
            //#if defined(HAS_WCID)
        case 4:
        {
            TUSB_LOGD("Get WCID descriptor\n");
            if (dev->descriptors && dev->descriptors->extra && dev->descriptors->extra->wcid_desc)
            {
                desc = dev->descriptors->extra->wcid_desc;
                len = desc[0] + (desc[1] << 8) + (desc[2] << 16) + (desc[3] << 24);
            }
            else
            {
                TUSB_LOGW("WCID descriptor not set\n");
            }
            break;
        }
        case 5:
        {
            TUSB_LOGD("Get WCID properties, %d\n", setup_req->wValue);
            if (dev->descriptors && dev->descriptors->extra && dev->descriptors->extra->wcid_properties)
            {
                desc = dev->descriptors->extra->wcid_properties[setup_req->wValue];
                if (desc)
                {
                    len = desc[0] + (desc[1] << 8) + (desc[2] << 16) + (desc[3] << 24);
                }
            }
            else
            {
                TUSB_LOGW("WCID properties not set\n");
            }
            break;
        }
            //#endif
            //#if defined(HAS_WCID_20)
        case 7:
        {
            TUSB_LOGD("Get WCID2 descriptor set\n");
            if (dev->descriptors && dev->descriptors->extra && dev->descriptors->extra->wcid_desc_set)
            {
                desc = dev->descriptors->extra->wcid_desc_set;
                if (desc)
                    len = *(uint16_t *)(desc + 8);
            }
            else
            {
                TUSB_LOGW("WCID2 descriptors set not set\n");
            }
            break;
        }
            //#endif
        }
    }else{
        TUSB_LOGW("Unknown Vendor request code %d\n", setup_req->bRequest);
    }
    // TODO: Handle length large than 0xffff
    len = setup_req->wLength > len ? len : setup_req->wLength;
#if defined(DESCRIPTOR_BUFFER_SIZE) && DESCRIPTOR_BUFFER_SIZE > 0
    if (len > DESCRIPTOR_BUFFER_SIZE)
    {
        TUSB_LOGE("Descriptor buffer size too small for vendor desc, require %d, current %d\n", (int)len, DESCRIPTOR_BUFFER_SIZE);
        return 0;
    }
    if (desc)
    {
        if (dev->desc_buffer != desc)
        {
            memcpy(dev->desc_buffer, desc, len);
            desc = dev->desc_buffer;
        }
    }
#endif
    if (desc)
    {
        tusb_control_send(dev, desc, len);
        return 1;
    }
    else
    {
        TUSB_LOGD("Vendor request fail, stall it\n");
    }
#else
    if (setup_req->bRequest == TUSB_WCID_VENDOR_CODE)
    {
        TUSB_LOGE("WCID Vendor request is not support, define TUSB_SUPPORT_WCID to enable it\n");
    }
#endif // #if defined(TUSB_SUPPORT_WCID)
    
    tusb_set_stall(dev, 0);
    return 1;
}

// Set device address after send status
static inline void tusb_set_addr_after_status(tusb_device_t *dev, const void *data, int len)
{
    tusb_dev_drv_set_address(GetDriver(dev), dev->addr, 1);
}

// init the endpoint from config descriptor
static int tusb_init_endpoint_by_config(tusb_device_t *dev)
{
    tusb_ep_config ep_cfg[TUSB_MAX_EP_PAIR_COUNT * 2];
    if (!dev->descriptors)
    {
        TUSB_LOGE("Device descriptors not set\n");
        return -1;
    }
    if (!dev->descriptors->configs || !dev->descriptors->config_count)
    {
        TUSB_LOGE("Device config descriptor not set\n");
        return -1;
    }
    if (!dev->config)
    {
        TUSB_LOGE("Wrong config number, %d\n", dev->config);
        return -1;
    }
    if (dev->descriptors->config_count < dev->config)
    {
        TUSB_LOGE("Device config descriptor number (%d) less than request (%d)\n",
                  (int)dev->descriptors->config_count, dev->config);
        return -1;
    }
    desc_t cfg = dev->descriptors->configs[dev->config - 1];
    if (!cfg)
    {
        TUSB_LOGE("Config descritor not set at %d\n", dev->config);
        return -1;
    }
    uint16_t total_len = cfg[2] | (cfg[3] << 8);
    uint16_t index = 0;
    uint8_t bmAttr = cfg[7];
    dev->status = 0;
    if (bmAttr & 0x40)
    {
        // bus powered
        SetBusPowered(dev);
    }
    if (bmAttr & 0x20)
    {
        // remote wakeup
        SetRemoteWakeup(dev);
    }
    ep_cfg[0].addr = 0x80;
    ep_cfg[0].attribute = USBD_EP_TYPE_CTRL;
    ep_cfg[0].mps = dev->descriptors->device[7];
    ep_cfg[1].addr = 0x00;
    ep_cfg[1].attribute = USBD_EP_TYPE_CTRL;
    ep_cfg[1].mps = dev->descriptors->device[7];
    int valid_intf = 0;
    int ep_count = 2;
    // collect endpoint config here
    while (index < total_len)
    {
        uint8_t bLength = cfg[index];
        uint8_t bType = cfg[index + 1];
        if (bType == USB_INTERFACE_DESCRIPTOR_TYPE)
        {
            valid_intf = 0;
            uint8_t bIntfNum = cfg[index + 2];
            uint8_t bIntfAlt = cfg[index + 3];
            if (bIntfNum < TUSB_MAX_INTERFACE_COUNT)
            {
                if (dev->alt_cfg[bIntfNum] == bIntfAlt)
                {
                    valid_intf = 1;
                }
            }
            else
            {
                TUSB_LOGW("Interface number(%d) large than TUSB_MAX_INTERFACE_COUNT, ignore it\n", bIntfNum);
            }
        }
        else if (bType == USB_ENDPOINT_DESCRIPTOR_TYPE)
        {
            if (valid_intf)
            {
                if (ep_count < TUSB_MAX_EP_PAIR_COUNT * 2)
                {
                    uint8_t ep_addr = cfg[index + 2];
                    if ((ep_addr & 0x7f) < TUSB_MAX_EP_PAIR_COUNT)
                    {
                        tusb_ep_config *epc = &ep_cfg[ep_count];
                        epc->addr = ep_addr;
                        epc->attribute = cfg[index + 3];
                        epc->mps = cfg[index + 4] | (cfg[index + 5] << 8);
                        ep_count++;
                    }
                    else
                    {
                        TUSB_LOGW("Endpoint addr(0x%02x) large than TUSB_MAX_EP_PAIR_COUNT, ignore it\n", ep_addr);
                    }
                }
                else
                {
                    TUSB_LOGW("Endpoint count overflow, ignore it\n");
                }
            }
        }
        index += bLength;
    }
    TUSB_LOGD("Endpoint config summary:\n");
    for (int i = 0; i < ep_count; i++)
    {
        const tusb_ep_config *epc = &ep_cfg[i];
        TUSB_LOGD("Addr: 0x%02x Attr: 0x%02x MPS:%d\n", epc->addr, epc->attribute, epc->mps);
    }
    tusb_dev_drv_setup_endpoint(GetDriver(dev), ep_cfg, ep_count, 0);
    return 0;
}

// Standard request process function
static int tusb_std_device_request(tusb_device_t *dev, tusb_setup_packet *setup_req)
{
    switch (setup_req->bRequest)
    {
    case USB_REQ_SET_ADDRESS:
        dev->addr = LO_BYTE(setup_req->wValue);
        tusb_dev_drv_set_address(GetDriver(dev), dev->addr, 0);
        dev->ep0_tx_done = tusb_set_addr_after_status;
        TUSB_LOGD("Device set address to %d\n", dev->addr);
        tusb_send_status(dev);
        return 1;
    case USB_REQ_GET_DESCRIPTOR:
        return tusb_get_descriptor(dev, setup_req);
    case USB_REQ_SET_DESCRIPTOR:
        // TODO: add support for set descriptor
        TUSB_LOGW("Device set descriptor not supported\n");
        return 0;
    case USB_REQ_GET_STATUS:
        dev->temp_buffer = dev->status;
        TUSB_LOGD("Device get status 0x%04x\n", dev->status);
        tusb_control_send(dev, &dev->temp_buffer, 2);
        return 1;
    case USB_REQ_GET_CONFIGURATION:
        dev->temp_buffer = dev->config;
        TUSB_LOGD("Device get configuration %d\n", dev->config);
        tusb_control_send(dev, &dev->temp_buffer, 1);
        return 1;
    case USB_REQ_SET_CONFIGURATION:
        dev->config = LO_BYTE(setup_req->wValue);
        TUSB_LOGD("Device set configuration %d\n", dev->config);
        tusb_init_endpoint_by_config(dev);
        tusb_reconfig(dev);
        tusb_send_status(dev);
        return 1;
    case USB_REQ_CLEAR_FEATURE:
        if (setup_req->wValue == USB_FEATURE_REMOTE_WAKEUP)
        {
            TUSB_LOGD("Device clear feature, remote wakeup\n");
            ClearRemoteWakeup(dev);
            tusb_send_status(dev);
            return 1;
        }
        TUSB_LOGW("Device clear feature, unknown %d\n", setup_req->wValue);
        return 0;
    case USB_REQ_SET_FEATURE:
        if (setup_req->wValue == USB_FEATURE_REMOTE_WAKEUP)
        {
            TUSB_LOGD("Device set feature, remote wakeup\n");
            SetRemoteWakeup(dev);
            tusb_send_status(dev);
            return 1;
        }
        else if (setup_req->wValue == USB_FEATURE_TEST_MODE)
        {
            TUSB_LOGD("Device set feature, test mode\n");
            tusb_send_status(dev);
            return 1;
        }
        TUSB_LOGW("Device set feature, unknown, %d\n", setup_req->wValue);
        return 0;
    default:
        TUSB_LOGW("Device request unknown, %d\n", setup_req->bRequest);
        break;
    }
    return 0;
}

static int tusb_std_interface_request(tusb_device_t *dev, tusb_setup_packet *setup_req)
{
    switch (setup_req->bRequest)
    {
    case USB_REQ_GET_INTERFACE:
        if (setup_req->wIndex < TUSB_MAX_INTERFACE_COUNT)
        {
            dev->temp_buffer = dev->alt_cfg[setup_req->wIndex];
            TUSB_LOGW("Interface, get interface %d, alt = %d\n", setup_req->wIndex, dev->temp_buffer);
            tusb_control_send(dev, &dev->temp_buffer, 1);
            return 1;
        }
        TUSB_LOGW("Interface, get unknown interface %d\n", setup_req->wIndex);
        return 0;
    case USB_REQ_SET_INTERFACE:
        if (setup_req->wIndex < TUSB_MAX_INTERFACE_COUNT)
        {
            if (dev->alt_cfg[setup_req->wIndex] != setup_req->wValue)
            {
                dev->alt_cfg[setup_req->wIndex] = setup_req->wValue;
                // reset the endpoint config here
                tusb_init_endpoint_by_config(dev);
            }
            TUSB_LOGW("Interface, set interface %d, alt = %d\n", setup_req->wIndex, setup_req->wValue);
            tusb_control_send(dev, &dev->temp_buffer, 1);
            return 1;
        }
        TUSB_LOGW("Interface, set unknown interface %d\n", setup_req->wIndex);
        return 0;
    case USB_REQ_CLEAR_FEATURE:
        TUSB_LOGW("Interface set feature, unknown %d\n", setup_req->wValue);
        return 0;
    case USB_REQ_SET_FEATURE:
        TUSB_LOGW("Interface clear feature, unknown %d\n", setup_req->wValue);
        return 0;
    default:
        TUSB_LOGW("Interface request unknown %d\n", setup_req->bRequest);
        break;
    }
    return 0;
}

static int tusb_std_endpoint_request(tusb_device_t *dev, tusb_setup_packet *setup_req)
{
    switch (setup_req->bRequest)
    {
    case USB_REQ_CLEAR_FEATURE:
        if (setup_req->wValue == USB_FEATURE_EP_HALT)
        {
            TUSB_LOGD("Endpoint clear feature, ep halt\n");
            uint8_t ep_addr = LO_BYTE(setup_req->wIndex);
            if ((ep_addr & 0x7f) < TUSB_MAX_EP_PAIR_COUNT)
            {
                tusb_clear_stall(dev, ep_addr);
            }
            tusb_send_status(dev);
            return 1;
        }
        TUSB_LOGW("Endpoint clear feature, unknown %d\n", setup_req->wValue);
        return 0;
    case USB_REQ_SET_FEATURE:
        if (setup_req->wValue == USB_FEATURE_EP_HALT)
        {
            TUSB_LOGD("Endpoint set feature, ep halt\n");
            uint8_t ep_addr = LO_BYTE(setup_req->wIndex);
            if ((ep_addr & 0x7f) < TUSB_MAX_EP_PAIR_COUNT)
            {
                tusb_set_stall(dev, ep_addr);
            }
            tusb_send_status(dev);
            return 1;
        }
        TUSB_LOGW("Endpoint set feature, unknown, %d\n", setup_req->wValue);
        return 0;
    case USB_REQ_SYNCH_FRAME:
        TUSB_LOGW("Endpoint sync frame not support, %d\n", setup_req->wValue);
        // TODO:
        // dev->temp_buffer = <current frame number>;
        // tusb_control_send(dev, &dev->temp_buffer, 2);
        return 0;
    default:
        TUSB_LOGW("Device request unknown, %d\n", setup_req->bRequest);
        break;
    }
    return 0;
}

static int tusb_standard_request(tusb_device_t *dev, tusb_setup_packet *setup_req)
{
    switch (setup_req->bmRequestType & USB_REQ_RECIPIENT_MASK)
    {
    case USB_REQ_RECIPIENT_DEVICE:
        return tusb_std_device_request(dev, setup_req);
    case USB_REQ_RECIPIENT_INTERFACE:
        return tusb_std_interface_request(dev, setup_req);
    case USB_REQ_RECIPIENT_ENDPOINT:
        return tusb_std_endpoint_request(dev, setup_req);
    default:
        TUSB_LOGW("Unknown standard request RECIPIENT %d\n", setup_req->bmRequestType & USB_REQ_RECIPIENT_MASK);
    }
    return 0;
}

// forward declare for some internal function
void tusb_recv_data(tusb_device_t *dev, uint8_t EPn);
void tusb_send_data_done(tusb_device_t *dev, uint8_t EPn);
uint32_t tusb_read_ep0(tusb_device_t *dev, void *buf);
void tusb_read_data(tusb_device_t *dev, void *buf, uint32_t len);

// Setup packet handler
void tusb_setup_handler(tusb_device_t *dev)
{
    tusb_setup_packet *setup_req = &dev->setup;
    // we pass all request to tusb_class_request, not only class request
    if (tusb_class_request(dev, setup_req))
    {
        return;
    }
    if ((setup_req->bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_VENDOR)
    {
        if (tusb_vendor_request(dev, setup_req))
        {
            return;
        }
    }
    else if ((setup_req->bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_STANDARD)
    {
        if (!tusb_standard_request(dev, setup_req))
        {
            tusb_set_stall(dev, 0); // stall ep0 in both direction, both status in/out will get the right state
        }
    }
}

// Public callback function override by user
// default data transmit done callback
WEAK void tusb_on_tx_done(tusb_device_t *dev, uint8_t EPn, const void *data, int len)
{
}

// default data received  callback, return 0 will continue receive data in end point rx buffer
// return other value will cause the OUT ep NAX, user can active the OUT ep by call tusb_set_rx_valid
WEAK int tusb_on_rx_done(tusb_device_t *dev, uint8_t EPn, const void *data, int len)
{
    return 0;
}
