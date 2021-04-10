/********************************** (C) COPYRIGHT *******************************
* File Name          : CH56x_emmc.c
* Author             : WCH
* Version            : V1.0
* Date               : 2020/07/31
* Description 
*******************************************************************************/
#include "CH56x_common.h"

/*******************************************************************************
* Function Name  : EMMCIO0Init
* Description    : EMMC Controller Initializtion
* Input          : None
* Return         : OP_SUCCESS
*******************************************************************************/
UINT8 EMMCIO0Init( void )
{
/* GPIO configuration */
    R32_PB_DRV |= bSDCMD;       //Command Line
    R32_PB_PU  |= bSDCMD;

    R32_PB_DIR |= bSDCK;        //CLK Line
    R32_PB_DRV |= bSDCK;

    R32_PB_PU  |= (0x1f<<17);   //Data Line
    R32_PA_PU  |= (7<<0);

    R32_PA_DRV |= (7<<0);     // Drive Capacity
    R32_PB_DRV |= (0x1f<<17); // Drive Capacity


/* Controller Register */
    R8_EMMC_CONTROL = RB_EMMC_ALL_CLR | RB_EMMC_RST_LGC ;                // reset all register
    R8_EMMC_CONTROL = RB_EMMC_NEGSMP | RB_EMMC_DMAEN ;                    // Enable EMMCcard
    R32_PB_DIR |= bSDCK;
    R16_EMMC_CLK_DIV = RB_EMMC_CLKOE | LOWEMMCCLK|RB_EMMC_PHASEINV;

/* Enable Interruption */
    R16_EMMC_INT_FG = 0xffff;
    R16_EMMC_INT_EN = RB_EMMC_IE_FIFO_OV |          //Enable error Interruption
                    RB_EMMC_IE_TRANERR |
                    RB_EMMC_IE_DATTMO |
                    RB_EMMC_IE_REIDX_ER |
                    RB_EMMC_IE_RECRC_WR |
                    RB_EMMC_IE_RE_TMOUT;

/* Overtime */
    R8_EMMC_TIMEOUT = 35;   // calculating overtime
    return OP_SUCCESS;
}

/*******************************************************************************
* Function Name  : CheckCMDComp
* Description    : Estimate the end of Command
* Input          : pEMMCPara
* Return         : CMD_NULL
*******************************************************************************/
UINT8 CheckCMDComp( PSD_PARAMETER pEMMCPara )
{
    if(R16_EMMC_INT_FG & RB_EMMC_IF_CMDDONE)
    {
        R16_EMMC_INT_FG = RB_EMMC_IF_CMDDONE;
        return CMD_SUCCESS;
    }
    if( pEMMCPara->EMMCOpErr )      return CMD_FAILED;
    return CMD_NULL;
}

/*******************************************************************************
* Function Name  : EMMCResetIdle
* Description    : when EMMC waiting status,do the OCR analysis
* Input          : pEMMCPara
* Return         : None
*******************************************************************************/
void EMMCResetIdle( PSD_PARAMETER pEMMCPara )
{
    UINT32 cmd_arg_val;
    UINT16 cmd_set_val;

    cmd_arg_val = 0x0;
    cmd_set_val = 0x0|EMMC_CMD0;
    EMMCSendCmd(cmd_arg_val, cmd_set_val);
    while( CheckCMDComp( pEMMCPara ) == CMD_NULL );
}


