/*-----------------------------------------------------
 |
 |      hw_log.c
 |
 |    Bifrost event log.
 |
 |------------------------------------------------------
 |
 |    Copyright (C) 2012  Microsoft Corporation
 |    All Rights Reserved
 |    Confidential and Proprietary
 |
 |------------------------------------------------------
 */

#include "bifrost_private.h"
#include "bifrost_thread.h"
#include "bifrost_intr.h"

void hw_log(HW_EVENT_CODE code, UINT64 data0, UINT64 data1, UINT64 data2, UINT64 data3)
{
    if(!verbosity_enabled(event_verbosity_levels[code]))
    {
        return;
    }
    
    UINT64 time = hw_getTime();
    HW_EVENT_LOG* log = NULL;
    HW_TS_PUBLIC_TESTDATA* publicTestData = (HW_TS_PUBLIC_TESTDATA*)hw_readptr(&testSlaveVars->publicTestData);
    //if (hw_read32(&testSlaveVars->dwCurrentXTest) == HW_NO_TEST_INDEX)
    //{
    //    log = (HW_EVENT_LOG*)hw_readptr(&testSlaveVars->eventLog);
    //}
    //else
    //{
    //    UINT32 dwCurrentXTest = hw_read32(&testSlaveVars->dwCurrentXTest);
    //    log = &publicTestData[dwCurrentXTest].eventLog;
    //}
    
    hw_irql_t prev_irql = hw_raise_irql(HW_IRQL_DPC_LEVEL);
    
    log = &hw_current_tcb()->event_log;
    
    UINT32 size = hw_read32(&log->size);
    if (size < _countof(log->entries))
    {
        HW_EVENT_LOG_ENTRY *entry = &log->entries[size];
        
        hw_write64(&entry->code, code);
        hw_write64(&entry->time, time);
        hw_write64(&entry->data[0], data0);
        hw_write64(&entry->data[1], data1);
        hw_write64(&entry->data[2], data2);
        hw_write64(&entry->data[3], data3);
        hw_cacheFlushBuffer(entry, sizeof(HW_EVENT_LOG_ENTRY));
        
        size++;
        hw_write32(&log->size, size);
        hw_cacheFlushAddr(&log->size);
        
        if (size == _countof(log->entries) - 1)
        {
            hw_log(HW_EVENT_LOG_FULL, 0, 0, 0, 0);
        }
    }
    
    hw_lower_irql(prev_irql);
}

