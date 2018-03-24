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
* File Name    : r_serial_control.c
* Version      : 1.1.0
* Description  : This file contains the serial command control. Compatible with Synergy SSP V1.1.0.
***********************************************************************************************************************/

#define __R_SERIAL_COMMAND_C__
/***********************************************************************************************************************
* Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Start user code for include. Do not edit comment generated here */
#include <string.h>
#include "bsp_api.h"
#include "r_ctsu_cfg.h"
#include "r_ctsu.h"
#include "../synergy/ssp/src/driver/r_ctsu/r_ctsu_private.h"
#include "../synergy/ssp/src/driver/r_ctsu/r_ctsu_private_api.h"
#include "r_serial_control.h"
#include "captouch_configs/ctsu_configs/utilities/r_touch.h"

#if defined(KEY_USE) && (USING_CAPTOUCH_BUTTONS > 0)
#include "sf_touch_ctsu_button_api.h"
#endif
#if (defined(SLIDER_USE) || defined(WHEEL_USE)) && (USING_CAPTOUCH_SLIDER > 0)
#include "r_touch_slider_if.h"
#endif
#include "captouch_configs/ctsu_configs/utilities/r_mcu_model_define.h"


/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/
#define TRUE                (1u)
#define FALSE               (0u)

#if !defined(MCU_MODEL)
#define MCU_MODEL	(MCU_MODEL_R5F5xxxxxxxx)
#endif

#if !defined(MAX_TS)
#if (BSP_CFG_MCU_PART_SERIES==7)
#define MAX_TS		(18)
#elif (BSP_CFG_MCU_PART_SERIES==5)
#define MAX_TS		(36)
#elif (BSP_CFG_MCU_PART_SERIES==1)
#define MAX_TS		(31)
#elif (BSP_CFG_MCU_PART_SERIES==3)
#define MAX_TS      (36)
#endif
#endif
/* The number of enable matrix key */
#define MAX_USR_MATRIX_KEY  (MUTUAL0_KEY_NUM + MUTUAL1_KEY_NUM + MUTUAL2_KEY_NUM + MUTUAL3_KEY_NUM + \
                             MUTUAL4_KEY_NUM + MUTUAL5_KEY_NUM + MUTUAL6_KEY_NUM + MUTUAL7_KEY_NUM)

#if (SELF_METHOD_NUM > 0)
#define MAX_SELF_SENSOR_ID   SELFCAP_SENSOR_MAX
#else   // (SELF_METHOD_NUM == 1)
#define MAX_SELF_SENSOR_ID   (0)
#endif  // (SELF_METHOD_NUM == 1)

#if (MUTUAL_METHOD_NUM > 7)
#define MAX_MUTUAL_SENSOR_ID   (MUTUAL0_NUM + MUTUAL1_NUM + MUTUAL2_NUM + MUTUAL3_NUM + \
                         MUTUAL4_NUM + MUTUAL5_NUM + MUTUAL6_NUM + MUTUAL7_NUM)
#elif (MUTUAL_METHOD_NUM > 6)
#define MAX_MUTUAL_SENSOR_ID   (MUTUAL0_NUM + MUTUAL1_NUM + MUTUAL2_NUM + MUTUAL3_NUM + \
                         MUTUAL4_NUM + MUTUAL5_NUM + MUTUAL6_NUM)
#elif (MUTUAL_METHOD_NUM > 5)
#define MAX_MUTUAL_SENSOR_ID   (MUTUAL0_NUM + MUTUAL1_NUM + MUTUAL2_NUM + MUTUAL3_NUM + MUTUAL4_NUM + MUTUAL5_NUM)
#elif (MUTUAL_METHOD_NUM > 4)
#define MAX_MUTUAL_SENSOR_ID   (MUTUAL0_NUM + MUTUAL1_NUM + MUTUAL2_NUM + MUTUAL3_NUM + MUTUAL4_NUM)
#elif (MUTUAL_METHOD_NUM > 3)
#define MAX_MUTUAL_SENSOR_ID   (MUTUAL0_NUM + MUTUAL1_NUM + MUTUAL2_NUM + MUTUAL3_NUM)
#elif (MUTUAL_METHOD_NUM > 2)
#define MAX_MUTUAL_SENSOR_ID   (MUTUAL0_NUM + MUTUAL1_NUM + MUTUAL2_NUM)
#elif (MUTUAL_METHOD_NUM > 1)
#define MAX_MUTUAL_SENSOR_ID   (MUTUAL0_NUM + MUTUAL1_NUM)
#elif (MUTUAL_METHOD_NUM > 0)
#define MAX_MUTUAL_SENSOR_ID   (MUTUAL0_NUM)
#else
#define MAX_MUTUAL_SENSOR_ID   (0)
#endif

#define MAX_SENSOR_ID   (MAX_SELF_SENSOR_ID + MAX_MUTUAL_SENSOR_ID)

#define BUF_SIZE_SND_TMP    (4/* HEADER */ +                                                        \
                             4/* BDATA */ * sizeof(uint16_t)/* WORD */ * METHOD_NUM +               \
                             2/* CV + RV */ * sizeof(uint16_t)/* WORD */ * MAX_SENSOR_ID +      \
                             1/* SLIDER POSITION */ * sizeof(uint16_t)/* WORD */ * SLIDER_NUMBER +  \
                             1/* WHEEL POSITION */ * sizeof(uint16_t)/* WORD */ * WHEEL_NUMBER +    \
                             2/* RETURN VALUE */)

#define HEAD_SIZE           (4)
#define BUF_SIZE_RCV        (HEAD_SIZE + 32)
#define MONITOR_CMD_SIZE    (24)

#define BUF_SIZE_SND        (BUF_SIZE_SND_TMP + BUF_SIZE_SND_TMP % 4)

// command definition
#define CMD_PROFILE    (0x00u)
#define CMD_MEASURE    (0x01u)
#define CMD_PARAMETER  (0x02)
#define CMD_REGISTER   (0x03)
#define CMD_UTILITY    (0x04)
#define CMD_RESERVED   (0x05)

// Workbench ---> Touch sensor
#define CMD_RQ    (0x00u)
#define CMD_READ  (0x00)
#define CMD_WRITE (0x40)

#define CMD_RQ_PROFILE_READ     (CMD_RQ | CMD_READ  | CMD_PROFILE)      // sensor data read request
#define CMD_RQ_PROFILE_WRITE    (CMD_RQ | CMD_WRITE | CMD_PROFILE)      // sensor data read request
#define CMD_RQ_MEASURE_READ     (CMD_RQ | CMD_READ  | CMD_MEASURE)      // sensor data write request
#define CMD_RQ_MEASURE_WRITE    (CMD_RQ | CMD_WRITE | CMD_MEASURE)      // sensor data write request
#define CMD_RQ_PARAMETER_READ   (CMD_RQ | CMD_READ  | CMD_PARAMETER)    // usb i/f board status getting request
#define CMD_RQ_PARAMETER_WRITE  (CMD_RQ | CMD_WRITE | CMD_PARAMETER)    // usb i/f board status getting request
#define CMD_RQ_REGISTER_READ    (CMD_RQ | CMD_READ  | CMD_REGISTER)     // usb i/f board status control request
#define CMD_RQ_REGISTER_WRITE   (CMD_RQ | CMD_WRITE | CMD_REGISTER)     // usb i/f board status control request
#define CMD_RQ_UTILITY_READ     (CMD_RQ | CMD_READ  | CMD_UTILITY)      // usb i/f board read request
#define CMD_RQ_UTILITY_WRITE    (CMD_RQ | CMD_WRITE | CMD_UTILITY)      // usb i/f board read request

// Workbench <=== Touch sensor 
#define CMD_RE    (0x80u)

#define CMD_RESULT_SUCCESS    (0x00)
#define CMD_RESULT_FAILURE    (0xff)

#define CMD_UTILITY_PARAM_UPDATE    (1)
#define CMD_UTILITY_PARAM_RESET     (2)

#define CMD_UTILITY_UPDATE_FINISH   (0)
#define CMD_UTILITY_UPDATE_RUNNING  (1)

#define CMD_UTILITY_RESET_FINISH    (0)
#define CMD_UTILITY_RESET_RUNNING   (1)

#define COMMAND_MODE_SERIAL (0)    // Serial port communication
#define COMMAND_MODE_IDE    (1)    // Cube suite+/e2studio communication

/* Define the number of matrix key in Matrix0 when Matrix0 is disabled.  */
#if !defined(MUTUAL0_KEY_NUM)
#define MUTUAL0_KEY_NUM  0
#endif

/* Define the number of matrix key in Matrix1 when Matrix1 is disabled.  */
#if !defined(MUTUAL1_KEY_NUM)
#define MUTUAL1_KEY_NUM  0
#endif

/* Define the number of matrix key in Matrix2 when Matrix2 is disabled.  */
#if !defined(MUTUAL2_KEY_NUM)
#define MUTUAL2_KEY_NUM  0
#endif

/* Define the number of matrix key in Matrix3 when Matrix3 is disabled.  */
#if !defined(MUTUAL3_KEY_NUM)
#define MUTUAL3_KEY_NUM  0
#endif

/* Define the number of matrix key in Matrix4 when Matrix4 is disabled.  */
#if !defined(MUTUAL4_KEY_NUM)
#define MUTUAL4_KEY_NUM  0
#endif

/* Define the number of matrix key in Matrix5 when Matrix5 is disabled.  */
#if !defined(MUTUAL5_KEY_NUM)
#define MUTUAL5_KEY_NUM  0
#endif

/* Define the number of matrix key in Matrix6 when Matrix6 is disabled.  */
#if !defined(MUTUAL6_KEY_NUM)
#define MUTUAL6_KEY_NUM  0
#endif

/* Define the number of matrix key in Matrix7 when Matrix7 is disabled.  */
#if !defined(MUTUAL7_KEY_NUM)
#define MUTUAL7_KEY_NUM  0
#endif

/* Decide the number of maxumum matrix keys in all matrix */
#if (MUTUAL0_KEY_NUM >= MUTUAL1_KEY_NUM)
    #define MUTUAL01_MAX MUTUAL0_KEY_NUM
#else
    #define MUTUAL01_MAX MUTUAL1_KEY_NUM
#endif

#if (MUTUAL01_MAX >= MUTUAL2_KEY_NUM)
    #define MUTUAL02_MAX MUTUAL01_MAX
#else
    #define MUTUAL02_MAX MUTUAL2_KEY_NUM
#endif

#if (MUTUAL02_MAX >= MUTUAL3_KEY_NUM)
    #define MUTUAL03_MAX MUTUAL02_MAX
#else
    #define MUTUAL03_MAX MUTUAL3_KEY_NUM
#endif

#if (MUTUAL03_MAX >= MUTUAL4_KEY_NUM)
    #define MUTUAL04_MAX MUTUAL03_MAX
#else
    #define MUTUAL04_MAX MUTUAL4_KEY_NUM
#endif

#if (MUTUAL04_MAX >= MUTUAL5_KEY_NUM)
    #define MUTUAL05_MAX MUTUAL04_MAX
#else
    #define MUTUAL05_MAX MUTUAL5_KEY_NUM
#endif

#if (MUTUAL05_MAX >= MUTUAL6_KEY_NUM)
    #define MUTUAL06_MAX MUTUAL05_MAX
#else
    #define MUTUAL06_MAX MUTUAL6_KEY_NUM
#endif

#if (MUTUAL06_MAX >= MUTUAL7_KEY_NUM)
    #define MUTUAL07_MAX MUTUAL06_MAX
#else
    #define MUTUAL07_MAX MUTUAL7_KEY_NUM
#endif

/* Maximum measurement time in case of the Self-capacitance */
#define SELF_METHOD_MAX_US  (SELF_ENABLE_NUM * SELF_MEASURE_TIME_US)

/* Maximum measurement time in case of the Mutual-capacitance */
#define MUTUAL_METHOD_MAX_US (MUTUAL07_MAX * MUTUAL_MEASURE_TIME_US)

/* Re-define the measurement time with minimum value when the measurement time is less than minimum value  */
#if (SELF_METHOD_MAX_US < TOUCH_MAIN_CYCLE_US)
    #undef  SELF_METHOD_MAX_US
    #define SELF_METHOD_MAX_US  TOUCH_MAIN_CYCLE_US
#endif

#if (MUTUAL_METHOD_MAX_US < TOUCH_MAIN_CYCLE_US)
    #undef  MUTUAL_METHOD_MAX_US
    #define MUTUAL_METHOD_MAX_US  TOUCH_MAIN_CYCLE_US
#endif

/* Convert unit of the maximum measurement time to mill-second */
#if (MUTUAL_METHOD_MAX_US >= SELF_METHOD_MAX_US)
    #define TOUCH_METHOD_MAX_MS ((MUTUAL_METHOD_MAX_US / 1000) + 1)
#else
    #define TOUCH_METHOD_MAX_MS ((SELF_METHOD_MAX_US / 1000) + 1)
#endif

/* Define all measurement time of all method */
#define TOUCH_METHOD_CYCLE_MS ((SELF_METHOD_NUM + MUTUAL_METHOD_NUM) * TOUCH_METHOD_MAX_MS)

#if (ADD_TIME == 4) 
#define WAIT_TIME    ADD4_WAIT_FREQUENCY * TOUCH_METHOD_CYCLE_MS   /* Wait-frequency:42 */
#elif ( ADD_TIME == 5 )
#define WAIT_TIME    ADD5_WAIT_FREQUENCY * TOUCH_METHOD_CYCLE_MS   /* Wait-frequency:56 */
#elif ( ADD_TIME == 6 )
#define WAIT_TIME    ADD6_WAIT_FREQUENCY * TOUCH_METHOD_CYCLE_MS   /* Wait-frequency:67 */
#elif ( ADD_TIME == 7 )
#define WAIT_TIME    ADD7_WAIT_FREQUENCY * TOUCH_METHOD_CYCLE_MS   /* Wait-frequency:78 */
#elif ( ADD_TIME == 8 )
#define WAIT_TIME    ADD8_WAIT_FREQUENCY * TOUCH_METHOD_CYCLE_MS   /* Wait-frequency:90 */
#else
#error    "Illagal additional time is specified. Check Macro definition ADD_TIME."
#endif

/***********************************************************************************************************************
* Typedef definitions
***********************************************************************************************************************/
typedef    unsigned char    BOOL;

typedef union type_com_data_rd
{
    struct
    {
        uint8_t main;
        uint8_t sub;
        uint8_t size;
        uint8_t sum;
        uint8_t data[BUF_SIZE_RCV - HEAD_SIZE];
    } fmt;
    uint8_t byte_acs[BUF_SIZE_RCV];
} com_data_rd_t;

typedef union type_com_data_tx
{
    uint32_t dw_acs[BUF_SIZE_SND / 4];
    struct
    {
        uint8_t main;
        uint8_t sub;
        uint8_t size;
        uint8_t sum;
        uint8_t data[BUF_SIZE_SND - HEAD_SIZE];
    } fmt;
    uint8_t byte_acs[BUF_SIZE_SND];
}com_data_tx_t;