/*******************************************************************************
* Function Name  : EMMCReadOCR
* Description    : when EMMC waiting status,do the OCR analysis
* Input          : pEMMCPara
* Return         : CMD_SUCCESS
*******************************************************************************/
UINT8 EMMCReadOCR( PSD_PARAMETER pEMMCPara )
{
    UINT8  i;
    UINT32 cmd_arg_val;
    UINT16 cmd_set_val;
    UINT8  sta = 0;
    UINT32 cmd_rsp_val;  //command returned value

    for(i=0; i<100; i++)
    {
        cmd_arg_val = 0x40FF0080;                //request switching voltage
//      cmd_arg_val = 0x40000800;                //request switching voltage
        cmd_set_val = 0 |                        //ACK's index
                      0 |                        //CRC
                      RESP_TYPE_48 |             //ACK type
                      EMMC_CMD1;                   //command's index this moment
        mDelaymS(10);
        EMMCSendCmd(cmd_arg_val,  cmd_set_val);
        mDelayuS(2);
        while(1)
        {
            sta = CheckCMDComp( pEMMCPara );
            if( sta!= CMD_NULL )
            {
                break;
            }
        }

        if(sta == CMD_SUCCESS)
        {
            cmd_rsp_val = R32_EMMC_RESPONSE3;
            if(cmd_rsp_val & (1<<31))
            {
                break;
            }
        }
        mDelaymS(50);
    }
    if(i == 100)        return OP_FAILED;

    return sta;
}


/*******************************************************************************
* Function Name  : EMMCReadCID
* Description    : acquire 128bit CID parameter
* Input          : pEMMCPara
* Return         : OP_SUCCESS
*******************************************************************************/
UINT8 EMMCReadCID( PSD_PARAMETER pEMMCPara )
{
    UINT32 cmd_arg_val;
    UINT16 cmd_set_val;
    UINT8  sta;

    cmd_arg_val = 0;
    cmd_set_val = 0 |
                  0 |
                  RESP_TYPE_136 |
                  EMMC_CMD2;
    EMMCSendCmd( cmd_arg_val, cmd_set_val);
    while(1)
    {
        sta = CheckCMDComp( pEMMCPara );
        if( sta != CMD_NULL ) break;
    }
    if(sta == CMD_SUCCESS)
    {
        pEMMCPara->EMMC_CID[0] = R32_EMMC_RESPONSE0;
        pEMMCPara->EMMC_CID[1] = R32_EMMC_RESPONSE1;
        pEMMCPara->EMMC_CID[2] = R32_EMMC_RESPONSE2;
        pEMMCPara->EMMC_CID[3] = R32_EMMC_RESPONSE3;
    }

    return sta;
}

/*******************************************************************************
* Function Name  : EMMCSetRCA
* Description    : assign relative address to deviceARC 16bit
* Input          : pEMMCPara
* Return         : OP_SUCCESS
*******************************************************************************/
UINT8 EMMCSetRCA( PSD_PARAMETER pEMMCPara )
{
    UINT32 cmd_arg_val;
    UINT16 cmd_set_val;
    UINT8  sta;

    cmd_arg_val = 0xAAAA0000;
    cmd_set_val = RB_EMMC_CKIDX |
                  RB_EMMC_CKCRC |
                  RESP_TYPE_48  |
                  EMMC_CMD3;
    mDelaymS(10);
    EMMCSendCmd(cmd_arg_val,cmd_set_val);
    while(1)
    {
        sta = CheckCMDComp( pEMMCPara );
        if( sta != CMD_NULL ) break;
    }
    if(sta == CMD_SUCCESS)
    {
        pEMMCPara->EMMC_RCA = 0xAAAA;
    }

    return sta;
}

