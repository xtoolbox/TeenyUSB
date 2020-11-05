/**
  ******************************************************************************
  * @file    system_stm32h7xx.c
  * @author  MCD Application Team
  * @brief   CMSIS Cortex-Mx Device Peripheral Access Layer System Source File.
  *
  *   This file provides two functions and one global variable to be called from
  *   user application:
  *      - SystemInit(): This function is called at startup just after reset and
  *                      before branch to main program. This call is made inside
  *                      the "startup_stm32h7xx.s" file.
  *
  *      - SystemCoreClock variable: Contains the core clock (HCLK), it can be used
  *                                  by the user application to setup the SysTick
  *                                  timer or configure other parameters.
  *
  *      - SystemCoreClockUpdate(): Updates the variable SystemCoreClock and must
  *                                 be called whenever the core clock is changed
  *                                 during program execution.
  *
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/** @addtogroup CMSIS
  * @{
  */

/** @addtogroup stm32h7xx_system
  * @{
  */

/** @addtogroup STM32H7xx_System_Private_Includes
  * @{
  */

#include "stm32h7xx.h"

#if !defined  (HSE_VALUE)
#define HSE_VALUE    ((uint32_t)25000000) /*!< Value of the External oscillator in Hz */
#endif /* HSE_VALUE */

#if !defined  (CSI_VALUE)
  #define CSI_VALUE    ((uint32_t)4000000) /*!< Value of the Internal oscillator in Hz*/
#endif /* CSI_VALUE */

#if !defined  (HSI_VALUE)
  #define HSI_VALUE    ((uint32_t)64000000) /*!< Value of the Internal oscillator in Hz*/
#endif /* HSI_VALUE */


/**
  * @}
  */

/** @addtogroup STM32H7xx_System_Private_TypesDefinitions
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32H7xx_System_Private_Defines
  * @{
  */

/************************* Miscellaneous Configuration ************************/
/*!< Uncomment the following line if you need to use external SRAM or SDRAM mounted
     on EVAL board as data memory  */
/*#define DATA_IN_ExtSRAM */
/*#define DATA_IN_ExtSDRAM*/

#if defined(DATA_IN_ExtSRAM) && defined(DATA_IN_ExtSDRAM)
 #error "Please select DATA_IN_ExtSRAM or DATA_IN_ExtSDRAM "
#endif /* DATA_IN_ExtSRAM && DATA_IN_ExtSDRAM */

/*!< Uncomment the following line if you need to relocate your vector Table in
     Internal SRAM. */
/* #define VECT_TAB_SRAM */
#define VECT_TAB_OFFSET  0x00       /*!< Vector Table base offset field.
                                      This value must be a multiple of 0x200. */
/******************************************************************************/

/**
  * @}
  */

/** @addtogroup STM32H7xx_System_Private_Macros
  * @{
  */

/**
  * @}
  */

/** @addtogroup STM32H7xx_System_Private_Variables
  * @{
  */
  /* This variable is updated in three ways:
      1) by calling CMSIS function SystemCoreClockUpdate()
      2) by calling HAL API function HAL_RCC_GetHCLKFreq()
      3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
         Note: If you use this function to configure the system clock; then there
               is no need to call the 2 first functions listed above, since SystemCoreClock
               variable is updated automatically.
  */
  uint32_t SystemCoreClock = 64000000;
  uint32_t SystemD2Clock = 64000000;
  const  uint8_t D1CorePrescTable[16] = {0, 0, 0, 0, 1, 2, 3, 4, 1, 2, 3, 4, 6, 7, 8, 9};

/**
  * @}
  */

/** @addtogroup STM32H7xx_System_Private_FunctionPrototypes
  * @{
  */
#if defined (DATA_IN_ExtSRAM) || defined (DATA_IN_ExtSDRAM)
  static void SystemInit_ExtMemCtl(void);
#endif /* DATA_IN_ExtSRAM || DATA_IN_ExtSDRAM */

/**
  * @}
  */

