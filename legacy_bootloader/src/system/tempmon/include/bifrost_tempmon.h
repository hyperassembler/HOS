/*-----------------------------------------------------
 |
 |    bifrost_tempmon.h
 |    =================
 |    Header file for JTM temperature monitoring module
 | 
 |------------------------------------------------------
 |
 |    Copyright (C) 2014  Microsoft Corporation
 |    All Rights Reserved
 |    Confidential and Proprietary
 |
 |------------------------------------------------------
 */

#ifndef __BIFROST_TEMPMON_H__
#define __BIFROST_TEMPMON_H__

#define JTM_NUM_INST 5

EXTERN_C VOID td_tempmon_init ();
EXTERN_C VOID td_tempmon_report_temps (UINT64 cur_time);

#endif
