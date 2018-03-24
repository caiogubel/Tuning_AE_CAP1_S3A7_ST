/* generated thread source file - do not edit */
#include "tuning_thread.h"

TX_THREAD tuning_thread;
void tuning_thread_create(void);
static void tuning_thread_func(ULONG thread_input);
/** Alignment requires using pragma for IAR. GCC is done through attribute. */
#if defined(__ICCARM__)
#pragma data_alignment = BSP_STACK_ALIGNMENT
#endif
static uint8_t tuning_thread_stack[1024] BSP_PLACE_IN_SECTION(".stack.tuning_thread") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);
void tx_startup_err_callback(void * p_instance, void * p_data);
#if (BSP_IRQ_DISABLED) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_transfer0) && !defined(SSP_SUPPRESS_ISR_DTCELC_EVENT_CTSU_END)
#define DTC_ACTIVATION_SRC_ELC_EVENT_CTSU_END
#if defined(DTC_ACTIVATION_SRC_ELC_EVENT_ELC_SOFTWARE_EVENT_0) && !defined(DTC_VECTOR_DEFINED_SOFTWARE_EVENT_0)
SSP_VECTOR_DEFINE(elc_software_event_isr, ELC, SOFTWARE_EVENT_0);
#define DTC_VECTOR_DEFINED_SOFTWARE_EVENT_0
#endif
#if defined(DTC_ACTIVATION_SRC_ELC_EVENT_ELC_SOFTWARE_EVENT_1) && !defined(DTC_VECTOR_DEFINED_SOFTWARE_EVENT_1)
SSP_VECTOR_DEFINE(elc_software_event_isr, ELC, SOFTWARE_EVENT_1);
#define DTC_VECTOR_DEFINED_SOFTWARE_EVENT_1
#endif
#endif
#endif

dtc_instance_ctrl_t g_transfer0_ctrl;
transfer_info_t g_transfer0_info =
{ .dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED, .repeat_area = TRANSFER_REPEAT_AREA_SOURCE, .irq = TRANSFER_IRQ_END,
  .chain_mode = TRANSFER_CHAIN_MODE_DISABLED, .src_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED, .size =
          TRANSFER_SIZE_2_BYTE,
  .mode = TRANSFER_MODE_BLOCK, .p_dest = (void *) NULL, .p_src = (void const *) NULL, .num_blocks = 1, .length = 1, };
const transfer_cfg_t g_transfer0_cfg =
{ .p_info = &g_transfer0_info, .activation_source = ELC_EVENT_CTSU_END, .auto_enable = false, .p_callback = NULL,
  .p_context = &g_transfer0, .irq_ipl = (BSP_IRQ_DISABLED) };
/* Instance structure to use this module. */
const transfer_instance_t g_transfer0 =
{ .p_ctrl = &g_transfer0_ctrl, .p_cfg = &g_transfer0_cfg, .p_api = &g_transfer_on_dtc };
#if (8) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_ctsu0) && !defined(SSP_SUPPRESS_ISR_CTSU)
SSP_VECTOR_DEFINE( ctsu_write_isr, CTSU, WRITE);
#endif
#endif
#if (8) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_ctsu0) && !defined(SSP_SUPPRESS_ISR_CTSU)
SSP_VECTOR_DEFINE( ctsu_read_isr, CTSU, READ);
#endif
#endif
#if (8) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_ctsu0) && !defined(SSP_SUPPRESS_ISR_CTSU)
SSP_VECTOR_DEFINE( ctsu_end_isr, CTSU, END);
#endif
#endif

/** DTC Structures that will be used by the CTSU */

extern const transfer_instance_t g_transfer0_ctsuwr_block_xfer;
extern const transfer_instance_t g_transfer0_ctsurd_block_xfer;

dtc_instance_ctrl_t g_transfer0_ctsuwr_block_xfer_ctrl;
transfer_info_t g_transfer0_ctsuwr_block_xfer_info[3] =
{ [0] =
{ .dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED, .repeat_area = TRANSFER_REPEAT_AREA_DESTINATION, .irq =
          TRANSFER_IRQ_END,
  .chain_mode = TRANSFER_CHAIN_MODE_END, .src_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED, .size = TRANSFER_SIZE_2_BYTE,
  .mode = TRANSFER_MODE_BLOCK, .p_src = (void *) NULL, .num_blocks = 65535, .length = 3, },
  [1] =
  {/* Reloads p_src in the 0th block */
  .dest_addr_mode = TRANSFER_ADDR_MODE_FIXED, .repeat_area = TRANSFER_REPEAT_AREA_SOURCE, .irq = TRANSFER_IRQ_END,
  .chain_mode = TRANSFER_CHAIN_MODE_END, .src_addr_mode = TRANSFER_ADDR_MODE_FIXED, .size = TRANSFER_SIZE_4_BYTE,
  .mode = TRANSFER_MODE_REPEAT, .p_dest = (void *) &g_transfer0_ctsuwr_block_xfer_info[0].p_src, .length = 1, },
  [2] =
  {/* Reloads num_blocks in the 0th block */
  .dest_addr_mode = TRANSFER_ADDR_MODE_FIXED, .repeat_area = TRANSFER_REPEAT_AREA_SOURCE, .irq = TRANSFER_IRQ_END,
  .chain_mode = TRANSFER_CHAIN_MODE_DISABLED, .src_addr_mode = TRANSFER_ADDR_MODE_FIXED, .size = TRANSFER_SIZE_2_BYTE,
  .mode = TRANSFER_MODE_REPEAT, .p_dest = (void *) &g_transfer0_ctsuwr_block_xfer_info[0].num_blocks, .length = 1, }, };
