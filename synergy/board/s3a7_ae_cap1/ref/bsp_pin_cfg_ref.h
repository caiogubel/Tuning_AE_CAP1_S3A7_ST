/***********************************************************************************************************************
 * Copyright [2015] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
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
* File Name    : bsp_pin_cfg_ref.h
* Description  : This configuration header file has information for setting up pins out of reset.
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @ingroup BSP_CONFIG_S3A7
 * @defgroup BSP_CONFIG_S3A7_PINS Build Time Configurations - Pin Settings
 *
 * This file contains an array of pin configurations. During start-up, and before main() is executed, the BSP will
 * iterate over this array and initialize the MCU's port pins based on the settings in the array.
 *
 * @note It is recommended to use the ISDE Pin Configurator to create this file.
 *
 * @{
***********************************************************************************************************************/

#ifndef BSP_PIN_CFG_REF_H_
#define BSP_PIN_CFG_REF_H_

/***********************************************************************************************************************
Pin Configuration Table
***********************************************************************************************************************/

/* This conditional makes sure that this table is only allocated once. Without this check the table would be allocated
 * in every module that includes this header file. */
#if defined(BSP_PIN_ALLOCATE)

/* Used for getting IOPORT types. */
#include "r_ioport.h"

/** Array of pin configurations. */
const ioport_pin_cfg_t g_bsp_pin_cfg_data[] =
{

    {
        .pin     = IOPORT_PORT_00_PIN_10,			// TS ID (CAP Touch) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_CTSU),
    },
    {
        .pin     = IOPORT_PORT_00_PIN_11,           // AN011 output
        .pin_cfg = (IOPORT_CFG_PORT_DIRECTION_OUTPUT),
    },
    {
        .pin     = IOPORT_PORT_00_PIN_12,           // AN012 output
        .pin_cfg = (IOPORT_CFG_PORT_DIRECTION_OUTPUT),
    },
    {
        .pin     = IOPORT_PORT_00_PIN_13,           // AN013 output (POT1)
        .pin_cfg = (IOPORT_CFG_PORT_DIRECTION_OUTPUT),
    },

    {
        .pin     = IOPORT_PORT_00_PIN_14,           // DA0 output
        .pin_cfg = (IOPORT_CFG_PORT_DIRECTION_OUTPUT),
    },
    {
        .pin     = IOPORT_PORT_00_PIN_15,           // DA1 output
        .pin_cfg = (IOPORT_CFG_PORT_DIRECTION_OUTPUT),
    },

    {
        .pin     = IOPORT_PORT_01_PIN_00,			// Segment LCD VL1 output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_01_PIN_01,			// Segment LCD VL2 output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_01_PIN_02,			// Segment LCD VL3 output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_01_PIN_03,			// Segment LCD VL4 output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },

    {
        .pin     = IOPORT_PORT_01_PIN_04,           // Segment LCD COM0 output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_01_PIN_05,           // Segment LCD COM1 output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_01_PIN_06,           // Segment LCD COM2 output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_01_PIN_07,           // Segment LCD COM3 output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_01_PIN_12,           // CAPL (CAP Touch) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_CTSU),
    },

    {
        .pin     = IOPORT_PORT_02_PIN_02,           // IRQ3-DS
        .pin_cfg = (IOPORT_CFG_IRQ_ENABLE),
    },
    {
        .pin     = IOPORT_PORT_02_PIN_03,           // TSCAP_B (CAP Touch)  output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_CTSU),
    },
    {
        .pin     = IOPORT_PORT_02_PIN_04,           // TS0 (CAP Touch) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_CTSU),
    },
    {
        .pin     = IOPORT_PORT_02_PIN_06,           // TS1 (CAP Touch) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_CTSU),
    },
    {
        .pin     = IOPORT_PORT_02_PIN_07,           // TS2 (CAP Touch) output or USB_VBUS
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_USB_FS | IOPORT_PERIPHERAL_CTSU),
    },
    {
        .pin     = IOPORT_PORT_02_PIN_08,           // TS3 (CAP Touch) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_CTSU),
    },
    {
        .pin     = IOPORT_PORT_02_PIN_09,           // TS4 (CAP Touch) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_CTSU),
    },
    {
        .pin     = IOPORT_PORT_02_PIN_10,           // TS5 (CAP Touch) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_CTSU),
    },
    {
        .pin     = IOPORT_PORT_02_PIN_11,           // TS6 (CAP Touch) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_CTSU),
    },
    {
        .pin     = IOPORT_PORT_02_PIN_13,           // IRQ2-B (Button1)
        .pin_cfg = (IOPORT_CFG_IRQ_ENABLE),
    },
    {
        .pin     = IOPORT_PORT_03_PIN_01,           // RXD2_A
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN |IOPORT_PERIPHERAL_SCI0_2_4_6_8),
    },
    {
        .pin     = IOPORT_PORT_03_PIN_02,           // TXD2_A
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN |IOPORT_PERIPHERAL_SCI0_2_4_6_8),
    },
    {
        .pin     = IOPORT_PORT_03_PIN_03,           // SEG3/COM7 (Segment LCD) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_03_PIN_04,           // IRQ9_B (Button 3)
        .pin_cfg = (IOPORT_CFG_IRQ_ENABLE),
    },
    {
        .pin     = IOPORT_PORT_03_PIN_05,           // IRQ8_B (Button 4)
        .pin_cfg = (IOPORT_CFG_IRQ_ENABLE),
    },
    {
        .pin     = IOPORT_PORT_03_PIN_06,           // SEG15 (Segment LCD) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_03_PIN_07,           // SEG14 (Segment LCD) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_03_PIN_08,           // SEG13 (Segment LCD) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_03_PIN_09,           // SEG12 (Segment LCD) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_03_PIN_10,           // SEG11 (Segment LCD) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_03_PIN_11,           // SEG10 (Segment LCD) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_03_PIN_12,           // SEG9 (Segment LCD) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_03_PIN_13,           // SEG20 (Segment LCD) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_03_PIN_14,           // SEG4 (Segment LCD) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_03_PIN_15,           // SEG5 (Segment LCD) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_04_PIN_00,           // TS20 (CAP Touch) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_CTSU),
    },
    {
        .pin     = IOPORT_PORT_04_PIN_01,           // CTX0_B (CAN0_B)
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_CAN),
    },
    {
        .pin     = IOPORT_PORT_04_PIN_02,           // CRX0_B (CAN0_B)
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_CAN),
    },
    {
        .pin     = IOPORT_PORT_04_PIN_03,           // TS17 (CAP Touch) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_CTSU),
    },
    {
        .pin     = IOPORT_PORT_04_PIN_04,           // TS16 (CAP Touch) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_CTSU),
    },
    {
        .pin     = IOPORT_PORT_04_PIN_05,           // TS15 (CAP Touch) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_CTSU),
    },
    {
        .pin     = IOPORT_PORT_04_PIN_06,           // TS14 (CAP Touch) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_CTSU),
    },
    {
        .pin     = IOPORT_PORT_04_PIN_07,           // USB_VBUS
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_USB_FS),
    },
    {
        .pin     = IOPORT_PORT_04_PIN_08,           // RXD3 (PMODA)
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_SCI1_3_5_7_9),
    },
    {
        .pin     = IOPORT_PORT_04_PIN_09,           // TXD3 (PMODA)
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_SCI1_3_5_7_9),
    },
    {
        .pin     = IOPORT_PORT_04_PIN_10,           // SCK3 (PMODA)
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_SCI1_3_5_7_9),
    },
    {
        .pin     = IOPORT_PORT_04_PIN_11,           // CTS3 (PMODA)
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_SCI1_3_5_7_9),
    },
    {
        .pin     = IOPORT_PORT_04_PIN_12,           // TS8 (CAP Touch) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_CTSU),
    },
    {
        .pin     = IOPORT_PORT_04_PIN_13,           // TS9 (CAP Touch) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_CTSU),
    },
    {
        .pin     = IOPORT_PORT_04_PIN_14,           // TS10 (CAP Touch) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_CTSU),
    },
    {
        .pin     = IOPORT_PORT_04_PIN_15,           // TS11 (CAP Touch) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_CTSU),
    },
    {
        .pin     = IOPORT_PORT_05_PIN_00,           // QSPCLK
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_QSPI | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_05_PIN_01,           // QSSL
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_QSPI | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_05_PIN_02,           // QIO0
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_QSPI | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_05_PIN_03,           // QIO1
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_QSPI | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_05_PIN_04,           // QIO2
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_QSPI | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_05_PIN_05,           // QIO3
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_QSPI | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_05_PIN_11,           // SDA2 (CapTouch LED)
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_RIIC),
    },
    {
        .pin     = IOPORT_PORT_05_PIN_12,           // SCL2 (CapTouch LED)
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_RIIC),
    },
    {
        .pin     = IOPORT_PORT_06_PIN_00,           // SEG41 (Segment LCD) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_06_PIN_01,           // SEG40 (Segment LCD) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_06_PIN_02,           // SEG39 (Segment LCD) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_06_PIN_03,           // SEG38 (Segment LCD) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_06_PIN_04,           // SEG37 (Segment LCD) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_06_PIN_05,           // SEG36 (Segment LCD) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_06_PIN_06,           // SEG35 (Segment LCD) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_06_PIN_08,           // SEG23 (Segment LCD) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_06_PIN_09,           // SEG29 (Segment LCD) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_06_PIN_10,           // SEG30 (Segment LCD) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_06_PIN_11,           // SEG31 (Segment LCD) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_06_PIN_12,           // SEG32 (Segment LCD) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_06_PIN_13,           // SEG33 (Segment LCD) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_06_PIN_14,           // SEG34 (Segment LCD) output
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
        .pin     = IOPORT_PORT_07_PIN_01,           // LED1 G
        .pin_cfg = (IOPORT_CFG_PORT_DIRECTION_OUTPUT),
    },
    {
        .pin     = IOPORT_PORT_07_PIN_02,           // LED1 R
        .pin_cfg = (IOPORT_CFG_PORT_DIRECTION_OUTPUT),
    },
    {
        .pin     = IOPORT_PORT_07_PIN_03,           // LED2 G
        .pin_cfg = (IOPORT_CFG_PORT_DIRECTION_OUTPUT),
    },
    {
        .pin     = IOPORT_PORT_07_PIN_04,           // LED2 R
        .pin_cfg = (IOPORT_CFG_PORT_DIRECTION_OUTPUT),
    },	
    {
        .pin     = IOPORT_PORT_07_PIN_08,           // RXD1_B(BLE EM9301 (SPI)
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_RSPI),
    },
    {
        .pin     = IOPORT_PORT_07_PIN_09,           // TXD1_B(BLE EM9301 (SPI)
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_RSPI),
    },
    {
        .pin     = IOPORT_PORT_07_PIN_10,           // SCK1_B(BLE EM9301 (SPI)
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_RSPI),
    },
    {
        .pin     = IOPORT_PORT_07_PIN_11,           // CTS1_B(BLE EM9301 (SPI)
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_RSPI),
    },
    {
         .pin     = IOPORT_PORT_08_PIN_00,           // SEG44 (Segment LCD) output
         .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
         .pin     = IOPORT_PORT_08_PIN_01,           // SEG45 (Segment LCD) output
         .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
         .pin     = IOPORT_PORT_08_PIN_04,           // SEG43 (Segment LCD) output
         .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
         .pin     = IOPORT_PORT_08_PIN_05,           // SEG42 (Segment LCD) output
         .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
         .pin     = IOPORT_PORT_08_PIN_06,           // SEG26 (Segment LCD) output
         .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
         .pin     = IOPORT_PORT_08_PIN_07,           // SEG27 (Segment LCD) output
         .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },

    {
         .pin     = IOPORT_PORT_08_PIN_08,           // SEG18 (Segment LCD) output
         .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
         .pin     = IOPORT_PORT_08_PIN_09,           // SEG19 (Segment LCD) output
         .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
         .pin     = IOPORT_PORT_09_PIN_00,           // SEG6 (Segment LCD) output
         .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
         .pin     = IOPORT_PORT_09_PIN_01,           // SEG7 (Segment LCD) output
         .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },
    {
         .pin     = IOPORT_PORT_09_PIN_02,           // SEG8 (Segment LCD) output
         .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_LCDC | IOPORT_CFG_DRIVE_HIGH),
    },

};

/** Has pointer to pin configuration array and number of pins to be configured. */
const ioport_cfg_t g_bsp_pin_cfg =
{
    .number_of_pins = sizeof(g_bsp_pin_cfg_data)/sizeof(ioport_pin_cfg_t),
    .p_pin_cfg_data = &g_bsp_pin_cfg_data[0],
};

#endif /* BSP_PIN_ALLOCATE */

/** @} (end defgroup BSP_CONFIG_S3A7_PINS) */

#endif /* BSP_PIN_CFG_REF_H_ */
