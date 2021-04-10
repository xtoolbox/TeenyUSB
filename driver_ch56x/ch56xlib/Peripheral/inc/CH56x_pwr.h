


#ifndef __CH56x_PWR_H__
#define __CH56x_PWR_H__

#ifdef __cplusplus
 extern "C" {
#endif


/**
  * @brief	Peripher CLK control bit define
  */
#define BIT_SLP_CLK_TMR0                 RB_SLP_CLK_TMR0   /*!< TMR0 peripher clk bit */
#define BIT_SLP_CLK_TMR1                 RB_SLP_CLK_TMR1   /*!< TMR1 peripher clk bit */
#define BIT_SLP_CLK_TMR2                 RB_SLP_CLK_TMR2   /*!< TMR2 peripher clk bit */
#define BIT_SLP_CLK_PWMX                 RB_SLP_CLK_PWMX   /*!< PWMX peripher clk bit */
#define BIT_SLP_CLK_UART0                RB_SLP_CLK_UART0  /*!< UART0 peripher clk bit */
#define BIT_SLP_CLK_UART1                RB_SLP_CLK_UART1  /*!< UART1 peripher clk bit */
#define BIT_SLP_CLK_UART2                RB_SLP_CLK_UART2  /*!< UART2 peripher clk bit */
#define BIT_SLP_CLK_UART3                RB_SLP_CLK_UART3  /*!< UART3 peripher clk bit */
#define BIT_SLP_CLK_SPI0                 RB_SLP_CLK_SPI0   /*!< SPI0 peripher clk bit */
#define BIT_SLP_CLK_SPI1                 RB_SLP_CLK_SPI1   /*!< SPI1 peripher clk bit */
#define BIT_SLP_CLK_EMMC                 RB_SLP_CLK_EMMC   /*!< EMMC peripher clk bit */
#define BIT_SLP_CLK_HSPI                 RB_SLP_CLK_HSPI   /*!< HSPI peripher clk bit */
#define BIT_SLP_CLK_USBHS                RB_SLP_CLK_USBHS  /*!< USBHS peripher clk bit */
#define BIT_SLP_CLK_USBSS                RB_SLP_CLK_USBSS  /*!< USBSS peripher clk bit */
#define BIT_SLP_CLK_SERD                 RB_SLP_CLK_SERD   /*!< SERD peripher clk bit */
#define BIT_SLP_CLK_DVP                  RB_SLP_CLK_DVP    /*!< DVP peripher clk bit */
#define BIT_SLP_CLK_ETH                  RB_SLP_CLK_ETH    /*!< ETH peripher clk bit */
#define BIT_SLP_CLK_ECDC                 RB_SLP_CLK_ECDC   /*!< ECDC peripher clk bit */


void PWR_PeriphClkCfg( UINT8 s, UINT16 perph );               /* 外设时钟控制位 */
void PWR_PeriphWakeUpCfg( UINT8 s, UINT16 perph );            /* 睡眠唤醒源配置 */
void LowPower_Idle( void );                                   /* 低功耗-IDLE模式 */
void LowPower_Halt( void );                                   /* 低功耗-Halt模式 */
void LowPower_Sleep( void );                                  /* 低功耗-Sleep模式 */


	 
#ifdef __cplusplus
}
#endif

#endif  // __CH56x_PWR_H__	