uint32_t HAL_RCC_GetSysClockFreq(void)
{
 uint32_t pllp = 1, pllsource = 0, pllm = 1 ,pllfracen =0 , hsivalue = 0;
 float fracn1=0, pllvco = 0;
 uint32_t sysclockfreq = 0;
  /* Get SYSCLK source -------------------------------------------------------*/

  switch (RCC->CFGR & RCC_CFGR_SWS)
  {
  case 0x00:  /* HSI used as system clock source */
  
   if (__HAL_RCC_GET_FLAG(RCC_FLAG_HSIDIV) != 0U)
      {
        sysclockfreq = (uint32_t) (HSI_VALUE >> (__HAL_RCC_GET_HSI_DIVIDER()>> 3));
      }
      else
      {
        sysclockfreq = (uint32_t) HSI_VALUE;
      }
      
    break;

  case 0x08:  /* CSI used as system clock  source */
    sysclockfreq = CSI_VALUE;
    break;

  case 0x10:  /* HSE used as system clock  source */
    sysclockfreq = HSE_VALUE;
    break;

  case 0x18:  /* PLL1 used as system clock  source */

    /* PLL_VCO = (HSE_VALUE or HSI_VALUE or CSI_VALUE/ PLLM) * PLLN
    SYSCLK = PLL_VCO / PLLR
    */
    pllsource = (RCC->PLLCKSELR & RCC_PLLCKSELR_PLLSRC);
    pllm = ((RCC->PLLCKSELR & RCC_PLLCKSELR_DIVM1)>> 4)  ;
    pllfracen = RCC->PLLCFGR & RCC_PLLCFGR_PLL1FRACEN;
    fracn1 = (pllfracen* ((RCC->PLL1FRACR & RCC_PLL1FRACR_FRACN1)>> 3));

    switch (pllsource)
    {
    case 0x00:  /* HSI used as PLL clock source */

     if (__HAL_RCC_GET_FLAG(RCC_FLAG_HSIDIV) != 0U)
      {
        hsivalue= (HSI_VALUE >> (__HAL_RCC_GET_HSI_DIVIDER()>> 3));
        pllvco = ( hsivalue / pllm) * ((RCC->PLL1DIVR & RCC_PLL1DIVR_N1) + (fracn1/0x2000) +1 );
      }
      else
      {
        pllvco = (HSI_VALUE / pllm) * ((RCC->PLL1DIVR & RCC_PLL1DIVR_N1) + (fracn1/0x2000) +1 );
      }
      break;

    case 0x01:  /* CSI used as PLL clock source */
      pllvco = (CSI_VALUE / pllm) * ((RCC->PLL1DIVR & RCC_PLL1DIVR_N1) + (fracn1/0x2000) +1 );
      break;

    case 0x02:  /* HSE used as PLL clock source */
      pllvco = (HSE_VALUE / pllm) * ((RCC->PLL1DIVR & RCC_PLL1DIVR_N1) + (fracn1/0x2000) +1 );
      break;

    default:
      pllvco = (CSI_VALUE / pllm) * ((RCC->PLL1DIVR & RCC_PLL1DIVR_N1) + (fracn1/0x2000) +1 );
      break;
    }
    pllp = (((RCC->PLL1DIVR & RCC_PLL1DIVR_P1) >>9) + 1 ) ;
    sysclockfreq =  (uint32_t)(pllvco/pllp);
    break;

  default:
    sysclockfreq = CSI_VALUE;
    break;
  }

  return sysclockfreq;
}
  
