/*-----------------------------------------------------
 |
 |      td_logging.c
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

void td_print_event_log_entry(HW_EVENT_LOG_ENTRY* entry)
{
    hw_cacheFlushBuffer(entry, sizeof(HW_EVENT_LOG_ENTRY));
    
    UINT64 time = hw_read64(&entry->time);
    UINT64 code = hw_read64(&entry->code);
    
    UINT64 data[HW_MAX_EVENT_LOG_DATA] = { hw_read64(&entry->data[0]), hw_read64(&entry->data[1]), hw_read64(&entry->data[2]), hw_read64(&entry->data[3]) };
    
    const char* header = " @ Time ";
    
    switch(code)
    {
        case HW_EVENT_FAILED_TEST:
            hw_errmsg("%s%llu: TEST FAILED: test \"%s\" reported failure from function \"%s\" on iteration %llu -- error count: %u\n", header, time, (char*)(UINTPTR)data[0], (char*)(UINTPTR)data[1], data[2], data[3]);
            break;
            
        case HW_EVENT_FAILED_RESULT:
            hw_errmsg("%s%llu: HW_RESULT: %s:%llu got error code %llu\n", header, time, (UINTPTR)data[1], data[2], data[0]);
            break;
            
        case HW_EVENT_INT3_BREAKPOINT:
            hw_critical("%s%llu: int 3 breakpoint\n", header, time);
            break;
            
        case HW_EVENT_UNHANDLED_INTERRUPT:
            hw_errmsg("%s%llu: unhandled interrupt %d\n", header, time, data[0]);
            break;

        case HW_EVENT_UNHANDLED_EXCEPTION:
            hw_errmsg("%s%llu: unhandled exception %d\n", header, time, data[0]);
            break;
            
        case HW_EVENT_LOG_FULL:
            hw_errmsg("%s%llu: Log full, subsequent events were ignored\n", header, time);
            break;
            
        case HW_EVENT_CHECKPOINT:
        case HW_EVENT_TRACEPOINT:
            // TODO: add specialized handling for SIM_PLATFORM
            hw_critical("%s%llu: %s %s\n", header, time, (UINTPTR)data[0], (UINTPTR)data[1]);
            break;
            
        default:
            hw_errmsg("%s%llu: Unrecognized event type %llu (fix %s!)\n", header, time, code, __func__);
    }
}

void td_print_event_log(HW_EVENT_LOG *log)
{
    hw_cacheInvalidateAddr(&log->size);
    UINT32 size = hw_read32(&log->size);
    
    for(UINT32 i = 0; i < size; i++)
    {
        td_print_event_log_entry(&log->entries[i]);
    }
}

void td_print_all_event_logs()
{
    UINT32 size = 0;
    UINT32 dwTestCnt = 0;
    UINT32 testID = 0;
    UINT32 testPointerIndex = 0;
    
    const HW_XLIST* p_tlp = NULL;
    HW_EVENT_LOG* p_log = NULL;
    HW_TS_PUBLIC_TESTDATA* p_publicTestData = NULL;
    
    for(UINT32 i = 0; i < HW_PROC_CNT; i++)
    {
        if(td_ts_slaveData[i].active)
        {
            p_publicTestData = (HW_TS_PUBLIC_TESTDATA*)hw_readptr(&testSlaveVarTable[i].publicTestData);
            p_log = (HW_EVENT_LOG*)hw_readptr(&testSlaveVarTable[i].eventLog);
            
            hw_cacheInvalidateAddr(&p_log->size);
            size = hw_read32(&p_log->size);
            if (size > 0)
            {
                hw_critical("\nEVENT LOG: CPU %d, test ID internal\n", i);
                td_print_event_log(p_log);
            }
            
            hw_cacheInvalidateAddr(&testSlaveVarTable[i].dwTestCnt);
            dwTestCnt = hw_read32(&testSlaveVarTable[i].dwTestCnt);
            for(UINT32 j = 0; j < dwTestCnt; j++)
            {
                p_log = &p_publicTestData[j].eventLog;
                
                hw_cacheInvalidateAddr(&p_log->size);
                size = hw_read32(&p_log->size);
                if (size > 0)
                {
                    testPointerIndex = hw_read32(&p_publicTestData[j].testPointerIndex);
                    p_tlp = &(hw_tl_master[hw_archTypeList[i]][testPointerIndex]);
                    
                    testID = hw_read32(&p_publicTestData[j].testInstanceID);
                    
                    hw_critical("\nEVENT LOG: CPU %d, test ID %d:%s\n", i, testID, p_tlp->testPathName);
                    td_print_event_log(p_log);
                }
            }
        }
    }
}