/*******************************************************************************
* Function Name  : EMMCReadCSD
* Description    : acquire 128bit CSD parameter and get it analyzed
* Input          : pEMMCPara
* Return         : OP_SUCCESS
*******************************************************************************/
UINT8 EMMCReadCSD( PSD_PARAMETER pEMMCPara )
{
    UINT32 cmd_arg_val;
    UINT16 cmd_set_val;
    UINT8  sta;
    UINT32 disk_block_num = 0;

    cmd_arg_val = pEMMCPara->EMMC_RCA<<16;
    cmd_set_val = 0 |
                  0 |
                  RESP_TYPE_136 |
                  EMMC_CMD9;
    EMMCSendCmd(cmd_arg_val, cmd_set_val);
    while(1)
    {
        sta = CheckCMDComp( pEMMCPara );
        if( sta != CMD_NULL ) break;
    }

    if(sta == CMD_SUCCESS)
    {
        pEMMCPara->EMMC_CSD[0] = R32_EMMC_RESPONSE0;
        pEMMCPara->EMMC_CSD[1] = R32_EMMC_RESPONSE1;
        pEMMCPara->EMMC_CSD[2] = R32_EMMC_RESPONSE2;
        pEMMCPara->EMMC_CSD[3] = R32_EMMC_RESPONSE3;


        disk_block_num = (((pEMMCPara->EMMC_CSD[2]&0x3ff)<<2) | ((pEMMCPara->EMMC_CSD[1])>>30));
        if(disk_block_num == 0xFFF)
        {
            pEMMCPara->EMMCSecNum = 0xFFF;
        }
        else
        {
            pEMMCPara->EMMCType = EMMCIO_CAPACITY_SD_CARD_V2_0;
            /* memory capacity = BLOCKNR*BLOCK_LEN = (C_SIZE+1)<<(C_SIZE_MULT+2)<<(READ_BL_LEN) */
            disk_block_num = ( (((pEMMCPara->EMMC_CSD[2]&0x3ff)<<2) | (pEMMCPara->EMMC_CSD[1]>>30)) + 1 );
            disk_block_num = ( (disk_block_num) << (((pEMMCPara->EMMC_CSD[1]>>15)&0x07) + 2));
        }
    }

    pEMMCPara->EMMCSecSize = 1<<((pEMMCPara->EMMC_CSD[2]>>16)&0x000f);
    return sta;
}

/*******************************************************************************
* Function Name  : SelectEMMCCard
* Description    : select card
* Input          : pEMMCPara
* Return         : OP_SUCCESS
*******************************************************************************/
UINT8 SelectEMMCCard(PSD_PARAMETER pEMMCPara)
{
    UINT32 cmd_arg_val;
    UINT16 cmd_set_val;
    UINT8  sta;

    cmd_arg_val = pEMMCPara->EMMC_RCA<<16;
    cmd_set_val = RB_EMMC_CKIDX |
                  RB_EMMC_CKCRC |
                  RESP_TYPE_48  |
                  EMMC_CMD7;
    EMMCSendCmd(cmd_arg_val, cmd_set_val);
    while(1)
    {
        sta = CheckCMDComp( pEMMCPara );
        if( sta != CMD_NULL ) break;
    }
    return sta;
}

UINT8 ReadEMMCStatus(PSD_PARAMETER pEMMCPara)
{
    UINT32 cmd_arg_val;
    UINT16 cmd_set_val;
    UINT8  sta;

    cmd_arg_val = pEMMCPara->EMMC_RCA<<16;
    cmd_set_val = RB_EMMC_CKIDX |
                  RB_EMMC_CKCRC |
                  RESP_TYPE_48  |
                  EMMC_CMD13;
    EMMCSendCmd(cmd_arg_val, cmd_set_val);
    while(1)
    {
        sta = CheckCMDComp( pEMMCPara );
        if( sta != CMD_NULL ) break;
    }
    return sta;
}

/*******************************************************************************
* Function Name  : SwitchEMMCIOBusType
* Description    : set the IO bus_mode
* Input          : pEMMCPara
*                  bus_mode
* Return         : OP_SUCCESS
*******************************************************************************/
UINT8 EMMCSetBusWidth(PSD_PARAMETER pEMMCPara, UINT8 bus_mode)
{
    UINT32 cmd_arg_val;
    UINT16 cmd_set_val;
    UINT8  sta;

    if(bus_mode == 0)       cmd_arg_val = 0x03B70100;
    else                    cmd_arg_val = 0x03B70200;
    cmd_set_val = RB_EMMC_CKIDX |
                  RB_EMMC_CKCRC |
                  RESP_TYPE_48  |
                  EMMC_CMD6;
    EMMCSendCmd(cmd_arg_val, cmd_set_val);
    while(1)
    {
        sta = CheckCMDComp( pEMMCPara );
        if( sta != CMD_NULL ) break;
    }
    return sta;
}

