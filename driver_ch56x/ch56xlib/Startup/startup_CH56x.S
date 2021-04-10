/********************************** (C) COPYRIGHT *******************************
* File Name          : startup_CH56x.s
* Author             : WCH
* Version            : V1.0
* Date               : 2020/07/31
* Description        : CH56x vector table for eclipse toolchain.
*******************************************************************************/

	.section	.init,"ax",@progbits
	.global	_start
	.align	1
_start:
	j	handle_reset

    .section    .vector,"ax",@progbits
    .align  1
_vector_base:
    .option norvc;
    .word   0
    .word   0
        j   NMI_Handler                 /* NMI Handler */
        j   HardFault_Handler           /* Hard Fault Handler */
    .word   0
    .word   0
    .word   0
    .word   0
    .word   0
    .word   0
    .word   0
    .word   0
        j   SysTick_Handler            /* SysTick Handler */
    .word   0
        j   SWI_Handler                 /* SWI Handler */
    .word   0
    /* External Interrupts */
        j   WDOG_IRQHandler			   /* WDOG */
        j   TMR0_IRQHandler            /* TMR0 */
        j   GPIO_IRQHandler            /* GPIO */
        j   SPI0_IRQHandler            /* SPI0 */
        j   USBSS_IRQHandler           /* USBSS */
        j   LINK_IRQHandler            /* LINK */
        j   TMR1_IRQHandler            /* TMR1 */
        j   TMR2_IRQHandler            /* TMR2 */
        j   UART0_IRQHandler           /* UART0 */
        j   USBHS_IRQHandler           /* USBHS */
        j   EMMC_IRQHandler            /* EMMC */
        j   DVP_IRQHandler             /* DVP */
        j   HSPI_IRQHandler            /* HSPI */
        j   SPI1_IRQHandler            /* SPI1 */
        j   UART1_IRQHandler           /* UART1 */
        j   UART2_IRQHandler           /* UART2 */
        j   UART3_IRQHandler           /* UART3 */
        j   SERDES_IRQHandler          /* SERDES */
        j   ETH_IRQHandler             /* ETH */
        j   PMT_IRQHandler             /* PMT */
        j   ECDC_IRQHandler            /* ECDC */

    .option rvc;

    .section    .text.vector_handler, "ax", @progbits
    .weak   NMI_Handler
    .weak   HardFault_Handler
    .weak   SysTick_Handler
    .weak   SWI_Handler
    .weak   WDOG_IRQHandler
    .weak   TMR0_IRQHandler
    .weak   GPIO_IRQHandler
    .weak   SPI0_IRQHandler
    .weak   USBSS_IRQHandler
    .weak   LINK_IRQHandler
    .weak   TMR1_IRQHandler
    .weak   TMR2_IRQHandler
    .weak   UART0_IRQHandler
    .weak   USBHS_IRQHandler
    .weak   EMMC_IRQHandler
    .weak   DVP_IRQHandler
    .weak   HSPI_IRQHandler
    .weak   SPI1_IRQHandler
    .weak   UART1_IRQHandler
    .weak   UART2_IRQHandler
    .weak   UART3_IRQHandler
    .weak   SERDES_IRQHandler
    .weak   ETH_IRQHandler
    .weak   PMT_IRQHandler
    .weak   ECDC_IRQHandler

NMI_Handler:  1:  j 1b
HardFault_Handler:  1:  j 1b
SysTick_Handler:  1:  j 1b
SWI_Handler:  1:  j 1b
WDOG_IRQHandler:  1:  j 1b
TMR0_IRQHandler:  1:  j 1b
GPIO_IRQHandler:  1:  j 1b
SPI0_IRQHandler:  1:  j 1b
USBSS_IRQHandler:  1:  j 1b
LINK_IRQHandler:  1:  j 1b
TMR1_IRQHandler:  1:  j 1b
TMR2_IRQHandler:  1:  j 1b
UART0_IRQHandler:  1:  j 1b
USBHS_IRQHandler:  1:  j 1b
EMMC_IRQHandler:  1:  j 1b
DVP_IRQHandler:  1:  j 1b
HSPI_IRQHandler:  1:  j 1b
SPI1_IRQHandler:  1:  j 1b
UART1_IRQHandler:  1:  j 1b
UART2_IRQHandler:  1:  j 1b
UART3_IRQHandler:  1:  j 1b
SERDES_IRQHandler:  1:  j 1b
ETH_IRQHandler:  1:  j 1b
PMT_IRQHandler:  1:  j 1b
ECDC_IRQHandler:  1:  j 1b

	.section	.text.handle_reset,"ax",@progbits
	.weak	handle_reset
	.align	1
handle_reset:
.option push 
.option	norelax 
	la gp, __global_pointer$
.option	pop 
1:
	la sp, _eusrstack 

/* Load data section from flash to RAM */
2:	
	la a0, _data_lma
	la a1, _data_vma
	la a2, _edata
	bgeu a1, a2, 2f
1:
	lw t0, (a0)
	sw t0, (a1)
	addi a0, a0, 4
	addi a1, a1, 4
	bltu a1, a2, 1b
2:
	/* clear bss section */
	la a0, _sbss
	la a1, _ebss
	bgeu a0, a1, 2f
1:
	sw zero, (a0)
	addi a0, a0, 4
	bltu a0, a1, 1b

/* clear dmadata section */

2:
    la a0, _dmadata_start
    la a1, _dmadata_end
    bgeu a0, a1, 2f
1:
    sw zero, (a0)
    addi a0, a0, 4
    bltu a0, a1, 1b
/**/
2:
	/* enable all interrupt */
    li t0, 0x88
    csrs mstatus, t0
	la t0, _vector_base
    ori t0, t0, 1
	csrw mtvec, t0
	la t0, main
	csrw mepc, t0

	
	mret


