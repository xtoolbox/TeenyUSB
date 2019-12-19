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
 * TeenyUSB - light weight usb stack for STM32 micro controllers
 * 
 * Copyright (c) 2019 XToolBox  - admin@xtoolbox.org
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

#include "tusbh_msc.h"
#include "../tusb_msc.h"
#include "string.h"

#define CBW_DIR_IN   0x80
#define CBW_DIR_OUT  0x00

#define GET_BE16(field)  \
   ( ( (uint16_t)(field)[0]<<8)  \
   | ( (uint16_t)(field) [1])    )

#define GET_BE32(field)  \
   ( ( (uint32_t)(field)[0]<<24) \
   | ( (uint32_t)(field)[1]<<16) \
   | ( (uint32_t)(field)[2]<< 8) \
   | ( (uint32_t)(field)[3]<< 0) )

#define SET_BE16(field, value)  \
  do{                                      \
    (field)[0] = (uint8_t) ((value)>>8);   \
    (field)[1] = (uint8_t) ((value)>>0);   \
  }while(0)

#define SET_BE24(field, value)  \
  do{                                       \
    (field)[0] = (uint8_t) ((value)>>16);   \
    (field)[1] = (uint8_t) ((value)>> 8);   \
    (field)[2] = (uint8_t) ((value)>> 0);   \
  }while(0)

#define SET_BE32(field, value)  \
  do{                                      \
    (field)[0] = (uint8_t) ((value)>>24);  \
    (field)[1] = (uint8_t) ((value)>>16);  \
    (field)[2] = (uint8_t) ((value)>> 8);  \
    (field)[3] = (uint8_t) ((value)>> 0);  \
  }while(0)

#define MSC_GetTag()      0x11223344
#define MSC_CheckTag(tag) (0x11223344 == (tag))

static const char* sense_key_name(uint8_t key)
{
 #define  FF(x)  case SCSI_##x: return #x;
    switch(key){
FF(SENSE_NONE              ) 
FF(SENSE_RECOVERED_ERROR   )     
FF(SENSE_NOT_READY         )     
FF(SENSE_MEDIUM_ERROR      )     
FF(SENSE_HARDWARE_ERROR    )     
FF(SENSE_ILLEGAL_REQUEST   )     
FF(SENSE_UNIT_ATTENTION    )     
FF(SENSE_DATA_PROTECT      )     
FF(SENSE_FIRMWARE_ERROR    )     
FF(SENSE_ABORTED_COMMAND   )     
FF(SENSE_EQUAL             )     
FF(SENSE_VOLUME_OVERFLOW   )     
FF(SENSE_MISCOMPARE        )     
    }
#undef   FF
    return "SENSE Unknown";
}

static const char* sense_code_name(uint8_t code)
{
 #define  FF(x)  case x: return #x;
    switch(code){
FF(INVALID_CDB                       )
FF(INVALID_FIELED_IN_COMMAND         )
FF(PARAMETER_LIST_LENGTH_ERROR       )
FF(INVALID_FIELD_IN_PARAMETER_LIST   )
FF(ADDRESS_OUT_OF_RANGE              )
FF(MEDIUM_NOT_PRESENT                )
FF(MEDIUM_HAVE_CHANGED               )
FF(WRITE_PROTECTED                   )
FF(UNRECOVERED_READ_ERROR            )
FF(WRITE_FAULT                       )
    }
#undef   FF
    return "SENSE Code Unknown";
}

static int tusbh_msc_request_sense(tusbh_interface_t* interface, uint8_t lun, scsi_sense_fixed_resp_t* response);


static int tusbh_msc_bot_xfer(tusbh_interface_t* interface, tusb_msc_cbw_t* cbw, void* buffer)
{
    tusbh_msc_info_t* info = tusbh_get_info(interface, tusbh_msc_info_t);
    TUSB_ASSERT(info && info->in_ep && info->out_ep);
    tusb_msc_csw_t csw;
    tusbh_ep_info_t* ep = info->out_ep;
    int res = 0;
    do{
        res = tusbh_ep_xfer(ep, cbw, BOT_CBW_LENGTH, 5000);
        if(res<0){
            break;
        }
        if(cbw->total_bytes){
            ep = cbw->dir & CBW_DIR_IN ? info->in_ep : info->out_ep;
            res = tusbh_ep_xfer(ep, buffer, cbw->total_bytes, 10000);
            if(res<0){
                break;
            }
        }
        ep = info->in_ep;
        res = tusbh_ep_xfer(ep, &csw, BOT_CSW_LENGTH, 5000);
        if(res<0){
            break;
        }
    }while(0);
    if(res == -(int)TUSB_CS_STALL){
        // clear stall
        tusbh_ep_clear_feature(ep);
        TUSB_ITF_INFO("MSC ep %02x stall\n", ep->desc->bEndpointAddress);
        
        ep = info->in_ep;
        int r = tusbh_ep_xfer(ep, &csw, BOT_CSW_LENGTH, 2000);
        if(r == -(int)TUSB_CS_STALL){
            TUSB_ITF_INFO("MSC stall CSW res = %d Fail\n", r);
        }else{
            TUSB_ITF_INFO("MSC stall CSW res = %d, residue %d, status %d\n", 
                r, (int)csw.data_residue, csw.status);
        }
    }
    
    if(  res != BOT_CSW_LENGTH
      || csw.tag != cbw->tag
      || csw.signature != MSC_CSW_SIGNATURE
      || csw.status != MSC_CSW_STATUS_PASSED
    ){
        TUSB_ITF_INFO("MSC xfer fail, res = %d, residue %d, status %d\n",
        res, (int)csw.data_residue, csw.status);
        return -1;
    }
    return res;
}