UINT8 EMMCSetHighSpeed(PSD_PARAMETER pEMMCPara)
{
    UINT32 cmd_arg_val;
    UINT16 cmd_set_val;
    UINT8  sta;

    cmd_arg_val = 0x03B90100;
    cmd_set_val = RB_EMMC_CKIDX |
                  RB_EMMC_CKCRC |
                  RESP_TYPE_R1b  |
                  EMMC_CMD6;
    EMMCSendCmd(cmd_arg_val, cmd_set_val);
    while(1)
    {
        sta = CheckCMDComp( pEMMCPara );
        if( sta != CMD_NULL ) break;
    }
    return sta;
}

/*******************************************************************************
* Function Name  : ConfigAllSD
* Description    : EMMC Information
* Input          : pEMMCPara
* Return         : OP_SUCCESS
*******************************************************************************/
#define  SD2CMD   EMMCSendCmd
__attribute__ ((aligned(8))) UINT8  buf[512]   __attribute__((section(".DMADATA")));
UINT8 EMMCCardConfig( PSD_PARAMETER pEMMCPara )
{
    UINT8  sta;
    //cmd0
        EMMCResetIdle( pEMMCPara );
        mDelaymS(30);
        sta = EMMCReadOCR( pEMMCPara );
        if(sta!=CMD_SUCCESS) return OP_FAILED;
        //cmd2
        sta = EMMCReadCID( pEMMCPara );
        if(sta!=CMD_SUCCESS)    return OP_FAILED;
        //cmd3
        mDelaymS(30);
        sta = EMMCSetRCA( pEMMCPara );
        if(sta!=CMD_SUCCESS)    return OP_FAILED;
        //cmd9
        sta = EMMCReadCSD( pEMMCPara );
        if(sta!=CMD_SUCCESS)    return OP_FAILED;
        //cmd7;
        mDelaymS(30);
        sta = SelectEMMCCard( pEMMCPara );
        if(sta!=CMD_SUCCESS)    return OP_FAILED;
        mDelaymS(30);
        if(pEMMCPara->EMMCSecNum == 0xFFF)
        {
            sta = EMMCCardReadEXCSD( pEMMCPara, buf );
            if(sta!=OP_SUCCESS)     return OP_FAILED;
            pEMMCPara->EMMCSecNum = *((PUINT32)&buf[212]);      // SEC_COUNT [215:212] MSB-LSB
        }
        //cmd6
        sta = EMMCSetBusWidth(pEMMCPara, 1);
        if(sta!=CMD_SUCCESS)    return OP_FAILED;
        R8_EMMC_CONTROL = (R8_EMMC_CONTROL&~RB_EMMC_LW_MASK) | bLW_OP_DAT8;     // 8line_mode
        mDelaymS(30);
        //configure high clock rate
        EMMCSetHighSpeed(pEMMCPara);
        //cmd13
        mDelaymS(30);
       sta = ReadEMMCStatus( pEMMCPara );
       if(sta!=CMD_SUCCESS)    return OP_FAILED;
       mDelaymS(30);
    //  R8_EMMC_CONTROL |= RB_EMMC_NEGSMP;
        R16_EMMC_CLK_DIV =  RB_EMMC_CLKMode|
                            RB_EMMC_PHASEINV|
                            RB_EMMC_CLKOE  |
                            8;
        return OP_SUCCESS;
}

