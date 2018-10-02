#pragma once

#include "hal.h"
#include "ke/intr.h"
#include "status.h"
#include "kernel.h"

k_status
kp_intr_init(struct boot_info *info);
