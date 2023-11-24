/**
 * APL_digInputDebounce
 * 
 * @file      APL_digInputDebounce.h
 *            
 * @defgroup  APL header
 *            
 * @brief     application specific functions
 *            
 * @version   Driver Version 0.9.0
 *            
 */

/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef APL_DIGINPUTDEBOUNCE_H
#define	APL_DIGINPUTDEBOUNCE_H

#include <xc.h> // include processor files - each processor file is guarded.  

//!< used to define consecutive indexes for the digital inputs
typedef enum _eButtonAvail_tag
{
    BTN1 = 0u,
  //  BTN2 = 1u,
    MAXBTN,
    FAULT = 255u
} _eButtonAvail;


//!< defined to classify the type of edge need to be identified
typedef enum
{
    BOTHEDG = 0u,
    RISING = 1u,
    FALLING = 2u,
    DETECTED = 5u,
    EDG_ILLIGAL = 255u
} _eButtonEdgeDetect;

//!< whow many events shall be consecutive to deternine stable state 
#define NBR_CONSECUTIVE_EVTS (3u)   

//!< structure used for digital debouncing and edge transition detection
typedef struct  _sVarDigInput_tag
{
    uint16_t u16State;      //!< each bit represents the state of an individual input after filtering
    uint16_t u16XorState;   //!< each bit represents the state of an individual input after edge detected
    uint16_t u16Transition2Low; //!< each bit represents a detected falling edge on individual input
    uint16_t u16Transition2High;//!< each bit represents a detected rising edge on individual input
    uint16_t au16Filter[MAXBTN];    //!< filter stream of individual inputs 
    uint16_t au16DetectType[MAXBTN];
} _sVarDigInput;

/**
 * @ingroup     APL header
 * @brief       initializes the button activity related variables 
 * @param[in]   void
 * @return      void
 * @sequence    on initialization
 * @note
 */
extern void APL_vidInitDigInputDebounce(uint16_t u16ChnIdx, uint16_t u16Mode);

/**
 * @ingroup     APL header     
 * @brief       input value from specific button is sorted into its dedicated
 *              decimation variable. If three consecutive bits are on same value
 *              the button state is changed  
 * @param[in]   uint16_t u16ChnIdx  which digital input is looked on 
 * @param[in]   uint16_t u16BtnVal  state of read digital input
 * @return      uint16_t 0 = button success, 0xFFFFu = not found 
 * @sequence    shall be called periodic  
 * @note
 */
extern uint16_t APL_u16DecimateDigInputDebounce(uint16_t u16ChnIdx, uint16_t u16BtnVal);

/**
 * @ingroup     APL header
 * @brief       check the filtered state of selected buffer depending on selected mode
 * @param[in]   uint16_t u16ChnIdx   index to check
 * @return      uint16_t 0           no change detected based on input
 * @sequence    on demand
 * @note
 */
extern uint16_t APL_u16GetDigInputDebouncedState(uint16_t u16ChnIdx);

#endif	/* APL_DIGINPUTDEBOUNCE_H */

