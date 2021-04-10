/********************************** (C) COPYRIGHT *******************************
* File Name          : CH56x_ecdc.c
* Author             : WCH
* Version            : V1.0
* Date               : 2020/07/31
* Description 
*******************************************************************************/

#include "CH56x_common.h"


/*******************************************************************************
* Function Name  : ECDC_Init
* Description    : 初始化
* Input          : ecdcmode:	0-SM4&ECB     1-AES&ECB     2-SM4&CTR     3-AES&CTR
*                  clkmode:     1-关闭        2-240M        3-160M
*                  keylen:      0-128bit      1-192bit      2-256bit
*                  pkey:    密钥值指针
*                  pcount:  计数器值指针
* Return         : None
*******************************************************************************/
void ECDC_Init( UINT8 ecdcmode, UINT8 clkmode, UINT8 keylen, PUINT32 pkey, PUINT32 pcount )
{
	R8_ECDC_INT_FG |= 0xFF;
	R16_ECEC_CTRL = 0;

	R16_ECEC_CTRL |= (ecdcmode&0x03)<<8;     		//工作模式选择
	R16_ECEC_CTRL |= (keylen&0x03)<<10;			     //密钥长度设置
	R16_ECEC_CTRL |= (clkmode&0x03)<<4;       		//加解密时钟分频系数,aes加解密工作在240Mhz下
	ECDC_SetKey(pkey, keylen);

	if(R16_ECEC_CTRL & RB_ECDC_CIPHER_MOD)			//只在CTR模式下执行，CTR与ECB模式编程时的唯一区别
			ECDC_SetCount(pcount);

	R8_ECDC_INT_FG |= RB_ECDC_IF_EKDONE;
	R16_ECEC_CTRL |= RB_ECDC_KEYEX_EN;
	R16_ECEC_CTRL &= ~RB_ECDC_KEYEX_EN;

	while(!(R8_ECDC_INT_FG & RB_ECDC_IF_EKDONE));
	R8_ECDC_INT_FG |= RB_ECDC_IF_EKDONE;
}

/*******************************************************************************
* Function Name  : ECDC_SetKey
* Description    : 设置密钥
* Input          : pkey:   密钥值指针
*                  keylen: 0-128bit   1-192bit   2-256bit
* Return         : None
*******************************************************************************/
void ECDC_SetKey( PUINT32 pkey, UINT8 keylen )
{
	keylen = keylen&0x03;

	R32_ECDC_KEY_31T0 = *pkey++;
	R32_ECDC_KEY_63T32 = *pkey++;
	R32_ECDC_KEY_95T64 = *pkey++;
	R32_ECDC_KEY_127T96 = *pkey++;

	if(keylen){
		R32_ECDC_KEY_159T128 = *pkey++;
		R32_ECDC_KEY_191T160 = *pkey++;
	}
	if(keylen>1){
		R32_ECDC_KEY_223T192 = *pkey++;
		R32_ECDC_KEY_255T224 = *pkey++;
	}
}

/*******************************************************************************
* Function Name  : ECDC_SetCount
* Description    : 设置计数器
* Input          : pcount:  计数器值指针
* Return         : None
*******************************************************************************/
void ECDC_SetCount( PUINT32 pcount )
{
	R32_ECDC_IV_31T0 = *pcount++;
	R32_ECDC_IV_63T32 = *pcount++;
	R32_ECDC_IV_95T64 = *pcount++;
	R32_ECDC_IV_127T96 = *pcount++;
}

/*******************************************************************************
* Function Name  : ECDC_Excute
* Description    : 设置方向和模式
* Input          : excutemode:	  RAMX加密			-0x84
*							      RAMX解密			-0x8c
*								  128bits数据单次加密	-0x02
*								  128bits数据单次解密	-0x0a
*								        外设到RAMX 加密  		-0x02
*				    			       外设到 RAMX 解密   		-0x0a
*								  RAMX到外设加密   		-0x04
*								  RAMX到外设解密   		-0x0c
*   			   endianmode:    big_endian-1      little_endian-0
* Return         : None
*******************************************************************************/
void ECDC_Excute( UINT8 excutemode, UINT8 endianmode )
{
	R16_ECEC_CTRL &= 0xDF71;
	R16_ECEC_CTRL |= excutemode;
	if(endianmode)
		R16_ECEC_CTRL |= RB_ECDC_DAT_MOD;
	else
		R16_ECEC_CTRL &= ~RB_ECDC_DAT_MOD;
}

/*******************************************************************************
* Function Name  : ECDC_SingleRegister
* Description    : 单次寄存器加解密
* Input          : pWdatbuff: 写入数据首地址
*                  pRdatbuff: 读取数据首地址
* Return         : None
*******************************************************************************/
void ECDC_SingleRegister( PUINT32 pWdatbuff, PUINT32 pRdatbuff )
{
	R32_ECDC_SGSD_127T96 = pWdatbuff[3];			//低地址
	R32_ECDC_SGSD_95T64 = pWdatbuff[2];
	R32_ECDC_SGSD_63T32 = pWdatbuff[1];
	R32_ECDC_SGSD_31T0 = pWdatbuff[0];				//高地址

	while(!(R8_ECDC_INT_FG & RB_ECDC_IF_SINGLE));
	R8_ECDC_INT_FG |= RB_ECDC_IF_SINGLE;

	pRdatbuff[3] = R32_ECDC_SGRT_127T96;
	pRdatbuff[2] = R32_ECDC_SGRT_95T64;
	pRdatbuff[1] = R32_ECDC_SGRT_63T32;
	pRdatbuff[0] = R32_ECDC_SGRT_31T0;
}

/*******************************************************************************
* Function Name  : ECDC_RAMX
* Description    : RAMX加解密
* Input          : ram_addr： 首地址
* 				   ram_len：     长度
* Return         : None
*******************************************************************************/
void ECDC_SelfDMA( UINT32 ram_addr, UINT32 ram_len )
{
	R32_ECDC_SRAM_ADDR = ram_addr;
	R32_ECDC_SRAM_LEN = ram_len;                    //开始转换

	while(!(R8_ECDC_INT_FG & RB_ECDC_IF_WRSRAM));   //完成标志位
	R8_ECDC_INT_FG |= RB_ECDC_IF_WRSRAM;
}

/*******************************************************************************
* Function Name  : ECDC_RloadCount
* Description    : CTR模式下，每加密/解密一块，重新载入计数器值
* Input          : pcount:  计数器值指针
* Return         : None
*******************************************************************************/
void ECDC_RloadCount( UINT8 excutemode, UINT8 endianmode, PUINT32 pcount )
{
	R16_ECEC_CTRL &= 0xDFF9;       //第二位第三位置0
	ECDC_SetCount(pcount);
	ECDC_Excute(excutemode, endianmode);
}

