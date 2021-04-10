


#ifndef __CH56x_SPI_H__
#define __CH56x_SPI_H__

#ifdef __cplusplus
 extern "C" {
#endif


/**
  * @brief  SPI0 interrupt bit define
  */
#define	SPI0_IT_FST_BYTE 		RB_SPI_IE_FST_BYTE
#define	SPI0_IT_FIFO_OV			RB_SPI_IE_FIFO_OV
#define	SPI0_IT_DMA_END 		RB_SPI_IE_DMA_END
#define	SPI0_IT_FIFO_HF 		RB_SPI_IE_FIFO_HF
#define	SPI0_IT_BYTE_END		RB_SPI_IE_BYTE_END
#define	SPI0_IT_CNT_END 		RB_SPI_IE_CNT_END



/**
  * @brief  Configuration data mode
  */
typedef enum
{
	Mode0_LowBitINFront = 0,
	Mode0_HighBitINFront,
	Mode3_LowBitINFront,
	Mode3_HighBitINFront,
}ModeBitOrderTypeDef;


/**
  * @brief  Configuration SPI slave mode
  */
typedef enum
{
Mode_DataStream = 0,
Mose_FirstCmd,
}Slave_ModeTypeDef;


/**************** SPI0 */
void SPI0_MasterDefInit( void );
void SPI0_DataMode( ModeBitOrderTypeDef m );

void SPI0_MasterSendByte( UINT8 d );
UINT8 SPI0_MasterRecvByte( void );

void SPI0_MasterTrans( UINT8 *pbuf, UINT16 len );
void SPI0_MasterRecv( UINT8 *pbuf, UINT16 len );

void SPI0_MasterDMATrans( PUINT8 pbuf, UINT16 len);
void SPI0_MasterDMARecv( PUINT8 pbuf, UINT16 len);


void SPI0_SlaveInit( void );
#define SetFirst0Data(d)		(R8_SPI0_SLAVE_PRE = d)
void SPI0_SlaveSendByte( UINT8 d );
UINT8 SPI0_SlaveRecvByte( void );

void SPI0_SlaveTrans( UINT8 *pbuf, UINT16 len );
void SPI0_SlaveRecv( PUINT8 pbuf, UINT16 len );

// refer to SPI0 interrupt bit define
#define SPI0_ITCfg(s,f)	((s)?(R8_SPI0_INTER_EN|=f):(R8_SPI0_INTER_EN&=~f))
#define SPI0_GetITFlag(f)		(R8_SPI0_INT_FLAG&f)
#define SPI0_ClearITFlag(f)		(R8_SPI0_INT_FLAG = f)


/**************** SPI1 */
void SPI1_MasterDefInit( void );
void SPI1_DataMode( ModeBitOrderTypeDef m );

void SPI1_MasterSendByte( UINT8 d );
UINT8 SPI1_MasterRecvByte( void );

void SPI1_MasterTrans( UINT8 *pbuf, UINT16 len );
void SPI1_MasterRecv( UINT8 *pbuf, UINT16 len );

void SPI1_MasterDMATrans( PUINT8 pbuf, UINT16 len);
void SPI1_MasterDMARecv( PUINT8 pbuf, UINT16 len);

void SPI1_SlaveInit( void );
#define SetFirst1Data(d)			(R8_SPI1_SLAVE_PRE = d)
void SPI1_SlaveSendByte( UINT8 d );
UINT8 SPI1_SlaveRecvByte( void );

void SPI1_SlaveTrans( UINT8 *pbuf, UINT16 len );
void SPI1_SlaveRecv( PUINT8 pbuf, UINT16 len );

// refer to SPI1 interrupt bit define
#define SPI1_ITCfg(s,f)		((s)?(R8_SPI1_INTER_EN|=f):(R8_SPI1_INTER_EN&=~f))
#define SPI1_GetITFlag(f)		(R8_SPI1_INT_FLAG&f)
#define SPI1_ClearITFlag(f)	(R8_SPI1_INT_FLAG = f)


	
	 
#ifdef __cplusplus
}
#endif

#endif  // __CH56x_SPI_H__

