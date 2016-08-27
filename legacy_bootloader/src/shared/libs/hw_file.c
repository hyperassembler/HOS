/*-----------------------------------------------------
 |
 |      hw_file.c
 |
 |  Contains functions for Bifrost "file handling"
 |
 |------------------------------------------------------
 |
 |    Copyright (C) 2013  Microsoft Corporation
 |    All Rights Reserved
 |    Confidential and Proprietary
 |
 |------------------------------------------------------
 */

#include "bifrost_private.h"

HW_FILE* hw_fopen(const char* filename, const char* mode)
{
    HW_FILE* ret = NULL;
    BOOL continueModeCheck = TRUE;
    UINT32 modeLen = 0;
    UINT32 globalIdx = hw_getGlobalIdx();
    
    UINT64 namehash = hw_strhash(filename);
    
    HW_FILE** p_local_file_list = hw_file_lists[globalIdx];
    
    // Search through the entire file list until
    // we find a file with a matching name.
    for(UINT32 ctr = 0; ctr < hw_file_counts[globalIdx]; ctr++)
    {
        if(namehash == p_local_file_list[ctr]->namehash)
        {
            ret = p_local_file_list[ctr];
            
            // Check access mode. First character is special
            modeLen = hw_strnlen(mode, 4);
            switch(mode[0])
            {
            case 'r':
                ret->state = FILE_OPENED_R;
                break;
            case 'w':
                ret->state = FILE_OPENED_W;
                break;
            case 'a':
                ret->state = FILE_OPENED_A;
                break;
            default:
                hw_errmsg("%s: invalid mode %s!\n", __func__, mode);
                continueModeCheck = FALSE;
                break;
            }
            
            for(UINT32 ctr = 1; ctr < modeLen; ctr++)
            {
                switch(mode[ctr])
                {
                // 'b' access mode doesn't mean anything in Bifrost
                case 'b':
                    break;
                case '+':
                    ret->plus = TRUE;
                    break;
                default:
                    hw_errmsg("%s: invalid mode %s!\n", __func__, mode);
                    continueModeCheck = FALSE;
                    break;
                }
            }
            
            break;
        }
    }
    
    if(ret == NULL)
    {
        hw_errmsg("%s: no match for file name %s!\n", __func__, filename);
    }
    
    return ret;
}

INT32 hw_fclose(HW_FILE* file)
{
    INT32 ret = EOF;
    
    if(file != NULL)
    {
        if(file->state != FILE_NOT_OPENED)
        {
            file->state = FILE_NOT_OPENED;
            ret = 0;
        }
        else
        {
            hw_errmsg("%s: file %s is unopened!\n", __func__, file->name);
        }
    }
    else
    {
        hw_errmsg("%s: file pointer is NULL!\n", __func__);
    }
    
    return ret;
}

UCHAR* hw_fgetbuf(HW_FILE* file)
{
    UCHAR* ret = NULL;
    
    if(file != NULL)
    {
        if(file->state != FILE_NOT_OPENED)
        {
            ret = file->arr;
        }
        else
        {
            hw_errmsg("%s: file %s is unopened!\n", __func__, file->name);
        }
    }
    else
    {
        hw_errmsg("%s: file pointer is NULL!\n", __func__);
    }
    
    return ret;
}

SIZE_T hw_fgetsize(HW_FILE* file)
{
    SIZE_T ret = 0;
    
    if(file != NULL)
    {
        if(file->state != FILE_NOT_OPENED)
        {
            ret = file->size;
        }
        else
        {
            hw_errmsg("%s: file %s is unopened!\n", __func__, file->name);
        }
    }
    else
    {
        hw_errmsg("%s: file pointer is NULL!\n", __func__);
    }
    
    return ret;
}