static void SystemClock_Config(void)
{
  //RCC_OscInitTypeDef RCC_OscInitStruct;
  //RCC_ClkInitTypeDef RCC_ClkInitStruct;
  //RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    /**Supply configuration update enable 
    */
  MODIFY_REG(PWR->CR3, PWR_CR3_SCUEN, 0);

    /**Configure the main internal regulator output voltage 
    */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while ((PWR->D3CR & (PWR_D3CR_VOSRDY)) != PWR_D3CR_VOSRDY) 
  {
    
  }
    /**Initializes the CPU, AHB and APB busses clocks 
    */
#define  RCC_OscInitStruct__OscillatorType   RCC_OSCILLATORTYPE_HSI48 | RCC_OSCILLATORTYPE_HSI
#define  RCC_OscInitStruct__HSIState   RCC_HSI_DIV1
#define  RCC_OscInitStruct__HSICalibrationValue RCC_HSICALIBRATION_DEFAULT
#define  RCC_OscInitStruct__HSI48State RCC_HSI48_ON
#define  RCC_OscInitStruct__PLL_PLLState   RCC_PLL_ON
#define  RCC_OscInitStruct__PLL_PLLSource   RCC_PLLSOURCE_HSI
#define  RCC_OscInitStruct__PLL_PLLM   4
#define  RCC_OscInitStruct__PLL_PLLN   50
#define  RCC_OscInitStruct__PLL_PLLP   2
#define  RCC_OscInitStruct__PLL_PLLQ   10
#define  RCC_OscInitStruct__PLL_PLLR   2
#define  RCC_OscInitStruct__PLL_PLLRGE   RCC_PLL1VCIRANGE_3
#define  RCC_OscInitStruct__PLL_PLLVCOSEL   RCC_PLL1VCOWIDE
#define  RCC_OscInitStruct__PLL_PLLFRACN   0
    uint32_t tickstart;

  /*----------------------------- HSI Configuration --------------------------*/
  if(((RCC_OscInitStruct__OscillatorType) & RCC_OSCILLATORTYPE_HSI) == RCC_OSCILLATORTYPE_HSI)
  {
    /* Check the parameters */
    //assert_param(IS_RCC_HSI(RCC_OscInitStruct__HSIState));
    //assert_param(IS_RCC_CALIBRATION_VALUE(RCC_OscInitStruct__HSICalibrationValue));

    /* When the HSI is used as system clock it will not disabled */
    if((__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_CFGR_SWS_HSI) || ((__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_CFGR_SWS_PLL1) && ((RCC->PLLCKSELR & RCC_PLLCKSELR_PLLSRC) == RCC_PLLCKSELR_PLLSRC_HSI)))
    {
      /* When HSI is used as system clock it will not disabled */
      if((__HAL_RCC_GET_FLAG(RCC_FLAG_HSIRDY) != RESET) && (RCC_OscInitStruct__HSIState == RCC_HSI_OFF))
      {
        while(1);
      }
      /* Otherwise, just the calibration is allowed */
      else
      {
      /* Enable the Internal High Speed oscillator (HSI, HSIDIV2,HSIDIV4, or HSIDIV8) */
        __HAL_RCC_HSI_CONFIG(RCC_OscInitStruct__HSIState);

        /* Get Start Tick*/
        tickstart = HAL_GetTick();

        /* Wait till HSI is ready */
        while(__HAL_RCC_GET_FLAG(RCC_FLAG_HSIRDY) == RESET)
        {
          if((int32_t) (HAL_GetTick() - tickstart ) > HSI_TIMEOUT_VALUE)
          {
            while(1);
          }
        }
        /* Adjusts the Internal High Speed oscillator (HSI) calibration value.*/
        __HAL_RCC_HSI_CALIBRATIONVALUE_ADJUST(RCC_OscInitStruct__HSICalibrationValue);
      }
    }
    
    else
    {
      /* Check the HSI State */
      if((RCC_OscInitStruct__HSIState)!= RCC_HSI_OFF)
      {
     /* Enable the Internal High Speed oscillator (HSI, HSIDIV2,HSIDIV4, or HSIDIV8) */
        __HAL_RCC_HSI_CONFIG(RCC_OscInitStruct__HSIState);

        /* Get Start Tick*/
        tickstart = HAL_GetTick();

        /* Wait till HSI is ready */
        while(__HAL_RCC_GET_FLAG(RCC_FLAG_HSIRDY) == RESET)
        {
          if((int32_t) (HAL_GetTick() - tickstart ) > HSI_TIMEOUT_VALUE)
          {
            while(1);
          }
        }

        /* Adjusts the Internal High Speed oscillator (HSI) calibration value.*/
        __HAL_RCC_HSI_CALIBRATIONVALUE_ADJUST(RCC_OscInitStruct__HSICalibrationValue);
      }
      else
      {
        /* Disable the Internal High Speed oscillator (HSI). */
        __HAL_RCC_HSI_DISABLE();

        /* Get Start Tick*/
        tickstart = HAL_GetTick();

        /* Wait till HSI is ready */
        while(__HAL_RCC_GET_FLAG(RCC_FLAG_HSIRDY) != RESET)
        {
          if((int32_t) (HAL_GetTick() - tickstart ) > HSI_TIMEOUT_VALUE)
          {
            while(1);
          }
        }
      }
    }
  }

  /*------------------------------ HSI48 Configuration -------------------------*/ 
  if(((RCC_OscInitStruct__OscillatorType) & RCC_OSCILLATORTYPE_HSI48) == RCC_OSCILLATORTYPE_HSI48)
  {
    /* Check the parameters */
    //assert_param(IS_RCC_HSI48(RCC_OscInitStruct__HSI48State));
    
    /* Check the HSI48 State */
    if((RCC_OscInitStruct__HSI48State)!= RCC_HSI48_OFF)
    {
      /* Enable the Internal Low Speed oscillator (HSI48). */
      __HAL_RCC_HSI48_ENABLE();
      
      /* Get time-out */
      tickstart = HAL_GetTick();
      
      /* Wait till HSI48 is ready */  
      while(__HAL_RCC_GET_FLAG(RCC_FLAG_HSI48RDY) == RESET)
      {
        if((HAL_GetTick() - tickstart ) > HSI48_TIMEOUT_VALUE)
        {
          while(1);
        }      
      } 
    }
    else
    {
      /* Disable the Internal Low Speed oscillator (HSI48). */
      __HAL_RCC_HSI48_DISABLE();
      
      /* Get time-out */
      tickstart = HAL_GetTick();
      
      /* Wait till HSI48 is ready */  
      while(__HAL_RCC_GET_FLAG(RCC_FLAG_HSI48RDY) != RESET)
      {
        if((HAL_GetTick() - tickstart ) > HSI48_TIMEOUT_VALUE)
        {
          while(1);
        }      
      } 
    }
  }
  
  /*-------------------------------- PLL Configuration -----------------------*/
  /* Check the parameters */
  //assert_param(IS_RCC_PLL(RCC_OscInitStruct__PLL_PLLState));
  if ((RCC_OscInitStruct__PLL_PLLState) != RCC_PLL_NONE)
  {
    /* Check if the PLL is used as system clock or not */
    if(__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_CFGR_SWS_PLL1)
    {
      if((RCC_OscInitStruct__PLL_PLLState) == RCC_PLL_ON)
      {
        /* Check the parameters */
        assert_param(IS_RCC_PLLSOURCE(RCC_OscInitStruct__PLL_PLLSource));
        assert_param(IS_RCC_PLLM_VALUE(RCC_OscInitStruct__PLL_PLLM));
        assert_param(IS_RCC_PLLN_VALUE(RCC_OscInitStruct__PLL_PLLN));
        assert_param(IS_RCC_PLLP_VALUE(RCC_OscInitStruct__PLL_PLLP));
        assert_param(IS_RCC_PLLQ_VALUE(RCC_OscInitStruct__PLL_PLLQ));
        assert_param(IS_RCC_PLLQ_VALUE(RCC_OscInitStruct__PLL_PLLR));

        /* Disable the main PLL_ */
        __HAL_RCC_PLL_DISABLE();

        /* Get Start Tick*/
        tickstart = HAL_GetTick();

        /* Wait till PLL is ready */
        while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY) != RESET)
        {
          if((int32_t) (HAL_GetTick() - tickstart ) > PLL_TIMEOUT_VALUE)
          {
            while(1);
          }
        }

        /* Configure the main PLL clock source, multiplication and division factors. */
        __HAL_RCC_PLL_CONFIG(RCC_OscInitStruct__PLL_PLLSource,
                             RCC_OscInitStruct__PLL_PLLM,
                             RCC_OscInitStruct__PLL_PLLN,
                             RCC_OscInitStruct__PLL_PLLP,
                             RCC_OscInitStruct__PLL_PLLQ,
                             RCC_OscInitStruct__PLL_PLLR);

         /* Configure PLL  PLL1FRACN */
         __HAL_RCC_PLLFRACN_CONFIG(RCC_OscInitStruct__PLL_PLLFRACN);

        /* Select PLL1 input reference frequency range: VCI */ 
        __HAL_RCC_PLL_VCIRANGE(RCC_OscInitStruct__PLL_PLLRGE) ;

        /* Select PLL1 output frequency range : VCO */
        __HAL_RCC_PLL_VCORANGE(RCC_OscInitStruct__PLL_PLLVCOSEL) ;

        /* Enable PLL System Clock output. */
         __HAL_RCC_PLLCLKOUT_ENABLE(RCC_PLL1_DIVP);

        /* Enable PLL1Q Clock output. */
         __HAL_RCC_PLLCLKOUT_ENABLE(RCC_PLL1_DIVQ);
 
        /* Enable PLL1R  Clock output. */
         __HAL_RCC_PLLCLKOUT_ENABLE(RCC_PLL1_DIVR);

        /* Enable PLL1FRACN . */
         __HAL_RCC_PLLFRACN_ENABLE();

        /* Enable the main PLL_ */
        __HAL_RCC_PLL_ENABLE();

        /* Get Start Tick*/
        tickstart = HAL_GetTick();

        /* Wait till PLL is ready */
        while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY) == RESET)
        {
          if((int32_t) (HAL_GetTick() - tickstart ) > PLL_TIMEOUT_VALUE)
          {
            while(1);
          }
        }
      }
      else
      {
        /* Disable the main PLL. */
        __HAL_RCC_PLL_DISABLE();

        /* Get Start Tick*/
        tickstart = HAL_GetTick();

        /* Wait till PLL is ready */
        while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY) != RESET)
        {
          if((int32_t) (HAL_GetTick() - tickstart ) > PLL_TIMEOUT_VALUE)
          {
            while(1);
          }
        }
      }
    }
    else
    {
      while(1);
    }
  }
    /**Initializes the CPU, AHB and APB busses clocks 
    */