int tusbh_msc_block_read(tusbh_interface_t* interface, int lun, uint32_t blockAddr, uint32_t blockCount, void* buffer)
{
    tusbh_msc_info_t* info = tusbh_get_info(interface, tusbh_msc_info_t);
    tusb_msc_cbw_t cbw;
    cbw.signature = MSC_CBW_SIGNATURE;
    cbw.tag = MSC_GetTag();
    cbw.total_bytes = info->blocks[lun].block_size * blockCount;
    cbw.dir = CBW_DIR_IN;
    cbw.lun = lun;
    cbw.cmd_len = sizeof(scsi_read_10_cmd_t);
    scsi_read_10_cmd_t* cmd = (scsi_read_10_cmd_t*)cbw.command;
    memset(cmd, 0, cbw.cmd_len);
    cmd->cmd_code = SCSI_CMD_READ_10;
    SET_BE32(cmd->logical_block_addr, blockAddr);
    SET_BE16(cmd->transfer_length, blockCount);
    int res = tusbh_msc_bot_xfer(interface, &cbw, buffer);
    if(res < 0){
        TUSB_ITF_INFO("MSC block read fail\n");
        tusbh_msc_request_sense(interface, lun, &info->sense);
    }
    return res;
}

int tusbh_msc_block_write(tusbh_interface_t* interface, int lun, uint32_t blockAddr, uint32_t blockCount, void* buffer)
{
    tusbh_msc_info_t* info = tusbh_get_info(interface, tusbh_msc_info_t);
    tusb_msc_cbw_t cbw;
    cbw.signature = MSC_CBW_SIGNATURE;
    cbw.tag = MSC_GetTag();
    cbw.total_bytes = info->blocks[lun].block_size * blockCount;
    cbw.dir = CBW_DIR_OUT;
    cbw.lun = lun;
    cbw.cmd_len = sizeof(scsi_write_10_cmd_t);
    scsi_write_10_cmd_t* cmd = (scsi_write_10_cmd_t*)cbw.command;
    memset(cmd, 0, cbw.cmd_len);
    cmd->cmd_code = SCSI_CMD_WRITE_10;
    SET_BE32(cmd->logical_block_addr, blockAddr);
    SET_BE16(cmd->transfer_length, blockCount);
    int res = tusbh_msc_bot_xfer(interface, &cbw, buffer);
    if(res < 0){
        TUSB_ITF_INFO("MSC block write fail\n");
        tusbh_msc_request_sense(interface, lun, &info->sense);
    }
    return res;
}

int tusbh_msc_is_unit_ready(tusbh_interface_t* interface, int lun)
{
    tusb_msc_cbw_t cbw;
    cbw.signature = MSC_CBW_SIGNATURE;
    cbw.tag = MSC_GetTag();
    cbw.total_bytes = 0;
    cbw.dir = CBW_DIR_OUT;
    cbw.lun = lun;
    cbw.cmd_len = sizeof(scsi_test_unit_ready_cmd_t);
    scsi_test_unit_ready_cmd_t* cmd = (scsi_test_unit_ready_cmd_t*)cbw.command;
    cmd->cmd_code = SCSI_CMD_TEST_UNIT_READY;
    cmd->lun = lun;
    int res = tusbh_msc_bot_xfer(interface, &cbw, 0);
    if(res<0)return 0;
    return 1;
}

