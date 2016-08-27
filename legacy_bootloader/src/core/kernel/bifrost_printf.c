///*-----------------------------------------------------
// |
// |      hw_printf.c
// |
// |  prints to screen and logs to file
// |
// |------------------------------------------------------
// |
// |  Revision History:
// |
// |    #5 -  3 March 2005       J. Hanes
// |          Cosmetic.
// |
// |    #4 -  18 Febrary 2005    J. Hanes
// |          Add ctp_print_control();
// |            replace hw_print_enable with hw_print_level.
// |
// |    #3 -  1 Febrary 2005     J. Hanes
// |          Use hw_print_enable to disable print
// |
// |    #2 - 30 June 2004        J. Hanes
// |         Add hw_ignore_printf()
// |
// |------------------------------------------------------
// |
// |    Copyright (C) 2003  Microsoft Corporation
// |    All Rights Reserved
// |    Confidential and Proprietary
// |
// |------------------------------------------------------
// */
//
//
//
// #include <bifrost_intr.h>


//
//void
//hw_set_verbosity( UINT32 verbosity )
//{
//    HW_TS_TESTDATA* testData = (HW_TS_TESTDATA*)hw_readptr(&testSlaveVars->testData);
//
//    // Mask out the print sync bit, this function isn't
//    // allowed to touch it
//    verbosity &= ~PRINT_TYPE(HW_PRINT_SYNC);
//
//    //UINT32 testIdx = hw_read32(&testSlaveVars->dwCurrentXTest);
//    //
//    //if(testIdx == HW_NO_TEST_INDEX || hw_getCoreNum() == bifrostCachedGlobals.tpid)
//    //{
//    //    // Grab the previous print sync setting and combine
//    //    // it with the requested verbosity
//    //    verbosity |= bifrostCachedGlobals.hw_global_verbosity & PRINT_TYPE(HW_PRINT_SYNC);
//    //    bifrostCachedGlobals.hw_global_verbosity = verbosity;
//    //}
//    //else
//    //{
//    //    // Grab the previous print sync setting and combine
//    //    // it with the requested verbosity
//    //    verbosity |= hw_read32(&testData[testIdx].verbosity) & PRINT_TYPE(HW_PRINT_SYNC);
//    //    hw_write32(&testData[testIdx].verbosity, verbosity);
//    //}
//}
//
//void hw_set_print_synchronization(BOOL value)
//{
//    HW_TS_TESTDATA* testData = (HW_TS_TESTDATA*)hw_readptr(&testSlaveVars->testData);
//
//    //UINT32 testIdx = hw_read32(&testSlaveVars->dwCurrentXTest);
//    //
//    //// Read and modify the verbosity settings to include
//    //// the HW_PRINT_SYNC bit
//    //UINT32 current_verbosity = hw_read32(&testData[testIdx].verbosity);
//    //current_verbosity &= ~PRINT_TYPE(HW_PRINT_SYNC);
//    //current_verbosity |= (value & 0x1) << HW_PRINT_SYNC;
//    //
//    //hw_write32(&testData[testIdx].verbosity, current_verbosity);
//}
//
//// Get the address of the empty buffer starting at 'tail'
//UCHAR* getBuffer(HW_PRINTF_BUFFER* bufArr, UINT32 len)
//{
//    hw_cacheFlushAddr(&bufArr->head);
//    hw_cacheFlushAddr(&bufArr->tail);
//
//    UINT32 head, tail;
//
//    head = hw_read32(&bufArr->head);
//    tail = hw_read32(&bufArr->tail);
//
//    UINT32 tLen; // tLen = total length of strings waiting to be printed + length of this string
//
//    if(head <= tail)
//    {
//        tLen = tail - head;
//    }
//    else
//    {
//        tLen = tail + HW_PRINTF_BUF_SIZE - head;
//    }
//    tLen += len;
//
//    UCHAR* rp = bufArr->buffer + tail;
//
//    // Check for buffer overflow.
//    // HW_PRINTF_BUF_SIZE - 2 is the maximum
//    // string size since 1) we're not allowed
//    // to completely fill the buffer, there
//    // has to be at least one padding element
//    // and 2) if the string loops around the
//    // end of the array, an additional \0
//    // character is added.
//    if(tLen > HW_PRINTF_BUF_SIZE - 2)
//    {
//        rp = 0;
//    }
//
//    return rp;
//}
//
//BOOL verbosity_enabled(HW_PRINT_TYPE print_type)
//{
//    HW_TS_TESTDATA* p_testData = (HW_TS_TESTDATA*)hw_readptr(&testSlaveVars->testData);
//
//    //UINT32 testIdx = hw_read32(&testSlaveVars->dwCurrentXTest);
//    UINT32 current_verbosity = 0x0;
//    BOOL ret = TRUE;
//
//    ////
//    //// If the current test is "None", we are in
//    //// kernel code, so use the global verbosity.
//    ////
//    //if(testIdx == HW_NO_TEST_INDEX)
//    //{
//    //    current_verbosity = bifrostCachedGlobals.hw_global_verbosity;
//    //}
//    //else
//    //{
//    //    if(p_testData != NULL)
//    //    {
//    //        current_verbosity = hw_read32(&p_testData[testIdx].verbosity);
//    //    }
//    //}
//    //
//    //if(!(PRINT_TYPE(print_type) & current_verbosity))
//    //{
//    //    ret = FALSE;
//    //}
//
//    return ret;
//}
//
//void sendPrintMessage(UINT32 totalLen, UINT64 time)
//{
//    HW_TS_TESTDATA* testData = (HW_TS_TESTDATA*)hw_readptr(&testSlaveVars->testData);
//
//    UINT32 testID = hw_getMyInstanceID();
//    UINT32 current_verbosity;
//
//    //
//    // If the current test is "None", we are in internal
//    // test slave code, so use the global verbosity.
//    // If this is the driver, always use global verbosity.
//    //
//    if(testID == HW_INTERNAL_TESTID || hw_getCoreNum() == bifrostCachedGlobals.tpid)
//    {
//        current_verbosity = bifrostCachedGlobals.hw_global_verbosity;
//    }
//    else
//    {
//        //current_verbosity = hw_read32(&testData[testIdx].verbosity);
//        current_verbosity = 0;
//    }
//
//    HW_MESSAGE message;
//    message.metadata.command = MSG_TS_CMD_PRINTF;
//    message.metadata.size = 3 * sizeof(UINT64);
//    UINT64* datafield = (UINT64*) message.data;
//    datafield[0] = totalLen;
//    datafield[1] = time;
//    datafield[2] = testID;
//
//    //
//    // Notify driver
//    //
//    placeMessage(&testSlaveVars->driverLowPrioMailbox, &message);
//
//    //
//    // If print synchronization is on, flush the buffer
//    // before continuing so the prints we see are
//    // synchronized with test execution and not
//    // delayed due to testdriver processing.
//    //
//    if(current_verbosity & PRINT_TYPE(HW_PRINT_SYNC))
//    {
//        hw_flushMailbox();
//    }
//}
//
//void hw_printf( HW_PRINT_TYPE print_type, const char* fmt, ... )
//{
//    if(!verbosity_enabled(print_type))
//    {
//        return;
//    }
//
//    va_list   args;
//    va_start( args, fmt );
//
//    if(hw_getCoreNum() == bifrostCachedGlobals.tpid || hw_pTestConfigs->bfinit.DIRECT_PRINT_EN)
//    //
//    // If this thread is the driver, or we're
//    // in an environment where each core has direct
//    // print access, place the string in the
//    // internal print buffer and call the plat_puts
//    // function.
//    //
//    {
//        CHAR* buffer = (CHAR*)hw_readptr(&testSlaveVars->internalPrintBuffer);
//        int str_length = hw_vsnprintf( buffer, HW_PRINTF_BUF_SIZE, fmt, args );
//        plat_puts( buffer, str_length );
//    }
//    else
//    {
//        hw_vprintf( fmt, args );
//    }
//
//    va_end( args );
//}
//
//void ts_copyToPrintBuffer( HW_PRINTF_BUFFER* buffer, BOOL write_null, const char* str, int str_length)
//{
//    UINT32 tail;
//    CHAR* destBuffer;
//    const char* bp;
//
//    //
//    // Flush and update buffer variables
//    //
//    hw_cacheFlushAddr(&buffer->tail);
//    tail = hw_read32(&buffer->tail);
//
//    //
//    // If the buffer is overflowing, just wait
//    //
//    while(!(destBuffer = (CHAR*) getBuffer(buffer, str_length)));
//
//    //
//    // If the message wraps around the end of the buffer,
//    // break it into two chunks
//    //
//    if(tail + str_length > HW_PRINTF_BUF_SIZE)
//    {
//        // Calculate length of each chunk
//        UINT32 len1 = HW_PRINTF_BUF_SIZE - tail;
//        UINT32 len2 = str_length - len1;
//
//        //
//        // Copy from temp buffer into real buffer until we reach wrap point.
//        // Then flush
//        //
//        hw_strncpytomem(destBuffer, str, len1);
//        hw_cacheFlushBuffer(destBuffer, len1);
//
//        //
//        // We've copied the first chunk, now get ready to copy the second
//        //
//        bp = str + len1;
//
//        // If we're writing the null terminator, add one to len2 to account
//        // for that additional character, and subtract 1 from bp because
//        // len1 counts the null terminator
//        if(write_null)
//        {
//            bp--;
//            len2++;
//        }
//        // Otherwise, write the character at bp - 1 on top of the
//        // null terminator at the very end of the buffer
//        else
//        {
//            hw_write8((UINT8*)destBuffer + len1 - 1, *(bp - 1));
//        }
//
//        destBuffer = (CHAR*) buffer->buffer;
//
//        //
//        // Copy rest of temp buffer into real buffer
//        // and flush
//        //
//        hw_strncpytomem(destBuffer, bp, len2);
//        hw_cacheFlushBuffer(destBuffer, len2);
//
//        //
//        // Update tail pointer
//        //
//        tail = len2;
//    }
//    else
//    {
//        //
//        // Copy from temp buffer into real buffer
//        // and flush
//        //
//        hw_strncpytomem(destBuffer, str, str_length);
//        hw_cacheFlushBuffer(destBuffer, str_length);
//
//        //
//        // Update tail pointer
//        //
//        tail += str_length;
//
//        if(tail == HW_PRINTF_BUF_SIZE)
//        {
//            tail = 0;
//        }
//    }
//
//    // If we don't want the null character, move the
//    // tail back so that the receiver doesn't pick it up
//    if(!write_null && tail != 0)
//    {
//        tail--;
//    }
//
//    //
//    // Update and flush tail pointer
//    //
//    hw_write32(&buffer->tail, tail);
//    hw_cacheFlushAddr(&buffer->tail);
//}
//
//void ts_vprintf( HW_PRINTF_BUFFER* buffer, BOOL write_null, const char* fmt, va_list args )
//{
//    //
//    // HW_PRINTF_BUF_SIZE - 2 is the maximum
//    // string size since 1) we're not allowed
//    // to completely fill the buffer, there
//    // has to be at least one padding element
//    // and 2) if the string loops around the
//    // end of the array, an additional \0
//    // character is added.
//    //
//    UINT32 totalLen;
//    CHAR tempBuffer[HW_PRINTF_BUF_SIZE - 2];
//
//    // snap the time
//    UINT64 time = hw_getTime();
//
//    //
//    // snprintf the passed argument into a temporary buffer.
//    // Add one to totalLen to account for null terminator
//    //
//    totalLen = hw_vsnprintf( tempBuffer, _countof(tempBuffer), fmt, args ) + 1;
//
//    //
//    // Set critical section during usage of print buffer and mailbox
//    //
//    hw_irql_t prev_irql = hw_raise_irql(HW_IRQL_DISABLED_LEVEL);
//
//    ts_copyToPrintBuffer(buffer, write_null, tempBuffer, totalLen);
//
//    // Notify testdriver that print message
//    // is ready
//    sendPrintMessage(totalLen, time);
//
//    hw_lower_irql(prev_irql);
//}
//
//void hw_vprintf( const char* fmt, va_list args )
//{
//    hw_log(HW_EVENT_CHECKPOINT, (UINTPTR)"printf", (UINTPTR)"enter", 0, 0);
//    HW_PRINTF_BUFFER* buffer = &testSlaveVars->printBuffer;
//    ts_vprintf(buffer, TRUE, fmt, args);
//    hw_log(HW_EVENT_CHECKPOINT, (UINTPTR)"printf", (UINTPTR)"exit", 0, 0);
//}
//
////
//// A wrapper function to prepend a string
//// to a print message.
////
//void hw_printfWithPrefix( const char* prefix, HW_PRINT_TYPE print_type, const char* fmt, ... )
//{
//    if(!verbosity_enabled(print_type))
//    {
//        return;
//    }
//
//    char hw_printf_internal_buffer[ HW_PRINTF_BUF_SIZE ];
//    char* dest = hw_printf_internal_buffer;
//    SIZE_T size = _countof(hw_printf_internal_buffer);
//    SIZE_T len;
//
//    len = hw_snprintf( dest, size, prefix );
//    size -= len + 1;
//    dest += len;
//
//    va_list   args;
//    va_start( args, fmt );
//    hw_vsnprintf( dest, size, fmt, args );
//    va_end( args );
//
//    hw_printf(print_type, "%s", hw_printf_internal_buffer);
//}
//
