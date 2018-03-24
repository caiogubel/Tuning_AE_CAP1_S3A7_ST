/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIESREGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2017 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : tuning_thread_entry.c
* Version      : 1.1.0
* Description  : This file contains the tuning control code. Compatible with Synergy SSP V1.1.0.
***********************************************************************************************************************/

/* CTSU Tuning Thread entry function */
/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "tuning_thread.h"
#include "r_serial_control.h"
#include "captouch_configs/ctsu_configs/utilities/r_touch.h"
#include "r_ctsu_tuning_cfg.h"

/**********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/** Macro for error logger. **/
#ifndef SF_TOUCH_CTSU_ERROR_RETURN
#define SF_TOUCH_CTSU_ERROR_RETURN(a, err) SSP_ERROR_RETURN((a), (err), g_module_name, &g_version)
#endif

extern ctsu_process_option_t comm_command;      ///< Allows Workbench6 to kick-off scans    (r_serial_control.c)
extern ctsu_status_t g_ctsu_status;             ///< Reports status to Workbench6           (r_serial_control.c)
#if (CTSU_TUNING_MODE == CTSU_MODE_SELF)
    extern ctsu_hw_cfg_t g_ctsu_config_self;    ///< CTSU Configuration (Channels used + other information) to be tuned   (TO BE PROVIDED)
#elif (CTSU_TUNING_MODE == CTSU_MODE_MUTUAL)
    extern ctsu_hw_cfg_t g_ctsu_config_mutual;  ///< CTSU Configuration (Channels used + other information) to be tuned   (TO BE PROVIDED)
#endif
extern const transfer_instance_t g_transfer0_ctsuwr_block_xfer;
extern const transfer_instance_t g_transfer0_ctsurd_block_xfer;
extern ssp_err_t R_CTSU_Process(ctsu_ctrl_t * p_ctrl, ctsu_process_option_t opts);

static void sf_ctsu_tune_common_handler(ctsu_callback_args_t *p_args);
void tuning_thread_entry(void);

/* Data functions to use for advanced usage.*/
ctsu_functions_t g_ctsu_functions_tune =
{ 0 };

//ctsu_ctrl_t g_ctsu_ctrl_tune;
ctsu_instance_ctrl_t g_ctsu_ctrl_tune;
ctsu_cfg_t g_ctsu_cfg_tune =
{
#if (USING_DTC_FOR_CTSU > 0)
 .p_lower_lvl_transfer_read =  &g_transfer0_ctsurd_block_xfer,
 .p_lower_lvl_transfer_write = &g_transfer0_ctsuwr_block_xfer,
#endif
#if (CTSU_TUNING_MODE == CTSU_MODE_SELF)
  .p_ctsu_hw_cfg = &g_ctsu_config_self,
#elif (CTSU_TUNING_MODE == CTSU_MODE_MUTUAL)
  .p_ctsu_hw_cfg = &g_ctsu_config_mutual,
#endif
  .p_ctsu_functions = &g_ctsu_functions_tune,
  .p_callback = sf_ctsu_tune_common_handler,
  .ctsu_soft_option = CTSU_PROCESS_OPTION_NONE,
  .ctsu_close_option = CTSU_CLOSE_OPTION_RESET_SFRS,
};

const ctsu_instance_t g_ctsu_tune =
{
     .p_ctrl = &g_ctsu_ctrl_tune,
     .p_cfg = &g_ctsu_cfg_tune,
     .p_api = &g_ctsu_on_ctsu,
};


static TX_SEMAPHORE semaphore_ctsu_scan_complete;

