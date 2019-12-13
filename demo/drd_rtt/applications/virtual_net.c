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

#include "teeny_usb.h"
#include "tusbh.h"
#include "tusbh_cdc_rndis.h"
#include "tusbd_cdc_rndis.h"
#include <rtthread.h>
#include <netif/ethernetif.h>
#include "string.h"
#include <lwip/api.h>

static rt_err_t rt_v_eth_init(rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t rt_v_eth_open(rt_device_t dev, rt_uint16_t oflag)
{
	return RT_EOK;
}

static rt_err_t rt_v_eth_close(rt_device_t dev)
{
	return RT_EOK;
}

static rt_size_t rt_v_eth_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
	rt_set_errno(-RT_ENOSYS);
	return 0;
}

static rt_size_t rt_v_eth_write (rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
	rt_set_errno(-RT_ENOSYS);
	return 0;
}

static const uint8_t eth_v_mac[6] = {0x00, 0x80, 0xE1, 0x12, 0x34, 0x56};
static rt_err_t rt_v_eth_control(rt_device_t dev, int cmd, void *args)
{
	switch(cmd)
	{
	case NIOCTL_GADDR:
		/* get mac address */
		if(args) rt_memcpy(args, eth_v_mac, 6);
		else return -RT_ERROR;
		break;

	default :
		break;
	}

	return RT_EOK;
}

static struct pbuf *rt_v_eth_rx(rt_device_t dev);

static rt_err_t rt_v_eth_tx( rt_device_t dev, struct pbuf* p);

struct eth_device v_eth_dev;
static struct rt_semaphore tx_wait;

static int v_eth_init(void)
{
    v_eth_dev.parent.init       = rt_v_eth_init;
    v_eth_dev.parent.open       = rt_v_eth_open;
    v_eth_dev.parent.close      = rt_v_eth_close;
    v_eth_dev.parent.read       = rt_v_eth_read;
    v_eth_dev.parent.write      = rt_v_eth_write;
    v_eth_dev.parent.control    = rt_v_eth_control;
    v_eth_dev.parent.user_data  = RT_NULL;
    v_eth_dev.eth_tx            = rt_v_eth_tx;
    v_eth_dev.eth_rx            = rt_v_eth_rx;
    
    rt_sem_init(&tx_wait, "tx_wait", 0, RT_IPC_FLAG_FIFO);
    eth_device_init(&v_eth_dev, "e0");
    
    return 0;
}

INIT_BOARD_EXPORT(v_eth_init);


extern tusbh_msg_q_t* dev_mq;
extern tusbh_msg_q_t* host_mq;
static struct pbuf * v_rx_buf = RT_NULL;
static struct pbuf *rt_v_eth_rx(rt_device_t dev)
{
    struct pbuf* res = v_rx_buf;
    if(v_rx_buf != RT_NULL){
        v_rx_buf = RT_NULL;
    }
    return res;
}

extern tusb_rndis_device_t rndis_dev;

static rt_err_t rt_v_eth_tx( rt_device_t dev, struct pbuf* p);


static void cdc_recv_msg(tusbh_message_t* msg)
{
    tusb_rndis_device_t* cdc = (tusb_rndis_device_t*)msg->param;
    if(v_rx_buf != RT_NULL){
        rt_kprintf("Eth Rx overflow\n");
        pbuf_free(v_rx_buf);
    }
    struct pbuf *p = pbuf_alloc(PBUF_RAW, msg->len, PBUF_POOL);
    pbuf_take(p, msg->data, msg->len);
    v_rx_buf = p;
    eth_device_ready(&v_eth_dev);
    tusb_set_rx_valid(cdc->dev, cdc->ep_out);
    
}

static int rndis_recv_data(tusb_rndis_device_t* cdc, const void* data, uint16_t len)
{
    POST_MESSAGE(dev_mq, cdc_recv_msg, (uint32_t)cdc, (void*)data, len);
    return 1;
}

static int rndis_send_done(tusb_rndis_device_t* cdc)
{
    rt_sem_release(&tx_wait);
    return 0;
}

static int v_eth_linkchange(tusb_rndis_device_t* cdc, int linkup)
{
    eth_device_linkchange(&v_eth_dev, linkup != 0 ? RT_TRUE : RT_FALSE);
    rt_sem_release(&tx_wait);
    return 0;
}

tusb_rndis_device_t rndis_dev = {
  .backend = &rndis_device_backend,
  .ep_in = 1,
  .ep_out = 1,
  .ep_int = 3,
  .on_recv_data = rndis_recv_data,
  .on_send_done = rndis_send_done,
  .eth_linkchange = v_eth_linkchange,
  .mac_addr = {0x00, 0x80, 0xE1, 0x11, 0x22, 0x33},
};