UINT8 EMMCCardConfig_N( PSD_PARAMETER pEMMCPara )
{
    UINT8  sta;

    EMMCResetIdle( pEMMCPara );
    mDelaymS(30);
    sta = EMMCReadOCR( pEMMCPara );
    if(sta!=CMD_SUCCESS)
    {
        return OP_FAILED;
    }
    sta = EMMCReadCID( pEMMCPara );
    if(sta!=CMD_SUCCESS)
    {
        return OP_FAILED;
    }
    mDelaymS(30);
    sta = EMMCSetRCA( pEMMCPara );
    if(sta!=CMD_SUCCESS)
    {
        return OP_FAILED;
    }
    sta = EMMCReadCSD( pEMMCPara );
    if(sta!=CMD_SUCCESS)
    {
        return OP_FAILED;
    }
    mDelaymS(30);
    sta = SelectEMMCCard( pEMMCPara );
    if(sta!=CMD_SUCCESS)
    {
        return OP_FAILED;
    }

    R16_EMMC_CLK_DIV = RB_EMMC_CLKMode|
                    RB_EMMC_CLKOE  |
                     LOWEMMCCLK;
    sta = ReadEMMCStatus( pEMMCPara );
    if(sta!=CMD_SUCCESS)
    {
        return OP_FAILED;
    }
    if(pEMMCPara->EMMCSecNum == 0xFFF)
    {
        sta = EMMCCardReadEXCSD( pEMMCPara, buf );
        if(sta!=OP_SUCCESS)     return OP_FAILED;
        pEMMCPara->EMMCSecNum = *((PUINT32)&buf[212]);      // SEC_COUNT [215:212] MSB-LSB
    }
    sta = EMMCSetBusWidth(pEMMCPara, 1);
    if(sta!=CMD_SUCCESS)    return OP_FAILED;
    R8_EMMC_CONTROL = (R8_EMMC_CONTROL&~RB_EMMC_LW_MASK) | bLW_OP_DAT8;     // 8line_mode

    //switching frequency
    while(!(R32_EMMC_STATUS & (1<<17)));
    sta = EMMCSetHighSpeed(pEMMCPara);
    if(sta!=CMD_SUCCESS)    return OP_FAILED;
    while(!(R32_EMMC_STATUS & (1<<17)));
    //configure higher clock rate
    R16_EMMC_CLK_DIV = RB_EMMC_CLKMode|
                RB_EMMC_CLKOE  |
                4|RB_EMMC_PHASEINV;
    return OP_SUCCESS;
}
/*******************************************************************************
* Function Name  : EMMCIOTransErrorDeal
* Description    : error processing
* Input          : None
* Return         : OP_SUCCESS
*******************************************************************************/
UINT8 EMMCIOTransErrorDeal( PSD_PARAMETER pEMMCPara )
{
    pEMMCPara->EMMCOpErr = R16_EMMC_INT_FG;
    R16_EMMC_INT_FG = 0xffff;

    return OP_SUCCESS;
}

/*******************************************************************************
* Function Name  : EMMCCardReadEXCSD
* Description    : read single section
* Input          : pEMMCPara
*                  Lbaaddr -- section first address
* Return         : OP_SUCCESS
*******************************************************************************/
UINT8 EMMCCardReadEXCSD( PSD_PARAMETER pEMMCPara, PUINT8 pRdatbuf )
{
    UINT32 cmd_arg_val;
    UINT16 cmd_set_val;

    R32_EMMC_DMA_BEG1 = (UINT32)pRdatbuf;
    R32_EMMC_TRAN_MODE = 0;
    R32_EMMC_BLOCK_CFG = 512<<16 | 1;

    cmd_arg_val = 0;
    cmd_set_val = RB_EMMC_CKIDX |
                  RB_EMMC_CKCRC |
                  RESP_TYPE_48  |
                  EMMC_CMD8;
    EMMCSendCmd(cmd_arg_val, cmd_set_val);

    while(1)
    {
        if(R16_EMMC_INT_FG & RB_EMMC_IF_TRANDONE)   break;
        if( pEMMCPara->EMMCOpErr )      return CMD_FAILED;
    }

    R16_EMMC_INT_FG = 0xffff;

    return  OP_SUCCESS;
}