#define  RCC_ClkInitStruct__ClockType   (RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK\
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2\
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1)
#define  RCC_ClkInitStruct__SYSCLKSource  RCC_SYSCLKSOURCE_PLLCLK
#define  RCC_ClkInitStruct__SYSCLKDivider  RCC_SYSCLK_DIV1
#define  RCC_ClkInitStruct__AHBCLKDivider  RCC_HCLK_DIV2
#define  RCC_ClkInitStruct__APB3CLKDivider  RCC_APB3_DIV2
#define  RCC_ClkInitStruct__APB1CLKDivider  RCC_APB1_DIV2
#define  RCC_ClkInitStruct__APB2CLKDivider  RCC_APB2_DIV2
#define  RCC_ClkInitStruct__APB4CLKDivider  RCC_APB4_DIV2
#define  FLatency                           FLASH_LATENCY_2
  //if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  //{
  //  _Error_Handler(__FILE__, __LINE__);
  //}
  if(FLatency > (FLASH->ACR & FLASH_ACR_LATENCY))
  {
    /* Program the new number of wait states to the LATENCY bits in the FLASH_ACR register */
    __HAL_FLASH_SET_LATENCY(FLatency);

    /* Check that the new number of wait states is taken into account to access the Flash
    memory by reading the FLASH_ACR register */
    if((FLASH->ACR & FLASH_ACR_LATENCY) != FLatency)
    {
      while(1);
    }

  }
   /*-------------------------- HCLK Configuration --------------------------*/
    if(((RCC_ClkInitStruct__ClockType) & RCC_CLOCKTYPE_HCLK) == RCC_CLOCKTYPE_HCLK)
    {
      //assert_param(IS_RCC_HCLK(RCC_ClkInitStruct__AHBCLKDivider));
      MODIFY_REG(RCC->D1CFGR, RCC_D1CFGR_HPRE, RCC_ClkInitStruct__AHBCLKDivider);
    }

    /*------------------------- SYSCLK Configuration -------------------------*/
    if(((RCC_ClkInitStruct__ClockType) & RCC_CLOCKTYPE_SYSCLK) == RCC_CLOCKTYPE_SYSCLK)
    {
      //assert_param(IS_RCC_SYSCLK(RCC_ClkInitStruct__SYSCLKDivider));
      //assert_param(IS_RCC_SYSCLKSOURCE(RCC_ClkInitStruct__SYSCLKSource));
      MODIFY_REG(RCC->D1CFGR, RCC_D1CFGR_D1CPRE, RCC_ClkInitStruct__SYSCLKDivider);
      /* HSE is selected as System Clock Source */
      if(RCC_ClkInitStruct__SYSCLKSource == RCC_SYSCLKSOURCE_HSE)
      {
        /* Check the HSE ready flag */
        if(__HAL_RCC_GET_FLAG(RCC_FLAG_HSERDY) == RESET)
        {
          while(1);
        }
      }
      /* PLL is selected as System Clock Source */
      else if(RCC_ClkInitStruct__SYSCLKSource == RCC_SYSCLKSOURCE_PLLCLK)
      {
        /* Check the PLL ready flag */
        if(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY) == RESET)
        {
          while(1);
        }
      }
      /* CSI is selected as System Clock Source */
      else if(RCC_ClkInitStruct__SYSCLKSource == RCC_SYSCLKSOURCE_CSI)
      {
        /* Check the PLL ready flag */
        if(__HAL_RCC_GET_FLAG(RCC_FLAG_CSIRDY) == RESET)
        {
          while(1);
        }
      }
      /* HSI is selected as System Clock Source */
      else
      {
        /* Check the HSI ready flag */
        if(__HAL_RCC_GET_FLAG(RCC_FLAG_HSIRDY) == RESET)
        {
          while(1);
        }
      }
      MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_ClkInitStruct__SYSCLKSource);

      /* Get Start Tick*/
      tickstart = HAL_GetTick();

      if(RCC_ClkInitStruct__SYSCLKSource == RCC_SYSCLKSOURCE_HSE)
      {
        while (__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_CFGR_SWS_HSE)
        {
          if((int32_t) (HAL_GetTick() - tickstart ) > CLOCKSWITCH_TIMEOUT_VALUE)
          {
            while(1);
          }
        }
      }
      else if(RCC_ClkInitStruct__SYSCLKSource == RCC_SYSCLKSOURCE_PLLCLK)
      {
        while (__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_CFGR_SWS_PLL1)
        {
          if((int32_t) (HAL_GetTick() - tickstart ) > CLOCKSWITCH_TIMEOUT_VALUE)
          {
            while(1);
          }
        }
      }
      else if(RCC_ClkInitStruct__SYSCLKSource == RCC_SYSCLKSOURCE_CSI)
      {
        while (__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_CFGR_SWS_CSI)
        {
          if((int32_t) (HAL_GetTick() - tickstart ) > CLOCKSWITCH_TIMEOUT_VALUE)
          {
            while(1);
          }
        }
      }
      else
      {
        while(__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_CFGR_SWS_HSI)
        {
          if((int32_t) (HAL_GetTick() - tickstart ) > CLOCKSWITCH_TIMEOUT_VALUE)
          {
            while(1);
          }
        }
      }
    }
  
  /* Decreasing the number of wait states because of lower CPU frequency */
  if(FLatency < (FLASH->ACR & FLASH_ACR_LATENCY))
  {
    /* Program the new number of wait states to the LATENCY bits in the FLASH_ACR register */
    __HAL_FLASH_SET_LATENCY(FLatency);

    /* Check that the new number of wait states is taken into account to access the Flash
    memory by reading the FLASH_ACR register */
    if((FLASH->ACR & FLASH_ACR_LATENCY) != FLatency)
    {
      while(1);
    }
 }

  /*-------------------------- D1PCLK1 Configuration ---------------------------*/
  if(((RCC_ClkInitStruct__ClockType) & RCC_CLOCKTYPE_D1PCLK1) == RCC_CLOCKTYPE_D1PCLK1)
  {
    //assert_param(IS_RCC_D1PCLK1(RCC_ClkInitStruct__APB3CLKDivider));
    MODIFY_REG(RCC->D1CFGR, RCC_D1CFGR_D1PPRE, RCC_ClkInitStruct__APB3CLKDivider);
  }

  /*-------------------------- PCLK1 Configuration ---------------------------*/
  if(((RCC_ClkInitStruct__ClockType) & RCC_CLOCKTYPE_PCLK1) == RCC_CLOCKTYPE_PCLK1)
  {
    //assert_param(IS_RCC_PCLK1(RCC_ClkInitStruct__APB1CLKDivider));
    MODIFY_REG(RCC->D2CFGR, RCC_D2CFGR_D2PPRE1, (RCC_ClkInitStruct__APB1CLKDivider));
  }

  /*-------------------------- PCLK2 Configuration ---------------------------*/
  if(((RCC_ClkInitStruct__ClockType) & RCC_CLOCKTYPE_PCLK2) == RCC_CLOCKTYPE_PCLK2)
  {
    //assert_param(IS_RCC_PCLK2(RCC_ClkInitStruct__APB2CLKDivider));
    MODIFY_REG(RCC->D2CFGR, RCC_D2CFGR_D2PPRE2, (RCC_ClkInitStruct__APB2CLKDivider));
  }


  /*-------------------------- D3PCLK1 Configuration ---------------------------*/
  if(((RCC_ClkInitStruct__ClockType) & RCC_CLOCKTYPE_D3PCLK1) == RCC_CLOCKTYPE_D3PCLK1)
  {
    //assert_param(IS_RCC_D3PCLK1(RCC_ClkInitStruct__APB4CLKDivider));
    MODIFY_REG(RCC->D3CFGR, RCC_D2CFGR_D2PPRE1, (RCC_ClkInitStruct__APB4CLKDivider) );
  }
  
  /* Update the SystemCoreClock global variable */
  SystemCoreClock = HAL_RCC_GetSysClockFreq() >> D1CorePrescTable[(RCC->D1CFGR & RCC_D1CFGR_D1CPRE)>> POSITION_VAL(RCC_D1CFGR_D1CPRE_0)];
  
  

  __HAL_RCC_USART234578_CONFIG(RCC_USART234578CLKSOURCE_D2PCLK1);
  __HAL_RCC_PLLCLKOUT_ENABLE(RCC_PLL1_DIVQ);
  __HAL_RCC_USB_CONFIG(RCC_USBCLKSOURCE_HSI48);
  SET_BIT(PWR->CR3, PWR_CR3_USB33DEN);
  
  //PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3|RCC_PERIPHCLK_USB;
  //PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
  //PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
  //if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  //{
  //  _Error_Handler(__FILE__, __LINE__);
  //}
}