static int tusbh_msc_read_capacity(tusbh_interface_t* interface, uint8_t lun)
{
    tusbh_msc_info_t* info = tusbh_get_info(interface, tusbh_msc_info_t);
    tusb_msc_cbw_t cbw;
    cbw.signature = MSC_CBW_SIGNATURE;
    cbw.tag = MSC_GetTag();
    cbw.total_bytes = sizeof(scsi_read_capacity_10_resp_t);
    cbw.dir = CBW_DIR_IN;
    cbw.lun = lun;
    cbw.cmd_len = sizeof(scsi_read_capacity_10_cmd_t);
    scsi_read_capacity_10_cmd_t* cmd = (scsi_read_capacity_10_cmd_t*)cbw.command;
    cmd->cmd_code = SCSI_CMD_READ_CAPACITY_10;
    SET_BE32(cmd->logical_block_address, 0);
    cmd->partial_medium_indicator = 0;
    scsi_read_capacity_10_resp_t response;
    
    int res = tusbh_msc_bot_xfer(interface, &cbw, &response);
    if(res<0)return res;
    info->blocks[lun].block_count = GET_BE32(response.last_logical_block_address) + 1;
    info->blocks[lun].block_size = GET_BE32(response.block_size);
    return 0;
}

static int tusbh_msc_request_sense(tusbh_interface_t* interface, uint8_t lun, scsi_sense_fixed_resp_t* response)
{
    tusb_msc_cbw_t cbw;
    cbw.signature = MSC_CBW_SIGNATURE;
    cbw.tag = MSC_GetTag();
    cbw.total_bytes = sizeof(scsi_sense_fixed_resp_t);
    cbw.dir = CBW_DIR_IN;
    cbw.lun = lun;
    cbw.cmd_len = sizeof(scsi_request_sense_cmd_t);
    scsi_request_sense_cmd_t* cmd = (scsi_request_sense_cmd_t*)cbw.command;
    cmd->cmd_code = SCSI_CMD_REQUEST_SENSE,
    cmd->length = sizeof(scsi_sense_fixed_resp_t);
    
    int res = tusbh_msc_bot_xfer(interface, &cbw, response);
    if(res<0){
        TUSB_ITF_INFO("MSC Sense fail\n");
        return res;
    }
    TUSB_ITF_INFO("MSC Sense: %s, %s, qua %d\n", sense_key_name(response->sense_key), 
        sense_code_name(response->add_sense_code), response->add_sense_qualifier);
    return res;
}

static int tusbh_msc_inquiry(tusbh_interface_t* interface, uint8_t lun)
{
    tusbh_msc_info_t* info = tusbh_get_info(interface, tusbh_msc_info_t);
    tusb_msc_cbw_t cbw;
    cbw.signature = MSC_CBW_SIGNATURE;
    cbw.tag = MSC_GetTag();
    cbw.total_bytes = sizeof(scsi_inquiry_std_response_t);
    cbw.dir = CBW_DIR_IN;
    cbw.lun = lun;
    cbw.cmd_len = sizeof(scsi_inquiry_cmd_t);
    scsi_inquiry_cmd_t* cmd = (scsi_inquiry_cmd_t*)cbw.command;
    cmd->cmd_code = SCSI_CMD_INQUIRY,
    cmd->EVPD = 0; // make sure EVPD is zero
    SET_BE16(cmd->length, sizeof(scsi_inquiry_std_response_t));
    
    return tusbh_msc_bot_xfer(interface, &cbw, &info->blocks[lun].inquiry);
}