/*******************************************************************************
* Function Name  : EMMCCardReadOneSec
* Description    : read single section
* Input          : pEMMCPara
*                  pRdatbuf -- read buffer address
*                  Lbaaddr
* Return         : OP_SUCCESS
*******************************************************************************/
UINT8 EMMCCardReadOneSec( PSD_PARAMETER pEMMCPara, PUINT8 pRdatbuf, UINT32 Lbaaddr )
{
    UINT32 cmd_arg_val;
    UINT16 cmd_set_val;

    if(Lbaaddr > (pEMMCPara->EMMCSecNum))       return  OP_INVALID_ADD;

    R32_EMMC_DMA_BEG1 = (UINT32)pRdatbuf;
    R32_EMMC_TRAN_MODE = 0;
    R32_EMMC_BLOCK_CFG = (pEMMCPara->EMMCSecSize)<<16 | 1;

    cmd_arg_val = Lbaaddr;
    cmd_set_val = RB_EMMC_CKIDX |
                  RB_EMMC_CKCRC |
                  RESP_TYPE_48  |
                  EMMC_CMD17;
    EMMCSendCmd(cmd_arg_val, cmd_set_val);

    while(1)
    {
        if(R16_EMMC_INT_FG & RB_EMMC_IF_TRANDONE)   break;
        if( pEMMCPara->EMMCOpErr )      return CMD_FAILED;
    }

    R16_EMMC_INT_FG = 0xffff;

    return  OP_SUCCESS;
}

/*******************************************************************************
* Function Name  : EMMCCardReadMulSec
* Description    : read continuous multiple sections
* Input          : pEMMCPara
*                  pReqnum -- request continuous sections address
*                  pRdatbuf
*                  Lbaaddr
* Return         : OP_SUCCESS
*******************************************************************************/
UINT8 EMMCCardReadMulSec( PSD_PARAMETER pEMMCPara, PUINT16 pReqnum, PUINT8 pRdatbuf, UINT32 Lbaaddr )
{
    UINT32 cmd_arg_val;
    UINT16 cmd_set_val;
    UINT8  sta;

    if(Lbaaddr > (pEMMCPara->EMMCSecNum))       return  OP_INVALID_ADD;

    R32_EMMC_DMA_BEG1 = (UINT32)pRdatbuf;                                       //data buffer address
    R32_EMMC_TRAN_MODE = 0;                                                     //EMMC to controller
    R32_EMMC_BLOCK_CFG = (pEMMCPara->EMMCSecSize)<<16 | (*pReqnum);

    //cmd18
    cmd_arg_val = Lbaaddr;
    cmd_set_val = RB_EMMC_CKIDX |
                  RB_EMMC_CKCRC |
                  RESP_TYPE_48  |
                  EMMC_CMD18;
    EMMCSendCmd(cmd_arg_val, cmd_set_val);
    while(1)
    {
        if(R16_EMMC_INT_FG & RB_EMMC_IF_TRANDONE)
        {
            R16_EMMC_INT_FG = RB_EMMC_IF_CMDDONE;
            //cmd12
            cmd_arg_val = 0;
            cmd_set_val = RB_EMMC_CKIDX |
                          RB_EMMC_CKCRC |
                          RESP_TYPE_R1b |
                          EMMC_CMD12;
            EMMCSendCmd(cmd_arg_val, cmd_set_val);
            break;
        }
        if( pEMMCPara->EMMCOpErr )      return CMD_FAILED;
    }

    while(1)
    {
        sta = CheckCMDComp( pEMMCPara );
        if( sta != CMD_NULL ) break;
    }

    R16_EMMC_INT_FG = 0xffff;
    *pReqnum = (UINT16)R32_EMMC_STATUS;     // successfully transferred sections

    return  sta;
}

