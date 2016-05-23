#include "hal_print.h"
#include "k_test_driver.h"
#include "k_type.h"
#include "hal_mem.h"

#define GAT_SIZE 256
#define CASE_NUM 32

typedef struct
{
    char *case_name;
    bool success;
    bool used;
} case_info;

static case_info ginfo[CASE_NUM];
static void *gat[GAT_SIZE];
static char *test_name;

static void test_info()
{
    hal_printf("[TD-INFO][%s] - ", test_name);
}

static void test_warning()
{
    hal_printf("[TD-WARN][%s] - ", test_name);
}

static void test_error()
{
    hal_printf("[TD-ERR][%s] - ", test_name);
}

static void gat_push(void *ptr)
{
    for (int i = 0; i < GAT_SIZE; i++)
    {
        if (gat[i] == NULL)
        {
            gat[i] = ptr;
            return;
        }
    }
}

static bool gat_full()
{
    for (int i = 0; i < GAT_SIZE; i++)
    {
        if (gat[i] == NULL)
        {
            return false;
        }
    }
    return true;
}

static void gat_free()
{
    for (int i = 0; i < GAT_SIZE; i++)
    {
        if (gat[i] != NULL)
        {
            hfree(gat[i]);
            gat[i] = NULL;
        }
    }
}

static void ginfo_push(char *case_name, bool success)
{
    char *r_case_name = (case_name == NULL ? "Anonymous Case" : case_name);
    for (int i = 0; i < CASE_NUM; i++)
    {
        if (!ginfo[i].used)
        {
            ginfo[i].case_name = r_case_name;
            ginfo[i].success = success;
            ginfo[i].used = true;
            return;
        }
    }
    test_warning();
    hal_printf("GINFO full, [%s] result not recorded.\n", r_case_name);
}

void SAPI test_begin(char *name)
{
    test_name = (name == NULL ? "Anonymous Test" : name);
    for (int i = 0; i < GAT_SIZE; i++)
    {
        gat[i] = NULL;
    }
    for (int i = 0; i < CASE_NUM; i++)
    {
        ginfo[i].used = false;
    }
}

void SAPI test_end()
{
    gat_free();
    int32_t total = 0, failed = 0, success = 0;
    for (int i = 0; i < CASE_NUM; i++)
    {
        if (ginfo[i].used)
        {
            total++;
            if (ginfo[i].success)
            {
                success++;
            }
            else
            {
                failed++;
            }
        }
    }
    test_info();
    hal_printf("%s\n", failed > 0 ? "FAIL" : "PASS");
    hal_printf("    %d cases executed. S: %d. F: %d.\n", total, success, failed);
    if (failed > 0)
    {
        for (int i = 0; i < CASE_NUM; i++)
        {
            if (ginfo[i].used && !ginfo[i].success)
            {
                hal_printf("        %s FAILED\n", ginfo[i].case_name);
            }
        }
    }
    for (int i = 0; i < CASE_NUM; i++)
    {
        ginfo[i].used = false;
    }
}

void SAPI *talloc(uint32_t size)
{
    if (!gat_full())
    {
        void *result = halloc(size);
        gat_push(result);
        return result;
    }
    else
    {
        test_error();
        hal_printf("GAT full, rejecting further allocations.\n");
    }
    return NULL;
}

void SAPI run_case(char *name, bool result)
{
    ginfo_push(name, result);
}



