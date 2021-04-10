/********************************** (C) COPYRIGHT *******************************
* File Name          : CH56x_uart.c
* Author             : WCH
* Version            : V1.0
* Date               : 2020/07/31
* Description 
*******************************************************************************/

#include "CH56x_common.h"


/*******************************************************************************
* Function Name  : UART0_DefInit
* Description    : 串口默认初始化配置：使能了FIFO,触发点字节数，串口数据长度设置，波特率及分频系数
* Input          : None
* Return         : None
*******************************************************************************/
void UART0_DefInit( void )
{	
    UART0_BaudRateCfg( 115200 );
    R8_UART0_FCR = (2<<6) | RB_FCR_TX_FIFO_CLR | RB_FCR_RX_FIFO_CLR | RB_FCR_FIFO_EN;		// FIFO打开，触发点4字节
    R8_UART0_LCR = RB_LCR_WORD_SZ;
    R8_UART0_IER = RB_IER_TXD_EN;
    R8_UART0_DIV = 1;
}

/*******************************************************************************
* Function Name  : UART1_DefInit
* Description    : 串口默认初始化配置：使能了FIFO,触发点字节数，串口数据长度设置，波特率及分频系数
* Input          : None
* Return         : None
*******************************************************************************/
void UART1_DefInit( void )
{
    UART1_BaudRateCfg( 115200 );
    R8_UART1_FCR = (2<<6) | RB_FCR_TX_FIFO_CLR | RB_FCR_RX_FIFO_CLR | RB_FCR_FIFO_EN;		// FIFO打开，触发点4字节
    R8_UART1_LCR = RB_LCR_WORD_SZ;
    R8_UART1_IER = RB_IER_TXD_EN;
    R8_UART1_DIV = 1;
}

/*******************************************************************************
* Function Name  : UART2_DefInit
* Description    : 串口默认初始化配置：使能了FIFO,触发点字节数，串口数据长度设置，波特率及分频系数
* Input          : None
* Return         : None
*******************************************************************************/
void UART2_DefInit( void )
{
    UART2_BaudRateCfg( 115200 );
    R8_UART2_FCR = (2<<6) | RB_FCR_TX_FIFO_CLR | RB_FCR_RX_FIFO_CLR | RB_FCR_FIFO_EN;		// FIFO打开，触发点4字节
    R8_UART2_LCR = RB_LCR_WORD_SZ;
    R8_UART2_IER = RB_IER_TXD_EN;
    R8_UART2_DIV = 1;
}

/*******************************************************************************
* Function Name  : UART3_DefInit
* Description    : 串口默认初始化配置：使能了FIFO,触发点字节数，串口数据长度设置，波特率及分频系数
* Input          : None
* Return         : None
*******************************************************************************/
void UART3_DefInit( void )
{
    UART3_BaudRateCfg( 115200 );
    R8_UART3_FCR = (2<<6) | RB_FCR_TX_FIFO_CLR | RB_FCR_RX_FIFO_CLR | RB_FCR_FIFO_EN;		// FIFO打开，触发点4字节
    R8_UART3_LCR = RB_LCR_WORD_SZ;
    R8_UART3_IER = RB_IER_TXD_EN;
    R8_UART3_DIV = 1;
}

/*******************************************************************************
* Function Name  : UART0_BaudRateCfg
* Description    : 串口波特率配置
* Input          : 波特率
* Return         : None
*******************************************************************************/
void UART0_BaudRateCfg( UINT32 baudrate )
{
    UINT32	x;

    x = 10 * FREQ_SYS / 8 / baudrate;
    x = ( x + 5 ) / 10;
    R16_UART0_DL = (UINT16)x;
}

/*******************************************************************************
* Function Name  : UART1_BaudRateCfg
* Description    : 串口波特率配置
* Input          : 波特率
* Return         : None
*******************************************************************************/
void UART1_BaudRateCfg( UINT32 baudrate )
{
    UINT32	x;

    x = 10 * FREQ_SYS / 8 / baudrate;
    x = ( x + 5 ) / 10;
    R16_UART1_DL = (UINT16)x;
}

/*******************************************************************************
* Function Name  : UART2_BaudRateCfg
* Description    : 串口波特率配置
* Input          : 波特率
* Return         : None
*******************************************************************************/
void UART2_BaudRateCfg( UINT32 baudrate )
{
    UINT32	x;

    x = 10 * FREQ_SYS / 8 / baudrate;
    x = ( x + 5 ) / 10;
    R16_UART2_DL = (UINT16)x;
}

/*******************************************************************************
* Function Name  : UART3_BaudRateCfg
* Description    : 串口波特率配置
* Input          : 波特率
* Return         : None
*******************************************************************************/
void UART3_BaudRateCfg( UINT32 baudrate )
{
    UINT32	x;

    x = 10 * FREQ_SYS / 8 / baudrate;
    x = ( x + 5 ) / 10;
    R16_UART3_DL = (UINT16)x;
}

