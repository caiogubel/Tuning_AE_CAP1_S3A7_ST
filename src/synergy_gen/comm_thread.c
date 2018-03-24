/* generated thread source file - do not edit */
#include "comm_thread.h"

TX_THREAD comm_thread;
void comm_thread_create(void);
static void comm_thread_func(ULONG thread_input);
/** Alignment requires using pragma for IAR. GCC is done through attribute. */
#if defined(__ICCARM__)
#pragma data_alignment = BSP_STACK_ALIGNMENT
#endif
static uint8_t comm_thread_stack[1024] BSP_PLACE_IN_SECTION(".stack.comm_thread") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);
void tx_startup_err_callback(void * p_instance, void * p_data);
#if defined(__ICCARM__)
#define g_sf_comms0_err_callback_WEAK_ATTRIBUTE
#pragma weak g_sf_comms0_err_callback  = g_sf_comms0_err_callback_internal
#define ux_cdc_device0_instance_deactivate_WEAK_ATTRIBUTE
#pragma weak ux_cdc_device0_instance_deactivate  = ux_cdc_device0_instance_deactivate_internal
#elif defined(__GNUC__)
#define g_sf_comms0_err_callback_WEAK_ATTRIBUTE   __attribute__ ((weak, alias("g_sf_comms0_err_callback_internal")))
#define ux_cdc_device0_instance_deactivate_WEAK_ATTRIBUTE   __attribute__ ((weak, alias("ux_cdc_device0_instance_deactivate_internal")))
#endif
void g_sf_comms0_err_callback(void * p_instance, void * p_data)
g_sf_comms0_err_callback_WEAK_ATTRIBUTE;
void ux_cdc_device0_instance_deactivate(VOID *cdc_instance)
ux_cdc_device0_instance_deactivate_WEAK_ATTRIBUTE;
/***********************************************************************************************************************
 * USB Communications Framework (SF_EL_UX_COMMS) Instance for g_sf_comms0
 **********************************************************************************************************************/
sf_el_ux_comms_instance_ctrl_t g_sf_comms0_instance_ctrl =
{ .p_cdc = NULL, };

/* Instance structure to use this module. */
const sf_comms_instance_t g_sf_comms0 =
{ .p_ctrl = &g_sf_comms0_instance_ctrl, .p_cfg = NULL, .p_api = &g_sf_el_ux_comms_on_sf_comms };

/***********************************************************************************************************************
 * USBX Device CDC-ACM Instance Activate Callback function required for g_sf_comms0
 **********************************************************************************************************************/
VOID ux_cdc_device0_instance_activate(VOID *cdc_instance)
{
    /* Save the CDC instance for g_sf_comms0. */
    g_sf_comms0_instance_ctrl.p_cdc = (UX_SLAVE_CLASS_CDC_ACM *) cdc_instance;

    /* Inform the CDC instance activation event for g_sf_comms0. */
    if (NULL != g_sf_comms0.p_ctrl)
    {
        sf_el_ux_comms_instance_ctrl_t * p_ux_comms_ctrl = (sf_el_ux_comms_instance_ctrl_t *) g_sf_comms0.p_ctrl;

        /* Check if the semaphore for g_sf_comms0 is ready. */
        if (0x53454D41 != p_ux_comms_ctrl->semaphore.tx_semaphore_id)
        {
            tx_semaphore_create (&p_ux_comms_ctrl->semaphore, (CHAR *) "UX_COMMS_SEMAPHORE", 0);
        }

        /* Put a semaphore if the instance for g_sf_comms0 to inform CDC instance is ready. */
        tx_semaphore_ceiling_put (&p_ux_comms_ctrl->semaphore, 1);
    }
    return;
}
/***********************************************************************************************************************
 * @brief      This is a weak USBX Device CDC-ACM Instance Deactivate Callback function required for g_sf_comms0
 *             It should be overridden by defining a user function with the prototype mentioned below.
 *             - VOID ux_cdc_device0_instance_deactivate (VOID *cdc_instance)
 *
 * @param[in]  cdc_instance argument is used to identify the CDC-ACM Instance
 **********************************************************************************************************************/