/*******************************************************************************
* Function Name  : EMMCCardWriteMulSec
* Description    : write continuous multiple sections
* Input          : pSDPara
*                  pReqnum
*                  pWdatbuf
*                  Lbaaddr
* Return         : OP_SUCCESS
*******************************************************************************/
UINT8 EMMCCardWriteMulSec( PSD_PARAMETER pEMMCPara, PUINT16 pReqnum, PUINT8 pWdatbuf, UINT32 Lbaaddr )
{
    UINT32 cmd_arg_val;
    UINT16 cmd_set_val;
    UINT8  sta;

    if(Lbaaddr > (pEMMCPara->EMMCSecNum))       return  OP_INVALID_ADD;

    //cmd25
    cmd_arg_val = Lbaaddr;
    cmd_set_val = RB_EMMC_CKIDX |
                  RB_EMMC_CKCRC |
                  RESP_TYPE_48  |
                  EMMC_CMD25;
    EMMCSendCmd(cmd_arg_val, cmd_set_val);
    while(1)
    {
        sta = CheckCMDComp( pEMMCPara );
        if( sta != CMD_NULL ) break;
    }
    if( sta == CMD_FAILED )     return OP_FAILED;

    //DAT

    R32_EMMC_TRAN_MODE = RB_EMMC_DMA_DIR;
    R32_EMMC_DMA_BEG1 = (UINT32)pWdatbuf;
    R32_EMMC_BLOCK_CFG = (pEMMCPara->EMMCSecSize)<<16 | (*pReqnum);

    while(1)
    {
        if(R16_EMMC_INT_FG & RB_EMMC_IF_BKGAP)
        {
            R32_EMMC_RESPONSE3 = 0;
            R16_EMMC_INT_FG = RB_EMMC_IF_BKGAP;
        }
        else if(R16_EMMC_INT_FG & RB_EMMC_IF_TRANDONE)
        {
            R16_EMMC_INT_FG = RB_EMMC_IF_CMDDONE;
            //cmd12
            cmd_arg_val = 0;
            cmd_set_val = RB_EMMC_CKIDX |
                          RB_EMMC_CKCRC |
                          RESP_TYPE_R1b |
                          EMMC_CMD12;
            EMMCSendCmd(cmd_arg_val, cmd_set_val);
            break;
        }
        if( pEMMCPara->EMMCOpErr )      return CMD_FAILED;
    }

    while(1)
    {
        sta = CheckCMDComp( pEMMCPara );
        if( sta != CMD_NULL ) break;
    }

    R16_EMMC_INT_FG = 0xffff;
    *pReqnum = (UINT16)R32_EMMC_STATUS;

    return  sta;
}

