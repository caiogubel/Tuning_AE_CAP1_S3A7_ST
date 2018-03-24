/* generated thread header file - do not edit */
#ifndef COMM_THREAD_H_
#define COMM_THREAD_H_
#include "bsp_api.h"
#include "tx_api.h"
#include "hal_data.h"
#ifdef __cplusplus 
extern "C" void comm_thread_entry(void);
#else 
extern void comm_thread_entry(void);
#endif
#include "sf_el_ux_comms_v2.h"
#include "sf_comms_api.h"
#ifdef __cplusplus
extern "C"
{
#endif
/* USB Communication Framework Instance */
extern const sf_comms_instance_t g_sf_comms0;
/* USBX CDC-ACM Instance Activate User Callback Function */
VOID ux_cdc_device0_instance_activate(VOID *cdc_instance);
/* USBX CDC-ACM Instance Deactivate User Callback Function */
VOID ux_cdc_device0_instance_deactivate_internal(VOID *cdc_instance);
void g_sf_comms0_err_callback(void * p_instance, void * p_data);
void sf_comms_init0(void);
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* COMM_THREAD_H_ */