uint32_t HAL_GetREVID(void)
{
   return((DBGMCU->IDCODE) >> 16);
}

/** @addtogroup STM32H7xx_System_Private_Functions
  * @{
  */

/**
  * @brief  Setup the microcontroller system
  *         Initialize the FPU setting, vector table location and External memory
  *         configuration.
  * @param  None
  * @retval None
  */
void SystemInit (void)
{
  /* FPU settings ------------------------------------------------------------*/
  #if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
  #endif
  /* Reset the RCC clock configuration to the default reset state ------------*/
  /* Set HSION bit */
  RCC->CR |= RCC_CR_HSION;

  /* Reset CFGR register */
  RCC->CFGR = 0x00000000;

  /* Reset HSEON, CSSON , CSION,RC48ON, CSIKERON PLL1ON, PLL2ON and PLL3ON bits */
  RCC->CR &= (uint32_t)0xEAF6ED7F;

  /* Reset D1CFGR register */
  RCC->D1CFGR = 0x00000000;

  /* Reset D2CFGR register */
  RCC->D2CFGR = 0x00000000;

  /* Reset D3CFGR register */
  RCC->D3CFGR = 0x00000000;

  /* Reset PLLCKSELR register */
  RCC->PLLCKSELR = 0x00000000;

  /* Reset PLLCFGR register */
  RCC->PLLCFGR = 0x00000000;
  /* Reset PLL1DIVR register */
  RCC->PLL1DIVR = 0x00000000;
  /* Reset PLL1FRACR register */
  RCC->PLL1FRACR = 0x00000000;

  /* Reset PLL2DIVR register */
  RCC->PLL2DIVR = 0x00000000;

  /* Reset PLL2FRACR register */

  RCC->PLL2FRACR = 0x00000000;
  /* Reset PLL3DIVR register */
  RCC->PLL3DIVR = 0x00000000;

  /* Reset PLL3FRACR register */
  RCC->PLL3FRACR = 0x00000000;

  /* Reset HSEBYP bit */
  RCC->CR &= (uint32_t)0xFFFBFFFF;

  /* Disable all interrupts */
  RCC->CIER = 0x00000000;

  /* Change  the switch matrix read issuing capability to 1 for the AXI SRAM target (Target 7) */
  *((__IO uint32_t*)0x51008108) = 0x00000001;

#if defined (DATA_IN_ExtSRAM) || defined (DATA_IN_ExtSDRAM)
  SystemInit_ExtMemCtl();
#endif /* DATA_IN_ExtSRAM || DATA_IN_ExtSDRAM */

  /* Configure the Vector Table location add offset address ------------------*/
#ifdef VECT_TAB_SRAM
  SCB->VTOR = D1_AXISRAM_BASE  | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal ITCMSRAM */
#else
  SCB->VTOR = FLASH_BANK1_BASE | VECT_TAB_OFFSET;       /* Vector Table Relocation in Internal FLASH */
#endif

    SystemClock_Config();
    SystemCoreClockUpdate();
}