typedef struct type_monitor_command
{
    uint8_t command[MONITOR_CMD_SIZE];
    uint8_t size;
} monitor_command_t;

#if (SELFCAP_SENSOR_MAX > MUTUAL07_MAX)
    #define MAX_KEY_NUM SELFCAP_SENSOR_MAX
#else
    #define MAX_KEY_NUM MUTUAL07_MAX
#endif

typedef struct
{
    uint8_t index[MAX_KEY_NUM];
}    touch_key_index_table_t;

#define METHOD_TYPE_SLFCP   (0)
#define METHOD_TYPE_MTLCP   (1)
typedef struct
{
    uint8_t type;   /* Method type (METHOD_TYPE_SLFCP, METHOD_TYPE_MTLCP ) */
    uint8_t enable; /* Number of enabled touch sensor */
}   method_info_t;

/***********************************************************************************************************************
* Exported global variables (to be accessed by other files);
***********************************************************************************************************************/
#if defined(KEY_USE) && defined(SF_TOUCH_CTSU_BUTTON_API_H)
extern sf_touch_ctsu_button_individual_t* touch_buttons[];
extern const size_t num_touch_buttons;
#endif

#if (defined(SLIDER_USE) || defined(WHEEL_USE)) && (USING_CAPTOUCH_SLIDER > 0)
extern const size_t num_touch_sliders;
extern touch_slider_t* touch_sliders[];
#endif

#ifdef SLIDER_USE
slider_info_t    g_sliderInfo[SLIDER_NUMBER];
#endif

#ifdef WHEEL_USE
wheel_info_t     g_wheelInfo[WHEEL_NUMBER];
#endif

/***********************************************************************************************************************
* Private global variables and functions
***********************************************************************************************************************/
#ifdef WORKBENCH_COMMAND_USE
com_data_rd_t com_data;    /* Received data buffer */
com_data_tx_t rsp_cmd;    /* Transmit data buffer */
monitor_command_t monitor_command;    /* Monitor commands buffer */
BOOL        serial_transmit_ready;
uint8_t     command_mode;

/* MCU model name */
static const uint8_t g_mcu_model_name[] = MCU_MODEL;

const uint8_t g_matrix_index[] = {
    0,   1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
    32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
};

key_info_t              g_key_info[METHOD_NUM];
touch_group_param_t     g_touch_key_group[METHOD_NUM];

volatile uint8_t       g_ctsu_soft_mode = CTSU_STOP_MODE;               		/* CTSU operating mode flag     */
volatile ctsu_status_t           g_ctsu_status[METHOD_NUM];                  	/* CTSU measurement status flag */


extern ctsu_hw_cfg_t* all_ctsu_configs[];
extern ctsu_instance_ctrl_t g_ctsu_ctrl_tune;
extern ctsu_cfg_t g_ctsu_cfg_tune;
ctsu_ctrl_t* p_ctsu_ctrl = &g_ctsu_ctrl_tune;
ctsu_cfg_t* p_ctsu_cfg = &g_ctsu_cfg_tune;

static uint8_t g_burst_mode = 0;

uint8_t g_access_method  = 0;

static const method_info_t g_method_info[] = {
/* --[Self capacitance]----------------- */
#if ( SELF_METHOD_NUM > 0 )
    { METHOD_TYPE_SLFCP,    SELFCAP_SENSOR_MAX },
#endif  // ( SELF_METHOD_NUM > 0 )
/* --[Mutual capacitance 0]------------- */
#if ( MUTUAL_METHOD_NUM > 0 )
    { METHOD_TYPE_MTLCP,    MUTUAL0_NUM },
#endif  // ( MUTUAL_METHOD_NUM > 0 )
/* --[Mutual capacitance 1]------------- */
#if ( MUTUAL_METHOD_NUM > 1 )
    { METHOD_TYPE_MTLCP,   MUTUAL1_NUM },
#endif  // ( MUTUAL_METHOD_NUM > 1 )
/* --[Mutual capacitance 2]------------- */
#if ( MUTUAL_METHOD_NUM > 2 )
    { METHOD_TYPE_MTLCP,   MUTUAL2_NUM },
#endif  // ( MUTUAL_METHOD_NUM > 2 )
/* --[Mutual capacitance 3]------------- */
#if ( MUTUAL_METHOD_NUM > 3 )
    { METHOD_TYPE_MTLCP,   MUTUAL3_NUM },
#endif  // ( MUTUAL_METHOD_NUM > 3 )
/* --[Mutual capacitance 4]------------- */
#if ( MUTUAL_METHOD_NUM > 4 )
    { METHOD_TYPE_MTLCP,   MUTUAL4_NUM },
#endif  // ( MUTUAL_METHOD_NUM > 4 )
/* --[Mutual capacitance 5]------------- */
#if ( MUTUAL_METHOD_NUM > 5 )
    { METHOD_TYPE_MTLCP,   MUTUAL5_NUM },
#endif  // ( MUTUAL_METHOD_NUM > 5 )
/* --[Mutual capacitance 6]------------- */
#if ( MUTUAL_METHOD_NUM > 6 )
    { METHOD_TYPE_MTLCP,   MUTUAL6_NUM },
#endif  // ( MUTUAL_METHOD_NUM > 6 )
/* --[Mutual capacitance 7]------------- */
#if ( MUTUAL_METHOD_NUM > 7 )
    { METHOD_TYPE_MTLCP,   MUTUAL7_NUM },
#endif  // ( MUTUAL_METHOD_NUM > 7 )
};

static touch_key_index_table_t  g_touch_key_index[METHOD_NUM];

static void CreateResponceCommand(com_data_tx_t * pcmd);
static uint8_t SetChecksum(uint8_t main, uint8_t sub, uint8_t size, uint8_t * pdata);
static BOOL IsRightChecksum(com_data_rd_t * pcmd);
static uint8_t GetSensorValue(uint8_t code, uint16_t channel, uint16_t * pval);
static void SensorProfileReadResponse(com_data_tx_t * pcmd, uint16_t channel);
static void SensorMeasureReadResponse(com_data_tx_t * pcmd, uint16_t channel);
static void SensorParameterReadResponse(com_data_tx_t * pcmd, uint16_t channel);
static void SensorParameterWriteResponse(com_data_tx_t * pcmd, uint16_t channel);
static void SensorRegisterReadResponse(com_data_tx_t * pcmd, uint16_t channel);
static void SensorRegisterWriteResponse(com_data_tx_t * pcmd, uint16_t channel);
static void SensorUtilityReadResponse(com_data_tx_t * pcmd, uint16_t channel);
static void SensorUtilityWriteResponse(com_data_tx_t * pcmd, uint16_t channel);


#endif // WORKBENCH_COMMAND_USE

/***********************************************************************************************************************
* Function Name: SerialCommandInitial
* Description  : Serial command initialization
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
uint8_t g_key_sensor_index[METHOD_NUM][(MAX_MUTUAL_SENSOR_ID > SELFCAP_SENSOR_MAX) ? MAX_MUTUAL_SENSOR_ID : SELFCAP_SENSOR_MAX];
uint16_t g_touch_key_group_group[METHOD_NUM][(MAX_MUTUAL_SENSOR_ID > SELFCAP_SENSOR_MAX) ? ((MAX_MUTUAL_SENSOR_ID >> 4) + 1) : ((SELFCAP_SENSOR_MAX >> 4) + 1)];

#if (MAX_MUTUAL_SENSOR_ID > 0)
static uint8_t tx_ch[MAX_TS];
static uint8_t rx_ch[MAX_TS];
#endif

void SerialCommandInitial(void)
{
#ifdef WORKBENCH_COMMAND_USE
    serial_transmit_ready = FALSE;
    command_mode          = COMMAND_MODE_SERIAL;
    uint8_t ch_num;
    uint16_t index = 0;

    (void)index;


    for(int i = 0; i < METHOD_NUM; i++)
    {
        uint16_t rx_count = 0;

    	uint64_t ch_en = 0;
    	uint64_t ch_tx = 0;
#if !defined(BSP_MCU_RX113)
    	ch_en = all_ctsu_configs[i]->ctsu_settings.CTSUCHAC4;
		ch_tx = all_ctsu_configs[i]->ctsu_settings.CTSUCHTRC4;
    	ch_en <<= 8;
    	ch_tx <<= 8;
    	ch_en |= all_ctsu_configs[i]->ctsu_settings.CTSUCHAC3;
    	ch_tx |= all_ctsu_configs[i]->ctsu_settings.CTSUCHTRC3;
    	ch_en <<= 8;
    	ch_tx <<= 8;
    	ch_en |= all_ctsu_configs[i]->ctsu_settings.CTSUCHAC2;
    	ch_tx |= all_ctsu_configs[i]->ctsu_settings.CTSUCHTRC2;
    	ch_en <<= 8;
    	ch_tx <<= 8;
#endif
    	ch_en |= all_ctsu_configs[i]->ctsu_settings.CTSUCHAC1;
    	ch_tx |= all_ctsu_configs[i]->ctsu_settings.CTSUCHTRC1;
    	ch_en <<= 8;
    	ch_tx <<= 8;
    	ch_en |= all_ctsu_configs[i]->ctsu_settings.CTSUCHAC0;
    	ch_tx |= all_ctsu_configs[i]->ctsu_settings.CTSUCHTRC0;

    	/* Assign touch key groups */
    	g_touch_key_group[i].group = g_touch_key_group_group[i];
        /* Assign the sensor index number array. */
        g_key_info[i].sensor_index = g_key_sensor_index[i];
    	if(g_method_info[i].type==METHOD_TYPE_SLFCP)
    	{/* Self-capacitance. */
#if (SELFCAP_SENSOR_MAX > 0)
    		for( ch_num = 0; ch_num < SELFCAP_SENSOR_MAX; ch_num++)
    		{
    			g_touch_key_index[i].index[ch_num] = 0xff;
    			g_key_info[i].sensor_index[ch_num] = 0xff;
    			if((ch_en & ((uint64_t)1<<ch_num))==((uint64_t)1<<ch_num))
    			{
    				g_key_info[i].ena_num = (uint8_t)(g_key_info[i].ena_num + 1);
    				g_key_info[i].sensor_index[ch_num] = (uint8_t)(rx_count & 0xFF);
    				g_touch_key_index[i].index[ch_num] = (uint8_t)(rx_count & 0xFF);
#if defined(KEY_USE) && defined(SF_TOUCH_CTSU_BUTTON_API_H)
    				for(index = 0; index < num_touch_buttons; index++)
    				{
    					if((touch_buttons[index]->button_channel.rx == ch_num) && (touch_buttons[index]->button_channel.tx==-1))
    					{/* Found a button with the corresponding channel #'s enabled. */
    						g_key_info[i].key_num = (uint8_t)(g_key_info[i].key_num + 1);
    						g_touch_key_group_group[i][rx_count >> 4] = (uint16_t)(g_touch_key_group_group[i][rx_count >> 4] | (1<<(rx_count%16)));		//Right shift by 4 because group is a uint16_t.
    					}
    				}
#endif//KEY_USE

    				rx_count = (uint16_t)(rx_count + 1);
    			}
    		}
    		g_key_info[i].key_max_group = (uint8_t)((g_key_info[i].ena_num >> 4) + 1);
    		g_key_info[i].touch_result = (uint16_t*)all_ctsu_configs[i]->binary_result;
#endif
    	}
    	else if(g_method_info[i].type==METHOD_TYPE_MTLCP)
    	{/* Mutual-capacitance. */
#if (MAX_MUTUAL_SENSOR_ID > 0)
            uint16_t tx_count = 0;
            uint16_t index_tx = 0;
            uint16_t index_rx = 0;
    		for( ch_num = 0, rx_count = 0; ch_num < MAX_TS; ch_num++)
    		{
    			if(((ch_en & ((uint64_t)1<<ch_num))==((uint64_t)1<<ch_num)) && ((ch_tx & ((uint64_t)1<<ch_num))==0))
    			{
    				rx_ch[rx_count] = ch_num;
    				rx_count = (uint16_t)(rx_count + 1);
    			}
    		}
    		for( ch_num = 0, tx_count = 0; ch_num < MAX_TS; ch_num++)
    		{
    			if((ch_tx & ((uint64_t)1<<ch_num))==((uint64_t)1<<ch_num))
    			{
    				tx_ch[tx_count] = ch_num;
    				tx_count = (uint16_t)(tx_count + 1);
    			}
    		}

    		g_key_info[i].ena_num = (uint8_t)(rx_count*tx_count) & 0xFF;

    		if(g_key_info[i].ena_num > 0)
    		{/* Save the number of channels enabled. */
    			g_key_info[i].key_max_group = (uint8_t)((g_key_info[i].ena_num >> 4) + 1);
    			g_key_info[i].touch_result = (uint16_t*)all_ctsu_configs[i]->binary_result;
    		}
    		else
    		{
    			return;// TODO error;
    		}

    		for(index_rx = 0; index_rx < rx_count; index_rx++)
    		{
    			for(index_tx = 0; index_tx < tx_count; index_tx++)
    			{
#if defined(KEY_USE) && defined(SF_TOUCH_CTSU_BUTTON_API_H)
    				for(index = 0; index < num_touch_buttons; index++)
    				{/* Save the number of channels enabled as keys. */
    					if((touch_buttons[index]->button_channel.rx == rx_ch[index_rx]) && (touch_buttons[index]->button_channel.tx==tx_ch[index_tx]))
    					{/* Found a button with the corresponding channel #'s enabled. */
    						g_key_info[i].sensor_index[index_rx*tx_count+index_tx] = (uint8_t)(index_rx*tx_count+index_tx);
    						g_touch_key_index[i].index[index_rx*tx_count+index_tx] = (uint8_t)(index_rx*tx_count+index_tx);
    						g_key_info[i].key_num = (uint8_t)(g_key_info[i].key_num + 1);
    					}
    				}
#else
    				g_key_info[i].sensor_index[index_rx*tx_count+index_tx] = (uint8_t)(index_rx*tx_count+index_tx);
    				g_touch_key_index[i].index[index_rx*tx_count+index_tx] = (uint8_t)(index_rx*tx_count+index_tx);
    				g_key_info[i].key_num = (uint8_t)(g_key_info[i].key_num + 1);
#endif
    			}
    		}
#endif
    	}


#if defined(SLIDER_USE) && (USING_CAPTOUCH_SLIDER > 0)
    	/* Initialize workbench structures from the widgets. */
    	for(int slider_idx = 0, slider_itr = 0; (slider_idx < num_touch_sliders) && (slider_itr < SLIDER_NUMBER); slider_idx++)
    	{
    		touch_slider_t* slider = touch_sliders[slider_idx];
    		if(slider->info->type==SLIDER_LAYOUT_LINEAR)
    		{
    			g_sliderInfo[slider_itr].num =  slider->info->num_elements;
    			for(int slider_ch_idx = 0; slider_ch_idx < slider->info->num_elements; slider_ch_idx++)
    			{
    				g_sliderInfo[slider_itr].ts[slider_ch_idx] = slider->info->channels[slider_ch_idx].rx;
    			}
    			g_sliderInfo[slider_itr].threshold = slider->info->threshold & 0x7FFF;
    			g_sliderInfo[slider_itr].resolution = slider->info->threshold/(slider->info->cutoff);
    			slider_itr++;
    		}
    	}
