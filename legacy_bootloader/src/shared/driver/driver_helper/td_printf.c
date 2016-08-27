/*-----------------------------------------------------
 |
 |      td_printf.c
 |
 |  Contains print functions for the Bifrost test
 |  driver, which respond to the printf API called
 |  by tests.
 |
 |------------------------------------------------------
 |
 |    Copyright (C) 2011  Microsoft Corporation
 |    All Rights Reserved
 |    Confidential and Proprietary
 |
 |------------------------------------------------------
 */
 
#include <bifrost_private.h>

//
// hw_printf handler functions 
//

// Get the address of the null-terminated string starting at 'head'
UCHAR* getString(HW_PRINTF_BUFFER* bufArr)
{
    UINT32 head, tail;
    
    hw_cacheFlushAddr(&bufArr->head);
    hw_cacheFlushAddr(&bufArr->tail);
    head = hw_read32(&bufArr->head);
    tail = hw_read32(&bufArr->tail);
    
    if(head == tail)
    //
    // The buffer is empty.
    //
    {
        return 0;
    }
    
    //
    // Head should be pointing to the start
    // of the first non-processed string.
    //
    UCHAR* rp = bufArr->buffer + head;
    
    return rp;
}

void td_printf_reply(HW_MESSAGE tempIn, UINT32 dwpid)
{
    HW_PRINTF_BUFFER* bufArr = &testSlaveVarTable[dwpid].printBuffer;
    UINT64* datafield = (UINT64*) tempIn.data;
    
    CHAR buffer[HW_PRINTF_BUF_SIZE];
    
    UINT32 head;
    hw_cacheFlushAddr(&bufArr->head);
    head = hw_read32(&bufArr->head);
    
    CHAR* tempBuffer;
    UINT32 len1, len2;
    
    UINT32 expectedLen = datafield[0];
    UINT64 time = datafield[1];
    UINT32 testInstanceID = datafield[2];
    
    if(!(tempBuffer = (CHAR*) getString(bufArr)))
    //
    // Empty buffer. Since buffer only gets consumed when
    // td_printf_reply is called, and td_printf_reply should
    // only be called by hw_printf which populates the buffer,
    // this means that somehow we misfired a PRINTF message,
    // something is seriously wrong.
    //
    {
        hw_errmsg("ERROR %s: blank printf buffer! Head pointer: %d\n", __func__, head); 
        return;
    }
    
    // Check if the message comes in two parts
    // (because of buffer wraparound)
    BOOL twoPartMessage = FALSE;
    if(head + expectedLen > HW_PRINTF_BUF_SIZE)
    {
        // Calculate length of each chunk
        // Add one to len2 to account for an additional
        // null terminator
        len1 = HW_PRINTF_BUF_SIZE - head;
        len2 = expectedLen - len1 + 1;
        twoPartMessage = TRUE;
    }
    else
    {
        len1 = expectedLen;
        len2 = 0;
    }
    
    //
    // Invalidate cache and copy the buffer into a local buffer.
    //
    hw_cacheFlushBuffer(tempBuffer, len1);
    hw_strncpyfrommem(buffer, tempBuffer, len1);
    
    // If test ID == ~0u, this is a Bifrost internal message,
    // not from a test. So omit test ID
    if(testInstanceID == HW_INTERNAL_TESTID)
    {
        td_printf("Time %llu, core %d: %s", time, dwpid, buffer);
    }
    else
    {
        td_printf("Time %llu, core %d, test ID %d: %s", time, dwpid, testInstanceID, buffer);
    }
    
    // Check if it's a two-part message
    if(twoPartMessage)
    {
        tempBuffer = (CHAR*) bufArr->buffer;
        
        hw_cacheFlushBuffer(tempBuffer, len2);
        hw_strncpyfrommem(buffer, tempBuffer, len2);
        td_printf("%s", buffer);
        
        //
        // Update head pointer
        //
        head = len2;
    }
    else
    {
        //
        // Update head pointer
        //
        head += len1;
        head %= HW_PRINTF_BUF_SIZE;
    }
    
    //
    // Write and flush head pointer
    //
    hw_write32(&bufArr->head, head);
    hw_cacheFlushAddr(&bufArr->head);
}

int
td_printf( const char * fmt, ... )
{
    int       rc = 0;
    va_list   args;
    
    CHAR* buffer = (CHAR*)hw_readptr(&testSlaveVars->internalPrintBuffer);
    int str_length;
    
    va_start( args, fmt );
    
    str_length = hw_vsnprintf( buffer, HW_PRINTF_BUF_SIZE, fmt, args );
    
    va_end( args );
    
    rc = plat_puts(buffer, str_length);
    
    return rc;
}

CHAR td_internal_print_buffer[HW_PRINTF_BUF_SIZE] __attribute__ ((aligned(HW_MAX_CACHELINE_SIZE)));