/**
   * @brief  Update SystemCoreClock variable according to Clock Register Values.
  *         The SystemCoreClock variable contains the core clock , it can
  *         be used by the user application to setup the SysTick timer or configure
  *         other parameters.
  *
  * @note   Each time the core clock changes, this function must be called
  *         to update SystemCoreClock variable value. Otherwise, any configuration
  *         based on this variable will be incorrect.
  *
  * @note   - The system frequency computed by this function is not the real
  *           frequency in the chip. It is calculated based on the predefined
  *           constant and the selected clock source:
  *
  *           - If SYSCLK source is CSI, SystemCoreClock will contain the CSI_VALUE(*)
  *           - If SYSCLK source is HSI, SystemCoreClock will contain the HSI_VALUE(**)
  *           - If SYSCLK source is HSE, SystemCoreClock will contain the HSE_VALUE(***)
  *           - If SYSCLK source is PLL, SystemCoreClock will contain the CSI_VALUE(*),
  *             HSI_VALUE(**) or HSE_VALUE(***) multiplied/divided by the PLL factors.
  *
  *         (*) CSI_VALUE is a constant defined in stm32h7xx_hal.h file (default value
  *             4 MHz) but the real value may vary depending on the variations
  *             in voltage and temperature.
  *         (**) HSI_VALUE is a constant defined in stm32h7xx_hal.h file (default value
  *             64 MHz) but the real value may vary depending on the variations
  *             in voltage and temperature.
  *
  *         (***)HSE_VALUE is a constant defined in stm32h7xx_hal.h file (default value
  *              25 MHz), user has to ensure that HSE_VALUE is same as the real
  *              frequency of the crystal used. Otherwise, this function may
  *              have wrong result.
  *
  *         - The result of this function could be not correct when using fractional
  *           value for HSE crystal.
  * @param  None
  * @retval None
  */