#endif //SLIDER_USE

#if defined(WHEEL_USE) && (USING_CAPTOUCH_SLIDER > 0)
    	for(int wheel_idx = 0, wheel_itr = 0; (wheel_idx < num_touch_sliders) && (wheel_itr < SLIDER_NUMBER); wheel_idx++)
    	{
    		touch_slider_t* slider = touch_sliders[wheel_idx];
    		if(slider->info->type==SLIDER_LAYOUT_CIRCULAR)
    		{
    			g_wheelInfo[wheel_itr].num =  slider->info->num_elements;
    			for(int wheel_ch_idx = 0; wheel_ch_idx < slider->info->num_elements; wheel_ch_idx++)
    			{
    				g_wheelInfo[wheel_itr].ts[wheel_ch_idx] = slider->info->channels[wheel_ch_idx].rx;
    			}
    			g_wheelInfo[wheel_itr].threshold = slider->info->threshold & 0x7FFF;
    			g_wheelInfo[wheel_itr].resolution = slider->info->threshold/(slider->info->cutoff);
    			wheel_itr++;
    		}
    	}
#endif //WHEEL_USE

    }
#endif  // WORKBENCH_COMMAND_USE
}

/***********************************************************************************************************************
* Function Name: PrepareReplayMessage
* Description  :
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void PrepareReplayMessage(void)
{
#ifdef WORKBENCH_COMMAND_USE
    if (serial_transmit_ready)
    {
        if (COMMAND_MODE_IDE == command_mode)
        {
            /* preparing transmittion buffer */
            CreateResponceCommand(&rsp_cmd);
        }
        serial_transmit_ready    = FALSE;
    }
#endif // WORKBENCH_COMMAND_USE
}

/***********************************************************************************************************************
* Function Name: Serial command receive
* Description  : SerialCommandReceive
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
uint8_t    SerialCommandReceive(uint8_t * value, uint16_t length)
{
    uint8_t result = FALSE;

#ifdef WORKBENCH_COMMAND_USE
    uint8_t i;

    for (i = 0; i < length; i++)
    {
        com_data.byte_acs[i]    = value[i];
    }
    if (FALSE != IsRightChecksum(&com_data))
    {
        serial_transmit_ready    = TRUE;

        result    = TRUE;
    }
#endif // WORKBENCH_COMMAND_USE
    return result;
}

/***********************************************************************************************************************
* Function Name: GetReplayMessage
* Description  : Replay the serial command from USB port.
* Arguments    : 
* Return Value : TRUE: success, FALSE: failure
***********************************************************************************************************************/
uint8_t    GetReplayMessage(uint8_t * value, uint16_t * length)
{
    BOOL    result = FALSE;

#ifdef WORKBENCH_COMMAND_USE
    uint16_t i;
    uint16_t size;

    if (serial_transmit_ready)
    {
        /* preparing transmittion buffer */
        CreateResponceCommand(&rsp_cmd);

        /* UARTi transmission data is set */
        *length    =
        size    = (uint8_t)(rsp_cmd.fmt.size + HEAD_SIZE);
        if (rsp_cmd.fmt.main & 0x20)
        {
            size = (uint16_t)(size + 256);
            *length = size;
        }
        for (i = 0; i < size; i++)
        {
            value[i]    = rsp_cmd.byte_acs[i];
        }
        serial_transmit_ready    = FALSE;
        result                   = TRUE;
    }
#endif // WORKBENCH_COMMAND_USE
    return result;
}

#ifdef WORKBENCH_COMMAND_USE // [
/***********************************************************************************************************************
* Function Name: GetMeasureSensorCounter
* Description  : Getting the value of Sensor counter value (Count value)
* Arguments    : uint16_t channel   : Touch sensor channel number
*              : uint16_t * value   : Value of Sensor counter value (Count value)
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
uint16_t results_buffer[MAX_KEY_NUM] = {0};
static uint8_t GetMeasureSensorCounter(uint16_t channel, uint16_t * value)
{
    uint8_t result;
    uint8_t index;

    *value = 0xffff;
    result = CMD_RESULT_FAILURE;
    if (channel < g_method_info[g_access_method].enable)
    {
        if (g_method_info[g_access_method].type == 0)
        {
        	index = g_key_info[g_access_method].sensor_index[channel];
            if ( index != 0xff)
            {
                ctsu_channel_data_self_t* filtered_values = all_ctsu_configs[g_access_method]->raw_result;
                *value = filtered_values[index].sensor_count;
            }
        }
        else
        {
        	index = g_key_info[g_access_method].sensor_index[channel];
            if (g_key_info[g_access_method].sensor_index[channel] != 0xff)
            {
            	ctsu_channel_data_mutual_t* filtered_values = all_ctsu_configs[g_access_method]->raw_result;
                *value = (uint16_t)(filtered_values[index].sen_cnt_2 - filtered_values[index].sen_cnt_1);
            }
        }
        result = CMD_RESULT_SUCCESS;
    }
    return result;
}

/***********************************************************************************************************************
* Function Name: GetMeasureReferenceValue
* Description  : Getting the value of Reference value
* Arguments    : uint16_t channel   : Touch sensor channel number
*              : uint16_t * value   : Value of Reference value
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetMeasureReferenceValue(uint16_t channel, uint16_t * value)
{
    uint8_t result;
    uint8_t index;

    *value = 0xffff;
    result = CMD_RESULT_FAILURE;
    if (channel < g_method_info[g_access_method].enable)
    {
        index = g_touch_key_index[g_access_method].index[channel];
        if (0xff != index)
        {
            R_CTSU_Read_Results( p_ctsu_ctrl, results_buffer, CTSU_READ_BASELINE_COUNT_ALL, NULL, 0);
            *value = results_buffer[index];
        }
        result = CMD_RESULT_SUCCESS;
    }
    return result;
}

/***********************************************************************************************************************
* Function Name: GetMeasureReferenceCounter
* Description  : Getting the value of Reference counter
* Arguments    : uint16_t channel   : Touch sensor channel number
*              : uint16_t * value   : Value of Reference value
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetMeasureReferenceCounter(uint16_t channel, uint16_t * value)
{
    uint8_t result;
    uint8_t index;

    *value = 0xffff;
    result = CMD_RESULT_FAILURE;
    if (channel < g_method_info[g_access_method].enable)
    {
        if (g_method_info[g_access_method].type == 0)
        {
        	index = g_key_info[g_access_method].sensor_index[channel];
            if ( index != 0xff)
            {
            	ctsu_channel_data_self_t* filtered_values = all_ctsu_configs[g_access_method]->raw_result;
            	if(filtered_values!=NULL)
            	{
            		*value = filtered_values[index].reference_count;
            		result = CMD_RESULT_SUCCESS;
            	}
            }
        }
        else
        {
        	index = g_key_info[g_access_method].sensor_index[channel];
            if ( index != 0xff)
            {
            	ctsu_channel_data_mutual_t* filtered_values = all_ctsu_configs[g_access_method]->raw_result;
            	if(filtered_values!=NULL)
            	{
            		*value = (uint16_t)(filtered_values[index].ref_cnt_1);
            		result = CMD_RESULT_SUCCESS;
            	}

                result = CMD_RESULT_SUCCESS;
            }
        }
        result = CMD_RESULT_SUCCESS;
    }
    return result;
}

/***********************************************************************************************************************
* Function Name: GetMeasureSliderPosition
* Description  : Getting the value of Slider position
* Arguments    : uint16_t channel   : Touch sensor channel number
*              : uint16_t * value   : Value of Slider position
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetMeasureSliderPosition(uint16_t channel, uint16_t * value)
{
    uint8_t result;
    (void)channel;
    (void)value;
    result = CMD_RESULT_FAILURE;
#ifdef  SLIDER_USE
    if (SLIDER_NUMBER > channel)
    {
        *value = g_sliderInfo[channel].value;
        result = CMD_RESULT_SUCCESS;
    }
#endif  // SLIDER_USE
    return result;
}

/***********************************************************************************************************************
* Function Name: GetMeasureWheelPosition
* Description  : Getting the value of Wheel position
* Arguments    : uint16_t channel   : Touch sensor channel number
*              : uint16_t * value   : Value of Wheel position
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetMeasureWheelPosition(uint16_t channel, uint16_t * value)
{
    uint8_t result;
    (void)channel;
    (void)value;

    result = CMD_RESULT_FAILURE;
#ifdef  WHEEL_USE
    if (WHEEL_NUMBER > channel)
    {
        *value = g_wheelInfo[channel].value;
        result = CMD_RESULT_SUCCESS;
    }
#endif  // WHEEL_USE
    return result;
}

/***********************************************************************************************************************
* Function Name: GetMeasureTouchResult
* Description  : Getting the value of result of touch judgement
* Arguments    : uint16_t channel   : Touch result ID
*              : uint16_t * value   : Value of result of touch judgement
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetMeasureTouchResult(uint16_t channel, uint16_t * value)
{
    uint8_t result;

    *value = 0;
    if (g_key_info[g_access_method].key_num > 0)
    {
        if (g_key_info[g_access_method].key_max_group > channel)
        {
            *value = g_key_info[g_access_method].touch_result[channel];
        }
    }
    result = CMD_RESULT_SUCCESS;
    return result;
}

/***********************************************************************************************************************
* Function Name: GetMeasurePrimarySensorCounter
* Description  : Getting the value of Primary sensor counter
* Arguments    : uint16_t channel   : Touch sensor channel number
*              : uint16_t * value   : Value of Primary sensor counter
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetMeasurePrimarySensorCounter(uint16_t channel, uint16_t * value)
{
    uint8_t result;

    *value = 0xffff;
    result = CMD_RESULT_FAILURE;
    if (channel < g_method_info[g_access_method].enable)
    {
        if (g_method_info[g_access_method].type != 0)
        {
        	ctsu_channel_data_mutual_t* filtered_values = all_ctsu_configs[g_access_method]->raw_result;
        	*value = filtered_values[channel].sen_cnt_1;
        }
        result = CMD_RESULT_SUCCESS;
    }
    return result;
}


/***********************************************************************************************************************
* Function Name: GetMeasureSecondarySensorCounter
* Description  : Getting the value of Primary sensor counter
* Arguments    : uint16_t channel   : Touch sensor channel number
*              : uint16_t * value   : Value of Primary sensor counter
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetMeasureSecondarySensorCounter(uint16_t channel, uint16_t * value)
{
    uint8_t result;

    *value = 0xffff;
    result = CMD_RESULT_FAILURE;
    if (channel < g_method_info[g_access_method].enable)
    {
        if (g_method_info[g_access_method].type != 0)
        {
        	ctsu_channel_data_mutual_t* filtered_values = all_ctsu_configs[g_access_method]->raw_result;
        	*value = filtered_values[channel].sen_cnt_2;
        }
        result = CMD_RESULT_SUCCESS;
    }
    return result;
}


/***********************************************************************************************************************
* Function Name: GetMeasurePrimaryReferenceCounter
* Description  : Getting the value of Primary reference counter
* Arguments    : uint16_t channel   : Touch sensor channel number
*              : uint16_t * value   : Value of Primary reference counter
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetMeasurePrimaryReferenceCounter(uint16_t channel, uint16_t * value)
{
    uint8_t result;

    *value = 0xffff;
    result = CMD_RESULT_FAILURE;
    if (channel < g_method_info[g_access_method].enable)
    {
        if (g_method_info[g_access_method].type != 0)
        {
        	ctsu_channel_data_mutual_t* filtered_values = all_ctsu_configs[g_access_method]->raw_result;
        	*value = filtered_values[channel].ref_cnt_1;
        }
        result = CMD_RESULT_SUCCESS;
    }
    return result;
}

/***********************************************************************************************************************
* Function Name: GetMeasureSecondaryReferenceCounter
* Description  : Getting the value of Secondary reference counter
* Arguments    : uint16_t channel   : Touch sensor channel number
*              : uint16_t * value   : Value of Primary reference counter
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetMeasureSecondaryReferenceCounter(uint16_t channel, uint16_t * value)
{
    uint8_t result;

    *value = 0xffff;
    result = CMD_RESULT_FAILURE;
    if (channel < g_method_info[g_access_method].enable)
    {
        if (g_method_info[g_access_method].type != 0)
        {
        	ctsu_channel_data_mutual_t* filtered_values = all_ctsu_configs[g_access_method]->raw_result;
        	*value = filtered_values[channel].ref_cnt_2;
        }
        result = CMD_RESULT_SUCCESS;
    }
    return result;
}

/***********************************************************************************************************************
* Function Name: GetParameterTouchFuncMode
* Description  : Getting the value of Touch sensor function mode flags
* Arguments    : uint16_t * value    : Value of Touch function mode flags
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetParameterTouchFuncMode(uint16_t * value)
{
    *value = 0;
    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: SetParameterTouchFuncMode
* Description  : Set the value to Touch sensor function mode flags
* Arguments    : uint16_t value      : Value to set Touch sensor function mode flags
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t SetParameterTouchFuncMode(uint16_t value)
{
    (void)value;
    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: GetParameterDriftInterval
* Description  : Getting the value of Drift correction interval
* Arguments    : uint16_t * value   : Value of Drift correction interval
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetParameterDriftInterval(uint16_t * value)
{
#if defined(DC_TIMING)
    *value = DC_TIMING;//g_touch_paramter[g_access_method].drift_freq;
#else
    *value = 500;
#endif
    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: SetParameterDriftInterval
* Description  : Set the value to Drift correction interval
* Arguments    : uint16_t value      : Value to set Drift correction interval
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t SetParameterDriftInterval(uint16_t value)
{
    (void)value;

    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: GetParameterMsa
* Description  : Getting the value of MSA
* Arguments    : uint16_t * value   : Value of MSA
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetParameterMsa(uint16_t * value)
{
#if defined(AT_TIMING)
    *value = AT_TIMING;//g_touch_paramter[g_access_method].msa_freq;
#else
    *value = 100;
#endif
    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: SetParameterMsa
* Description  : Set the value to MSA
* Arguments    : uint16_t value      : Value to set MSA
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t SetParameterMsa(uint16_t value)
{
    (void)value;

    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: GetParaAcdToTouch
* Description  : Getting the value of ACD to touch
* Arguments    : uint16_t * value    : Value of ACD to touch
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetParameterAcdToTouch(uint16_t * value)
{
    *value = (1<<CTSU_CFG_FILTER_DEPTH);//g_touch_paramter[g_access_method].touch_freq;
    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: SetParaAcdToTouch
* Description  : Set the value to ACD to Touch
* Arguments    : uint16_t value      : Value to set ACD to Touch
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t SetParameterAcdToTouch(uint16_t value)
{
    (void)value;

    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: GetParameterAcdToNoTouch
* Description  : Getting the value of ACD to No touch
* Arguments    : uint16_t * value    : Value of ACD to No touch
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetParameterAcdToNoTouch(uint16_t * value)
{
    *value = (1<<CTSU_CFG_FILTER_DEPTH);//g_touch_paramter[g_access_method].not_touch_freq;
    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: SetParameterAcdToNoTouch
* Description  : Set the value to ACD to No touch
* Arguments    : uint16_t value    : Value to set ACD to No touch
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t SetParameterAcdToNoTouch(uint16_t value)
{
    (void)value;

    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: GetParameterThreshold
* Description  : Getting the value of threshold
* Arguments    : uint16_t channel   : Touch sensor channel number
*              : uint16_t * value   : Value of Sensor counter value (Count value)
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetParameterThreshold(uint16_t channel, uint16_t * value)
{
    uint8_t result;
    uint8_t index;

    *value = 0xffff;
    result = CMD_RESULT_FAILURE;
    if (channel < g_method_info[g_access_method].enable)
    {
        index = g_touch_key_index[g_access_method].index[channel];
        if (0xff != index)
        {
            *value = all_ctsu_configs[g_access_method]->threshold[index];
        }
        result = CMD_RESULT_SUCCESS;
    }
    return result;
}

/***********************************************************************************************************************
* Function Name: SetParameterThreshold
* Description  : Set the value to threshold
* Arguments    : uint16_t channel   : Touch sensor channel number
*              : uint16_t * value   : Value of Sensor counter value (Count value)
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t SetParameterThreshold(uint16_t channel, uint16_t value)
{
    uint8_t result;
    uint8_t index;

    result = CMD_RESULT_FAILURE;
    if (channel < g_method_info[g_access_method].enable)
    {
        index = g_touch_key_index[g_access_method].index[channel];
        if (0xff != index)
        {
        	all_ctsu_configs[g_access_method]->threshold[index] = value;
        }
        result = CMD_RESULT_SUCCESS;
    }
    return result;
}

/***********************************************************************************************************************
* Function Name: GetParameterHysteresis
* Description  : Getting the value of hysteresis
* Arguments    : uint16_t channel   : Touch sensor channel number
*              : uint16_t * value   : Value of Sensor counter value (Count value)
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetParameterHysteresis(uint16_t channel, uint16_t * value)
{
    uint8_t result;
    uint8_t index;

    *value = 0xffff;
    result = CMD_RESULT_FAILURE;
    if (channel < g_method_info[g_access_method].enable)
    {
        index = g_touch_key_index[g_access_method].index[channel];
        if (0xff != index)
        {
            *value = all_ctsu_configs[g_access_method]->hysteresis[index];
        }
        result = CMD_RESULT_SUCCESS;
    }
    return result;
}

/***********************************************************************************************************************
* Function Name: SetParameterHysteresis
* Description  : Set the value to hysteresis
* Arguments    : uint16_t channel   : Touch sensor channel number
*              : uint16_t * value   : Value of Sensor counter value (Count value)
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t SetParameterHysteresis(uint16_t channel, uint16_t value)
{
    uint8_t result;
    uint8_t index;

    result = CMD_RESULT_FAILURE;
    if (channel < g_method_info[g_access_method].enable)
    {
        index = g_touch_key_index[g_access_method].index[channel];
        if (0xff != index)
        {
        	all_ctsu_configs[g_access_method]->hysteresis[index] = value;
        }
        result = CMD_RESULT_SUCCESS;
    }
    return result;
}

/***********************************************************************************************************************
* Function Name: GetParameterSliderNumber
* Description  : Getting the value of number of slider
* Arguments    : uint16_t * value    : Value of number of slider
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetParameterSliderNumber(uint16_t * value)
{
    *value = 0;
    if (g_method_info[g_access_method].type == 0)
    {
        *value = SLIDER_NUMBER;
    }
    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: GetParameterSliderSensor
* Description  : Gets the touch sensor channel number of the specified slider
* Arguments    : uint16_t channel   : Slider ID
*              : uint16_t * no      : Slider sensor number
*              : uint16_t * value   : Value of Slider sensor numebr
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t    GetParameterSliderSensor(uint16_t channel, uint8_t no, uint16_t * value)
{
    uint8_t    result;
    (void)channel;
    (void)no;
    (void)value;

    result    = CMD_RESULT_FAILURE;
#ifdef    SLIDER_USE
    if (SLIDER_NUMBER > channel)
    {
        result = CMD_RESULT_SUCCESS;
        *value = g_sliderInfo[channel].ts[no];
    }
#endif  // SLIDER_USE
    return result;
}

/***********************************************************************************************************************
* Function Name: GetParameterSliderSensorNumber
* Description  : Getting the value of number of slider touch sensor
* Arguments    : uint16_t channel   : Touch sensor channel number
*              : uint16_t * value   : Value of Sensor counter value (Count value)
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetParameterSliderSensorNumber(uint16_t channel, uint16_t * value)
{
    uint16_t result;
    (void)channel;
    (void)value;

    result = CMD_RESULT_FAILURE;
#ifdef SLIDER_USE
    if (SLIDER_NUMBER > channel)
    {
        *value  = g_sliderInfo[channel].num;
        result  = CMD_RESULT_SUCCESS;
    }
#endif  // SLIDER_USE
    return (uint8_t)result;
}

/***********************************************************************************************************************
* Function Name: GetParameterSliderResolution
* Description  : Getting the value of slider resolution
* Arguments    : uint16_t channel   : Touch sensor channel number
*              : uint16_t * value   : Value of Sensor counter value (Count value)
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetParameterSliderResolution(uint16_t channel, uint16_t * value)
{
    uint8_t result;
    (void)channel;
    (void)value;

    result = CMD_RESULT_FAILURE;
#ifdef  SLIDER_USE
    if (SLIDER_NUMBER > channel)
    {
        *value  = g_sliderInfo[channel].resolution;
        result  = CMD_RESULT_SUCCESS;
    }
#endif  // SLIDER_USE
    return result;
}

/***********************************************************************************************************************
* Function Name: SetParameterSliderResolution
* Description  : Set the value to slider resolution
* Arguments    : uint16_t channel   : Touch sensor channel number
*              : uint16_t * value   : Value of Sensor counter value (Count value)
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t SetParameterSliderResolution(uint16_t channel, uint16_t value)
{
    uint8_t result;
    (void)channel;
    (void)value;

    result = CMD_RESULT_FAILURE;
#ifdef  SLIDER_USE
    if (SLIDER_NUMBER > channel)
    {
        g_sliderInfo[channel].resolution = value;
        result = CMD_RESULT_SUCCESS;
    }
#endif  // SLIDER_USE
    return result;
}

/***********************************************************************************************************************
* Function Name: GetParameterSliderThreshold
* Description  : Getting the value of slider threshold
* Arguments    : uint16_t channel   : Touch sensor channel number
*              : uint16_t * value   : Value of Sensor counter value (Count value)
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetParameterSliderThreshold(uint16_t channel, uint16_t * value)
{
    uint8_t result;
    (void)channel;
    (void)value;

    result = CMD_RESULT_FAILURE;
#ifdef  SLIDER_USE
    if (SLIDER_NUMBER > channel)
    {
        *value  = g_sliderInfo[channel].threshold;
        result  = CMD_RESULT_SUCCESS;
    }
#endif  // SLIDER_USE
    return result;
}

/***********************************************************************************************************************
* Function Name: SetParameterSliderThreshold
* Description  : Set the value to slider threshold
* Arguments    : uint16_t channel   : Touch sensor channel number
*              : uint16_t * value   : Value of Sensor counter value (Count value)
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t SetParameterSliderThreshold(uint16_t channel, uint16_t value)
{
    uint8_t result;
    (void)channel;
    (void)value;

    result = CMD_RESULT_FAILURE;
#ifdef  SLIDER_USE
    if (SLIDER_NUMBER > channel)
    {
        g_sliderInfo[channel].threshold = value;
        result = CMD_RESULT_SUCCESS;
    }
#endif  // SLIDER_USE
    return result;
}

/***********************************************************************************************************************
* Function Name: GetParameterWheelNumber
* Description  : Getting the value of number of wheel
* Arguments    : uint16_t * value    : Value of number of wheel
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetParameterWheelNumber(uint16_t * value)
{
    *value = 0;
    if (g_method_info[g_access_method].type == 0)
    {
        *value = WHEEL_NUMBER;
    }
    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: GetParameterWheelSensor
* Description  : Gets the touch sensor channel number of the specified wheel
* Arguments    : uint16_t channel   : Wheel ID
*              : uint16_t * no      : Wheel sensor number
*              : uint16_t * value   : Value of Wheel sensor numebr
* Return Value : Touch sensor number
***********************************************************************************************************************/
#ifdef    WHEEL_USE
static uint8_t    GetParameterWheelSensor(uint16_t channel, uint8_t no, uint16_t * value)
{
    uint8_t    result;
    (void)channel;
    (void)no;
    (void)value;

    result    = CMD_RESULT_FAILURE;

    if (WHEEL_NUMBER > channel)
    {
        result = CMD_RESULT_SUCCESS;
        *value = g_wheelInfo[channel].ts[no];
    }
    return result;
}
#endif    // WHEEL_USE

