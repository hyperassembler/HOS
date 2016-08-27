/*-------------------------------------------------------
 |
 |  bifrost_import.h
 |
 |  Bifrost header that only includes the
 |  core headers--for use by external tools
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2015  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

// This file serves the same purpose as bifrost.h, so
// use the same define guard--so that only one can be included
#ifndef _BIFROST_H
#define _BIFROST_H

//
// Standard libraries
//
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <inttypes.h>

// Pull in auto-generated conf_defs header.
// This file MUST BE REGENERATED if a config change has
// altered the number of cores by doing:
// 'make iss/hup_chip.conf_defs'
// 'cp $(SIMWS)/conf_defs.h bifrost/src/shared/include'
#include "conf_defs.h"

// Core Bifrost headers
#include "bifrost_macros.h"
#include "g_types.h"
#include "bifrost_types_constants.h"
#include "bifrost_project.h"
#include "bifrost_system.h"
#include "bifrost_facility.h"
#include "bifrost_statuscodes.h"
#include "bifrost_mailbox.h"
#include "bifrost_logging.h"
#include "bifrost_common.h"

#endif // _BIFROST_H

