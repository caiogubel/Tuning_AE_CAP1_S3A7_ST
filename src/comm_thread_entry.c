/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2017 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

#include "comm_thread.h"
#include "r_serial_control.h"
#define             UX_DEMO_BUFFER_TX_SIZE (273)
#define             UX_DEMO_BUFFER_RX_SIZE (36)
unsigned char rx_buffer[UX_DEMO_BUFFER_RX_SIZE];
unsigned char tx_buffer[UX_DEMO_BUFFER_TX_SIZE];
uint16_t rcv_length = 4;
uint16_t trn_length = 0;
uint16_t offset = 0;
#define APP_ERR_TRAP(a)     if(a) {__asm("BKPT #0\n");} /* trap the error location */

void comm_thread_entry(void);

/* Workbench6 Communication Thread entry function */
void comm_thread_entry(void)
{
    ssp_err_t ssp_err_g_sf_comm0;
    ULONG error_wr = 0;
    ULONG error_rd = 0;
    SerialCommandInitial();
    while (true)
    { /* Capture incoming data into buffer. */
        ssp_err_g_sf_comm0 = g_sf_comms0.p_api->read(g_sf_comms0.p_ctrl, &rx_buffer[offset], rcv_length, TX_WAIT_FOREVER);
        if (SSP_SUCCESS!=ssp_err_g_sf_comm0)
        { /* Error accumulator */
            error_rd++;
        }
        offset = (uint16_t)(offset + rcv_length);
        if(offset >= (4+rx_buffer[2]))
        { /* Check if command reception is complete. */
            if (SerialCommandReceive(rx_buffer, offset))
            { /* Checksum is OK, command was received successfully */
                if (GetReplayMessage(tx_buffer, (uint16_t *)&trn_length))
                {/* Check the status. If OK, we will write to the CDC instance. */
                    ssp_err_g_sf_comm0 = g_sf_comms0.p_api->write(g_sf_comms0.p_ctrl, tx_buffer, trn_length, TX_NO_WAIT);
                    if (SSP_SUCCESS!=ssp_err_g_sf_comm0)
                    { /* Error accumulator */
                        error_wr++;
                    }
                }
                /* Response to command was sent. Reset buffer, and associated variables. */
                memset(rx_buffer, 0, sizeof(rx_buffer));
                rcv_length = 4;
                offset = 0;
            }
        }
        else
        { /* Command reception is not complete. Receive remaining bytes. */
            rcv_length =  rx_buffer[2];
        }
        if(rcv_length > UX_DEMO_BUFFER_RX_SIZE)
        { /* Should not happen */
            APP_ERR_TRAP(true);
        }
        tx_thread_sleep(1);
    }
}