const transfer_cfg_t g_transfer0_ctsuwr_block_xfer_cfg =
{ .p_info = &g_transfer0_ctsuwr_block_xfer_info[0], .activation_source = ELC_EVENT_CTSU_WRITE, .auto_enable = true,
  .p_callback = NULL, .p_context = &g_transfer0_ctsuwr_block_xfer, };
/* Instance structure to use this module. */
const transfer_instance_t g_transfer0_ctsuwr_block_xfer =
{ .p_ctrl = &g_transfer0_ctsuwr_block_xfer_ctrl, .p_cfg = &g_transfer0_ctsuwr_block_xfer_cfg, .p_api =
          &g_transfer_on_dtc };
/******************************************************/
/* CTSUWR Transfer element to use for Synergy driver  */
/******************************************************/
dtc_instance_ctrl_t g_transfer0_ctsurd_block_xfer_ctrl;
transfer_info_t g_transfer0_ctsurd_block_xfer_info[3] =
{ [0] =
{ .dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED, .repeat_area = TRANSFER_REPEAT_AREA_SOURCE, .irq = TRANSFER_IRQ_END,
  .chain_mode = TRANSFER_CHAIN_MODE_END, .src_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED, .size = TRANSFER_SIZE_2_BYTE,
  .mode = TRANSFER_MODE_BLOCK, .p_dest = (void *) NULL, .num_blocks = 65535, .length = 2, },
  [1] =
  {/* Reloads p_src in the 0th block */
  .dest_addr_mode = TRANSFER_ADDR_MODE_FIXED, .repeat_area = TRANSFER_REPEAT_AREA_SOURCE, .irq = TRANSFER_IRQ_END,
  .chain_mode = TRANSFER_CHAIN_MODE_END, .src_addr_mode = TRANSFER_ADDR_MODE_FIXED, .size = TRANSFER_SIZE_4_BYTE,
  .mode = TRANSFER_MODE_REPEAT, .p_dest = (void *) &g_transfer0_ctsurd_block_xfer_info[0].p_dest, .length = 1, },
  [2] =
  {/* Reloads num_blocks in the 0th block */
  .dest_addr_mode = TRANSFER_ADDR_MODE_FIXED, .repeat_area = TRANSFER_REPEAT_AREA_SOURCE, .irq = TRANSFER_IRQ_END,
  .chain_mode = TRANSFER_CHAIN_MODE_DISABLED, .src_addr_mode = TRANSFER_ADDR_MODE_FIXED, .size = TRANSFER_SIZE_2_BYTE,
  .mode = TRANSFER_MODE_REPEAT, .p_dest = (void *) &g_transfer0_ctsurd_block_xfer_info[0].num_blocks, .length = 1, }, };
const transfer_cfg_t g_transfer0_ctsurd_block_xfer_cfg =
{ .p_info = &g_transfer0_ctsurd_block_xfer_info[0], .activation_source = ELC_EVENT_CTSU_READ, .auto_enable = true,
  .p_callback = NULL, .p_context = &g_transfer0_ctsurd_block_xfer, };
/* Instance structure to use this module. */
const transfer_instance_t g_transfer0_ctsurd_block_xfer =
{ .p_ctrl = &g_transfer0_ctsurd_block_xfer_ctrl, .p_cfg = &g_transfer0_ctsurd_block_xfer_cfg, .p_api =
          &g_transfer_on_dtc };

/* Data functions to use for advanced usage.*/
ctsu_functions_t g_ctsu0_functions =
{ 0 };

ctsu_instance_ctrl_t g_ctsu0_ctrl;
ctsu_cfg_t g_ctsu0_cfg =
{ .p_lower_lvl_transfer_read = &g_transfer0_ctsurd_block_xfer, .p_lower_lvl_transfer_write =
          &g_transfer0_ctsuwr_block_xfer,
  .p_ctsu_hw_cfg = &g_ctsu_config, .p_ctsu_functions = &g_ctsu0_functions, .p_callback = NULL, .p_context =
          &g_ctsu0_ctrl,
  .ctsu_soft_option = CTSU_PROCESS_OPTION_DEFAULT_SETTING, .ctsu_close_option = CTSU_CLOSE_OPTION_RESET_SFRS,
  .write_ipl = (8), .read_ipl = (8), .end_ipl = (8), };

ctsu_instance_t g_ctsu0 =
{ .p_ctrl = &g_ctsu0_ctrl, .p_cfg = &g_ctsu0_cfg, .p_api = &g_ctsu_on_ctsu, };
const ctsu_api_t * g_ctsu0_api = &g_ctsu_on_ctsu;

extern bool g_ssp_common_initialized;
extern uint32_t g_ssp_common_thread_count;
extern TX_SEMAPHORE g_ssp_common_initialized_semaphore;
void g_hal_init(void);

void tuning_thread_create(void)
{
    /* Increment count so we will know the number of ISDE created threads. */
    g_ssp_common_thread_count++;

    /* Initialize each kernel object. */

    UINT err;
    err = tx_thread_create (&tuning_thread, (CHAR *) "CTSU Tuning Thread", tuning_thread_func, (ULONG) NULL,
                            &tuning_thread_stack, 1024, 1, 1, 1, TX_AUTO_START);
    if (TX_SUCCESS != err)
    {
        tx_startup_err_callback (&tuning_thread, 0);
    }
}

static void tuning_thread_func(ULONG thread_input)
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

    /* Enter user code for this thread. */
    tuning_thread_entry ();
}