/***********************************************************************************************************************
* Function Name: GetParameterWheelSensorNumber
* Description  : Getting the value of number of wheel touch sensor
* Arguments    : uint16_t channel   : Touch sensor channel number
*              : uint16_t * value   : Value of Sensor counter value (Count value)
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetParameterWheelSensorNumber(uint16_t channel, uint16_t * value)
{
    uint16_t result;
    (void)channel;
    (void)value;

    result = CMD_RESULT_FAILURE;
#ifdef  WHEEL_USE
    if (WHEEL_NUMBER > channel)
    {
        *value  = g_wheelInfo[channel].num;
        result  = CMD_RESULT_SUCCESS;
    }
#endif  // WHEEL_USE
    return (uint8_t)result;
}

/***********************************************************************************************************************
* Function Name: GetParameterWheelResolution
* Description  : Getting the value of wheel resolution
* Arguments    : uint16_t channel   : Touch sensor channel number
*              : uint16_t * value   : Value of Sensor counter value (Count value)
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetParameterWheelResolution(uint16_t channel, uint16_t * value)
{
    uint8_t result;
    (void)channel;
    (void)value;

    result = CMD_RESULT_FAILURE;
#ifdef  WHEEL_USE
    if (WHEEL_NUMBER > channel)
    {
        *value  = g_wheelInfo[channel].resolution;
        result  = CMD_RESULT_SUCCESS;
    }
#endif  // WHEEL_USE
    return result;
}

/***********************************************************************************************************************
* Function Name: SetParameterWheelResolution
* Description  : Set the value to wheel resolution
* Arguments    : uint16_t channel   : Touch sensor channel number
*              : uint16_t * value   : Value of Sensor counter value (Count value)
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t SetParameterWheelResolution(uint16_t channel, uint16_t value)
{
    uint8_t result;
    (void)channel;
    (void)value;

    result = CMD_RESULT_FAILURE;
#ifdef  WHEEL_USE
    if (WHEEL_NUMBER > channel)
    {
        g_wheelInfo[channel].resolution = value;
        result = CMD_RESULT_SUCCESS;
    }
#endif  // WHEEL_USE
    return result;
}

/***********************************************************************************************************************
* Function Name: GetParameterWheelThreshold
* Description  : Getting the value of wheel threshold
* Arguments    : uint16_t channel   : Touch sensor channel number
*              : uint16_t * value   : Value of Sensor counter value (Count value)
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetParameterWheelThreshold(uint16_t channel, uint16_t * value)
{
    uint8_t result;
    (void)channel;
    (void)value;

    result = CMD_RESULT_FAILURE;
#ifdef  WHEEL_USE
    if (WHEEL_NUMBER > channel)
    {
        *value  = g_wheelInfo[channel].threshold;
        result  = CMD_RESULT_SUCCESS;
    }
#endif  // WHEEL_USE
    return result;
}

/***********************************************************************************************************************
* Function Name: SetParameterWheelThreshold
* Description  : Set the value to wheel threshold
* Arguments    : uint16_t channel   : Touch sensor channel number
*              : uint16_t * value   : Value of Sensor counter value (Count value)
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t SetParameterWheelThreshold(uint16_t channel, uint16_t value)
{
    uint8_t result;
    (void)channel;
    (void)value;

    result = CMD_RESULT_FAILURE;
#ifdef  WHEEL_USE
    if (WHEEL_NUMBER > channel)
    {
        g_wheelInfo[channel].threshold = value;
        result = CMD_RESULT_SUCCESS;
    }
#endif  // WHEEL_USE
    return result;
}

/***********************************************************************************************************************
* Function Name: GetParameterKeyEnableControl
* Description  : Getting the value of setting of enable of touch key
* Arguments    : uint16_t channel   : Touch sensor channel number
*              : uint16_t * value   : Value of Sensor counter value (Count value)
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetParameterKeyEnableControl(uint16_t channel, uint16_t * value)
{
    uint8_t result;

    result = CMD_RESULT_FAILURE;
    if (g_key_info[g_access_method].key_max_group > channel)
    {
        *value = g_touch_key_group[g_access_method].group[channel];
        result = CMD_RESULT_SUCCESS;
    }
    return result;
}

/***********************************************************************************************************************
* Function Name: GetParameterTouchKeyNumber
* Description  : Getting the value of number of touch keys (Touch button, Matrix button)
* Arguments    : uint16_t * value    : Value of number of touch keys
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetParameterTouchKeyNumber(uint16_t * value)
{
    *value = g_key_info[g_access_method].ena_num;
    return CMD_RESULT_SUCCESS;
}


/***********************************************************************************************************************
* Function Name: GetRegisterCTSUCR0
* Description  : Getting the value of CTSUCR0
* Arguments    : uint16_t * value      : Value to set CTSUCR0
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetRegisterCTSUCR0(uint16_t * value)
{
    *value = (uint16_t)(all_ctsu_configs[g_access_method]->ctsu_settings.CTSUCR0    & 0xFF);

    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: SetRegisterCTSUCR0
* Description  : Set the value to CTSUCR0
* Arguments    : uint16_t value      : Value to set CTSUCR0
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t SetRegisterCTSUCR0(uint16_t value)
{
	all_ctsu_configs[g_access_method]->ctsu_settings.CTSUCR0 = (uint8_t)(value & 0xFF);

    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: GetRegisterCTSUCR1
* Description  : Getting the value of CTSUCR1
* Arguments    : uint16_t * value    : Value of CTSUCR1
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetRegisterCTSUCR1(uint16_t * value)
{
    *value = (uint16_t)(all_ctsu_configs[g_access_method]->ctsu_settings.CTSUCR1 );
    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: SetRegisterCTSUCR1
* Description  : Set the value to CTSUCR1
* Arguments    : uint16_t value      : Value to set CTSUCR1
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t SetRegisterCTSUCR1(uint16_t value)
{
	all_ctsu_configs[g_access_method]->ctsu_settings.CTSUCR1  = (uint8_t)(value & 0xFF);
    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: GetRegisterCTSUSDPRS
* Description  : Getting the value of CTSUSDPRS
* Arguments    : uint16_t * value    : Value of CTSUSDPRS
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetRegisterCTSUSDPRS(uint16_t * value)
{
	*value = (uint16_t)(all_ctsu_configs[g_access_method]->ctsu_settings.CTSUSDPRS  & 0xFF);
    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: SetRegisterCTSUSDPRS
* Description  : Set the value to CTSUSDPRS
* Arguments    : uint16_t value      : Value to set CTSUSDPRS
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t SetRegisterCTSUSDPRS(uint16_t value)
{
	all_ctsu_configs[g_access_method]->ctsu_settings.CTSUSDPRS  = (uint8_t)(value & 0xFF);
	return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: GetRegisterCTSUSST
* Description  : Getting the value of CTSUSST
* Arguments    : uint16_t * value    : Value of CTSUSST
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetRegisterCTSUSST(uint16_t * value)
{
	*value = (uint16_t)(all_ctsu_configs[g_access_method]->ctsu_settings.CTSUSST & 0xFF);
    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: SetRegisterCTSUSST
* Description  : Set the value to CTSUSST
* Arguments    : uint16_t value      : Value to set CTSUSST
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t SetRegisterCTSUSST(uint16_t value)
{
	all_ctsu_configs[g_access_method]->ctsu_settings.CTSUSST = (uint8_t)(value & 0xFF);
    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: GetRegisterCTSUMCH0
* Description  : Getting the value of CTSUMCH0
* Arguments    : uint16_t * value    : Value of CTSUMCH0
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetRegisterCTSUMCH0(uint16_t * value)
{
	*value = (uint16_t)(all_ctsu_configs[g_access_method]->ctsu_settings.CTSUSST & 0xFF);
    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: SetRegisterCTSUMCH0
* Description  : Set the value to CTSUMCH0
* Arguments    : uint16_t value    : Value to set CTSUMCH0
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t SetRegisterCTSUMCH0(uint16_t value)
{
	all_ctsu_configs[g_access_method]->ctsu_settings.CTSUMCH0 = (uint8_t)(value & 0xFF);
    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: GetRegisterCTSUMCH1
* Description  : Getting the value of CTSUMCH1
* Arguments    : uint16_t * value    : Value of CTSUMCH1
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetRegisterCTSUMCH1(uint16_t * value)
{
    *value = (uint16_t)HW_CTSU_CTSUMCH1Get();
    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: SetRegisterCTSUMCH1
* Description  : Set the value to CTSUMCH1
* Arguments    : uint16_t value    : Value to set CTSUMCH1
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t SetRegisterCTSUMCH1(uint16_t value)
{
	HW_CTSU_CTSUMCH1Set((uint8_t)(value & 0xFF));
    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: GetRegisterCTSUCHAC0
* Description  : Getting the value of CTSUCHAC0
* Arguments    : uint16_t * value    : Value of CTSUAC0
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetRegisterCTSUCHAC0(uint16_t * value)
{
	*value = (uint16_t)(all_ctsu_configs[g_access_method]->ctsu_settings.CTSUCHAC0 & 0xFF);;
    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: SetRegisterCTSUCHAC0
* Description  : Set the value to CTSUCHAC0
* Arguments    : uint16_t value      : Value to set CTSUCHAC0
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t SetRegisterCTSUCHAC0(uint16_t value)
{
	all_ctsu_configs[g_access_method]->ctsu_settings.CTSUCHAC0 = (uint8_t)(value & 0xFF);
    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: GetRegisterCTSUCHAC1
* Description  : Getting the value of CTSUCHAC1
* Arguments    : uint16_t * value    : Value of CTSUCHAC1
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetRegisterCTSUCHAC1(uint16_t * value)
{
	*value = (uint16_t)(all_ctsu_configs[g_access_method]->ctsu_settings.CTSUCHAC1 & 0xFF);;
    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: SetRegisterCTSUCHAC1
* Description  : Set the value to CTSUCHAC1
* Arguments    : uint16_t value      : Value to set CTSUCHAC1
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t SetRegisterCTSUCHAC1(uint16_t value)
{
	all_ctsu_configs[g_access_method]->ctsu_settings.CTSUCHAC1 = (uint8_t)(value & 0xFF);
    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: GetRegisterCTSUCHAC2
* Description  : Getting the value of CTSUCHAC2
* Arguments    : uint16_t * value    : Value of CTSUSSC
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetRegisterCTSUCHAC2(uint16_t * value)
{
#if !defined(BSP_MCU_RX113)
	*value = (uint16_t)(all_ctsu_configs[g_access_method]->ctsu_settings.CTSUCHAC2 & 0xFF);
	return CMD_RESULT_SUCCESS;
#else
	*value = 0;
	return CMD_RESULT_SUCCESS;
#endif

}

/***********************************************************************************************************************
* Function Name: SetRegisterCTSUCHAC2
* Description  : Set the value to CTSUCHAC2
* Arguments    : uint16_t value      : Value to set CTSUCHAC2
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t SetRegisterCTSUCHAC2(uint16_t value)
{
#if !defined(BSP_MCU_RX113)
	all_ctsu_configs[g_access_method]->ctsu_settings.CTSUCHAC2 = (uint8_t)(value & 0xFF);
    return CMD_RESULT_SUCCESS;
#else
	return CMD_RESULT_SUCCESS;
#endif
}

/***********************************************************************************************************************
* Function Name: GetRegisterCTSUCHAC3
* Description  : Getting the value of CTSUCHAC3
* Arguments    : uint16_t * value    : Value of CTSUCHAC3
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetRegisterCTSUCHAC3(uint16_t * value)
{
#if !defined(BSP_MCU_RX113)
	*value = (uint16_t)(all_ctsu_configs[g_access_method]->ctsu_settings.CTSUCHAC3 & 0xFF);
    return CMD_RESULT_SUCCESS;
#else
    *value = 0;
	return CMD_RESULT_SUCCESS;;
#endif
}

/***********************************************************************************************************************
* Function Name: SetRegisterCTSUCHAC3
* Description  : Set the value to CTSUCHAC3
* Arguments    : uint16_t value      : Value to set CTSUCHAC3
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t SetRegisterCTSUCHAC3(uint16_t value)
{
#if !defined(BSP_MCU_RX113)
	all_ctsu_configs[g_access_method]->ctsu_settings.CTSUCHAC3 = (uint8_t)(value & 0xFF);
    return CMD_RESULT_SUCCESS;
#else
    return CMD_RESULT_SUCCESS;;
#endif
}

/***********************************************************************************************************************
* Function Name: GetRegisterCTSUCHAC4
* Description  : Getting the value of CTSUCHAC4
* Arguments    : uint16_t * value    : Value of CTSUCHAC4
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetRegisterCTSUCHAC4(uint16_t * value)
{
#if !defined(BSP_MCU_RX113)
	*value = (uint16_t)(all_ctsu_configs[g_access_method]->ctsu_settings.CTSUCHAC4 & 0xFF);
    return CMD_RESULT_SUCCESS;
#else
    *value = 0;
	return CMD_RESULT_SUCCESS;;
#endif
}

/***********************************************************************************************************************
* Function Name: SetRegisterCTSUCHAC4
* Description  : Set the value to CTSUCHAC4
* Arguments    : uint16_t value    : Value to set CTSUCHAC4
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t SetRegisterCTSUCHAC4(uint16_t value)
{
#if !defined(BSP_MCU_RX113)
	all_ctsu_configs[g_access_method]->ctsu_settings.CTSUCHAC4 = (uint8_t)(value & 0x0F);
    return CMD_RESULT_SUCCESS;
#else
	return CMD_RESULT_SUCCESS;;
#endif
}

/***********************************************************************************************************************
* Function Name: GetRegisterCTSUCHTRC0
* Description  : Getting the value of CTSUCHTRC0
* Arguments    : uint16_t * value    : Value of CTSUCHTRC0
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetRegisterCTSUCHTRC0(uint16_t * value)
{
	*value = (uint16_t)(all_ctsu_configs[g_access_method]->ctsu_settings.CTSUCHTRC0 & 0xFF);
    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: SetRegisterCTSUCHTRC0
* Description  : Set the value to CTSUCHTRC0
* Arguments    : uint16_t value      : Value to set CTSUCHTRC0
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t SetRegisterCTSUCHTRC0(uint16_t value)
{
	all_ctsu_configs[g_access_method]->ctsu_settings.CTSUCHTRC0 = (uint8_t)(value & 0xFF);
    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: GetRegisterCTSUCHTRC1
* Description  : Getting the value of CTSUCHTRC1
* Arguments    : uint16_t * value    : Value of CTSUCHTRC1
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetRegisterCTSUCHTRC1(uint16_t * value)
{
	*value = (uint16_t)(all_ctsu_configs[g_access_method]->ctsu_settings.CTSUCHTRC1 & 0xFF);
    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: SetRegisterCTSUCHTRC1
* Description  : Set the value to CTSUCHTRC1
* Arguments    : uint16_t value      : Value to set CTSUCHTRC1
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t SetRegisterCTSUCHTRC1(uint16_t value)
{
	all_ctsu_configs[g_access_method]->ctsu_settings.CTSUCHTRC1 = (uint8_t)(value & 0xFF);
    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: GetRegisterCTSUCHTRC2
* Description  : Getting the value of CTSUCHTRC2
* Arguments    : uint16_t * value    : Value of CTSUCHTRC2
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetRegisterCTSUCHTRC2(uint16_t * value)
{
#if !defined(BSP_MCU_RX113)
	*value = (uint16_t)(all_ctsu_configs[g_access_method]->ctsu_settings.CTSUCHTRC2 & 0xFF);
    return CMD_RESULT_SUCCESS;
#else
    *value = 0;
	return CMD_RESULT_SUCCESS;;
#endif
}

/***********************************************************************************************************************
* Function Name: SetRegisterCTSUCHTRC2
* Description  : Set the value to CTSUCHTRC2
* Arguments    : uint16_t value      : Value to set CTSUCHTRC2
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t SetRegisterCTSUCHTRC2(uint16_t value)
{
#if !defined(BSP_MCU_RX113)
	all_ctsu_configs[g_access_method]->ctsu_settings.CTSUCHTRC2 = (uint8_t)(value & 0xFF);
    return CMD_RESULT_SUCCESS;
#else
	return CMD_RESULT_SUCCESS;;
#endif
}

/***********************************************************************************************************************
* Function Name: GetRegisterCTSUCHTRC3
* Description  : Getting the value of CTSUCHTRC3
* Arguments    : uint16_t * value    : Value of CTSUCHTRC3
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetRegisterCTSUCHTRC3(uint16_t * value)
{
#if !defined(BSP_MCU_RX113)
	*value = (uint16_t)(all_ctsu_configs[g_access_method]->ctsu_settings.CTSUCHTRC3 & 0xFF);
    return CMD_RESULT_SUCCESS;
#else
    *value = 0;
	return CMD_RESULT_SUCCESS;;
#endif
}

/***********************************************************************************************************************
* Function Name: SetRegisterCTSUCHTRC3
* Description  : Set the value to CTSUCHTRC3
* Arguments    : uint16_t value      : Value to set CTSUCHTRC3
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t SetRegisterCTSUCHTRC3(uint16_t value)
{
#if !defined(BSP_MCU_RX113)
	all_ctsu_configs[g_access_method]->ctsu_settings.CTSUCHTRC3 = (uint8_t)(value & 0xFF);
    return CMD_RESULT_SUCCESS;
#else
	return CMD_RESULT_SUCCESS;;
#endif
}

/***********************************************************************************************************************
* Function Name: GetRegisterCTSUCHTRC4
* Description  : Getting the value of CTSUCHTRC4
* Arguments    : uint16_t * value    : Value of CTSUCHTRC4
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetRegisterCTSUCHTRC4(uint16_t * value)
{
#if !defined(BSP_MCU_RX113)
	*value = (uint16_t)(all_ctsu_configs[g_access_method]->ctsu_settings.CTSUCHTRC4 & 0xFF);
    return CMD_RESULT_SUCCESS;
#else
    *value = 0;
	return CMD_RESULT_SUCCESS;;
#endif
}

/***********************************************************************************************************************
* Function Name: SetRegisterCTSUCHTRC4
* Description  : Set the value to CTSUCHTRC4
* Arguments    : uint16_t value      : Value to set CTSUCHTRC4
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t SetRegisterCTSUCHTRC4(uint16_t value)
{
#if !defined(BSP_MCU_RX113)
	all_ctsu_configs[g_access_method]->ctsu_settings.CTSUCHTRC4 = (uint8_t)(value & 0xFF);
    return CMD_RESULT_SUCCESS;
#else
    return CMD_RESULT_SUCCESS;;
#endif
}

/***********************************************************************************************************************
* Function Name: GetRegisterCTSUDCLKC
* Description  : Getting the value of CTSUDCLKC
* Arguments    : uint16_t * value    : Value of CTSUDCLKC
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetRegisterCTSUDCLKC(uint16_t * value)
{
	*value = (uint16_t)(all_ctsu_configs[g_access_method]->ctsu_settings.CTSUDCLKC & 0xFF);;
    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: SetRegisterCTSUDCLKC
* Description  : Set the value to CTSUDCLKC
* Arguments    : uint16_t value    : Value to set CTSUDCLKC
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t SetRegisterCTSUDCLKC(uint16_t value)
{
	all_ctsu_configs[g_access_method]->ctsu_settings.CTSUDCLKC = (uint8_t)(value & 0xFF);
    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: GetRegisterCTSUST
* Description  : Getting the value of CTSUST
* Arguments    : uint16_t * value    : Value of CTSUSSC
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetRegisterCTSUST(uint16_t * value)
{
#if 0
    *value = (uint16_t)((g_ctsu_status[g_access_method].flag.sens_over << 5) +
                        (g_ctsu_status[g_access_method].flag.ref_over  << 6));
#else
	*value = (uint16_t)(HW_CTSU_CTSUSTGet() & 0xFF);
#endif
    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: SetRegisterCTSUST
* Description  : Set the value to CTSUST
* Arguments    : uint16_t value    : Value to set CTSUST
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t SetRegisterCTSUST(uint16_t value)
{
    (void)value;
    return CMD_RESULT_FAILURE;
}

/***********************************************************************************************************************
* Function Name: GetRegisterCTSUSSC
* Description  : Getting the value of CTSUSSC
* Arguments    : uint16_t channel    : Touch sensor channel number
*              : uint16_t * value     : Value of CTSUSSC
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetRegisterCTSUSSC(uint16_t channel, uint16_t * value)
{
    uint8_t result;

    result = CMD_RESULT_FAILURE;
    if (channel < g_method_info[g_access_method].enable)
    {
        if (g_method_info[g_access_method].type == 0)
        {   /* Touch sensor measurement method : Self capacitance */
        	uint8_t index = g_key_info[g_access_method].sensor_index[channel];
            if (index != 0xff)
            {
            	R_CTSU_Control(CTSU_CMD_RD_CH_SET, all_ctsu_configs[g_access_method]->write_settings);
            	ctsu_channel_setting_t* ch_setting = all_ctsu_configs[g_access_method]->write_settings;
                *value = ch_setting[index].ctsussc;
            }
        }
        else
        {   /* Touch sensor measurement method : Mutual capacitance */
        	uint8_t index = g_key_info[g_access_method].sensor_index[channel];
            if (index != 0xff)
            {
            	R_CTSU_Control(CTSU_CMD_RD_CH_SET, all_ctsu_configs[g_access_method]->write_settings);
            	ctsu_channel_setting_t* ch_setting = all_ctsu_configs[g_access_method]->write_settings;
            	*value = ch_setting[index].ctsussc;
            }
        }
        result = CMD_RESULT_SUCCESS;
    }
    return result;
}

