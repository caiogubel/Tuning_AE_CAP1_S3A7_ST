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
* Copyright (C) 2015 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_mcu_model_define.h
* Version      : 1.00
* Device(s)    : R5F5113xAxFP,R5F5231xAxFP,R5F51305AxFN
* Description  : This file includes the MCU model.
***********************************************************************************************************************/

/***********************************************************************************************************************
* History      : DD.MM.YYYY Version    Description
*              : 03.20.2015   1.00     First Release
***********************************************************************************************************************/
#ifndef __R_MCU_MODEL_DEFINE_H__    //[
#define __R_MCU_MODEL_DEFINE_H__

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***** RX113 **********************************************************************************************************/
#define MCU_MODEL_R5F51138AxFP  "R5F51138AxFP    "
#define MCU_MODEL_R5F51138AxFM  "R5F51138AxFM    "
#define MCU_MODEL_R5F51138AxLJ  "R5F51138AxLJ    "
#define MCU_MODEL_R5F51137AxFP  "R5F51137AxFP    "
#define MCU_MODEL_R5F51137AxFM  "R5F51137AxFM    "
#define MCU_MODEL_R5F51137AxLJ  "R5F51137AxLJ    "
#define MCU_MODEL_R5F51136AxFP  "R5F51136AxFP    "
#define MCU_MODEL_R5F51136AxFM  "R5F51136AxFM    "
#define MCU_MODEL_R5F51136AxLJ  "R5F51136AxLJ    "
#define MCU_MODEL_R5F51135AxFP  "R5F51135AxFP    "
#define MCU_MODEL_R5F51135AxFM  "R5F51135AxFM    "
#define MCU_MODEL_R5F51135AxLJ  "R5F51135AxLJ    "

/***** RX130 **********************************************************************************************************/
#define MCU_MODEL_R5F51303AxFL  "R5F51303AxFL    "
#define MCU_MODEL_R5F51303AxNE  "R5F51303AxNE    "
#define MCU_MODEL_R5F51305AxFL  "R5F51305AxFL    "
#define MCU_MODEL_R5F51305AxNE  "R5F51305AxNE    "
#define MCU_MODEL_R5F51303AxFK  "R5F51303AxFK    "
#define MCU_MODEL_R5F51303AxFM  "R5F51303AxFM    "
#define MCU_MODEL_R5F51305AxFK  "R5F51305AxFK    "
#define MCU_MODEL_R5F51305AxFM  "R5F51305AxFM    "
#define MCU_MODEL_R5F51303AxFN  "R5F51303AxFN    "
#define MCU_MODEL_R5F51305AxFN  "R5F51305AxFN    "

/***** RX230 **********************************************************************************************************/
#define MCU_MODEL_R5F52305AxFL  "R5F52305AxFL    "
#define MCU_MODEL_R5F52305AxNE  "R5F52305AxNE    "
#define MCU_MODEL_R5F52306AxFL  "R5F52306AxFL    "
#define MCU_MODEL_R5F52306AxNE  "R5F52306AxNE    "
#define MCU_MODEL_R5F52305AxFM  "R5F52305AxFM    "
#define MCU_MODEL_R5F52305AxLF  "R5F52305AxLF    "
#define MCU_MODEL_R5F52305AxND  "R5F52305AxND    "
#define MCU_MODEL_R5F52306AxFM  "R5F52306AxFM    "
#define MCU_MODEL_R5F52306AxLF  "R5F52306AxLF    "
#define MCU_MODEL_R5F52306AxND  "R5F52306AxND    "
#define MCU_MODEL_R5F52305AxFP  "R5F52305AxFP    "
#define MCU_MODEL_R5F52305AxLA  "R5F52305AxLA    "
#define MCU_MODEL_R5F52306AxFP  "R5F52306AxFP    "
#define MCU_MODEL_R5F52306AxLA  "R5F52306AxLA    "