void SystemCoreClockUpdate (void)
{
    SystemCoreClock = HAL_RCC_GetSysClockFreq() >> D1CorePrescTable[(RCC->D1CFGR & RCC_D1CFGR_D1CPRE)>> POSITION_VAL(RCC_D1CFGR_D1CPRE_0)];
}
#if defined (DATA_IN_ExtSRAM) || defined (DATA_IN_ExtSDRAM)
/**
  * @brief  Setup the external memory controller.
  *         Called in startup_stm32h7xx.s before jump to main.
  *         This function configures the external memories (SRAM/SDRAM)
  *         This SRAM/SDRAM will be used as program data memory (including heap and stack).
  * @param  None
  * @retval None
  */
void SystemInit_ExtMemCtl(void)
{
#if defined (DATA_IN_ExtSDRAM)
  register uint32_t tmpreg = 0, timeout = 0xFFFF;
  register __IO uint32_t index;

  /* Enable GPIOD, GPIOE, GPIOF, GPIOG, GPIOH and GPIOI interface
      clock */
  RCC->AHB4ENR |= 0x000001F8;
  /* Connect PDx pins to FMC Alternate function */
  GPIOD->AFR[0]  = 0x000000CC;
  GPIOD->AFR[1]  = 0xCC000CCC;
  /* Configure PDx pins in Alternate function mode */
  GPIOD->MODER   = 0xAFEAFFFA;
  /* Configure PDx pins speed to 50 MHz */
  GPIOD->OSPEEDR = 0xA02A000A;
  /* Configure PDx pins Output type to push-pull */
  GPIOD->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PDx pins */
   GPIOD->PUPDR   = 0x55555505;
  /* Connect PEx pins to FMC Alternate function */
  GPIOE->AFR[0]  = 0xC00000CC;
  GPIOE->AFR[1]  = 0xCCCCCCCC;
  /* Configure PEx pins in Alternate function mode */
  GPIOE->MODER   = 0xAAAABFFA;
  /* Configure PEx pins speed to 50 MHz */
  GPIOE->OSPEEDR = 0xAAAA800A;
  /* Configure PEx pins Output type to push-pull */
  GPIOE->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PEx pins */
  GPIOE->PUPDR   = 0x55554005;
  /* Connect PFx pins to FMC Alternate function */
  GPIOF->AFR[0]  = 0x00CCCCCC;
  GPIOF->AFR[1]  = 0xCCCCC000;
  /* Configure PFx pins in Alternate function mode */
  GPIOF->MODER   = 0xAABFFAAA;
  /* Configure PFx pins speed to 50 MHz */
  GPIOF->OSPEEDR = 0xAA800AAA;
  /* Configure PFx pins Output type to push-pull */
  GPIOF->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PFx pins */
  GPIOF->PUPDR   = 0x55400555;
  /* Connect PGx pins to FMC Alternate function */
  GPIOG->AFR[0]  = 0x00CCCCCC;
  GPIOG->AFR[1]  = 0xC000000C;
  /* Configure PGx pins in Alternate function mode */
  GPIOG->MODER   = 0xBFFEFAAA;
 /* Configure PGx pins speed to 50 MHz */
  GPIOG->OSPEEDR = 0x80020AAA;
  /* Configure PGx pins Output type to push-pull */
  GPIOG->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PGx pins */
  GPIOG->PUPDR   = 0x40010515;
  /* Connect PHx pins to FMC Alternate function */
  GPIOH->AFR[0]  = 0xCCC00000;
  GPIOH->AFR[1]  = 0xCCCCCCCC;
  /* Configure PHx pins in Alternate function mode */
  GPIOH->MODER   = 0xAAAAABFF;
  /* Configure PHx pins speed to 50 MHz */
  GPIOH->OSPEEDR = 0xAAAAA800;
  /* Configure PHx pins Output type to push-pull */
  GPIOH->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PHx pins */
  GPIOH->PUPDR   = 0x55555400;
  /* Connect PIx pins to FMC Alternate function */
  GPIOI->AFR[0]  = 0xCCCCCCCC;
  GPIOI->AFR[1]  = 0x00000CC0;
  /* Configure PIx pins in Alternate function mode */
  GPIOI->MODER   = 0xFFEBAAAA;
  /* Configure PIx pins speed to 50 MHz */
  GPIOI->OSPEEDR = 0x0028AAAA;
  /* Configure PIx pins Output type to push-pull */
  GPIOI->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PIx pins */
  GPIOI->PUPDR   = 0x00145555;
/*-- FMC Configuration ------------------------------------------------------*/
  /* Enable the FMC interface clock */
  (RCC->AHB3ENR |= (RCC_AHB3ENR_FMCEN));
  /*SDRAM Timing and access interface configuration*/
  /*LoadToActiveDelay  = 2
    ExitSelfRefreshDelay = 6
    SelfRefreshTime      = 4
    RowCycleDelay        = 6
    WriteRecoveryTime    = 2
    RPDelay              = 2
    RCDDelay             = 2
    SDBank             = FMC_SDRAM_BANK2
    ColumnBitsNumber   = FMC_SDRAM_COLUMN_BITS_NUM_9
    RowBitsNumber      = FMC_SDRAM_ROW_BITS_NUM_12
    MemoryDataWidth    = FMC_SDRAM_MEM_BUS_WIDTH_32
    InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4
    CASLatency         = FMC_SDRAM_CAS_LATENCY_2
    WriteProtection    = FMC_SDRAM_WRITE_PROTECTION_DISABLE
    SDClockPeriod      = FMC_SDRAM_CLOCK_PERIOD_2
    ReadBurst          = FMC_SDRAM_RBURST_ENABLE
    ReadPipeDelay      = FMC_SDRAM_RPIPE_DELAY_0*/

  FMC_Bank5_6->SDCR[0] = 0x00001800;
  FMC_Bank5_6->SDCR[1] = 0x00000165;
  FMC_Bank5_6->SDTR[0] = 0x00105000;
  FMC_Bank5_6->SDTR[1] = 0x01010351;

  /* SDRAM initialization sequence */
  /* Clock enable command */
  FMC_Bank5_6->SDCMR = 0x00000009;
  tmpreg = FMC_Bank5_6->SDSR & 0x00000020;
  while((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6->SDSR & 0x00000020;
  }

  /* Delay */
  for (index = 0; index<1000; index++);

  /* PALL command */
    FMC_Bank5_6->SDCMR = 0x0000000A;
  timeout = 0xFFFF;
  while((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6->SDSR & 0x00000020;
  }

  FMC_Bank5_6->SDCMR = 0x000000EB;
  timeout = 0xFFFF;
  while((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6->SDSR & 0x00000020;
  }

  FMC_Bank5_6->SDCMR = 0x0004400C;
  timeout = 0xFFFF;
  while((tmpreg != 0) && (timeout-- > 0))
  {
    tmpreg = FMC_Bank5_6->SDSR & 0x00000020;
  }
  /* Set refresh count */
  tmpreg = FMC_Bank5_6->SDRTR;
  FMC_Bank5_6->SDRTR = (tmpreg | (0x00000603<<1));

  /* Disable write protection */
  tmpreg = FMC_Bank5_6->SDCR[1];
  FMC_Bank5_6->SDCR[1] = (tmpreg & 0xFFFFFDFF);

   /*FMC controller Enable*/
  FMC_Bank1->BTCR[0]  |= 0x80000000;


#endif /* DATA_IN_ExtSDRAM */

#if defined(DATA_IN_ExtSRAM)
/*-- GPIOs Configuration -----------------------------------------------------*/
   /* Enable GPIOD, GPIOE, GPIOF and GPIOG interface clock */
  RCC->AHB4ENR   |= 0x00000078;

  /* Connect PDx pins to FMC Alternate function */
  GPIOD->AFR[0]  = 0x00CCC0CC;
  GPIOD->AFR[1]  = 0xCCCCCCCC;
  /* Configure PDx pins in Alternate function mode */
  GPIOD->MODER   = 0xAAAA0A8A;
  /* Configure PDx pins speed to 100 MHz */
  GPIOD->OSPEEDR = 0xFFFF0FCF;
  /* Configure PDx pins Output type to push-pull */
  GPIOD->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PDx pins */
  GPIOD->PUPDR   = 0x55550545;

  /* Connect PEx pins to FMC Alternate function */
  GPIOE->AFR[0]  = 0xC00CC0CC;
  GPIOE->AFR[1]  = 0xCCCCCCCC;
  /* Configure PEx pins in Alternate function mode */
  GPIOE->MODER   = 0xAAAA828A;
  /* Configure PEx pins speed to 100 MHz */
  GPIOE->OSPEEDR = 0xFFFFC3CF;
  /* Configure PEx pins Output type to push-pull */
  GPIOE->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PEx pins */
  GPIOE->PUPDR   = 0x55554145;

  /* Connect PFx pins to FMC Alternate function */
  GPIOF->AFR[0]  = 0x00CCCCCC;
  GPIOF->AFR[1]  = 0xCCCC0000;
  /* Configure PFx pins in Alternate function mode */
  GPIOF->MODER   = 0xAA000AAA;
  /* Configure PFx pins speed to 100 MHz */
  GPIOF->OSPEEDR = 0xFF000FFF;
  /* Configure PFx pins Output type to push-pull */
  GPIOF->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PFx pins */
  GPIOF->PUPDR   = 0x55000555;

  /* Connect PGx pins to FMC Alternate function */
  GPIOG->AFR[0]  = 0x00CCCCCC;
  GPIOG->AFR[1]  = 0x000000C0;
  /* Configure PGx pins in Alternate function mode */
  GPIOG->MODER   = 0x00200AAA;
  /* Configure PGx pins speed to 100 MHz */
  GPIOG->OSPEEDR = 0x00300FFF;
  /* Configure PGx pins Output type to push-pull */
  GPIOG->OTYPER  = 0x00000000;
  /* No pull-up, pull-down for PGx pins */
  GPIOG->PUPDR   = 0x00100555;

/*-- FMC/FSMC Configuration --------------------------------------------------*/
  /* Enable the FMC/FSMC interface clock */
 (RCC->AHB3ENR |= (RCC_AHB3ENR_FMCEN));

  /* Configure and enable Bank1_SRAM2 */
  FMC_Bank1->BTCR[4]  = 0x00001091;
  FMC_Bank1->BTCR[5]  = 0x00110212;
  FMC_Bank1E->BWTR[4] = 0x0FFFFFFF;

  /*FMC controller Enable*/
  FMC_Bank1->BTCR[0]  |= 0x80000000;


#endif /* DATA_IN_ExtSRAM */
}
#endif /* DATA_IN_ExtSRAM || DATA_IN_ExtSDRAM */


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