/***********************************************************************************************************************
* Function Name: SetRegisterCTSUSSC
* Description  : Set the value to CTSUSSC
* Arguments    : uint16_t channel  : Touch sensor channel number
*              : uint16_t value    : Value to set CTSUSSC
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t SetRegisterCTSUSSC(uint16_t channel, uint16_t value)
{
    uint8_t result;

    result = CMD_RESULT_FAILURE;
    if (channel < g_method_info[g_access_method].enable)
    {
        if (g_method_info[g_access_method].type == 0)
        {   /* Touch sensor measurement method : Self capacitance */
        	uint8_t index = g_key_info[g_access_method].sensor_index[channel];
            if (index != 0xff)
            {
            	ctsu_channel_setting_t* ch_setting = all_ctsu_configs[g_access_method]->write_settings;
            	ch_setting[index].ctsussc = value;
            	R_CTSU_Control(CTSU_CMD_WR_CH_SET, all_ctsu_configs[g_access_method]->write_settings);
            }
        }
        else
        {   /* Touch sensor measurement method : Mutual capacitance */
        	uint8_t index = g_key_info[g_access_method].sensor_index[channel];
            if (index != 0xff)
            {
            	ctsu_channel_setting_t* ch_setting = all_ctsu_configs[g_access_method]->write_settings;
            	ch_setting[index].ctsussc = value;
            	R_CTSU_Control(CTSU_CMD_WR_CH_SET, all_ctsu_configs[g_access_method]->write_settings);
            }
        }
        result = CMD_RESULT_SUCCESS;
    }
    return result;
}

