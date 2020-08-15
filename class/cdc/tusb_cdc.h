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

#ifndef __TUSB_CDC_H__
#define __TUSB_CDC_H__
#include "teeny_usb.h"

#define TUSB_CDC_DTR   0x01
#define TUSB_CDC_RTS   0x02

/** Enums for \ref tusb_cdc_line_coding_t  stopbits field
 *  indicating the CDC stopbits
 */
typedef enum{
    CDC_1STOP = 0,
    CDC_1_5STOP = 1,
    CDC_2_STOP = 2,
}cdc_stopbits_t;

/** Enums for \ref tusb_cdc_line_coding_t  parity field
 *  indicating the CDC parity
 */
typedef enum{
    CDC_NONE = 0,
    CDC_ODD = 1,
    CDC_EVEN = 2,
    CDC_MARK = 3,
    CDC_SPACE = 4,
}cdc_parity_t;

/* Type Defines: */
/** Type define for CDC line coding
 */
typedef __PACK_BEGIN struct _tusb_cdc_line_coding
{
    uint32_t bitrate;        /**< bit rate */
    uint8_t  stopbits;       /**< stop bits, \ref cdc_stopbits_t*/
    uint8_t  parity;         /**< parity, \ref cdc_parity_t */
    uint8_t  databits;       /**< data bits: 5,6,7,8 */
}__PACK_END tusb_cdc_line_coding_t;

/* Type Defines: */
/** Type define for CDC line status
 */
typedef struct _tusb_cdc_line_state
{
    uint16_t CDC:1;
    uint16_t DSR:1;
    uint16_t Break:1;
    uint16_t Ring:1;
    uint16_t FramingError:1;
    uint16_t ParityError:1;
    uint16_t Overrun:1;
    uint16_t revserved: 9;
}tusb_cdc_line_state_t;

typedef struct _tusb_cdc_state
{
    tusb_setup_packet req;
    tusb_cdc_line_state_t line_state;
}tusb_cdc_state_t;


// Abstract Control Management Functional Descriptor bmCapabilities fields
#define CDC_CAP_COMM     1
#define CDC_CAP_LINE     2
#define CDC_CAP_BREAK    4
#define CDC_CAP_NETWORK  8

// CDC request code define
#define CDC_SEND_ENCAPSULATED_COMMAND    0x00
#define CDC_GET_ENCAPSULATED_RESPONSE    0x01
#define CDC_SET_COMM_FEATURE             0x02
#define CDC_GET_COMM_FEATURE             0x03
#define CDC_CLEAR_COMM_FEATURE           0x04
#define CDC_SET_LINE_CODING              0x20
#define CDC_GET_LINE_CODING              0x21
#define CDC_SET_CONTROL_LINE_STATE       0x22
#define CDC_SEND_BREAK                   0x23

// CDC notify code define
#define CDC_RING_DETECT                  0x09
#define CDC_SERIAL_STATE                 0x20

#endif