/*******************************************************************************
* Function Name  : UART0_ByteTrigCfg
* Description    : 串口字节触发中断配置
* Input          : b: 触发字节数
                    refer to UARTByteTRIGTypeDef
* Return         : None
*******************************************************************************/
void UART0_ByteTrigCfg( UARTByteTRIGTypeDef b )
{
    R8_UART0_FCR = (R8_UART0_FCR&~RB_FCR_FIFO_TRIG)|(b<<6);
}

/*******************************************************************************
* Function Name  : UART1_ByteTrigCfg
* Description    : 串口字节触发中断配置
* Input          : b: 触发字节数
                    refer to UARTByteTRIGTypeDef
* Return         : None
*******************************************************************************/
void UART1_ByteTrigCfg( UARTByteTRIGTypeDef b )
{
    R8_UART1_FCR = (R8_UART1_FCR&~RB_FCR_FIFO_TRIG)|(b<<6);
}

/*******************************************************************************
* Function Name  : UART2_ByteTrigCfg
* Description    : 串口字节触发中断配置
* Input          : b: 触发字节数
                    refer to UARTByteTRIGTypeDef
* Return         : None
*******************************************************************************/
void UART2_ByteTrigCfg( UARTByteTRIGTypeDef b )
{
    R8_UART2_FCR = (R8_UART2_FCR&~RB_FCR_FIFO_TRIG)|(b<<6);
}

/*******************************************************************************
* Function Name  : UART3_ByteTrigCfg
* Description    : 串口字节触发中断配置
* Input          : b: 触发字节数
                    refer to UARTByteTRIGTypeDef
* Return         : None
*******************************************************************************/
void UART3_ByteTrigCfg( UARTByteTRIGTypeDef b )
{
    R8_UART3_FCR = (R8_UART3_FCR&~RB_FCR_FIFO_TRIG)|(b<<6);
}

/*******************************************************************************
* Function Name  : UART0_INTCfg
* Description    : 串口中断配置
* Input          : s:  中断控制状态
					ENABLE  - 使能相应中断    
					DISABLE - 关闭相应中断
				   i:  中断类型
					RB_IER_MODEM_CHG  - 调制解调器输入状态变化中断使能位（仅 UART0 支持）
					RB_IER_LINE_STAT  - 接收线路状态中断
					RB_IER_THR_EMPTY  - 发送保持寄存器空中断
					RB_IER_RECV_RDY   - 接收数据中断
* Return         : None
*******************************************************************************/
void UART0_INTCfg( UINT8 s,  UINT8 i )
{
    if( s )
    {
        R8_UART0_IER |= i;
        R8_UART0_MCR |= RB_MCR_INT_OE;
    }
    else
    {
        R8_UART0_IER &= ~i;
    }
}

/*******************************************************************************
* Function Name  : UART1_INTCfg
* Description    : 串口中断配置
* Input          : s:  中断控制状态
					ENABLE  - 使能相应中断
					DISABLE - 关闭相应中断
				   i:  中断类型
					RB_IER_MODEM_CHG  - 调制解调器输入状态变化中断使能位（仅 UART0 支持）
					RB_IER_LINE_STAT  - 接收线路状态中断
					RB_IER_THR_EMPTY  - 发送保持寄存器空中断
					RB_IER_RECV_RDY   - 接收数据中断
* Return         : None
*******************************************************************************/
void UART1_INTCfg( UINT8 s,  UINT8 i )
{
    if( s )
    {
        R8_UART1_IER |= i;
        R8_UART1_MCR |= RB_MCR_INT_OE;
    }
    else
    {
        R8_UART1_IER &= ~i;
    }
}

/*******************************************************************************
* Function Name  : UART2_INTCfg
* Description    : 串口中断配置
* Input          : s:  中断控制状态
					ENABLE  - 使能相应中断
					DISABLE - 关闭相应中断
				   i:  中断类型
					RB_IER_MODEM_CHG  - 调制解调器输入状态变化中断使能位（仅 UART0 支持）
					RB_IER_LINE_STAT  - 接收线路状态中断
					RB_IER_THR_EMPTY  - 发送保持寄存器空中断
					RB_IER_RECV_RDY   - 接收数据中断
* Return         : None
*******************************************************************************/
void UART2_INTCfg( UINT8 s,  UINT8 i )
{
    if( s )
    {
        R8_UART2_IER |= i;
        R8_UART2_MCR |= RB_MCR_INT_OE;
    }
    else
    {
        R8_UART2_IER &= ~i;
    }
}

/*******************************************************************************
* Function Name  : UART3_INTCfg
* Description    : 串口中断配置
* Input          : s:  中断控制状态
					ENABLE  - 使能相应中断
					DISABLE - 关闭相应中断
				   i:  中断类型
					RB_IER_MODEM_CHG  - 调制解调器输入状态变化中断使能位（仅 UART0 支持）
					RB_IER_LINE_STAT  - 接收线路状态中断
					RB_IER_THR_EMPTY  - 发送保持寄存器空中断
					RB_IER_RECV_RDY   - 接收数据中断
* Return         : None
*******************************************************************************/
void UART3_INTCfg( UINT8 s,  UINT8 i )
{
    if( s )
    {
        R8_UART3_IER |= i;
        R8_UART3_MCR |= RB_MCR_INT_OE;
    }
    else
    {
        R8_UART3_IER &= ~i;
    }
}