/***********************************************************************************************************************
* Function Name: GetRegisterCTSUSO0
* Description  : Getting the value of CTSUSO0
* Arguments    : uint16_t channel    : Touch sensor channel number
*              : uint16_t value      : Value of CTSUSO0
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetRegisterCTSUSO0(uint16_t channel, uint16_t * value)
{
    uint8_t result;

    result = CMD_RESULT_FAILURE;
    if (channel < g_method_info[g_access_method].enable)
    {
        if (g_method_info[g_access_method].type == 0)
        {   /* Touch sensor measurement method : Self capacitance */
        	uint8_t index = g_key_info[g_access_method].sensor_index[channel];
            if (index != 0xff)
            {
            	R_CTSU_Control(CTSU_CMD_RD_CH_SET, all_ctsu_configs[g_access_method]->write_settings);
            	ctsu_channel_setting_t* ch_setting = all_ctsu_configs[g_access_method]->write_settings;
            	*value = ch_setting[index].ctsuso0;
            }
        }
        else
        {   /* Touch sensor measurement method : Mutual capacitance */
        	uint8_t index = g_key_info[g_access_method].sensor_index[channel];
            if (index != 0xff)
            {
            	R_CTSU_Control(CTSU_CMD_RD_CH_SET, all_ctsu_configs[g_access_method]->write_settings);
            	ctsu_channel_setting_t* ch_setting = all_ctsu_configs[g_access_method]->write_settings;
            	*value = ch_setting[index].ctsuso0;
            }
        }
        result = CMD_RESULT_SUCCESS;
    }
    return result;
}

/***********************************************************************************************************************
* Function Name: SetRegisterCTSUSO0
* Description  : Set the value to CTSUSO0
* Arguments    : uint16_t channel    : Touch sensor channel number
*              : uint16_t value      : Value to set CTSUSO0
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t SetRegisterCTSUSO0(uint16_t channel, uint16_t value)
{
    uint8_t result;

    result = CMD_RESULT_FAILURE;
    if (channel < g_method_info[g_access_method].enable)
    {
        if (g_method_info[g_access_method].type == 0)
        {   /* Touch sensor measurement method : Self capacitance */
        	uint8_t index = g_key_info[g_access_method].sensor_index[channel];
            if (index != 0xff)
            {
            	ctsu_channel_setting_t* ch_setting = all_ctsu_configs[g_access_method]->write_settings;
            	ch_setting[index].ctsuso0 = value;
            	R_CTSU_Control(CTSU_CMD_WR_CH_SET, all_ctsu_configs[g_access_method]->write_settings);
            }
        }
        else
        {   /* Touch sensor measurement method : Mutual capacitance */
        	uint8_t index = g_key_info[g_access_method].sensor_index[channel];
            if (index != 0xff)
            {
            	ctsu_channel_setting_t* ch_setting = all_ctsu_configs[g_access_method]->write_settings;
            	ch_setting[index].ctsuso0 = value;
            	R_CTSU_Control(CTSU_CMD_WR_CH_SET, all_ctsu_configs[g_access_method]->write_settings);
            }
        }
        result = CMD_RESULT_SUCCESS;
    }
    return result;
}

/***********************************************************************************************************************
* Function Name: GetRegisterCTSUSO1
* Description  : Getting the value of CTSUSO1
* Arguments    : uint16_t channel    : Touch sensor channel number
*              : uint16_t value      : Value to set CTSUERRS
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetRegisterCTSUSO1(uint16_t channel, uint16_t * value)
{
    uint8_t result;

    result = CMD_RESULT_FAILURE;
    if (channel < g_method_info[g_access_method].enable)
    {
        if (g_method_info[g_access_method].type == 0)
        {   /* Touch sensor measurement method : Self capacitance */
        	uint8_t index = g_key_info[g_access_method].sensor_index[channel];
            if (index != 0xff)
            {
            	R_CTSU_Control(CTSU_CMD_RD_CH_SET, all_ctsu_configs[g_access_method]->write_settings);
            	ctsu_channel_setting_t* ch_setting = all_ctsu_configs[g_access_method]->write_settings;
            	*value = ch_setting[index].ctsuso1;
            }
        }
        else
        {   /* Touch sensor measurement method : Mutual capacitance */
        	uint8_t index = g_key_info[g_access_method].sensor_index[channel];
            if (index != 0xff)
            {
            	R_CTSU_Control(CTSU_CMD_RD_CH_SET, all_ctsu_configs[g_access_method]->write_settings);
            	ctsu_channel_setting_t* ch_setting = all_ctsu_configs[g_access_method]->write_settings;
            	*value = ch_setting[index].ctsuso1;
            }
        }
        result = CMD_RESULT_SUCCESS;
    }
    return result;
}

/***********************************************************************************************************************
* Function Name: SetRegisterCTSUSO1
* Description  : Set the value to CTSUSO1
* Arguments    : uint16_t channel  : Touch sensor channel number
*              : uint16_t value    : Value to set CTSUSO1
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t SetRegisterCTSUSO1(uint16_t channel, uint16_t value)
{
    uint8_t result;

    result = CMD_RESULT_FAILURE;
    if (channel < g_method_info[g_access_method].enable)
    {
        if (g_method_info[g_access_method].type == 0)
        {   /* Touch sensor measurement method : Self capacitance */
        	uint8_t index = g_key_info[g_access_method].sensor_index[channel];
            if (index != 0xff)
            {
            	ctsu_channel_setting_t* ch_setting = all_ctsu_configs[g_access_method]->write_settings;
            	ch_setting[index].ctsuso1 = value;
            	R_CTSU_Control(CTSU_CMD_WR_CH_SET, all_ctsu_configs[g_access_method]->write_settings);
            }
        }
        else
        {   /* Touch sensor measurement method : Mutual capacitance */
        	uint8_t index = g_key_info[g_access_method].sensor_index[channel];
            if (index != 0xff)
            {
            	ctsu_channel_setting_t* ch_setting = all_ctsu_configs[g_access_method]->write_settings;
            	ch_setting[index].ctsuso1 = value;
            	R_CTSU_Control(CTSU_CMD_WR_CH_SET, all_ctsu_configs[g_access_method]->write_settings);
            }
        }
        result = CMD_RESULT_SUCCESS;
    }
    return result;
}

/***********************************************************************************************************************
* Function Name: GetRegisterCTSUERRS
* Description  : Getting the value of CTSUERRS
* Arguments    : uint16_t * value    : Value to set CTSUERRS
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetRegisterCTSUERRS(uint16_t * value)
{
    *value = (uint16_t)(g_ctsu_status[g_access_method].flag.icomp_error << 15);
    return CMD_RESULT_SUCCESS;
}

/***********************************************************************************************************************
* Function Name: SetRegisterCTSUERRS
* Description  : Set the value to CTSUERRS
* Arguments    : uint16_t value    : Value to set CTSUERRS
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t SetRegisterCTSUERRS(uint16_t value)
{
    (void)value;
    return CMD_RESULT_FAILURE;
}

/***********************************************************************************************************************
* Function Name: GetUtilityExecuteBatch
* Description  : Get the value of batch command execution
* Arguments    : uint16_t * value    : Value to set the value of the
* Return Value : CMD_RESULT_SUCCESS : success
*              : CMD_RESULT_FAILURE : failure
***********************************************************************************************************************/
static uint8_t GetUtilityExecuteBatch(uint8_t * pvalue, uint16_t * length)
{
    uint8_t     i;
    uint16_t    index;
    uint8_t     startMethod;
    uint8_t     stopMethod;
    uint8_t     sensor;
    uint8_t     endSensor;
    uint8_t     cmd;
    uint8_t     method;
    uint16_t    value;
    uint8_t     status;

    index = 0;
    status    = CMD_RESULT_SUCCESS;
    for (i = 0; (i < monitor_command.size) && (status != CMD_RESULT_FAILURE); )
    {
        cmd         = monitor_command.command[i++];
        startMethod = monitor_command.command[i++];
        stopMethod  = monitor_command.command[i++];

        for (method = startMethod; (method < stopMethod) && (status != CMD_RESULT_FAILURE); method++) {
            if (cmd == 0x06/* RSLT0 */        ||
                cmd == 0x07/* RSLT1 */        ||
                cmd == 0x08/* RSLT2 */        ||
                cmd == 0x09/* RSLT3 */)
            {
                endSensor = 1;
            }
            else
            {
                endSensor     = g_method_info[method].enable;
                if (cmd == 0x04/* SLDPOS */)
                {
                    endSensor = SLIDER_NUMBER;
                }
                else if (cmd == 0x05/* WHLPOS */)
                {
                    endSensor = WHEEL_NUMBER;
                }
            }
            g_access_method = method;
            for (sensor = 0; (sensor < endSensor) && (status != CMD_RESULT_FAILURE); sensor++)
            {
                if (GetSensorValue(cmd, sensor, &value) != CMD_RESULT_FAILURE)
                {
                    pvalue[index++] = (uint8_t)(value & 0xff);
                    pvalue[index++] = (uint8_t)(value >> 8);
                }
                else
                {
                    status    = CMD_RESULT_FAILURE;
                }
            }
        }
    }
    // Set buffer size to output parameter
    *length = index;
    return status;
}

/***********************************************************************************************************************
* Function Name: CreateResponceCommand
* Description  : Create responce command
* Arguments    : pcmd -
*                    Pointer of command
* Return Value : none
***********************************************************************************************************************/
static void CreateResponceCommand(com_data_tx_t *pcmd)
{
    pcmd->fmt.main = (uint8_t)(com_data.fmt.main | CMD_RE);
    pcmd->fmt.sub  = com_data.fmt.sub;

    switch (com_data.fmt.main)
    {
        case CMD_RQ_PROFILE_READ:
            SensorProfileReadResponse(pcmd, com_data.fmt.data[0]);
            break;
        case CMD_RQ_PROFILE_WRITE:
            break;
        case CMD_RQ_MEASURE_READ:
            SensorMeasureReadResponse(pcmd, com_data.fmt.data[0]);
            break;
        case CMD_RQ_PARAMETER_READ:
            SensorParameterReadResponse(pcmd, com_data.fmt.data[0]);
            break;
        case CMD_RQ_PARAMETER_WRITE:
            SensorParameterWriteResponse(pcmd, com_data.fmt.data[2]);
            break;
        case CMD_RQ_REGISTER_READ:
            SensorRegisterReadResponse(pcmd, com_data.fmt.data[0]);
            break;
        case CMD_RQ_REGISTER_WRITE:
            SensorRegisterWriteResponse(pcmd, com_data.fmt.data[2]);
            break;
        case CMD_RQ_UTILITY_READ:
            SensorUtilityReadResponse(pcmd, com_data.fmt.data[0]);
            break;
        case CMD_RQ_UTILITY_WRITE:
            SensorUtilityWriteResponse(pcmd, com_data.fmt.data[2]);
            break;
        default:
            pcmd->fmt.size = 1;
            pcmd->fmt.data[0] = CMD_RESULT_FAILURE;
            break;
    }

    pcmd->fmt.sum = SetChecksum(pcmd->fmt.main, pcmd->fmt.sub, pcmd->fmt.size, pcmd->fmt.data);
}

/***********************************************************************************************************************
* Function Name: SetChecksum
* Description  : Set checksum
* Arguments    : main -
*                    main command id
*                sub -
*                    sub command id
*                size -
*                    size of data
*                pdata -
*                    Pointer of data buffer
* Return Value : sum -
*                    checksum
***********************************************************************************************************************/
static uint8_t SetChecksum(uint8_t main, uint8_t sub, uint8_t size, uint8_t *pdata)
{
    uint8_t sum;
    uint8_t i;

    sum = (uint8_t)(main + sub + size);
    for (i = 0; i < size; i++)
    {
        sum = (uint8_t)(sum + pdata[i]);
    }

    return sum;
}

/***********************************************************************************************************************
* Function Name: IsRightChecksum
* Description  : Right checksum
* Arguments    : pcmd -
*                    Pointer of received data
* Return Value : result -
*                    TRUE , FALSE
***********************************************************************************************************************/
static BOOL IsRightChecksum(com_data_rd_t *pcmd)
{
    BOOL result;
    uint8_t sum;

    sum = SetChecksum(pcmd->fmt.main, pcmd->fmt.sub, pcmd->fmt.size, pcmd->fmt.data);
    result = FALSE;
    if (pcmd->fmt.sum == sum)
    {
        result = TRUE;
    }
    return result;
}


