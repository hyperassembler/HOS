/*-------------------------------------------------------
 |
 |     bifrost_mailbox.h
 |
 |  Contains structs, enums, and internal functions for
 |  Bifrost mailbox messaging.
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2011  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#ifndef _BIFROST_MAILBOX_H
#define _BIFROST_MAILBOX_H

#define MSG_TD_CMD_BASE                 0
#define MSG_TS_CMD_BASE                 128
typedef enum
{
    //
    // Test driver to test slave message types
    //
    MSG_TD_ACK_CMD = MSG_TD_CMD_BASE,
    MSG_TD_CMD_MESSAGE,
    MSG_TD_CMD_TERMINATE,
    MSG_TD_CMD_LOCK_ALLOCATED,
    MSG_TD_CMD_LOCK_PENDING,
    MSG_TD_CMD_LOCK_SUCCESS,
    MSG_TD_CMD_BARRIER_WAITING,
    MSG_TD_CMD_BARRIER_PASS,
    
    //
    // Test slave to test driver message types
    //
    
    //
    // Acknowledges receipt of command from test driver to
    // start processing of the given state
    //
    MSG_TS_ACK_TEST_SETUP = MSG_TS_CMD_BASE,
    MSG_TS_ACK_INIT,
    MSG_TS_ACK_RUN,
    MSG_TS_ACK_CLEANUP,
    MSG_TS_ACK_CMD,
    
    //
    // Bifrost API commands that must be routed from the test slave
    // back to the test driver
    //
    MSG_TS_CMD_MESSAGE,
    MSG_TS_CMD_ALLOC,
    MSG_TS_CMD_FREE,
    MSG_TS_CMD_FREE_ALL,
    MSG_TS_CMD_PRINTF,
    MSG_TS_CMD_FLUSH_MAILBOX,
    MSG_TS_CMD_REG_INT,
    MSG_TS_CMD_CLEAR_INT,
    MSG_TS_CMD_READ_INT,
    MSG_TS_CMD_SEM_SIGNAL,
    MSG_TS_CMD_SEM_WAIT,
    MSG_TS_CMD_MSI,
    MSG_TS_CMD_SUSPEND_TD,
    MSG_TS_CMD_STANDBY,
    
    //
    // Message sent from the test slave to the test driver indicating
    // completion of a given state
    //
    MSG_TS_DONE_TEST_SETUP,
    MSG_TS_DONE_INIT,
    MSG_TS_DONE_RUN,
    MSG_TS_DONE_CLEANUP,
    MSG_TS_EXIT,
    
    //
    // Message sent when the core has crashed
    //
    MSG_TS_FATAL
} HW_MSG_COMMAND;

//
// Struct sizes
//
#define HW_MSG_METADATA_SIZE            ((4 * sizeof(UINT32)))
#define HW_MESSAGE_SIZE                 (128)
#define MSG_DATASIZE                    (HW_MESSAGE_SIZE - sizeof(HW_MSG_METADATA))
#define HW_MAILBOX_SIZE                 (roundUpToMultipleUnsigned((2 * sizeof(PTR_FIELD)) + (HW_MESSAGE_SIZE * HW_MB_MAX_SIZE), HW_MAX_CACHELINE_SIZE))


//
// Header data in a message packet
//
typedef struct {
    HW_TESTID from;
    HW_TESTID to;
    HW_MSG_COMMAND command;
    UINT32 size;
} __attribute__ ((packed)) HW_MSG_METADATA;

//
// Full message which includes data
//
typedef struct {
    HW_MSG_METADATA metadata;
    UCHAR data[MSG_DATASIZE];
} __attribute__ ((packed)) HW_MESSAGE;

//
// Mailbox
//
typedef struct {
    PTR_FIELD headPtr;
    PTR_FIELD tailPtr;
    HW_MESSAGE messages[HW_MB_MAX_SIZE] __attribute__ ((aligned(HW_MAX_CACHELINE_SIZE)));
} __attribute__ ((packed, aligned (HW_MAX_CACHELINE_SIZE))) HW_MAILBOX;

typedef struct {
    UINT32 head;
    UINT32 tail;
} HW_MAILBOX_IDXS;

typedef enum {
    HW_DRIVER_MAILBOX,
    HW_DRIVER_LOW_PRIO_MAILBOX,    
    HW_SLAVE_MAILBOX,
    HW_SLAVE_LOW_PRIO_MAILBOX,
    HW_MAILBOX_ID_CNT
} HW_MAILBOX_ID;

extern MEM_SRAM_UC HW_MAILBOX_IDXS mailboxIdxs[HW_PROC_CNT][HW_MAILBOX_ID_CNT];
extern HW_MAILBOX_IDXS mailboxIdxs_dram[HW_PROC_CNT][HW_MAILBOX_ID_CNT];

HW_RESULT placeMessage(HW_MAILBOX *box, HW_MESSAGE* send);
BOOL getMessage(HW_MAILBOX *box, HW_MESSAGE *get);

#endif
