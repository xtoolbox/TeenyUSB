


#ifndef __CH56x_COMM_H__
#define __CH56x_COMM_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "stdio.h"
#include "string.h"
#include "core_riscv.h"
#include "CH56x_sys.h"
#include "CH56x_clk.h"
#include "CH56x_uart.h"
#include "CH56x_gpio.h"
#include "CH56x_pwm.h"
#include "CH56x_timer.h"
#include "CH56x_spi.h"
#include "CH56x_hspi.h"
#include "CH56x_dvp.h"
#include "CH56x_bus8.h"
#include "CH56x_ecdc.h"
#include "CH56x_pwr.h"
#include "CH56x_emmc.h"


/* UART Printf Definition */
#define Debug_UART0        0
#define Debug_UART1        1
#define Debug_UART2        2
#define Debug_UART3        3

/* SystemCoreClock */ 
#ifndef	 FREQ_SYS  
#define  FREQ_SYS		80000000   
#endif   


#define DelayMs(x)      mDelaymS(x)	  
#define DelayUs(x)      mDelayuS(x)	  



#ifdef __cplusplus
}
#endif

#endif  // __CH56x_COMM_H__	 

