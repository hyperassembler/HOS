/*-----------------------------------------------------
 |
 |      td_sendmessage.c
 |
 |  Contains messaging functions for the Bifrost test
 |  driver, which respond to the sendMessage API called
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
 
#include "bifrost_private.h"

//
// hw_sendMessage handler function
//

void td_sendMessage_reply(HW_MESSAGE message)
{
    UINT32 i, j;
    UINT32 slaveTestID;
    BOOL found = FALSE;
    HW_MAILBOX* box;
    HW_TS_TESTDATA* testData;
    
    //
    // Examine message's 'to' field to determine
    // recipient.
    //
    HW_TESTID testID = message.metadata.to;
    
    //
    // Search each test slaves's test list
    // to find where this test's mailbox is
    //
    for(i = 0; i < HW_PROC_CNT && found == FALSE; i++)
    {
        for(j = 0; j < td_ts_slaveData[i].testCnt && found == FALSE; j++)
        {
            if(td_ts_slaveData[i].testData[j].testInstanceID == testID)
            {
                found = TRUE;
            }
        }
    }
    
    if(!found)
    {
        hw_errmsg("ERROR: %s: test ID %d not found in records.  Make sure you are sending the message to the proper test ID! \n", __func__, testID);
        hw_assert(0); // Kill everything because obviously the test writer did something bad
    }
    else
    {
        HW_TS_PUBLIC_TESTDATA* publicTestData = (HW_TS_PUBLIC_TESTDATA*)hw_readptr(&testSlaveVarTable[i - 1].publicTestData);
        box = &publicTestData[j - 1].mailbox;
        
        //
        // Send message.
        //
        placeMessage(box, &message);
    }
}