/***********************************************************************************************************************
* Function Name: GetSensorValue
* Description  : Get sensor value
* Arguments    : code - 
*                    Where to start looking
*                channel - 
*                    channel data
*                pval - 
*                    Pointer of serial output data
* Return Value : result - 
*                    TRUE , FALSE
***********************************************************************************************************************/
static uint8_t GetSensorValue(uint8_t code, uint16_t channel, uint16_t *pval)
{
    uint8_t result;

    switch (code)
    {
        case 0x00:    // SC
            result = GetMeasureSensorCounter(channel, pval);
            break;
        case 0x01:    // RV
            result = GetMeasureReferenceValue(channel, pval);
            break;
        case 0x02:    // RC
            result = GetMeasureReferenceCounter(channel, pval);
            break;
        case 0x04:    // SDLPOS
            result = GetMeasureSliderPosition(channel, pval);
            break;
        case 0x05:
            result = GetMeasureWheelPosition(channel, pval);
            break;
        case 0x06: // Touch sensor On/Off judgement 1 (RSLT0)
            result = GetMeasureTouchResult(0, pval);
            break;
        case 0x07: // Touch sensor On/Off judgement 2 (RSLT1)
            result = GetMeasureTouchResult(1, pval);
            break;
        case 0x08: // Touch sensor On/Off judgement 3 (RSLT2)
            result = GetMeasureTouchResult(2, pval);
            break;
        case 0x09: // Touch sensor On/Off judgement 4 (RSLT3)
            result = GetMeasureTouchResult(3, pval);
            break;
        case 0x0a:    // SCFRST (Sensor counter at 1st measurement)
            result = GetMeasurePrimarySensorCounter(channel, pval);
            break;
        case 0x0b:    // SCSCND (Sensor counte at 2nd measurement)
            result = GetMeasureSecondarySensorCounter(channel, pval);
            break;
        case 0x0c:    // RCFRST (Sensor counter at 1st measurement)
            result = GetMeasurePrimaryReferenceCounter(channel, pval);
            break;
        case 0x0d:    // RCSCND (Sensor counte at 2nd measurement)
            result = GetMeasureSecondaryReferenceCounter(channel, pval);
            break;            
        default:
            result = CMD_RESULT_FAILURE;
            break;
    }
    return result;
}

/***********************************************************************************************************************
* Function Name: SensorProfileReadResponse
* Description  : Sensor read response
* Arguments    : pcmd - 
*                    Pointer of transfer data
*                channel - 
*                    channel data
* Return Value : none
***********************************************************************************************************************/
static void SensorProfileReadResponse(com_data_tx_t *pcmd, uint16_t channel)
{
    uint16_t value;
    uint8_t status;

    (void)channel;

    pcmd->fmt.size    = 3;
    pcmd->fmt.data[0] = CMD_RESULT_FAILURE;    /* Error status */
    pcmd->fmt.data[1] = 0;
    pcmd->fmt.data[2] = 0;

    status = CMD_RESULT_SUCCESS;
    value  = 0;
    switch (pcmd->fmt.sub)
    {
        case 0x00:
            /* ID */
            value = DF_CHIP_ID;
            break;
        case 0x01:
            /* VER1(Version Lower) */
            value = DF_VERSIONu;
            break;
        case 0x02:
            /* VER2(Version Upper) */
            value = DF_VERSIONd;
            break;
        case 0x03:
            /* PROFILE */
            value = DF_PROFILE;
            break;
        case 0x04:
            /* MCU model name 0 */
            value =  (uint16_t)((uint16_t)g_mcu_model_name[0] + ((uint16_t)g_mcu_model_name[1] << 8));
            break;
        case 0x05:
            /* MCU model name 1 */
            value =  (uint16_t)((uint16_t)g_mcu_model_name[2] + ((uint16_t)g_mcu_model_name[3] << 8));
            break;
        case 0x06:
            /* MCU model name 2 */
            value =  (uint16_t)((uint16_t)g_mcu_model_name[4] + ((uint16_t)g_mcu_model_name[5] << 8));
            break;
        case 0x07:
            /* MCU model name 3 */
            value =  (uint16_t)((uint16_t)g_mcu_model_name[6] + ((uint16_t)g_mcu_model_name[7] << 8));
            break;
        case 0x08:
            /* MCU model name 4 */
            value =  (uint16_t)((uint16_t)g_mcu_model_name[8] + ((uint16_t)g_mcu_model_name[9] << 8));
            break;
        case 0x09:
            /* MCU model name 5 */
            value =  (uint16_t)((uint16_t)g_mcu_model_name[10] + ((uint16_t)g_mcu_model_name[11] << 8));
            break;
        case 0x0a:
            /* MCU model name 6 */
            value = (uint16_t)((uint16_t)g_mcu_model_name[12] + ((uint16_t)g_mcu_model_name[13] << 8));
            break;
        case 0x0b:
            /* MCU model name 7 */
            value =  (uint16_t)((uint16_t)g_mcu_model_name[14] + ((uint16_t)g_mcu_model_name[15] << 8));
            break;
        case 0x0c:
            /* Date information 0 */
            value =  (uint16_t)CREATE_SOURCE_DATE0;
            break;
        case 0x0d:
            /* Date information 1 */
            value =  (uint16_t)CREATE_SOURCE_DATE1;
            break;
        case 0x0e:
            /* Date information 2 */
            value =  (uint16_t)CREATE_SOURCE_DATE2;
            break;
        case 0x0f:
            /* Date information 3 */
            value =  (uint16_t)CREATE_SOURCE_DATE3;
            break;
        default:
            status = CMD_RESULT_FAILURE;
            break;
    }
    pcmd->fmt.data[0] = status;
    if (CMD_RESULT_FAILURE != status)
    {
        /* Normal status */
        pcmd->fmt.data[1] = (uint8_t)(value & 0xff);
        pcmd->fmt.data[2] = (uint8_t)(value >> 8);
    }
}

/***********************************************************************************************************************
* Function Name: SensorMeasureReadResponse
* Description  : Sensor write response
* Arguments    : pcmd - 
*                    Pointer of transfer data
*                channel - 
*                    channel data
* Return Value : none
***********************************************************************************************************************/
static void SensorMeasureReadResponse(com_data_tx_t *pcmd, uint16_t channel)
{
    uint16_t value;

    pcmd->fmt.size    = 1;
    pcmd->fmt.data[0] = CMD_RESULT_FAILURE;    /* Error status */

    if (GetSensorValue(pcmd->fmt.sub, channel, &value) != CMD_RESULT_FAILURE)
    {
        pcmd->fmt.size = 3;
        pcmd->fmt.data[0] = CMD_RESULT_SUCCESS;
        pcmd->fmt.data[1] = (uint8_t)(value & 0xff);
        pcmd->fmt.data[2] = (uint8_t)(value >> 8);
    }
}

/***********************************************************************************************************************
* Function Name: SensorParameterReadResponse
* Description  : State response
* Arguments    : pcmd - 
*                    Pointer of transfer data
* Return Value : none
***********************************************************************************************************************/
static void SensorParameterReadResponse(com_data_tx_t * pcmd, uint16_t channel)
{
    uint16_t value;
    uint8_t  status;

    pcmd->fmt.size    = 1;
    pcmd->fmt.data[0] = CMD_RESULT_FAILURE;    /* Error status */
    value             = 0;
    status            = CMD_RESULT_SUCCESS;

    switch (pcmd->fmt.sub)
    {
        case 0x00:
            status = GetParameterTouchFuncMode(&value);
            break;
        case 0x01:
            status = GetParameterDriftInterval(&value);
            break;
        case 0x02:
            status = GetParameterMsa(&value);
            break;
        case 0x03:
            status = GetParameterAcdToTouch(&value);
            break;
        case 0x04:
            status = GetParameterAcdToNoTouch(&value);
            break;
        case 0x05:
        case 0x06:
        case 0x07:
            status = CMD_RESULT_SUCCESS;
            break;
        case 0x08:
            status = GetParameterThreshold(channel, &value);
            break;
        case 0x09:
            status = GetParameterHysteresis(channel, &value);
            break;
        case 0x0a:
            status = GetParameterSliderNumber(&value);
            break;
        case 0x0b:
            status = GetParameterSliderSensorNumber(channel, &value);
            break;
        case 0x0c:
            status  = GetParameterSliderSensor(channel, 0, &value);
            break;
        case 0x0d:
            status  = GetParameterSliderSensor(channel, 1, &value);
            break;
        case 0x0e:
            status  = GetParameterSliderSensor(channel, 2, &value);
            break;
        case 0x0f:
            status  = GetParameterSliderSensor(channel, 3, &value);
            break;
        case 0x10:
            status  = GetParameterSliderSensor(channel, 4, &value);
            break;
        case 0x11:
            status  = GetParameterSliderSensor(channel, 5, &value);
            break;
        case 0x12:
            status  = GetParameterSliderSensor(channel, 6, &value);
            break;
        case 0x13:
            status  = GetParameterSliderSensor(channel, 7, &value);
            break;
        case 0x14:
            status  = GetParameterSliderSensor(channel, 8, &value);
            break;
        case 0x15:
            status  = GetParameterSliderSensor(channel, 9, &value);
            break;
        case 0x16:
            status = GetParameterSliderResolution(channel, &value);
            break;
        case 0x17:
            status = GetParameterSliderThreshold(channel, &value);
            break;
        case 0x18:
            status = GetParameterWheelNumber(&value);
            break;
        case 0x19:
            status = GetParameterWheelSensorNumber(channel, &value);
            break;
#if defined(WHEEL_USE) && (WHEEL_NUMBER > 0)
        case 0x1a:
            status = GetParameterWheelSensor(channel, 0, &value);
            break;
#endif
#if defined(WHEEL_USE) && (WHEEL_NUMBER > 1)
        case 0x1b:
            status = GetParameterWheelSensor(channel, 1, &value);
            break;
#endif
#if defined(WHEEL_USE) && (WHEEL_NUMBER > 2)
        case 0x1c:
            status = GetParameterWheelSensor(channel, 2, &value);
            break;
#endif
#if defined(WHEEL_USE) && defined(WHEEL_NUMBER) && (WHEEL_NUMBER > 3)
        case 0x1d:
            status = GetParameterWheelSensor(channel, 3, &value);
            break;
#endif
#if defined(WHEEL_USE) && (WHEEL_NUMBER > 4)
        case 0x1e:
            status = GetParameterWheelSensor(channel, 4, &value);
            break;
#endif
#if defined(WHEEL_USE) && (WHEEL_NUMBER > 5)
        case 0x1f:
            status = GetParameterWheelSensor(channel, 5, &value);
            break;
#endif
#if defined(WHEEL_USE) && (WHEEL_NUMBER > 6)
        case 0x20:
            status = GetParameterWheelSensor(channel, 6, &value);
            break;
#endif
#if defined(WHEEL_USE) && (WHEEL_NUMBER > 7)
        case 0x21:
            status = GetParameterWheelSensor(channel, 7, &value);
            break;
#endif
#if defined(WHEEL_USE) && (WHEEL_NUMBER > 8)
        case 0x22:
            status = GetParameterWheelSensor(channel, 8, &value);
            break;
#endif
#if defined(WHEEL_USE) && (WHEEL_NUMBER > 9)
        case 0x23:
            status = GetParameterWheelSensor(channel, 9, &value);
            break;
#endif
        case 0x24:
            status = GetParameterWheelResolution(channel, &value);
            break;
        case 0x25:
            status = GetParameterWheelThreshold(channel, &value);
            break;
        case 0x28:
            status = GetParameterKeyEnableControl(0, &value);
            break;
        case 0x29:
            status = GetParameterKeyEnableControl(1, &value);
            break;
        case 0x2a:
            status = GetParameterKeyEnableControl(2, &value);
            break;
        case 0x2b:
            status = GetParameterKeyEnableControl(3, &value);
            break;
        case 0x2c:
            status = GetParameterTouchKeyNumber(&value);
            break;
        default:
            status  = CMD_RESULT_FAILURE;
            break;
    }

    if (status == CMD_RESULT_SUCCESS)
    {
        pcmd->fmt.size        = 3;
        pcmd->fmt.data[0]    = status;
        pcmd->fmt.data[1]    = (uint8_t)(value & 0xff);
        pcmd->fmt.data[2]    = (uint8_t)(value >> 8);
    }
}

/***********************************************************************************************************************
* Function Name: SensorParameterWriteResponse
* Description  : Process for the write command of Parameter 
* Arguments    : pcmd - Pointer of transfer data
*                channel - touch sensor number
* Return Value : none
***********************************************************************************************************************/
static void SensorParameterWriteResponse(com_data_tx_t * pcmd, uint16_t channel)
{
    uint16_t value;
    uint8_t  status;

    pcmd->fmt.size = 1;
    status  = CMD_RESULT_FAILURE;

    value = (uint16_t)(((uint16_t)com_data.fmt.data[1] << 8) + com_data.fmt.data[0]);

    switch (pcmd->fmt.sub)
    {
        case 0x00:
            status = SetParameterTouchFuncMode(value);
            break;
        case 0x01:
            status = SetParameterDriftInterval(value);
            break;
        case 0x02:
            status = SetParameterMsa(value);
            break;
        case 0x03:
            status = SetParameterAcdToTouch(value);
            break;
        case 0x04:
            status = SetParameterAcdToNoTouch(value);
            break;
        case 0x05:
        case 0x06:
        case 0x07:
            status = CMD_RESULT_SUCCESS;
            break;
        case 0x08:
            status = SetParameterThreshold(channel, value);
            break;
        case 0x09:
            status = SetParameterHysteresis(channel, value);
            break;
        case 0x16:
            status = SetParameterSliderResolution(channel, value);
            break;
        case 0x17:
            status = SetParameterSliderThreshold(channel, value);
            break;
        case 0x24:
            status = SetParameterWheelResolution(channel, value);
            break;
        case 0x25:
            status = SetParameterWheelThreshold(channel, value);
            break;
        default:
            status = CMD_RESULT_FAILURE;
            break;
    }
    pcmd->fmt.data[0] = status;
}