static tusb_device_interface_t* rndis_interfaces[] = {
  (tusb_device_interface_t*)&rndis_dev, 0,   // RNDIS need two interfaces
};
static void rndis_init_ep(tusb_device_t* dev)
{
  RNDIS_TUSB_INIT(dev);
}
tusb_device_config_t rndis_device_config = {
  .if_count = sizeof(rndis_interfaces)/sizeof(rndis_interfaces[0]),
  .interfaces = &rndis_interfaces[0],
  .ep_init = rndis_init_ep,
};

extern tusb_device_config_t*  config_fs;
extern tusb_device_config_t*  config_hs;

///////////////////////////////////////////////////
/// RNDIS host
///////////////////////////////////////////////////
// use the sof to read packet data
static char host_rx_buf[2048];
static char host_tx_buf[2048];
static int host_tx_size;
static tusbh_interface_t* v_eth_host_itf = 0;
static tusbh_xfered_set_event_t rndis_event;
static struct rt_messagequeue rndis_mq;
static uint32_t rndis_mq_pool[8];

#define MOUNT_MSG    1
#define UNMOUNT_MSG  2
#define DATA_RX_MSG  3
#define DATA_TX_MSG  4

static rt_err_t rt_v_eth_tx( rt_device_t dev, struct pbuf* p)
{
    if(config_fs != &rndis_device_config && config_hs != &rndis_device_config && v_eth_host_itf == 0){
        eth_device_linkchange(&v_eth_dev, RT_FALSE);
        return -RT_ERROR;
    }
    rt_err_t result;
    if(v_eth_host_itf){
        if(pbuf_copy_partial(p, host_tx_buf, sizeof(host_tx_buf), 0) == p->tot_len){
            result = rt_sem_take(&tx_wait, RT_WAITING_FOREVER);
            host_tx_size = p->tot_len;
            uint8_t msg = DATA_TX_MSG;
            rt_mq_send( &rndis_mq, &msg, 1);
        }else{
            rt_sem_release(&tx_wait);
        }
        return RT_EOK;
    }
    result = rt_sem_take(&tx_wait, RT_WAITING_FOREVER);
    if (result != RT_EOK) return -RT_ERROR;
    uint8_t* data = rndis_dev.tx_buf + sizeof(rndis_data_packet_t);
    if(pbuf_copy_partial(p, data, sizeof(rndis_dev.tx_buf) - sizeof(rndis_data_packet_t), 0) 
        != p->tot_len){
        rt_sem_release(&tx_wait);
        rt_kprintf("Eth Tx overflow\n");
        return -RT_ERROR;
    }
    tusb_rndis_send_packet(&rndis_dev, data, p->tot_len);
    return RT_EOK;
}

int v_eth_host_unmount(tusbh_interface_t* itf)
{
    // No eth interface
    if(config_fs != &rndis_device_config && config_hs != &rndis_device_config){
        eth_device_linkchange(&v_eth_dev, RT_FALSE);
    }
    rt_sem_release(&tx_wait);
    v_eth_host_itf = 0;
    uint8_t msg = UNMOUNT_MSG;
    rt_mq_send( &rndis_mq, &msg, 1);
    return 0;
}

int v_eth_host_response(tusbh_interface_t* itf, rndis_generic_msg_t* msg)
{
    // Only OID_GEN_CURRENT_PACKET_FILTER is set by the default init process
    if(msg->MessageType == REMOTE_NDIS_SET_CMPLT){
        rndis_set_cmplt_t* c = (rndis_set_cmplt_t*)msg;
        if(c->Status == RNDIS_STATUS_SUCCESS){
            eth_device_linkchange(&v_eth_dev, RT_TRUE);
            rt_sem_release(&tx_wait);
            v_eth_host_itf = itf;
            uint8_t msg = MOUNT_MSG;
            rt_mq_send( &rndis_mq, &msg, 1);
        }
    }else if(msg->MessageType == REMOTE_NDIS_KEEPALIVE_CMPLT){
        tusbh_evt_set(rndis_event.event);
    }
    return 0;
}

static void v_eth_set_event(tusb_host_t* host, uint8_t hc_num, tusbh_xfered_set_event_t* data)
{
    if(data->event){
        tusbh_evt_set(data->event);
    }
}

