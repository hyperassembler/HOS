/*-------------------------------------------------------
 |
 |      bifrost_system.h
 |
 |      Contains project-specific Bifrost information,
 |      such as timer, interrupt, and exception types.
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2016  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#ifndef _BIFROST_SYSTEM_H
#define _BIFROST_SYSTEM_H

//
// Interrupt
//

#define BF_SYSTEM_CONSTANTS_HEADER <CONCAT(bifrost_system_constants_,ARCHTYPE).h>
#define BF_SYSTEM_API_HEADER <CONCAT(bifrost_system_api_,ARCHTYPE).h>

#include <bifrost_system_constants.h>

#ifdef ARCHTYPE
#include BF_SYSTEM_CONSTANTS_HEADER
#else
#define HW_TS_NUM_INTVECTORS            (0)
#define HW_TS_NUM_EXCVECTORS            (0)
#endif

#endif