static int tusbh_msc_init(tusbh_device_t* dev, tusbh_interface_t* interface, int cfg_offset)
{
    usb_interface_descriptor_t* itf = (usb_interface_descriptor_t*)(dev->config_desc+cfg_offset);
    uint16_t wTotalLength = ((usb_config_descriptor_t*)dev->config_desc)->wTotalLength;
    uint8_t ep_index = 0;
    uint8_t itf_cnt = 0;
    interface->ep_num = itf->bNumEndpoints;
    create_info_pool(interface, tusbh_msc_info_t);
    memset(interface->info_pool,0,sizeof(tusbh_msc_info_t));
    tusbh_msc_info_t* info = tusbh_get_info(interface, tusbh_msc_info_t);
    memset(interface->endpoints, 0, sizeof(interface->endpoints));
    int r = 0;
    
    while(cfg_offset < wTotalLength){
        uint8_t len = dev->config_desc[cfg_offset];
        uint8_t t = dev->config_desc[cfg_offset+1];
        if(t == USB_ENDPOINT_DESCRIPTOR_TYPE){
            usb_endpoint_descriptor_t* ep = (usb_endpoint_descriptor_t*)(dev->config_desc+cfg_offset);
            if(ep->bEndpointAddress & 0x80){
                if(!info->in_ep){
                    info->in_ep = &interface->endpoints[ep_index];
                }
            }else{
                if(!info->out_ep){
                    info->out_ep = &interface->endpoints[ep_index];
                }
            }
            if(ep_index < TUSBH_MAX_EP){
                setup_ep_info(&interface->endpoints[ep_index], ep, interface);
            }else{
                TUSB_ITF_INFO("Endpoint count large than TUSBH_MAX_EP in this interface\n");
            }
            ep_index++;
        }else if(t == USB_INTERFACE_DESCRIPTOR_TYPE){
            if(itf_cnt > 0){
                break;
            }
            itf_cnt++;
        }
        cfg_offset+=len;
    }
    if(ep_index != interface->ep_num){
        TUSB_ITF_INFO("Endpoint count error espect %d, got %d\n", interface->ep_num, ep_index);
        goto error;
    }
    if(!(info->in_ep && info->out_ep)){
        TUSB_ITF_INFO("Endpoint number error\n");
        goto error;
    }
    
    if(tusbh_ep_allocate_pipe(info->in_ep)<0){
        TUSB_ITF_INFO("Fail to allocate pipe for MSC in\n");
        goto error;
    }else{
        tusbh_ep_info_t* ep = info->in_ep;
        TUSB_EP_INFO("MSC IN ep pipe = %d\n", ep->pipe_num);
    }
    if(tusbh_ep_allocate_pipe(info->out_ep)<0){
        TUSB_ITF_INFO("Fail to allocate pipe for MSC out\n");
        goto error;
    }else{
        tusbh_ep_info_t* ep = info->out_ep;
        TUSB_EP_INFO("MSC OUT ep pipe = %d\n", ep->pipe_num);
    }
    
    r = tusbh_control_xfer(
       dev,
       USB_D2H | USB_REQ_RECIPIENT_INTERFACE | USB_REQ_TYPE_CLASS,
       BOT_GET_MAX_LUN,
       0,
       interface->desc->bInterfaceNumber,
       &info->max_lun, 1);
    
    if( r<0 ){
        TUSB_ITF_INFO("MSC Fail to get MAX lun");
        goto error;
    }
    
    info->blocks = (tusbh_block_info_t*)tusbh_malloc(  (info->max_lun+1) * sizeof(tusbh_block_info_t) );
    if(!info->blocks){
        TUSB_ITF_INFO("MSC Fail to allocate memory for block info\n");
        goto error;
    }
    
    for(uint8_t i=0;i<=info->max_lun;i++){
        if(tusbh_msc_inquiry(interface, i)<0){
            TUSB_ITF_INFO("MSC Fail to inquiry, lun = %d\n", i);
            tusbh_msc_request_sense(interface, i, &info->sense);
            goto error;
        }
        scsi_inquiry_std_response_t* inquiry = &info->blocks[i].inquiry;
        TUSB_ITF_INFO("MSC lun(%d) %8.8s - %16.16s - %4.4s\n", i, inquiry->vid, inquiry->pid, inquiry->product_version);
        if(tusbh_msc_read_capacity(interface, i) < 0){
            //TUSB_ITF_INFO("MSC Fail to get capacity, lun = %d\n", i);
            tusbh_msc_request_sense(interface, i, &info->sense);
            if(tusbh_msc_read_capacity(interface, i) < 0){
                TUSB_ITF_INFO("MSC Fail to get capacity, lun = %d\n", i);
                goto error;
            }
        }
        tusbh_block_info_t* block = &info->blocks[i];
        TUSB_ITF_INFO("MSC lun(%d) Block Count:%d Size:%d\n", i, (int)block->block_count, (int)block->block_size);
    }
    
    if(((tusbh_msc_class_t*)interface->cls)->mount){
        ((tusbh_msc_class_t*)interface->cls)->mount(interface, info->max_lun, info->blocks);
    }

error:
    
    TUSB_ITF_INFO("MSC interface init\n");
    return cfg_offset;
}

static int tusbh_msc_deinit(tusbh_device_t* dev, tusbh_interface_t* interface)
{
    tusbh_msc_info_t* info = tusbh_get_info(interface, tusbh_msc_info_t);
    
    tusbh_msc_class_t* cls = (tusbh_msc_class_t*)interface->cls;
    if(cls->unmount){
        cls->unmount(interface);
    }
    
    TUSB_ITF_INFO("MSC interface deinit\n");
    
    if(info->in_ep){
        tusbh_ep_free_pipe(info->in_ep);
    }
    if(info->out_ep){
        tusbh_ep_free_pipe(info->out_ep);
    }
    if(info->blocks){
        tusbh_free(info->blocks);
    }
    
    tusbh_free_info(interface);
    
    
    return 0;
}


const tusbh_interface_backend_t  tusbh_msc_bot_backend = {
    .vid = 0,
    .pid = 0,
    .bInterfaceClass = 0x08,
    .bInterfaceSubClass = 0x06,
    .bInterfaceProtocol = 0x50,
    .init = tusbh_msc_init,
    .deinit = tusbh_msc_deinit,
    .desc = "Mass Storage",
};

