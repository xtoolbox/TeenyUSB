


#ifndef __CH56x_GPIO_H__
#define __CH56x_GPIO_H__

#ifdef __cplusplus
 extern "C" {
#endif


#define GPIO_Pin_0                 (0x00000001)  /*!< Pin 0 selected */
#define GPIO_Pin_1                 (0x00000002)  /*!< Pin 1 selected */
#define GPIO_Pin_2                 (0x00000004)  /*!< Pin 2 selected */
#define GPIO_Pin_3                 (0x00000008)  /*!< Pin 3 selected */
#define GPIO_Pin_4                 (0x00000010)  /*!< Pin 4 selected */
#define GPIO_Pin_5                 (0x00000020)  /*!< Pin 5 selected */
#define GPIO_Pin_6                 (0x00000040)  /*!< Pin 6 selected */
#define GPIO_Pin_7                 (0x00000080)  /*!< Pin 7 selected */
#define GPIO_Pin_8                 (0x00000100)  /*!< Pin 8 selected */
#define GPIO_Pin_9                 (0x00000200)  /*!< Pin 9 selected */
#define GPIO_Pin_10                (0x00000400)  /*!< Pin 10 selected */
#define GPIO_Pin_11                (0x00000800)  /*!< Pin 11 selected */
#define GPIO_Pin_12                (0x00001000)  /*!< Pin 12 selected */
#define GPIO_Pin_13                (0x00002000)  /*!< Pin 13 selected */
#define GPIO_Pin_14                (0x00004000)  /*!< Pin 14 selected */
#define GPIO_Pin_15                (0x00008000)  /*!< Pin 15 selected */
#define GPIO_Pin_16                (0x00010000)  /*!< Pin 16 selected */
#define GPIO_Pin_17                (0x00020000)  /*!< Pin 17 selected */
#define GPIO_Pin_18                (0x00040000)  /*!< Pin 18 selected */
#define GPIO_Pin_19                (0x00080000)  /*!< Pin 19 selected */
#define GPIO_Pin_20                (0x00100000)  /*!< Pin 20 selected */
#define GPIO_Pin_21                (0x00200000)  /*!< Pin 21 selected */
#define GPIO_Pin_22                (0x00400000)  /*!< Pin 22 selected */
#define GPIO_Pin_23                (0x00800000)  /*!< Pin 23 selected */
#define GPIO_Pin_24                (0x01000000)  /*!< PinB23 selected */
#define GPIO_Pin_All               (0xFFFFFFFF)  /*!< All pins selected */
 
 
/**
  * @brief  GPIO模式结构体配置
  */
typedef enum
{
	GPIO_ModeIN_Floating,			//浮空输入
	GPIO_ModeIN_PU_NSMT,
	GPIO_ModeIN_PD_NSMT,
	GPIO_ModeIN_PU_SMT,
	GPIO_ModeIN_PD_SMT,
	GPIO_Slowascent_PP_8mA,
	GPIO_Slowascent_PP_16mA,
	GPIO_Highspeed_PP_8mA,
	GPIO_Highspeed_PP_16mA,
	GPIO_ModeOut_OP_8mA,
	GPIO_ModeOut_OP_16mA,
}GPIOModeTypeDef;


/**
  * @brief  GPIO中断结构体配置
  */
typedef enum
{
	GPIO_ITMode_LowLevel,			//低电平触发
	GPIO_ITMode_HighLevel,			//高电平触发
	GPIO_ITMode_FallEdge,			//下降沿触发
	GPIO_ITMode_RiseEdge,			//上升沿触发

}GPIOITModeTpDef;


/**
  * @brief  GPIOMCO结构体配置
  */
typedef enum
{
	MCO_125 = 0,
	MCO_25 = 4,
	MCO_2d5 = 0xC,
}MCOMode;

void GPIOA_ModeCfg( UINT32 pin, GPIOModeTypeDef mode );				/* GPIOA端口引脚模式配置 */
void GPIOB_ModeCfg( UINT32 pin, GPIOModeTypeDef mode );				/* GPIOB端口引脚模式配置 */
#define	GPIOA_ResetBits( pin )			(R32_PA_CLR |= pin)			/* GPIOA端口引脚输出置低 */
#define	GPIOA_SetBits( pin )			(R32_PA_OUT |= pin)			/* GPIOA端口引脚输出置高 */
#define	GPIOB_ResetBits( pin )			(R32_PB_CLR |= pin)			/* GPIOB端口引脚输出置低 */
#define	GPIOB_SetBits( pin )			(R32_PB_OUT |= pin)			/* GPIOB端口引脚输出置高 */	 
#define	GPIOA_InverseBits( pin )		(R32_PA_OUT ^= pin)			/* GPIOA端口引脚输出电平翻转 */
#define	GPIOB_InverseBits( pin )		(R32_PB_OUT ^= pin)			/* GPIOB端口引脚输出电平翻转 */
#define	GPIOA_ReadPort()				(R32_PA_PIN)				/* GPIOA端口32位数据返回，低16位有效 */
#define	GPIOB_ReadPort()				(R32_PB_PIN)				/* GPIOB端口32位数据返回，低24位有效 */
#define	GPIOA_ReadPortPin( pin )		(R32_PA_PIN&pin)			/* GPIOA端口引脚状态，0-引脚低电平，(!0)-引脚高电平 */
#define	GPIOB_ReadPortPin( pin )		(R32_PB_PIN&pin)			/* GPIOB端口引脚状态，0-引脚低电平，(!0)-引脚高电平 */

void GPIOA_ITModeCfg( UINT32 pin, GPIOITModeTpDef mode );			/* GPIOA引脚中断模式配置 */
void GPIOB_ITModeCfg( UINT32 pin, GPIOITModeTpDef mode );			/* GPIOB引脚中断模式配置 */
#define	GPIOA_ReadITFlagPort()			(R8_GPIO_INT_FLAG)				/* 读取GPIOA端口中断标志状态 */
#define	GPIOB_ReadITFlagPort()			(R8_GPIO_INT_FLAG)				/* 读取GPIOB端口中断标志状态 */

/*************************************读中断位标志************************************/
#define	GPIOA_2_ReadITFlagBit(  )		(R8_GPIO_INT_FLAG & 0x01)		    /* 读取GPIOA端口引脚中断标志状态 */
#define	GPIOA_3_ReadITFlagBit(  )		(R8_GPIO_INT_FLAG & 0x02)
#define	GPIOA_4_ReadITFlagBit(  )		(R8_GPIO_INT_FLAG & 0x04)

#define	GPIOB_3_ReadITFlagBit(  )		(R8_GPIO_INT_FLAG & 0x08)		    /* 读取GPIOB端口引脚中断标志状态 */
#define	GPIOB_4_ReadITFlagBit(  )		(R8_GPIO_INT_FLAG & 0x10)
#define	GPIOB_11_ReadITFlagBit(  )		(R8_GPIO_INT_FLAG & 0x20)
#define	GPIOB_12_ReadITFlagBit(  )		(R8_GPIO_INT_FLAG & 0x40)
#define	GPIOB_15_ReadITFlagBit(  )		(R8_GPIO_INT_FLAG & 0x80)


/*************************************清中断位标志************************************/
#define	GPIOA_2_ClearITFlagBit(  )		(R8_GPIO_INT_FLAG = 0x01)		/* 清除GPIOA端口引脚中断标志状态 */
#define	GPIOA_3_ClearITFlagBit(  )		(R8_GPIO_INT_FLAG = 0x02)
#define	GPIOA_4_ClearITFlagBit(  )		(R8_GPIO_INT_FLAG = 0x04)

#define	GPIOB_3_ClearITFlagBit(  )		(R8_GPIO_INT_FLAG = 0x08)		/* 清除GPIOB端口引脚中断标志状态 */
#define	GPIOB_4_ClearITFlagBit(  )		(R8_GPIO_INT_FLAG = 0x10)
#define	GPIOB_11_ClearITFlagBit(  )		(R8_GPIO_INT_FLAG = 0x20)
#define	GPIOB_12_ClearITFlagBit(  )		(R8_GPIO_INT_FLAG = 0x40)
#define	GPIOB_15_ClearITFlagBit(  )		(R8_GPIO_INT_FLAG = 0x80)



void GPIOPinRemap( UINT8 s, UINT16 perph );				/* 外设功能引脚映射 */
void GPIOMco( UINT8 s, UINT16 freq );                     /* MCO功能 */



#ifdef __cplusplus
}
#endif

#endif  // __CH56x_GPIO_H__
