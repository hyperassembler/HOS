/*-------------------------------------------------------
 |
 |  bifrost.h
 |
 |  Master Bifrost header (public)
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2011  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#ifndef _BIFROST_H
#define _BIFROST_H

//
// TIE headers
//
#if defined(xtensa_sfp_TIE)

#include TIE_HEADER

#elif defined(xtensa_vfp_TIE)

#include <xtensa/tie/VectraFP.h>
#include <xtensa/tie/MSET.h>
#include <xtensa/tie/MSIVEC.h>
#include <xtensa/tie/MSHT.h>
#include <xtensa/tie/MSVectraFPExt.h>

#endif

//
// Standard libraries
//
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <inttypes.h>
#include <limits.h>

// Pull in auto-generated conf_defs header.
// This file MUST BE REGENERATED if a config change has
// altered the number of cores by doing:
// 'make iss/hup_chip.conf_defs'
// 'cp $(SIMWS)/conf_defs.h bifrost/src/shared/include'
#include "conf_defs.h"

//
// Bifrost headers
//
#include "bifrost_macros.h"
#include "g_types.h"
#include "bifrost_hs_intr.h"
#include "bifrost_types_constants.h"
#include "bifrost_project.h"
#include "bifrost_system.h"
#include "bifrost_facility.h"
#include "bifrost_statuscodes.h"
#include "bifrost_mailbox.h"
#include "bifrost_logging.h"
#include "bifrost_common.h"
#include "bifrost_cfg_utils.h"
#include "bifrost_api.h"
#include BF_SYSTEM_API_HEADER
#include "bifrost_alloc.h"
#include "bifrost_linker.h"

#include "bifrost_perfmon.h"
#include "td_perfmon.h"
#include "bifrost_tempmon.h"
// VITAG Header
#include "vitags.h"

//
// Port-specific includes
//

#if defined (__x86_64__)
//#include <sys/mman.h>

#elif defined(__XTENSA__)
#include <xtensa/simcall.h>
#include <xtensa/config/core.h>
//#include <xtensa/xtruntime.h>
#include <xtensa/tie/xt_sync.h>
#endif

#if defined (__x86_64__)
//#include <stdlib.h>
#define XCHAL_DCACHE_LINESIZE (64)
#define XT_RSR_CCOUNT() (0)
#endif

// Build information functions for tests to retrieve
// information such as testsuite name or timestamp
// of build
extern const char* get_bifrost_build_timestamp();
extern const char* get_bifrost_build_builddir();
extern const char* get_bifrost_build_tsname();
extern const char* get_bifrost_build_p4rev();
extern const char* get_bifrost_build_cflags();
extern const char* get_bifrost_build_gcc_path();
extern const char* get_bifrost_build_python_path();
extern const char* get_bifrost_build_make_path();
extern const char* get_bifrost_build_gcc_ver();
extern const char* get_bifrost_build_gcc_xt_ver();
extern const char* get_bifrost_build_python_ver();
extern const char* get_bifrost_build_make_ver();

#endif // _BIFROST_H

