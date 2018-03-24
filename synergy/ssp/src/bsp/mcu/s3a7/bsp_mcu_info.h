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
* File Name    : bsp_mcu_info.h
* Description  : Information about the MCU on this board
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @ingroup BSP_MCUs
 * @defgroup BSP_MCU_S3A7 S3A7
 * @brief Code that is common to S3A7 MCUs.
 *
 * Implements functions that are common to S3A7 MCUs.
 *
 * @{
***********************************************************************************************************************/

#ifndef BSP_MCU_INFO_H_
#define BSP_MCU_INFO_H_

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/

/* BSP Common Includes. */
#include "../../src/bsp/mcu/all/bsp_common.h"
#include "../../inc/ssp_common_api.h"

#if defined(__GNUC__)
/* CMSIS-CORE currently generates 2 warnings when compiling with GCC. One in core_cmInstr.h and one in core_cm4_simd.h.
 * We are not modifying these files so we will ignore these warnings temporarily. */
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#endif

/* The following files are provided by CMSIS, so violations are not reported during static analysis. */
/*LDRA_NOANALYSIS */

/* CMSIS-CORE Renesas Device Files. */
#include "../../src/bsp/cmsis/Device/RENESAS/S3A7/Include/S3A7.h"
#include "../../src/bsp/cmsis/Device/RENESAS/S3A7/Include/system_S3A7.h"

/* Static analysis resumes as normal after CMSIS files. */
/*LDRA_ANALYSIS */

#if defined(__GNUC__)
/* Restore warning settings for 'conversion' and 'sign-conversion' to as specified on command line. */
#pragma GCC diagnostic pop
#endif

/* BSP MCU Specific Includes. */
#include "../../src/bsp/mcu/s3a7/bsp_mcu_info.h"
#include "../../src/bsp/mcu/all/bsp_register_protection.h"
#include "../../src/bsp/mcu/all/bsp_locking.h"
#include "../../src/bsp/mcu/all/bsp_irq.h"
#include "../../src/bsp/mcu/all/bsp_group_irq.h"
#include "../../src/bsp/mcu/all/bsp_clocks.h"
#include "../../src/bsp/mcu/s3a7/bsp_elc.h"
#include "../../src/bsp/mcu/s3a7/bsp_cache.h"

/* I/O port pin definitions. */
#include "r_ioport_api.h"

/* Factory MCU information. */
#include "../../inc/ssp_features.h"
#include "r_fmi.h"

/* BSP Common Includes (Other than bsp_common.h) */
#include "../../src/bsp/mcu/all/bsp_common_leds.h"
#include "../../src/bsp/mcu/all/bsp_delay.h"
#include "../../src/bsp/mcu/all/bsp_feature.h"

#include "../../src/bsp/mcu/all/bsp_mcu_api.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global variables
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/

#endif /* BSP_MCU_INFO_H_ */

/** @} (end defgroup BSP_MCU_S3A7) */