/*******************************************************************************
* Function Name  : AES_EMMCWriteMulSec
* Description    : Write continuous multiple sections
* Input          : pEMMCPara - SD information structure pointer
*                  pReqnum - Request to the sector number variable address consecutively
*                  pWdatbuf - Write to the data cache address
*                  Lbaaddr - Write the sector head address continuously
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 AES_EMMCWriteMulSec( PSD_PARAMETER pEMMCPara, PUINT32 pReqnum, PUINT32 pWdatbuf, UINT32 Lbaaddr, UINT8 excutemode, UINT8 endianmode, PUINT32 pcount)//闁哄洤鐡ㄩ弫鍏肩閸℃ɑ娈堕柟璇″枤鐞氼偊宕归敓锟�?
{
    UINT32 cmd_arg_val;
    UINT16 cmd_set_val;
    UINT8  sta;

    if(Lbaaddr > (pEMMCPara->EMMCSecNum))       return  OP_INVALID_ADD;

    //cmd25
    cmd_arg_val = Lbaaddr;
    cmd_set_val = RB_EMMC_CKIDX |
                  RB_EMMC_CKCRC |
                  RESP_TYPE_48  |
                  EMMC_CMD25;
    EMMCSendCmd(cmd_arg_val, cmd_set_val);
    while(1)
    {
        sta = CheckCMDComp( pEMMCPara );
        if( sta != CMD_NULL ) break;
    }
    if( sta == CMD_FAILED )     return OP_FAILED;

    //DAT

    ECDC_Excute(excutemode, endianmode);

    R32_EMMC_TRAN_MODE |= RB_EMMC_DMA_DIR;
    R32_EMMC_DMA_BEG1 = (UINT32)pWdatbuf;
    R32_EMMC_BLOCK_CFG = (pEMMCPara->EMMCSecSize)<<16 | (*pReqnum);

    while(1)
    {
        if(R16_EMMC_INT_FG & RB_EMMC_IF_BKGAP)
        {
            ///////////////////////////////////////////////CTR mode
            if(R16_ECEC_CTRL & RB_ECDC_CIPHER_MOD)
                ECDC_RloadCount( excutemode, endianmode, pcount );

            R32_EMMC_RESPONSE3 = 0;                 //start transform
            R16_EMMC_INT_FG |= RB_EMMC_IF_BKGAP;
        }
        else if(R16_EMMC_INT_FG & RB_EMMC_IF_TRANDONE)
        {
            R16_EMMC_INT_FG = RB_EMMC_IF_CMDDONE;
            //cmd12
            cmd_arg_val = 0;
            cmd_set_val = RB_EMMC_CKIDX |
                          RB_EMMC_CKCRC |
                          RESP_TYPE_R1b |
                          EMMC_CMD12;
            EMMCSendCmd(cmd_arg_val, cmd_set_val);
            break;
        }
        if( pEMMCPara->EMMCOpErr )      return CMD_FAILED;
    }
    while(1)
    {
        sta = CheckCMDComp( pEMMCPara );
        if( sta != CMD_NULL ) break;
    }

    R16_EMMC_INT_FG = 0xffff;
    *pReqnum = (UINT16)R32_EMMC_STATUS;     //the number of blocks transferred successfully

    return  sta;
}

/*******************************************************************************
* Function Name  : AES_EMMCReadMulSec
* Description    : Read continuous multiple sections
* Input          : pEMMCPara - SD information structure pointer
*                  pReqnum - Request to the sector number variable address consecutively
*                  pWdatbuf - Write to the data cache address
*                  Lbaaddr - Write the sector head address continuously
* Output         : None
* Return         : OP_SUCCESS - suc
*                  other - err
*******************************************************************************/
UINT8 AES_EMMCReadMulSec( PSD_PARAMETER pEMMCPara, PUINT32 pReqnum, PUINT32 pRdatbuf, UINT32 Lbaaddr, UINT8 excutemode, UINT8 endianmode, PUINT32 pcount)//闁哄洤鐡ㄩ弫鍏肩閸℃ɑ娈堕柟璇″枤鐞氼偊宕归敓锟�?
{
    UINT32 cmd_arg_val;
    UINT16 cmd_set_val;

    if(Lbaaddr > (pEMMCPara->EMMCSecNum))       return  OP_INVALID_ADD;

    //CTR mode.Once the transmission is completed ,turn off the clock.
    if(R16_ECEC_CTRL & RB_ECDC_CIPHER_MOD)
    {
        R16_ECEC_CTRL &= 0xFFF9;       //bit2 and bit3 set 0
        ECDC_SetCount(pcount);
        R32_EMMC_TRAN_MODE |= RB_EMMC_AUTOGAPSTOP | RB_EMMC_GAP_STOP;
    }

    ECDC_Excute(excutemode, endianmode);

    R32_EMMC_DMA_BEG1 = (UINT32)pRdatbuf;
    R32_EMMC_TRAN_MODE &= ~RB_EMMC_DMA_DIR;
    R32_EMMC_BLOCK_CFG = (pEMMCPara->EMMCSecSize)<<16 | (*pReqnum);

    //cmd18
    cmd_arg_val = Lbaaddr;
    cmd_set_val = RB_EMMC_CKIDX |
                  RB_EMMC_CKCRC |
                  RESP_TYPE_48  |
                  EMMC_CMD18;
    EMMCSendCmd(cmd_arg_val, cmd_set_val);

    while(1)
    {
        if(R16_ECEC_CTRL & RB_ECDC_CIPHER_MOD)
        {
            if(R16_EMMC_INT_FG & RB_EMMC_IF_BKGAP)
            {
            ///////////////////////////////////////////////CTR mode
                if(R16_ECEC_CTRL & RB_ECDC_CIPHER_MOD)
                    ECDC_RloadCount( excutemode, endianmode, pcount );

                R32_EMMC_TRAN_MODE &= ~RB_EMMC_GAP_STOP;
                R16_EMMC_INT_FG |= RB_EMMC_IF_BKGAP;
            }
        }
        if(R16_EMMC_INT_FG & RB_EMMC_IF_TRANDONE)   break;
        if( pEMMCPara->EMMCOpErr )      return CMD_FAILED;
    }


    R16_EMMC_INT_FG = 0xffff;
    *pReqnum = (UINT16)R32_EMMC_STATUS;     //the number of blocks transferred successfully

    return  OP_SUCCESS;
}