/***** RX231 **********************************************************************************************************/
#define MCU_MODEL_R5F52315AxFL  "R5F52315AxFL    "
#define MCU_MODEL_R5F52315AxNE  "R5F52315AxNE    "
#define MCU_MODEL_R5F52315CxFL  "R5F52315CxFL    "
#define MCU_MODEL_R5F52315CxNE  "R5F52315CxNE    "
#define MCU_MODEL_R5F52316AxFL  "R5F52316AxFL    "
#define MCU_MODEL_R5F52316AxNE  "R5F52316AxNE    "
#define MCU_MODEL_R5F52316CxFL  "R5F52316CxFL    "
#define MCU_MODEL_R5F52316CxNE  "R5F52316CxNE    "
#define MCU_MODEL_R5F52317AxFL  "R5F52317AxFL    "
#define MCU_MODEL_R5F52317AxNE  "R5F52317AxNE    "
#define MCU_MODEL_R5F52317BxFL  "R5F52317BxFL    "
#define MCU_MODEL_R5F52317BxNE  "R5F52317BxNE    "
#define MCU_MODEL_R5F52318AxFL  "R5F52318AxFL    "
#define MCU_MODEL_R5F52318AxNE  "R5F52318AxNE    "
#define MCU_MODEL_R5F52318BxFL  "R5F52318BxFL    "
#define MCU_MODEL_R5F52318BxNE  "R5F52318BxNE    "
#define MCU_MODEL_R5F52315AxFM  "R5F52315AxFM    "
#define MCU_MODEL_R5F52315AxND  "R5F52315AxND    "
#define MCU_MODEL_R5F52315CxFM  "R5F52315CxFM    "
#define MCU_MODEL_R5F52315CxLF  "R5F52315CxLF    "
#define MCU_MODEL_R5F52315CxND  "R5F52315CxND    "
#define MCU_MODEL_R5F52316AxFM  "R5F52316AxFM    "
#define MCU_MODEL_R5F52316AxND  "R5F52316AxND    "
#define MCU_MODEL_R5F52316CxFM  "R5F52316CxFM    "
#define MCU_MODEL_R5F52316CxLF  "R5F52316CxLF    "
#define MCU_MODEL_R5F52316CxND  "R5F52316CxND    "
#define MCU_MODEL_R5F52317AxFM  "R5F52317AxFM    "
#define MCU_MODEL_R5F52317AxND  "R5F52317AxND    "
#define MCU_MODEL_R5F52317BxFM  "R5F52317BxFM    "
#define MCU_MODEL_R5F52317BxND  "R5F52317BxND    "
#define MCU_MODEL_R5F52318AxFM  "R5F52318AxFM    "
#define MCU_MODEL_R5F52318AxND  "R5F52318AxND    "
#define MCU_MODEL_R5F52318BxFM  "R5F52318BxFM    "
#define MCU_MODEL_R5F52318BxND  "R5F52318BxND    "
#define MCU_MODEL_R5F52315AxFP  "R5F52315AxFP    "
#define MCU_MODEL_R5F52315AxLA  "R5F52315AxLA    "
#define MCU_MODEL_R5F52315CxFP  "R5F52315CxFP    "
#define MCU_MODEL_R5F52315CxLA  "R5F52315CxLA    "
#define MCU_MODEL_R5F52316AxFP  "R5F52316AxFP    "
#define MCU_MODEL_R5F52316AxLA  "R5F52316AxLA    "
#define MCU_MODEL_R5F52316CxFP  "R5F52316CxFP    "
#define MCU_MODEL_R5F52316CxLA  "R5F52316CxLA    "
#define MCU_MODEL_R5F52317AxFP  "R5F52317AxFP    "
#define MCU_MODEL_R5F52317AxLA  "R5F52317AxLA    "
#define MCU_MODEL_R5F52317BxFP  "R5F52317BxFP    "
#define MCU_MODEL_R5F52317BxLA  "R5F52317BxLA    "
#define MCU_MODEL_R5F52318AxFP  "R5F52318AxFP    "
#define MCU_MODEL_R5F52318AxLA  "R5F52318AxLA    "
#define MCU_MODEL_R5F52318BxFP  "R5F52318BxFP    "
#define MCU_MODEL_R5F52318BxLA  "R5F52318BxLA    "


/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global variables
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/

#endif //] __R_MCU_MODEL_DEFINE_H__