/***********************************************************************************************************************
* Function Name: SensorRegisterReadResponse
* Description  : Control response
* Arguments    : pcmd - 
*                    Pointer of transfer data
* Return Value : none
***********************************************************************************************************************/
static void SensorRegisterReadResponse(com_data_tx_t * pcmd, uint16_t channel)
{
    uint16_t value;
    uint8_t  status;

    value = 0;

    switch (pcmd->fmt.sub)
    {
        case 0x00:    // CR0
            status = GetRegisterCTSUCR0(&value);
            break;
        case 0x01:    // CR1
            status = GetRegisterCTSUCR1(&value);
            break;
        case 0x02:    // SDPRS
            status = GetRegisterCTSUSDPRS(&value);
            break;
        case 0x03:    // SST
            status = GetRegisterCTSUSST(&value);
            break;
        case 0x04:    // MCH0
            status = GetRegisterCTSUMCH0(&value);
            break;
        case 0x05:    // MCH1
            status = GetRegisterCTSUMCH1(&value);
            break;
        case 0x06:    // CHAC0
            status = GetRegisterCTSUCHAC0(&value);
            break;
        case 0x07:    // CHAC1
            status = GetRegisterCTSUCHAC1(&value);
            break;
        case 0x08:    // CHAC2
            status = GetRegisterCTSUCHAC2(&value);
            break;
        case 0x09:    // CHAC3
            status = GetRegisterCTSUCHAC3(&value);
            break;
        case 0x0a:    // CHAC4
            status = GetRegisterCTSUCHAC4(&value);
            break;
        case 0x0b:    // CHTRC0
            status = GetRegisterCTSUCHTRC0(&value);
            break;
        case 0x0c:    // CHTRC1
            status = GetRegisterCTSUCHTRC1(&value);
            break;
        case 0x0d:    // CHTRC2
            status = GetRegisterCTSUCHTRC2(&value);
            break;
        case 0x0e:    // CHTRC3
            status = GetRegisterCTSUCHTRC3(&value);
            break;
        case 0x0f:    // CHTRC4
            status = GetRegisterCTSUCHTRC4(&value);
            break;
        case 0x10:    // DCLKC
            status = GetRegisterCTSUDCLKC(&value);
            break;
        case 0x11:    // ST
            status = GetRegisterCTSUST(&value);
            break;
        case 0x12:    // SSC
            status = GetRegisterCTSUSSC(channel, &value);
            break;
        case 0x13:    // SO0
            status = GetRegisterCTSUSO0(channel, &value);
            break;
        case 0x14:    // SO1
            status = GetRegisterCTSUSO1(channel, &value);
            break;
        case 0x15: // CTSUSC - No support command
        case 0x16: // CTSURC - No support command
            status = CMD_RESULT_FAILURE;
            break;
        case 0x17:    // ERRS
            status = GetRegisterCTSUERRS(&value);
            break;
        default:
            status = CMD_RESULT_FAILURE;
            break;
    }

    pcmd->fmt.data[0] = status; /* Set command status */
    pcmd->fmt.size    = 1;      /* Set size of data buffer */
    if (status != CMD_RESULT_FAILURE)
    {
        pcmd->fmt.size    = 3;  /* Set size of data buffer */
        pcmd->fmt.data[1] = (uint8_t)(value & 0xff);
        pcmd->fmt.data[2] = (uint8_t)(value >> 8);
    }
}

/***********************************************************************************************************************
* Function Name: SensorRegisterWriteResponse
* Description  : Process for the write command of Register
* Arguments    : pcmd - Pointer of transfer data
*                channel - touch sensor number
* Return Value : none
***********************************************************************************************************************/
ctsu_functions_t no_user_funcs = {0};
static void SensorRegisterWriteResponse(com_data_tx_t * pcmd, uint16_t channel)
{
    uint16_t value;
    uint8_t  status;

    /* Wait for scan to complete */
    while(0 != HW_CTSU_CTSUSTGetBitCTSUSTC());


    /* Set the value to write CTSU register */
    value = (uint16_t)(((uint16_t)com_data.fmt.data[1] << 8) + com_data.fmt.data[0]);

    switch (pcmd->fmt.sub)
    {
        case 0x00:    // CR0
            status = SetRegisterCTSUCR0(value);
            break;
        case 0x01:    // CR1
            status = SetRegisterCTSUCR1(value);
            break;
        case 0x02:    // SDPRS
            status = SetRegisterCTSUSDPRS(value);
            break;
        case 0x03:    // SST
            status = SetRegisterCTSUSST(value);
            break;
        case 0x04:    // MCH0
            status = SetRegisterCTSUMCH0(value);
            break;
        case 0x05:    // MCH1
            status = SetRegisterCTSUMCH1(value);
            break;
        case 0x06:    // CHAC0
            status = SetRegisterCTSUCHAC0(value);
            break;
        case 0x07:    // CHAC1
            status = SetRegisterCTSUCHAC1(value);
            break;
        case 0x08:    // CHAC2
            status = SetRegisterCTSUCHAC2(value);
            break;
        case 0x09:    // CHAC3
            status = SetRegisterCTSUCHAC3(value);
            break;
        case 0x0a:    // CHAC4
            status = SetRegisterCTSUCHAC4(value);
            break;
        case 0x0b:    // CHTRC0
            status = SetRegisterCTSUCHTRC0(value);
            break;
        case 0x0c:    // CHTRC1
            status = SetRegisterCTSUCHTRC1(value);
            break;
        case 0x0d:    // CHTRC2
            status = SetRegisterCTSUCHTRC2(value);
            break;
        case 0x0e:    // CHTRC3
            status = SetRegisterCTSUCHTRC3(value);
            break;
        case 0x0f:    // CHTRC4
            status = SetRegisterCTSUCHTRC4(value);
            break;
        case 0x10:    // DCLKC
            status = SetRegisterCTSUDCLKC(value);
            break;
        case 0x11:    // ST
            status = SetRegisterCTSUST(value);
            break;
        case 0x12:    // SSC
            status = SetRegisterCTSUSSC(channel, value);
            break;
        case 0x13:    // SO0
            status = SetRegisterCTSUSO0(channel, value);
            break;
        case 0x14:    // SO1
            status = SetRegisterCTSUSO1(channel, value);
            break;
        case 0x15: // CTSUSC - No support command
        case 0x16: // CTSURC - No support command
            status = CMD_RESULT_FAILURE;
            break;
        case 0x17:
            status = SetRegisterCTSUERRS(value);
            break;
        default:
            status = CMD_RESULT_FAILURE;
            break;
    }
    pcmd->fmt.size = 1;
    pcmd->fmt.data[0] = status;

}

/***********************************************************************************************************************
* Function Name: SensorUtilityReadResponse
* Description  : Process for the read command of Utlility
* Arguments    : pcmd - Pointer of transfer data
*                channel - touch sensor number
* Return Value : none
***********************************************************************************************************************/
static void SensorUtilityReadResponse(com_data_tx_t *pcmd, uint16_t channel)
{
    uint16_t value;
    uint16_t tmpval = 0;
    uint8_t  status = CMD_RESULT_FAILURE;
    uint8_t  size;
    uint16_t monitorSize;

    /* Set default value */
    size  = 1;
    value = 0;

    switch (pcmd->fmt.sub)
    {
        case 0x00:    // UPDATE
            break;
        case 0x01:    // RESET
            break;
        case 0x02:    // SET_BATCH
            break;
        case 0x03:    // EXEC_BATCH
            status = CMD_RESULT_FAILURE;
            if (GetUtilityExecuteBatch(&pcmd->fmt.data[1], &monitorSize) != CMD_RESULT_FAILURE)
            {
                monitorSize = (uint16_t)(monitorSize + 1);
                status = CMD_RESULT_SUCCESS;
            }
            if (CMD_RESULT_FAILURE != status)
            {
                size = (uint8_t)(monitorSize & 0xFF);
                if  (monitorSize > 255)
                {
                    pcmd->fmt.main |= 0x20;
                }
            }
            break;
        case 0x04:    // MEASURE
            size  = 2;
            value = 1;
            if (CTSU_STOP_MODE == g_ctsu_soft_mode)
            {
                value = 0;
            }
            status = CMD_RESULT_SUCCESS;
            break;
        case 0x05:    // FLAGS
            size  = 3;
            value = g_ctsu_status[g_access_method].byte;
            status = CMD_RESULT_SUCCESS;
            break;
        case 0x06:    // WAIT
            size   = 3;
            value  = WAIT_TIME;
            status = CMD_RESULT_SUCCESS;
            break;
        case 0x07:  // Burst mode
            size   = 3;
            value  = g_burst_mode;
#if !defined(USB_SERIAL_USE)
            status = CMD_RESULT_FAILURE;
#else
            status = CMD_RESULT_SUCCESS;
#endif
            break;
        case 0x08:    // METHOD
            size   = 3;
            value  = METHOD_NUM;
            if (MUTUAL_METHOD_NUM > 7)
            {
                value |= (uint8_t)(0x10);
            }
            for (int i = 0; i < MUTUAL_METHOD_NUM; i++) {
                tmpval = (uint16_t)(tmpval | (1 << + i));
            }
            if (0 != SELF_METHOD_NUM )
            {
                tmpval = (uint16_t)(tmpval << 1);
            }
            value = (uint16_t)(value | (tmpval << 8));
            status = CMD_RESULT_SUCCESS;
            break;
        case 0x09:    // METHOD_INFO
            size   = 3;
            value  = g_method_info[channel].enable;
            status = CMD_RESULT_SUCCESS;
            break;
        case 0x0a:    // ACCESS_METHOD
            size   = 3;
            value  = g_access_method;
            status = CMD_RESULT_SUCCESS;
            break;
        default:
            break;
    }
    pcmd->fmt.size    = 1;
    pcmd->fmt.data[0] = status;
    if (CMD_RESULT_FAILURE != status)
    {
        pcmd->fmt.size    = size;
        pcmd->fmt.data[0] = status;
        if (pcmd->fmt.sub != 0x03/* EXEC_BATCH */)
        {
            pcmd->fmt.data[1] = (uint8_t)(value & 0xff);
            pcmd->fmt.data[2] = (uint8_t)(value >> 8);
        }
    }
}

/***********************************************************************************************************************
* Function Name: SensorUtilityWriteResponse
* Description  : Process for the write command of Utlility 
* Arguments    : pcmd - Pointer of transfer data
*                channel - touch sensor number
* Return Value : none
***********************************************************************************************************************/
static void SensorUtilityWriteResponse(com_data_tx_t *pcmd, uint16_t channel)
{
    uint8_t i;
    uint8_t status;
    ssp_err_t err;
    (void)channel;

    ctsu_close_option_t close_opt = 0;

    status = CMD_RESULT_FAILURE;
    switch (pcmd->fmt.sub)
    {
        case 0x00:    // UPDATE
            /* Nothing to do. */
            status = CMD_RESULT_SUCCESS;
            break;
        case 0x01:    // RESET
        	err = R_CTSU_Close( p_ctsu_ctrl, close_opt);
        	/* Measurement Re-start */
        	p_ctsu_cfg->p_ctsu_hw_cfg = all_ctsu_configs[0];
        	err = R_CTSU_Open( p_ctsu_ctrl, p_ctsu_cfg);
        	if( (SSP_SUCCESS==err) || (SSP_ERR_ALREADY_OPEN==err) )
        	{
        		g_ctsu_soft_mode = CTSU_READY_MODE;
        	}
        	status = CMD_RESULT_SUCCESS;
            break;
        case 0x02:    // SET_BATCH
            for (i = 0; i < com_data.fmt.size; i++)
            {
                monitor_command.command[i] = com_data.fmt.data[i];
            }
            monitor_command.size = com_data.fmt.size;
            status = CMD_RESULT_SUCCESS;
            break;
        case 0x04:    // MEASURE
            if (0 != com_data.fmt.data[0])
            {  	/* Measurement Re-start */
                p_ctsu_cfg->p_ctsu_hw_cfg = all_ctsu_configs[g_access_method];
                err = R_CTSU_Open( p_ctsu_ctrl, p_ctsu_cfg);
                if( (SSP_SUCCESS==err) || (SSP_ERR_ALREADY_OPEN==err) )
            	{
            		g_ctsu_soft_mode = CTSU_READY_MODE;
            		status = CMD_RESULT_SUCCESS;
            	}
            }
            else
            {
                err = R_CTSU_Close( p_ctsu_ctrl, close_opt);
                g_ctsu_soft_mode = CTSU_STOP_MODE;
            }
            status = CMD_RESULT_SUCCESS;
            break;
        case 0x07:  // Burst mode
            g_burst_mode    = com_data.fmt.data[0];
            status = CMD_RESULT_SUCCESS;
            break;
        case 0x0a:  // ACCESS_METHOD
            if (com_data.fmt.data[0] < METHOD_NUM)
            {
                g_access_method = com_data.fmt.data[0];
                status = CMD_RESULT_SUCCESS;
            }
            break;
        default:     /* Non-support commands */
            status    = CMD_RESULT_FAILURE;
            break;
    }
    pcmd->fmt.size    = 1;
    pcmd->fmt.data[0] = status;
}

/***********************************************************************************************************************
* Function Name: CTSUGetReferenceCounter
* Description  : This API gets data of Reference counter.
* Arguments    : None
* Return Value : counter
***********************************************************************************************************************/
void BurstMonitorSendSensorValue(void)
{
#ifdef  USB_SERIAL_USE
    uint16_t    loop;
    uint16_t    length;

    // Check the Burst monitor mode flag
    if (1 == g_burst_mode)
    {
        length = 0; // Initialize send value size

        if (g_method_info[0].type == METHOD_TYPE_MTLCP)
        {
            for (loop = 0; loop < METHOD_NUM; loop++)
            {
                for (uint16_t keynum = 0; keynum < g_key_info[loop].ena_num; keynum++)
                {
                    // Primary sensor counter
                    rsp_cmd.fmt.data[length++] = (uint8_t)(*(g_mutual_ico_pri_sensor_pt[loop] + (keynum * 4)) &  0xff);
                    rsp_cmd.fmt.data[length++] = (uint8_t)(*(g_mutual_ico_pri_sensor_pt[loop] + (keynum * 4)) >> 8);

                    // Secondary sensor counter
                    rsp_cmd.fmt.data[length++] = (uint8_t)(*(g_mutual_ico_snd_sensor_pt[loop] + (keynum * 4)) & 0xff);
                    rsp_cmd.fmt.data[length++] = (uint8_t)(*(g_mutual_ico_snd_sensor_pt[loop] + (keynum * 4)) >> 8);
                }
            }
        }
        else
        {
            uint16_t    value0;
            uint16_t    value1;

            for (loop = 0; loop < SELFCAP_SENSOR_MAX; loop++)
            {
                value0  = 0;
                value1  = 0;
                if (g_key_info[0].sensor_index[loop] != 0xff)
                {
                    // Sensor counter
                    value0  = g_self_ico_data_pt[0][g_key_info[0].sensor_index[loop]].sen;

                    // Reference counter
                    value1  = g_self_ico_data_pt[0][g_key_info[0].sensor_index[loop]].ref;
                }

                // Sensor counter
                rsp_cmd.fmt.data[length++] = (uint8_t)(value0 &  0xff);
                rsp_cmd.fmt.data[length++] = (uint8_t)(value0 >> 8);

                // Reference counter
                rsp_cmd.fmt.data[length++] = (uint8_t)(value1 & 0xff);
                rsp_cmd.fmt.data[length++] = (uint8_t)(value1 >> 8);
            }
        }

        rsp_cmd.fmt.main = 0x00;
        rsp_cmd.fmt.sub  = 'B';
        rsp_cmd.fmt.sum  = 'M';
        rsp_cmd.fmt.size = length;
        if (length > 255) {
            // Enable the Carry flag
            rsp_cmd.fmt.main |= 0x20;
        }

        // Send sensor values to USB-Serial port
        R_usb_pcdc_SendData( &rsp_cmd.byte_acs[0], length + HEAD_SIZE, (usb_cbinfo_t)&usb_psmpl_TxCB );
    }
#endif  // USB_SERIAL_USE
}
#endif    //] WORKBENCH_COMMAND_USE

/******************************************************************************
* END OF TEXT
******************************************************************************/