VOID ux_cdc_device0_instance_deactivate_internal(VOID *cdc_instance)
{
    SSP_PARAMETER_NOT_USED (cdc_instance);

    /* Reset the CDC instance for g_sf_comms0. */
    g_sf_comms0_instance_ctrl.p_cdc = UX_NULL;

    return;
}
/*******************************************************************************************************************//**
 * @brief      This is a weak example initialization error function.  It should be overridden by defining a user  function 
 *             with the prototype below.
 *             - voidg_sf_comms0_err_callback(void * p_instance, void * p_data)
 *
 * @param[in]  p_instance arguments used to identify which instance caused the error and p_data Callback arguments used to identify what error caused the callback.
 **********************************************************************************************************************/
void g_sf_comms0_err_callback_internal(void * p_instance, void * p_data);
void g_sf_comms0_err_callback_internal(void * p_instance, void * p_data)
{
    /** Suppress compiler warning for not using parameters. */
    SSP_PARAMETER_NOT_USED (p_instance);
    SSP_PARAMETER_NOT_USED (p_data);

    /** An error has occurred. Please check function arguments for more information. */
    BSP_CFG_HANDLE_UNRECOVERABLE_ERROR (0);
}
/*******************************************************************************************************************//**
 * @brief     Initialization function that the user can choose to have called automatically during thread entry.
 *            The user can call this function at a later time if desired using the prototype below.
 *            - void sf_comms_init0(void)
 **********************************************************************************************************************/
void sf_comms_init0(void)
{
    ssp_err_t ssp_err_g_sf_comms0;
    /** Open USB Communications Framework */
    ssp_err_g_sf_comms0 = g_sf_comms0.p_api->open (g_sf_comms0.p_ctrl, g_sf_comms0.p_cfg);
    if (UX_SUCCESS != ssp_err_g_sf_comms0)
    {
        g_sf_comms0_err_callback ((void *) &g_sf_comms0, &ssp_err_g_sf_comms0);
    }
}
extern bool g_ssp_common_initialized;
extern uint32_t g_ssp_common_thread_count;
extern TX_SEMAPHORE g_ssp_common_initialized_semaphore;
void g_hal_init(void);

void comm_thread_create(void)
{
    /* Increment count so we will know the number of ISDE created threads. */
    g_ssp_common_thread_count++;

    /* Initialize each kernel object. */

    UINT err;
    err = tx_thread_create (&comm_thread, (CHAR *) "Workbench Communication Thread", comm_thread_func, (ULONG) NULL,
                            &comm_thread_stack, 1024, 1, 1, 1, TX_AUTO_START);
    if (TX_SUCCESS != err)
    {
        tx_startup_err_callback (&comm_thread, 0);
    }
}

static void comm_thread_func(ULONG thread_input)
{
    /* Not currently using thread_input. */
    SSP_PARAMETER_NOT_USED (thread_input);

    /* First thread will take care of common initialization. */
    UINT err;
    err = tx_semaphore_get (&g_ssp_common_initialized_semaphore, TX_WAIT_FOREVER);

    if (TX_SUCCESS != err)
    {
        /* Check err, problem occurred. */
        tx_startup_err_callback (&g_ssp_common_initialized_semaphore, 0);
    }

    /* Only perform common initialization if this is the first thread to execute. */
    if (false == g_ssp_common_initialized)
    {
        /* Later threads will not run this code. */
        g_ssp_common_initialized = true;

        /* Perform common module initialization. */
        g_hal_init ();

        /* Now that common initialization is done, let other threads through. */
        /* First decrement by 1 since 1 thread has already come through. */
        g_ssp_common_thread_count--;
        while (g_ssp_common_thread_count > 0)
        {
            err = tx_semaphore_put (&g_ssp_common_initialized_semaphore);

            if (TX_SUCCESS != err)
            {
                /* Check err, problem occurred. */
                tx_startup_err_callback (&g_ssp_common_initialized_semaphore, 0);
            }

            g_ssp_common_thread_count--;
        }
    }

    /* Initialize each module instance. */
    /** Call initialization function if user has selected to do so.*/
#if (1)
    sf_comms_init0 ();
#endif

    /* Enter user code for this thread. */
    comm_thread_entry ();
}
