/* generated thread header file - do not edit */
#ifndef TUNING_THREAD_H_
#define TUNING_THREAD_H_
#include "bsp_api.h"
#include "tx_api.h"
#include "hal_data.h"
#ifdef __cplusplus 
extern "C" void tuning_thread_entry(void);
#else 
extern void tuning_thread_entry(void);
#endif
#include "r_dtc.h"
#include "r_transfer_api.h"
/* Include the headers for the r_touch API use */
#include "r_ctsu_api.h"
#include "r_ctsu.h"
#include "r_dtc.h"
#ifdef __cplusplus
extern "C"
{
#endif
/* Transfer on DTC Instance. */
extern const transfer_instance_t g_transfer0;
#ifndef NULL
void NULL(transfer_callback_args_t * p_args);
#endif
/** Pointer to a tuned CTSU configuration */
extern ctsu_hw_cfg_t g_ctsu_config;

/** Pointer to CTSU API */
extern const ctsu_api_t * g_ctsu0_api;

/** Pointer to CTSU configuration */
extern ctsu_cfg_t g_ctsu0_cfg_on_ctsu;

/** Pointer to control handle */
extern ctsu_instance_ctrl_t g_ctsu0_ctrl;

/**  CTSU instance */
extern ctsu_instance_t g_ctsu0;

/** Pointer to Advanced Functions*/
extern ctsu_functions_t g_ctsu0_functions;

#ifndef NULL
void NULL(ctsu_callback_args_t * p_args);
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* TUNING_THREAD_H_ */