static void rndis_thread(void* p)
{
    uint8_t msg;
    int ready = 0;
    while(1){
        if(rt_mq_recv(&rndis_mq, &msg, 1, RT_WAITING_FOREVER) != RT_EOK){
            continue;
        }
        switch(msg){
            case MOUNT_MSG:{
                ready = 1;
                msg = DATA_RX_MSG;
                rt_mq_send( &rndis_mq, &msg, 1);
                break;
            }
            case UNMOUNT_MSG:{
                ready = 0;
                break;
            }
            case DATA_RX_MSG:{
                if(!ready)break;
                if(!v_eth_host_itf)break;
                tusbh_evt_clear(rndis_event.event);
                int r = tusbh_rndis_recv_packet_with_event(v_eth_host_itf, host_rx_buf, sizeof(host_rx_buf), &rndis_event, 1000);
                if( r > 0){
                    if(v_rx_buf != RT_NULL){
                        rt_kprintf("Eth Rx overflow\n");
                        pbuf_free(v_rx_buf);
                    }
                    struct pbuf *p = pbuf_alloc(PBUF_RAW, r, PBUF_POOL);
                    pbuf_take(p, host_rx_buf, r);
                    v_rx_buf = p;
                    //rt_kprintf("Eth Rx %d\n", r);
                    eth_device_ready(&v_eth_dev);
                }else{
                    tusb_delay_ms(200);
                    // TODO: keep alive int the host thread
                    //if(v_eth_host_itf){
                    //    tusbh_evt_clear(rndis_event.event);
                    //    tusbh_rndis_keepalive(v_eth_host_itf);
                    //    // wait keep alive ack
                    //    tusbh_evt_wait(rndis_event.event, 1000);
                    //}
                }
                
                msg = DATA_RX_MSG;
                rt_mq_send( &rndis_mq, &msg, 1);
                break;
            }
            case DATA_TX_MSG:{
                if(ready && v_eth_host_itf){
                    tusbh_evt_clear(rndis_event.event);
                    tusbh_rndis_send_packet_with_event(v_eth_host_itf, host_tx_buf, host_tx_size, &rndis_event, 2000);
                }
                rt_sem_release(&tx_wait);
                break;
            }
        }
    }
}

static int rndis_host_init(void)
{
    rndis_event.func = v_eth_set_event;
    rndis_event.event = tusbh_evt_create();
    tusbh_evt_init(rndis_event.event);
    
    rt_mq_init(&rndis_mq, "rndis", rndis_mq_pool, 1, sizeof(rndis_mq_pool), RT_IPC_FLAG_FIFO);
    
    rt_thread_t tid;
	tid = rt_thread_create("rndis", rndis_thread, RT_NULL,
		1024, 22, 5);
    if (tid != RT_NULL){
        rt_thread_startup(tid);
    }
    return 0;
}

INIT_APP_EXPORT(rndis_host_init);


///////////////////////////////////////////////////
/// Simple web server
///////////////////////////////////////////////////

static const char static_web[] = 
    "HTTP/1.0 200 OK\r\n"
    "Content-Type: text/html\r\n\r\n"
    "<html> \
	<head><title>TeenyUSB RNDIS Web</title></head> \
	<body> \
	Hello from TeenyUSB\
	</body> \
	</html>";

static void send_web_content(struct netconn *conn)
{
	struct netbuf *inbuf;
	char *rq;
	rt_err_t err;
	rt_uint16_t len;
	err = netconn_recv(conn,&inbuf);
    if(err != RT_EOK){
        return;
    }

	netbuf_data(inbuf, (void**)&rq, &len);

	if( rq[0] == 'G' &&
		rq[1] == 'E' &&
		rq[2] == 'T' &&
		rq[3] == ' ')
	{
		netconn_write(conn, static_web, sizeof(static_web), NETCONN_NOCOPY);
		netconn_close(conn);
	}
	netbuf_delete(inbuf);
}

static void web_thread(void* paramter)
{
	struct netconn *conn, *newconn;
	rt_err_t err;
	conn = netconn_new(NETCONN_TCP);
	netconn_bind(conn, NULL, 80);
	netconn_listen(conn);
	while(1)
	{
		err = netconn_accept(conn, &newconn);
        if(err == RT_EOK){
            send_web_content(newconn);
            netconn_delete(newconn);
        }
	}
}

static int web_server_init(void)
{
    rt_thread_t tid;
	tid = rt_thread_create("web", web_thread, RT_NULL,
		1024, 24, 5);
	if (tid != RT_NULL){
        rt_thread_startup(tid);
    }
    return 0;
}

INIT_APP_EXPORT(web_server_init);
