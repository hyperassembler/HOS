/*-------------------------------------------------------
 |
 |  platform_init.c
 |
 |  Platform specific initialization for linux
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2013  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
*/

#include <pthread.h>
#include <time.h>

#include <stdio.h>
#include <stddef.h>
#include <unistd.h>
#include <bifrost_private.h>

#define LINUX_TIMEOUT_SECONDS (10)

//
// Pthread barrier to prevent program exit
// until all threads have fully finished.
//
static pthread_barrier_t barr;

//
// Thread-unique identifier for Linux port
//
__thread UINT32 thread_pid;

void* spawnThread(void* arg)
{
    unsigned int* pid = (unsigned int*) arg;
    thread_pid = *pid;
    
    // Check if this core is active
    if(hw_activeCoreList[thread_pid])
    {
        //
        // Init globals that would typically be at fixed locations in the
        // system memory map
        //
        initGlobals(); // if the file access failed
        
        if(thread_pid == bifrostCachedGlobals.tpid)
        {
            testDriver(*pid);
        }
        else
        {
            testSlave(*pid);
        }
    }
    
    pthread_barrier_wait(&barr);
    return NULL;
}

int platform_init()
{
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
    
    UINT32 rc, i;
    pthread_t threads[HW_PROC_CNT];
    int thread_args[HW_PROC_CNT];
    
    //
    // Spawn pthreads
    //
    pthread_barrier_init( &barr, NULL, HW_PROC_CNT);
    printf("Spawn %u threads\n", HW_PROC_CNT);
    for (i = 0; i < HW_PROC_CNT; i++)
    {
        thread_args[i] = i;
        rc = pthread_create(&threads[i], NULL, spawnThread, (void*)&thread_args[i]);
    }
    
    // Set timeout for thread join
    const timespec curTime = {time(NULL) + LINUX_TIMEOUT_SECONDS, 0};
    
    //
    // Check for completion from threads
    //
    for (i = 0; i < HW_PROC_CNT; i++)
    {
        rc = pthread_timedjoin_np(threads[i], NULL, &curTime);
    }
    
    pthread_barrier_destroy(&barr);
    
    return 0;
}

