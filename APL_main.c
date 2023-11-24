/**
  Generated main.c file from MPLAB Code Configurator

  @Company
    Microchip Technology Inc.

  @File Name
    main.c

  @Summary
    This is the generated main.c using PIC24 / dsPIC33 / PIC32MM MCUs.

  @Description
    This source file provides main entry point for system initialization and application code development.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.171.4
        Device            :  dsPIC33CK256MP508
    The generated drivers are tested against the following:
        Compiler          :  XC16 v2.10
        MPLAB 	          :  MPLAB X v6.05
 */

/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
 */

/**
  Section: Included Files
 */
#include <xc.h>

#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/system/clock.h"
#include "mcc_generated_files/cmp/cmp1.h"
#include "mcc_generated_files/dma/dma.h"
#include "mcc_generated_files/pulse_output/sccp7.h"
#include "mcc_generated_files/timer/tmr1.h"
#include "mcc_generated_files/system/pins.h"
#include "APL/Debounce/APL_digInputDebounce.h"

#define SINS_ARRAY_SIZE    (128u)

#define BASECNT_VALUE  (CLOCK_PeripheralFrequencyGet()*78.125e-6)
#define BASECNT_VALUE_1us  (CLOCK_PeripheralFrequencyGet()*1e-6)

volatile uint16_t u16PeriodSCCP7;

/* 
 * LUT for base sine shall reside in RAM for correct DMA transfer as 
 * DMA is just dealing with RAM and not PSV based data
 */
volatile int16_t sins[SINS_ARRAY_SIZE] = {

    0x800, 0x864, 0x8c8, 0x92c, 0x98f, 0x9f1, 0xa52, 0xab1,
    0xb0f, 0xb6b, 0xbc5, 0xc1c, 0xc71, 0xcc3, 0xd12, 0xd5f,
    0xda7, 0xded, 0xe2e, 0xe6c, 0xea6, 0xedc, 0xf0d, 0xf3a,
    0xf63, 0xf87, 0xfa7, 0xfc2, 0xfd8, 0xfe9, 0xff5, 0xffd,
    0xfff, 0xffd, 0xff5, 0xfe9, 0xfd8, 0xfc2, 0xfa7, 0xf87,
    0xf63, 0xf3a, 0xf0d, 0xedc, 0xea6, 0xe6c, 0xe2e, 0xded,
    0xda7, 0xd5f, 0xd12, 0xcc3, 0xc71, 0xc1c, 0xbc5, 0xb6b,
    0xb0f, 0xab1, 0xa52, 0x9f1, 0x98f, 0x92c, 0x8c8, 0x864,
    0x800, 0x79b, 0x737, 0x6d3, 0x670, 0x60e, 0x5ad, 0x54e,
    0x4f0, 0x494, 0x43a, 0x3e3, 0x38e, 0x33c, 0x2ed, 0x2a0,
    0x258, 0x212, 0x1d1, 0x193, 0x159, 0x123, 0xf2, 0xc5,
    0x9c, 0x78, 0x58, 0x3d, 0x27, 0x16, 0xa, 0x2,
    0x0, 0x2, 0xa, 0x16, 0x27, 0x3d, 0x58, 0x78,
    0x9c, 0xc5, 0xf2, 0x123, 0x159, 0x193, 0x1d1, 0x212,
    0x258, 0x2a0, 0x2ed, 0x33c, 0x38e, 0x3e3, 0x43a, 0x494,
    0x4f0, 0x54e, 0x5ad, 0x60e, 0x670, 0x6d3, 0x737, 0x79b,

};

/*
 * check if there is a change in trigger time
 * update is done at end of timer period to prevent 
 * stuck while value is above the counter
 * only update if value is different 
 */
void APL_DmaTrigger(void)
{
    if ( CCP7RB != u16PeriodSCCP7)
    {
        CCP7RB = u16PeriodSCCP7;
        CCP7PRL = u16PeriodSCCP7;
    }
    else
    {
        /* do nothing */
    }
}

/* just toggle LED at end of any DMA transfer completion */
void DMA_Channel0_CallBack(void)
{
    LED1_Toggle();
}

/*
                         Main application
 */
int main(void)
{
    
    // initialize the device
    SYSTEM_Initialize();

    DMA_ChannelDisable(DMA_CHANNEL_0);

    DMA_SourceAddressSet(DMA_CHANNEL_0, &sins[0]);

    DMA_TransferCountSet(DMA_CHANNEL_0, (uint16_t) (SINS_ARRAY_SIZE));

    DMA_DestinationAddressSet(DMA_CHANNEL_0, (uint16_t) & DAC1DATH);

    
    /* end of DMA trigger timer , default is 78.125us */
    SCCP7_PulseOutput_CallbackRegister(&APL_DmaTrigger);
    
    /* just get the default 78.125us time stamp value */
    u16PeriodSCCP7 = (uint16_t)BASECNT_VALUE;
    
    /*update timer value*/
    APL_DmaTrigger();
    
    /* enable DMA transfer module */
    DMA_ChannelEnable(DMA_CHANNEL_0);

    
    /* start DMA trigger */
    SCCP7_PulseOutput_Enable();

    while (1)
    {


        // Add your application code
    }
    return 1;
}
/**
 End of File
 */

