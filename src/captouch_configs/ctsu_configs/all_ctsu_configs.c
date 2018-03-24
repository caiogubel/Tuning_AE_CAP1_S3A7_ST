#include "r_ctsu.h"
#include "./utilities/r_touch.h"

extern ctsu_hw_cfg_t g_ctsu_config_mutual;
ctsu_hw_cfg_t* all_ctsu_configs[] = {
    &g_ctsu_config_mutual,
};
uint16_t num_ctsu_configs = sizeof(all_ctsu_configs)/sizeof(ctsu_hw_cfg_t*);