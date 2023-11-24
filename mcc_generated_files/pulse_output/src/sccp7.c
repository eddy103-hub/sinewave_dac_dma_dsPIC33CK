/**
 * SCCP7 Generated Driver Source File
 * 
 * @file 	  sccp7.c
 * 
 * @ingroup   pulseoutputdriver
 * 
 * @brief 	  This is the generated driver source file for SCCP7 driver
 *
 * @skipline @version   Firmware Driver Version 2.1.1
 *
 * @skipline @version   PLIB Version 1.6.2
 *
 * @skipline  Device : dsPIC33CK256MP508
*/

/*
© [2023] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

// Section: Included Files

#include <xc.h>
#include <stddef.h>
#include "../sccp7.h"

// Section: File specific functions

static void (*SCCP7_PulseOutputHandler)(void) = NULL;

// Section: Driver Interface

const struct PULSEOUTPUT_INTERFACE DacDmaTrigger = {
    .Initialize          = &SCCP7_PulseOutput_Initialize,
    .Deinitialize        = &SCCP7_PulseOutput_Deinitialize,
    .Enable              = &SCCP7_PulseOutput_Enable,
    .Disable             = &SCCP7_PulseOutput_Disable,
    .CountSet            = &SCCP7_PulseOutput_CountSet,
    .SoftwareTriggerSet  = &SCCP7_PulseOutput_SoftwareTriggerSet,
    .CallbackRegister = &SCCP7_PulseOutput_CallbackRegister,
    .Tasks               = NULL
};

// Section: SCCP7 Module APIs

void SCCP7_PulseOutput_Initialize (void)
{
    // MOD Dual Edge Compare; CCSEL disabled; TMR32 16 Bit; TMRPS 1:1; CLKSEL FOSC/2; TMRSYNC disabled; CCPSLP disabled; CCPSIDL disabled; CCPON disabled; 
    CCP7CON1L = 0x4;
    // SYNC SCCP7; ALTSYNC disabled; ONESHOT disabled; TRIGEN disabled; IOPS Each Time Base Period Match; RTRGEN disabled; OPSRC Timer Interrupt Event; 
    CCP7CON1H = 0x7;
    // ASDG 0x0; SSDG disabled; ASDGM disabled; PWMRSEN disabled; 
    CCP7CON2L = 0x0;
    // ICSEL ; AUXOUT Disabled; ICGSM Level-Sensitive mode; OCAEN disabled; OENSYNC disabled; 
    CCP7CON2H = 0x0;
    // PSSACE Tri-state; POLACE disabled; OSCNT None; OETRIG disabled; 
    CCP7CON3H = 0x0;
    // ICOV disabled; ICDIS disabled; SCEVT disabled; ASEVT disabled; TRCLR disabled; TRSET disabled; ICGARM disabled; 
    CCP7STATL = 0x0;
    // TMRL 0x0000; 
    CCP7TMRL = 0x0;
    // TMRH 0x0000; 
    CCP7TMRH = 0x0;
    // PRL 5000; 
    CCP7PRL = 0x1388;
    // PRH 0; 
    CCP7PRH = 0x0;
    // CMPA 0; 
    CCP7RA = 0x0;
    // CMPB 5000; 
    CCP7RB = 0x1388;
    // BUFL 0x0000; 
    CCP7BUFL = 0x0;
    // BUFH 0x0000; 
    CCP7BUFH = 0x0;
    
    SCCP7_PulseOutput_CallbackRegister(&SCCP7_PulseOutput_Callback);
    
    IFS9bits.CCP7IF = 0;
    IEC9bits.CCP7IE = 1;
    
}

void SCCP7_PulseOutput_Deinitialize (void)
{
    CCP7CON1Lbits.CCPON = 0;
    
    
    IFS9bits.CCP7IF = 0;
    IEC9bits.CCP7IE = 0;
    
    CCP7CON1L = 0x0;
    CCP7CON1H = 0x0;
    CCP7CON2L = 0x0;
    CCP7CON2H = 0x100;
    CCP7CON3H = 0x0;
    CCP7STATL = 0x0;
    CCP7TMRL = 0x0;
    CCP7TMRH = 0x0;
    CCP7PRL = 0xFFFF;
    CCP7PRH = 0xFFFF;
    CCP7RA = 0x0;
    CCP7RB = 0x0;
    CCP7BUFL = 0x0;
    CCP7BUFH = 0x0;
}

void SCCP7_PulseOutput_Enable( void )
{
    CCP7CON1Lbits.CCPON = 1;
}


void SCCP7_PulseOutput_Disable( void )
{
    CCP7CON1Lbits.CCPON = 0;
}

void SCCP7_PulseOutput_CountSet(uint16_t onTime, uint16_t pulseCount)
{
    CCP7RB = onTime+pulseCount;
    CCP7RA = onTime;
    CCP7PRL = onTime+pulseCount;
}

void SCCP7_PulseOutput_SoftwareTriggerSet( void )
{
    CCP7STATLbits.TRSET = 1;
}

void SCCP7_PulseOutput_CallbackRegister(void (*handler)(void))
{
    if(NULL != handler)
    {
        SCCP7_PulseOutputHandler = handler;
    }
}

void __attribute__ ((weak)) SCCP7_PulseOutput_Callback ( void )
{ 

} 


void __attribute__ ( ( interrupt, no_auto_psv ) ) _CCP7Interrupt ( void )
{    
    // SCCP7 callback function 
    if(NULL != SCCP7_PulseOutputHandler)
    {
        (*SCCP7_PulseOutputHandler)();
    }
    IFS9bits.CCP7IF = 0;
}
/**
 End of File
*/