/*******************************************************************************
* Function Name  : UART0_Reset
* Description    : 串口软件复位
* Input          : None
* Return         : None
*******************************************************************************/
void UART0_Reset( void )
{
    R8_UART0_IER = RB_IER_RESET;
}

/*******************************************************************************
* Function Name  : UART1_Reset
* Description    : 串口软件复位
* Input          : None
* Return         : None
*******************************************************************************/
void UART1_Reset( void )
{
    R8_UART1_IER = RB_IER_RESET;
}

/*******************************************************************************
* Function Name  : UART2_Reset
* Description    : 串口软件复位
* Input          : None
* Return         : None
*******************************************************************************/
void UART2_Reset( void )
{
    R8_UART2_IER = RB_IER_RESET;
}

/*******************************************************************************
* Function Name  : UAR3_Reset
* Description    : 串口软件复位
* Input          : None
* Return         : None
*******************************************************************************/
void UART3_Reset( void )
{
    R8_UART3_IER = RB_IER_RESET;
}

/*******************************************************************************
* Function Name  : UART0_SendString
* Description    : 串口多字节发送
* Input          : buf - 待发送的数据内容首地址
                     l - 待发送的数据长度
* Return         : None
*******************************************************************************/
void UART0_SendString( PUINT8 buf, UINT16 l )
{
    UINT16 len = l;

    while(len)
    {
        if(R8_UART0_TFC != UART_FIFO_SIZE)
        {
            R8_UART0_THR = *buf++;
            len--;
        }
    }
}

/*******************************************************************************
* Function Name  : UART1_SendString
* Description    : 串口多字节发送
* Input          : buf - 待发送的数据内容首地址
                     l - 待发送的数据长度
* Return         : None
*******************************************************************************/
void UART1_SendString( PUINT8 buf, UINT16 l )
{
    UINT16 len = l;

    while(len)
    {
        if(R8_UART1_TFC != UART_FIFO_SIZE)
        {
            R8_UART1_THR = *buf++;
            len--;
        }		
    }
}

/*******************************************************************************
* Function Name  : UART2_SendString
* Description    : 串口多字节发送
* Input          : buf - 待发送的数据内容首地址
                     l - 待发送的数据长度
* Return         : None
*******************************************************************************/
void UART2_SendString( PUINT8 buf, UINT16 l )
{
    UINT16 len = l;

    while(len)
    {
        if(R8_UART2_TFC != UART_FIFO_SIZE)
        {
            R8_UART2_THR = *buf++;
            len--;
        }
    }
}

/*******************************************************************************
* Function Name  : UART3_SendString
* Description    : 串口多字节发送
* Input          : buf - 待发送的数据内容首地址
                     l - 待发送的数据长度
* Return         : None
*******************************************************************************/
void UART3_SendString( PUINT8 buf, UINT16 l )
{
    UINT16 len = l;

    while(len)
    {
        if(R8_UART3_TFC != UART_FIFO_SIZE)
        {
            R8_UART3_THR = *buf++;
            len--;
        }
    }
}

/*******************************************************************************
* Function Name  : UART0_RecvString
* Description    : 串口读取多字节
* Input          : buf - 读取数据存放缓存区首地址
* Return         : 读取数据长度
*******************************************************************************/
UINT16 UART0_RecvString( PUINT8 buf )
{
    UINT16 len = 0;

    while( R8_UART0_RFC )
    {
        *buf++ = R8_UART0_RBR;
        len ++;
    }

    return (len);
}

/*******************************************************************************
* Function Name  : UART1_RecvString
* Description    : 串口读取多字节
* Input          : buf - 读取数据存放缓存区首地址
* Return         : 读取数据长度
*******************************************************************************/
UINT16 UART1_RecvString( PUINT8 buf )
{
    UINT16 len = 0;

    while( R8_UART1_RFC )
    {
        *buf++ = R8_UART1_RBR;
        len ++;
    }

    return (len);
}

/*******************************************************************************
* Function Name  : UART2_RecvString
* Description    : 串口读取多字节
* Input          : buf - 读取数据存放缓存区首地址
* Return         : 读取数据长度
*******************************************************************************/
UINT16 UART2_RecvString( PUINT8 buf )
{
    UINT16 len = 0;

    while( R8_UART2_RFC )
    {
        *buf++ = R8_UART2_RBR;
        len ++;
    }

    return (len);
}

/*******************************************************************************
* Function Name  : UART3_RecvString
* Description    : 串口读取多字节
* Input          : buf - 读取数据存放缓存区首地址
* Return         : 读取数据长度
*******************************************************************************/
UINT16 UART3_RecvString( PUINT8 buf )
{
    UINT16 len = 0;

    while( R8_UART3_RFC )
    {
        *buf++ = R8_UART3_RBR;
        len ++;
    }

    return (len);
}


