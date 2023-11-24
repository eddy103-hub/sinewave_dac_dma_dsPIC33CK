/**
  @Company
    Microchip Technology Germany GmbH

  @File Name
    APL_digInputDEbounce.c

  @Summary
    This file is used to provide support for debouncing of individual digital 
    inputs and provide information about rising or falling edge detection

  @Description
    usable on various products
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
#include "APL_digInputDebounce.h"

//!< use method to debounce RUN button and nIRQ from ATA device
volatile _sVarDigInput sVarButtons;

#define DETECTION_MSK ((1u << NBR_CONSECUTIVE_EVTS)- 1u)

/*
 * description initializes the variables used for button decimation filter
 * param       void
 * return      void
 * sequence    on startup
 * note
 */
void APL_vidInitDigInputDebounce(uint16_t u16ChnIdx, uint16_t u16Mode)
{
    if (u16ChnIdx < (uint16_t) MAXBTN)
    {
        if ((uint16_t) RISING == u16Mode)
        {
            sVarButtons.au16Filter[u16ChnIdx] = 0; /* button is high active */
            sVarButtons.au16DetectType[u16ChnIdx] = (uint16_t) RISING;

            sVarButtons.u16State &= (uint16_t) (~(1u << u16ChnIdx));
            sVarButtons.u16XorState &= (uint16_t) (~(1u << u16ChnIdx));

        }
        else if ((uint16_t) FALLING == u16Mode)
        {
            sVarButtons.au16Filter[u16ChnIdx] = 0xFFFFu; /* button is low active */
            sVarButtons.au16DetectType[u16ChnIdx] = (uint16_t) FALLING;

            sVarButtons.u16State |= (uint16_t) ((1u << u16ChnIdx));
            sVarButtons.u16XorState |= (uint16_t) ((1u << u16ChnIdx));
        }
    }
}

/*
 * description  input value from specific button is sorted into its dedicated
 *              decimation variable. If three consecutive bits are on same value
 *              the button state is changed  
 * param[in]    uint16_t u16ChnIdx  which digital input is looked on 
 * param[in]    uint16_t u16BtnVal  state of read digital input
 * return       uint16_t 0 = button success, 0xFFFFu = not found 
 * sequence     shall be called periodic  
 * note
 */
uint16_t APL_u16DecimateDigInputDebounce(uint16_t u16ChnIdx, uint16_t u16BtnVal)
{
    uint16_t u16retval;
    uint16_t u16result;

    /* check if index is in range */
    if (u16ChnIdx < (uint16_t) MAXBTN)
    {
        uint16_t u16Mask = (uint16_t) (1u << u16ChnIdx);

        sVarButtons.au16Filter[u16ChnIdx] |= (u16BtnVal & 1u); /* add button u16BtnVal to filter */

        /* check decimation filter for expected consecutive bits */
        u16result = sVarButtons.au16Filter[u16ChnIdx] & (uint16_t) DETECTION_MSK;

        if (0x0000u == u16result)
        {
         //   sVarButtons.u16State |= (u16Mask); /* set button as active */
            sVarButtons.u16State &= ~(u16Mask); /* reset button to inactive */
        }
        else if ((uint16_t) DETECTION_MSK == u16result)
        {
            sVarButtons.u16State |= (u16Mask); /* set button as active */
            //sVarButtons.u16State &= ~(u16Mask); /* reset button to inactive */
        }

        /* check for falling or rising edge */
        uint16_t u16Detection = (sVarButtons.u16State ^ sVarButtons.u16XorState) & u16Mask;

        if (0u != u16Detection)
        {
            /* 
             * edge had been detected
             * now differentiate between falling or rising 
             */
            if (1u == (u16BtnVal & 1u))
            {
                sVarButtons.u16Transition2High |= u16Detection;
            }
            else
            {
                sVarButtons.u16Transition2Low |= u16Detection;
            }

            /* in case of edge detection then store the event for next edge detection cycle */
            sVarButtons.u16XorState = (sVarButtons.u16XorState & ~u16Mask) | (sVarButtons.u16State & u16Mask);
        }
        else
        {
            /* do nothing */
        }

        /* prepare buffer for next cycle */
        sVarButtons.au16Filter[u16ChnIdx] = (uint16_t)(sVarButtons.au16Filter[u16ChnIdx] << 1u);

        u16retval = 0u;
    }
    else
    {
        u16retval = 0xFFFFu; /*button index out of scope */
    }
    return u16retval;
}

/*
 * description check selected button if expected mode had been detected
 *             function only check for single transition into expected state
 *             and not steady state signal
 * param       uint16_t u16ChnIdx  -> defined in _eButtonAvail
 * return      uint16_t            -> 0 - no event
 * sequence    on demand
 * note        
 */
uint16_t APL_u16GetDigInputDebouncedState(uint16_t u16ChnIdx)
{
    uint16_t u16retval;

    u16retval = (uint16_t) FAULT;

    /* check for index-in-range */
    if (u16ChnIdx < (uint16_t) MAXBTN)
    {
        /* check the configured edge detection type */
        uint16_t u16DetectType = sVarButtons.au16DetectType[u16ChnIdx];
        if (u16DetectType < (uint16_t) DETECTED)
        {
            /* generate dependent mask */
            uint16_t u16mask = (1u << u16ChnIdx);

            /* check what edge detection type selected for quenstioned input */
            if (u16DetectType == (uint16_t) FALLING)
            {
                /* check if transition had been detected */
                if (u16mask == (sVarButtons.u16Transition2Low & u16mask))
                {
                    /* reset detection marker */
                    sVarButtons.u16Transition2Low = (sVarButtons.u16Transition2Low & ~u16mask);
                    /* respond with edge detection index */
                    u16retval = (uint16_t) DETECTED;
                }
                else
                {
                    u16retval = (uint16_t) 0;
                }
            }
            else if (u16DetectType == (uint16_t) RISING)
            {
                /* check if transition had been detected */
                if (u16mask == (sVarButtons.u16Transition2High & u16mask))
                {
                    /* reset detection marker */
                    sVarButtons.u16Transition2High = (sVarButtons.u16Transition2High & ~u16mask);
                    /* respond with edge detection index */
                    u16retval = (uint16_t) DETECTED;
                }
                else
                {
                    u16retval = (uint16_t) 0;
                }
            }
            else
            {
                /* check for both edge events */
                if (u16mask == ((sVarButtons.u16Transition2High | sVarButtons.u16Transition2High) & u16mask))
                {
                    /* reset detection marker */
                    sVarButtons.u16Transition2High = (sVarButtons.u16Transition2High & ~u16mask);
                    sVarButtons.u16Transition2Low = (sVarButtons.u16Transition2Low & ~u16mask);
                    /* respond with edge detection index */
                    u16retval = (uint16_t) DETECTED;
                }
                else
                {
                    u16retval = (uint16_t) 0;
                }
            }
        }
        else
        {
            /* do nothing */
        }
    }
    else
    {
        /* do nothing */
    }

    return u16retval;
}


/*
 * end of file
 */
