/***********************************************************************************************************************
 * Copyright [2015-2017] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 * 
 * This file is part of Renesas SynergyTM Software Package (SSP)
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * This file is subject to a Renesas SSP license agreement. Unless otherwise agreed in an SSP license agreement with
 * Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
 * or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
 * MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
 * "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
 * CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
 * included in this file may be subject to different terms.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : bsp_clocks.c
* Description  : Calls the CGC module to setup the system clocks. Settings for clocks are based on macros in
*                bsp_clock_cfg.h.
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @ingroup BSP_MCU_S3A7
 * @defgroup BSP_MCU_CLOCKS Clock Initialization
 *
 * Functions in this file configure the system clocks based upon the macros in bsp_clock_cfg.h.
 *
 * @{
 **********************************************************************************************************************/



/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "bsp_api.h"

#if defined(BSP_MCU_GROUP_S3A7)

#include "r_cgc_api.h"
#include "r_cgc.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define CGC_SRAM_ZERO_WAIT_CYCLES (0U)              ///< Specify zero wait states for SRAM
#define CGC_SRAM_ONE_WAIT_CYCLES  (1U)              ///< Specify one wait states for SRAM
#define CGC_ZERO_WAIT_CYCLES      (0U)
#define CGC_TWO_WAIT_CYCLES       (1U)

#define CGC_MAX_ZERO_WAIT_FREQ    (32000000U)

/* Key code for writing PRCR register. */
#define CGC_PRCR_KEY                      (0xA500U)
#define CGC_PRCR_UNLOCK                   ((CGC_PRCR_KEY) | 0x1U)
#define CGC_PRCR_LOCK                     ((CGC_PRCR_KEY) | 0x0U)


/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables (to be accessed by other files)
***********************************************************************************************************************/
 
/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief      Sets up system clocks.
 **********************************************************************************************************************/
void bsp_clock_init (void)
{
    cgc_system_clock_cfg_t sys_cfg;
    cgc_clock_t clock;


    g_cgc_on_cgc.init();

    R_BSP_CacheSet(BSP_CACHE_STATE_ON);                            // Turn on cache.

    /** MOCO is default clock out of reset. Enable new clock if chosen. */

    if (BSP_CFG_CLOCK_SOURCE != CGC_CLOCK_PLL)
    {
        clock = BSP_CFG_CLOCK_SOURCE;
        g_cgc_on_cgc.clockStart(clock, NULL);
    }
    else
    {
        /** PLL Source clock is always the main oscillator. */
        clock = CGC_CLOCK_MAIN_OSC;

        /** Need to start PLL source clock and let it stabilize before starting PLL */
        g_cgc_on_cgc.clockStart(clock, NULL);

        cgc_clock_cfg_t pll_cfg;

        /** Set PLL Divider. */
        pll_cfg.divider = BSP_CFG_PLL_DIV;

        /** Set PLL Multiplier. */
        pll_cfg.multiplier = BSP_CFG_PLL_MUL;

        /** PLL Source clock is always the main oscillator. */
        pll_cfg.source_clock = clock;

        while (SSP_ERR_STABILIZED != g_cgc_on_cgc.clockCheck(clock))
        {
            /** Wait for PLL clock source to stabilize */
        }


        // Start the PLL running. clockStart() will check the requested system clock frequency and
        // switch to High Speed mode if the requested frequency > 32 MHz.
        g_cgc_on_cgc.clockStart(CGC_CLOCK_PLL, &pll_cfg);
        clock = CGC_CLOCK_PLL;
    }


    /** MOCO, LOCO, and subclock do not have stabilization flags that can be checked. */
    if ((CGC_CLOCK_MOCO != clock) && (CGC_CLOCK_LOCO != clock) && (CGC_CLOCK_SUBCLOCK != clock))
    {
        while (SSP_ERR_STABILIZED != g_cgc_on_cgc.clockCheck(clock))
        {
            /** Wait for clock source to stabilize */
        }
    }

    sys_cfg.iclk_div  = BSP_CFG_ICK_DIV;
    sys_cfg.pclka_div = BSP_CFG_PCKA_DIV;
    sys_cfg.pclkb_div = BSP_CFG_PCKB_DIV;
    sys_cfg.pclkc_div = BSP_CFG_PCKC_DIV;
    sys_cfg.pclkd_div = BSP_CFG_PCKD_DIV;
    sys_cfg.fclk_div  = BSP_CFG_FCK_DIV;
    sys_cfg.bclk_div  = BSP_CFG_BCK_DIV;

    /** Set which clock to use for system clock and divisors for all system clocks. */
    g_cgc_on_cgc.systemClockSet(clock, &sys_cfg);

}