void tuning_thread_entry(void)
{
    /** Hold the status */
    UINT threadx_status;
    ssp_err_t ctsu_status = SSP_SUCCESS;
    extern volatile uint8_t g_ctsu_soft_mode;

    /* Create and initialize a semaphore. */
    threadx_status = tx_semaphore_create( &semaphore_ctsu_scan_complete, (CHAR*)"CTSU Scan Complete", 0);
    if(threadx_status != TX_SUCCESS)
    {/* TODO: Log error. Failed to create semaphore. */
        return;
    }

    SerialCommandInitial();

    while (true)
    {
        if(CTSU_STOP_MODE == g_ctsu_soft_mode)
        {   /* No need to perform any other steps */
            threadx_status = tx_thread_sleep(1);
            continue;
        }
        else if(CTSU_READY_MODE == g_ctsu_soft_mode)
        {   /* Configuration has been opened. */
            g_ctsu_soft_mode = CTSU_RUN_MODE;
            /* Start a scan. */
            ctsu_status = g_ctsu_tune.p_api->scan(g_ctsu_tune.p_ctrl);
        }
        else if(CTSU_RUN_MODE == g_ctsu_soft_mode)
        {   /* This condition should never be reached. Below statement is for fail-safe. */
            ctsu_status = g_ctsu_tune.p_api->scan(g_ctsu_tune.p_ctrl);
        }
        else if(CTSU_FINISH_MODE == g_ctsu_soft_mode)
        {   /* CTSU Finished scanning. Ready to restart. */
            g_ctsu_soft_mode = CTSU_READY_MODE;
            continue;
        }


        if(SSP_ERR_IN_USE==ctsu_status)
        {   /* Hardware is busy. Sleep.  */
            threadx_status = tx_thread_sleep(1);
        }
        else if(SSP_ERR_NOT_OPEN==ctsu_status)
        {   /* Configuration is not open */
            g_ctsu_soft_mode = CTSU_STOP_MODE;
        }
        else if(SSP_SUCCESS==ctsu_status)
        {   /* Scan started. */
            g_ctsu_status.flag.ctsu_measure = 1;
            g_ctsu_status.flag.data_update = 0;
            /*Pend until scan completes. */
            threadx_status = tx_semaphore_get(&semaphore_ctsu_scan_complete, TX_WAIT_FOREVER);

            g_ctsu_status.flag.icomp_error = HW_CTSU_CTSUERRSGetBitCTSUICOMP() & 0x01;
            g_ctsu_status.flag.sens_over = HW_CTSU_CTSUSTGetBitCTSUSOVF() & 0x01;
            g_ctsu_status.flag.ref_over = HW_CTSU_CTSUSTGetBitCTSUROVF() & 0x01;

            if(g_ctsu_status.flag.icomp_error == 1)
            {   /* Clear ICOMP errors */
                uint32_t ctsucr1_val = HW_CTSU_CTSUCR1Get();
                /* Clear bit 0 and 1 and set it as new SFR value  */
                HW_CTSU_CTSUCR1Set(ctsucr1_val & 0xFC);
                /* Delay */
                for(uint32_t i = 0; i < 65535; i++);
                /* Put the value back in to SFR. */
                HW_CTSU_CTSUCR1Set((uint8_t)(ctsucr1_val & 0xFF));
            }

            if(g_ctsu_status.flag.sens_over == 1)
            {   /* Clear Sensor overflow errors */
                HW_CTSU_CTSUSTSetBitCTSUSOVF(0);
            }

            if(g_ctsu_status.flag.ref_over == 1)
            {   /* Clear Reference overflow errors */
                HW_CTSU_CTSUSTSetBitCTSUROVF(0);
            }

            /* Inform communication I/F about completion. */
            g_ctsu_status.flag.data_update = 1;
            g_ctsu_status.flag.ctsu_measure = 0;
            g_ctsu_soft_mode = CTSU_FINISH_MODE;
        }

        tx_thread_sleep(1);
    }
}

/** Notification function: This is invoked as a callback when a scan completes.
 * This function is being invoked by a ISR. */
static void sf_ctsu_tune_common_handler(ctsu_callback_args_t *p_args)
{
    UINT threadx_status;
    switch(p_args->event)
    {
    case CTSU_EVENT_SCAN_COMPLETE:
        /** Increment the counting semaphore "semaphore_ctsu_scan_complete." */
        threadx_status = tx_semaphore_put(&semaphore_ctsu_scan_complete);
        if(threadx_status != TX_SUCCESS)
        {/* TODO: Log error. */
            return;
        }
        break;

    case CTSU_EVENT_PARAMETERS_UPDATED:
        /* All data has been updated */
        g_ctsu_status.flag.data_update = 1;
        break;

    default:

        break;
    }
    return;
}
