/*-----------------------------------------------------
 |
 |      hw_get_param.c
 |
 |  Provides a function for retrieving
 |  Bifrost parameters efficiently.
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

//
// Given a parameter key, retrieve the HW_PARAM
// struct associated with it. Returns TRUE if
// a match was found for the key, otherwise FALSE.
//
BOOL getParamInternal(UINT32 key, HW_PARAM* param)
{
    HW_TS_TESTDATA* testData = (HW_TS_TESTDATA*)hw_readptr(&testSlaveVars->testData);

    UINT32 dwCurrentXTest = hw_read32(&testSlaveVars->dwCurrentXTest);
    UINT32 paramCount = hw_read32(&testData[dwCurrentXTest].paramCount);
    HW_PARAM* paramPtr = (HW_PARAM*)hw_readptr(&testData[dwCurrentXTest].paramPtr);

    UINT32 testID = hw_getMyInstanceID();

    UINT32 lowerBound = 0;
    UINT32 upperBound = paramCount;
    UINT32 index = (upperBound + lowerBound) / 2;
    UINT32 curKey = 0xDEADBEEF;

    BOOL found = FALSE;
    while(!found && lowerBound < upperBound)
    {
        curKey = hw_read32(&paramPtr[index].key);
        if(key == curKey)
        {
            param->raw = hw_read64(&paramPtr[index].raw);
            found = TRUE;
        }
        else
        {
            if(key < curKey)
            {
                upperBound = index;
            }
            else
            {
                lowerBound = index + 1;
            }
            index = (upperBound + lowerBound) / 2;
        }
    }

    return found;
}

//
// Given a parameter key, retrieves the value
// of the parameter as a UINT32 and stores it
// into <val>. Returns TRUE if a match was found
// for the key, or raises a fatal error if no match
// was found.
//
BOOL hw_getParam(UINT32 key, UINT32* val)
{
    HW_PARAM param;
    param.raw = 0x0;

    BOOL ret = getParamInternal(key, &param);

    if(!ret)
    {
        hw_errmsg("%s: no match found for key %d!\n", __func__, key);

        //
        // Raise a fatal error if the parameter was not found
        //
        hw_fatal();
    }
    else
    {
        *val = param.i;
    }

    return ret;
}

//
// Given a parameter key, retrieves the value
// of the parameter as a FLOAT32 and stores it
// into <val>. Returns TRUE if a match was found
// for the key, or raises a fatal error if no match
// was found.
//
BOOL hw_getParam(UINT32 key, FLOAT32* val)
{
    HW_PARAM param;
    param.raw = 0x0;

    BOOL ret = getParamInternal(key, &param);

    if(!ret)
    {
        hw_errmsg("%s: no match found for key %d!\n", __func__, key);

        //
        // Raise a fatal error if the parameter was not found
        //
        hw_fatal();
    }
    else
    {
        *val = param.f;
    }

    return ret;
}

//
// Given a parameter key, retrieves the value
// of the parameter as four characters and stores it
// into <val>. Returns TRUE if a match was found
// for the key, or raises a fatal error if no match
// was found. Adds a NULL character at the end of
// the character array.
//
BOOL hw_getParam(UINT32 key, char* val)
{
    HW_PARAM param;
    param.raw = 0x0;

    BOOL ret = getParamInternal(key, &param);

    if(!ret)
    {
        hw_errmsg("%s: no match found for key %d!\n", __func__, key);

        //
        // Raise a fatal error if the parameter was not found
        //
        hw_fatal();
    }
    else
    {
        *(UINT32*)val = param.i;
    }

    val[4] = '\0';

    return ret;
}

//
// Given a parameter key, retrieves the value
// of the parameter as a UINT32 and stores it
// into <val>. Returns TRUE if a match was found
// for the key, otherwise returns false and
// stores <def_val> into <val>.
//
BOOL hw_getParam(UINT32 key, UINT32* val, UINT32 def_val)
{
    HW_PARAM param;
    param.raw = 0x0;

    BOOL ret = getParamInternal(key, &param);

    if(!ret)
    {
        *val = def_val;
    }
    else
    {
        *val = param.i;
    }

    return ret;
}

//
// Given a parameter key, retrieves the value
// of the parameter as a FLOAT32 and stores it
// into <val>. Returns TRUE if a match was found
// for the key, otherwise returns false and
// stores <def_val> into <val>.
//
BOOL hw_getParam(UINT32 key, FLOAT32* val, FLOAT32 def_val)
{
    HW_PARAM param;
    param.raw = 0x0;

    BOOL ret = getParamInternal(key, &param);

    if(!ret)
    {
        *val = def_val;
    }
    else
    {
        *val = param.f;
    }

    return ret;
}

//
// Given a parameter key, retrieves the value
// of the parameter as four characters and stores it
// into <val>. Returns TRUE if a match was found
// for the key, otherwise returns false and
// stores <def_val> into <val>. Adds a NULL
// character at the end of the character array.
//
BOOL hw_getParam(UINT32 key, char* val, char* def_val)
{
    HW_PARAM param;
    param.raw = 0x0;

    BOOL ret = getParamInternal(key, &param);

    if(!ret)
    {
        *(UINT32*)val = *(UINT32*)def_val;
    }
    else
    {
        *(UINT32*)val = param.i;
    }

    val[4] = '\0';

    return ret;
}

//
// Returns the number of params that the current
// test has.
//
UINT32 hw_getParamCount()
{
    HW_TS_TESTDATA* testData = (HW_TS_TESTDATA*)hw_readptr(&testSlaveVars->testData);

    UINT32 dwCurrentXTest = hw_read32(&testSlaveVars->dwCurrentXTest);
    UINT32 paramCount = hw_read32(&testData[dwCurrentXTest].paramCount);

    return paramCount;
}

//
// Given a parameter index, returns the key
// of the parameter at that index
//
UINT32 hw_getKeyAtIdx(UINT32 index)
{
    HW_TS_TESTDATA* testData = (HW_TS_TESTDATA*)hw_readptr(&testSlaveVars->testData);

    UINT32 dwCurrentXTest = hw_read32(&testSlaveVars->dwCurrentXTest);
    UINT32 paramCount = hw_read32(&testData[dwCurrentXTest].paramCount);
    HW_PARAM* paramPtr = (HW_PARAM*)hw_readptr(&testData[dwCurrentXTest].paramPtr);

    UINT32 ret;

    if(index >= paramCount)
    {
        hw_errmsg("%s: index %d is out of bounds (maximum index is %d)!\n", __func__, index, paramCount - 1);
        ret = 0xDEADBEEF;
    }
    else
    {
        ret = hw_read32(&paramPtr[index].key);
    }

    return ret;
}

//
// V3 Parameters
//

BifrostTestParams* hw_initTestParams(BifrostTestParams* pParams)
{
    if(!pParams->init_done())
    {
        // initialize members
        pParams->initParams();
        pParams->yamlParams();
        pParams->finalize();

        // set init_done
        pParams->init_done(1);
    }
    return pParams;
}

BifrostTestVars* hw_initTestVars(BifrostTestVars* pVars)
{
    if(!pVars->init_done())
    {
        // initialize members
        pVars->initVars();

        // set init_done
        pVars->init_done(1);
    }
    return pVars;
}