/*******************************************************************************************************************//**
 * @brief      Returns frequency of CPU clock in Hz.
 *
 * @retval     Frequency of the CPU in Hertz
 **********************************************************************************************************************/
uint32_t bsp_cpu_clock_get (void)
{
    uint32_t freq = (uint32_t)0;

    g_cgc_on_cgc.systemClockFreqGet(CGC_SYSTEM_CLOCKS_ICLK, &freq);

    return freq;
}

/*******************************************************************************************************************//**
 * @brief      This function sets the value of the MEMWAIT register which controls wait cycles for flash read access.
 * @param[in]  setting
 * @retval     none
 **********************************************************************************************************************/
__STATIC_INLINE void bsp_clocks_mem_wait_set (uint32_t setting)
{
    R_SYSTEM->PRCR = CGC_PRCR_UNLOCK;
    R_SYSTEM->MEMWAITCR_b.MEMWAIT = (uint8_t)(setting & 0x01);
    R_SYSTEM->PRCR = CGC_PRCR_LOCK;
}


/*******************************************************************************************************************//**
 * @brief      This function gets the value of the MEMWAIT register
 * @retval     MEMWAIT setting
 **********************************************************************************************************************/
__STATIC_INLINE uint32_t bsp_clocks_mem_wait_get (void)
{
    return (R_SYSTEM->MEMWAITCR_b.MEMWAIT);
}

ssp_err_t bsp_clock_set_callback(bsp_clock_set_callback_args_t * p_args)
{
    uint32_t current_frequency   = p_args->current_freq_hz;
    if (BSP_CLOCK_SET_EVENT_PRE_CHANGE == p_args->event)
    {
        uint32_t requested_frequency = p_args->requested_freq_hz;

        /* If the requested frequency is greater than 32 MHz, the current frequency must be less than 32 MHz and the
         * mcu must be in high speed mode, before changing wait cycles to 2.
         * S3A7 does not require SRAM wait state control. In fact the register is unimplemented for the S3A7.         */
        if (requested_frequency > CGC_MAX_ZERO_WAIT_FREQ)
        {
           if ((((current_frequency <= CGC_MAX_ZERO_WAIT_FREQ)) &&
                    (0 == R_SYSTEM->OPCCR_b.OPCM)))
            {
                 bsp_clocks_mem_wait_set(CGC_TWO_WAIT_CYCLES);             // change ROM wait cycles if frequency not above 32 MHz and in
                                                                           // high speed mode
            }
            else if (bsp_clocks_mem_wait_get() != CGC_TWO_WAIT_CYCLES)
            {
                return SSP_ERR_INVALID_MODE;      // else, error if not in 2 wait cycles already
            }
            else
            {
                /* following coding rules */
            }
        }
    }
    if (BSP_CLOCK_SET_EVENT_POST_CHANGE == p_args->event)
    {
        /** The current frequency must be less than 32 MHz and the mcu must be in high speed mode, before changing
         * wait cycles to 0.
         */
        if (((current_frequency <= CGC_MAX_ZERO_WAIT_FREQ)) && (0 == R_SYSTEM->OPCCR_b.OPCM))
        {
            bsp_clocks_mem_wait_set(CGC_ZERO_WAIT_CYCLES);        ///< No MEMWAIT cycles
        }
    }
    return SSP_SUCCESS;
}

#endif

/** @} (end defgroup BSP_MCU_CLOCKS) */
