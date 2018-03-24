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
* File Name    : bsp_rom_registers.c
* Description  : Defines MCU registers that are in ROM (e.g. OFS) and must be set at compile-time. All registers
*                can be set using bsp_cfg.h.
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @ingroup BSP_MCU_S3A7
 * @defgroup BSP_MCU_ROM_REGISTER ROM Registers
 *
 * Defines MCU registers that are in ROM (e.g. OFS) and must be set at compile-time. All registers can be set
 * using bsp_cfg.h.
 *
 * @{
 **********************************************************************************************************************/



/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "bsp_api.h"

#if defined(BSP_MCU_GROUP_S3A7)

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/** OR in the HOCO frequency setting from bsp_clock_cfg.h with the OFS1 setting from bsp_cfg.h. */
#define BSP_ROM_REG_OFS1_SETTING        (((uint32_t)BSP_CFG_ROM_REG_OFS1 & 0xFFFF8FFFU) | ((uint32_t)BSP_CFG_HOCO_FREQUENCY << 12))

/** Build up SECMPUAC register based on MPU settings. */
#define BSP_ROM_REG_MPU_CONTROL_SETTING     ((0xFFFFFCFEU) | \
                                             ((uint32_t)BSP_CFG_ROM_REG_MPU_PC0_ENABLE << 8) | \
                                             ((uint32_t)BSP_CFG_ROM_REG_MPU_PC1_ENABLE << 9) | \
                                             ((uint32_t)BSP_CFG_ROM_REG_MPU_REGION0_ENABLE))

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables (to be accessed by other files)
***********************************************************************************************************************/
 
/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
/** ROM registers defined here. Some have masks to make sure reserved bits are set appropriately. */
/*LDRA_INSPECTED 57 D - This global is being initialized at it's declaration below. */
BSP_DONT_REMOVE static const uint32_t g_bsp_rom_registers[] BSP_PLACE_IN_SECTION(BSP_SECTION_ROM_REGISTERS) =
{
    (uint32_t)BSP_CFG_ROM_REG_OFS0,
    (uint32_t)BSP_ROM_REG_OFS1_SETTING,
    ((uint32_t)BSP_CFG_ROM_REG_MPU_PC0_START      & 0x000FFFFCU),
    ((uint32_t)BSP_CFG_ROM_REG_MPU_PC0_END        | 0x00000003U),    ((uint32_t)BSP_CFG_ROM_REG_MPU_PC1_START      & 0x000FFFFCU),    ((uint32_t)BSP_CFG_ROM_REG_MPU_PC1_END        | 0x00000003U),    ((uint32_t)BSP_CFG_ROM_REG_MPU_REGION0_START  & 0x000FFFFCU),    (((uint32_t)BSP_CFG_ROM_REG_MPU_REGION0_END   & 0x000FFFFFU) | 0x00000003U),
    0xFFFFFFFFU,
    0xFFFFFFFFU,
    0xFFFFFFFFU,
    0xFFFFFFFFU,
    0xFFFFFFFFU,
    0xFFFFFFFFU,
    (uint32_t)BSP_ROM_REG_MPU_CONTROL_SETTING
};

#endif


/** @} (end defgroup BSP_MCU_ROM_REGISTER) */
