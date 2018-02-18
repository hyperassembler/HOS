#include "test/driver.h"
#include "lib/salloc.h"

typedef union
{
	uint32_t size;
	uint32_t flags;
} _salloc_header;

const uint32_t salloc_header_size = sizeof(_salloc_header);

static char buffer[1024];

static bool salloc_init_test(void)
{
	lb_salloc_init(buffer, 1024);
	uint32_t blk_size[] = {1024};
	bool blk_free[] = {true};
	return lb_salloc_assert(buffer, blk_size, blk_free, 1);
}

static bool salloc_basic_alloc(void)
{
	bool result = true;
	lb_salloc_init(buffer, 1024);
	result = result && (lb_salloc(buffer, 10) != NULL);
	uint32_t blk_size[] = {10 + salloc_header_size, 1024 - 10 - salloc_header_size};
	bool blk_free[] = {false, true};
	result = result && lb_salloc_assert(buffer, blk_size, blk_free, 2);
	return result;
}

static bool salloc_full_alloc(void)
{
	bool result = true;
	lb_salloc_init(buffer, 1024);
	result = result && (lb_salloc(buffer, 1024 - salloc_header_size) != NULL);
	uint32_t blk_size[] = {1024};
	bool blk_free[] = {false};
	result = result && lb_salloc_assert(buffer, blk_size, blk_free, 1);
	return result;
}

static bool salloc_overflow_alloc(void)
{
	bool result = true;
	lb_salloc_init(buffer, 1024);
	result = result && (lb_salloc(buffer, 1024 - salloc_header_size + 1) == NULL);
	uint32_t blk_size[] = {1024};
	bool blk_free[] = {true};
	result = result && lb_salloc_assert(buffer, blk_size, blk_free, 1);
	return result;
}

static bool salloc_multiple_alloc(void)
{
	bool result = true;
	lb_salloc_init(buffer, 1024);
	result = result && (lb_salloc(buffer, 10) != NULL);
	result = result && (lb_salloc(buffer, 10) != NULL);
	result = result && (lb_salloc(buffer, 10) != NULL);
	uint32_t blk_size[] = {10 + salloc_header_size,
	                       10 + salloc_header_size,
	                       10 + salloc_header_size,
	                       1024 - 3 * (10 + salloc_header_size)};
	bool blk_free[] = {false, false, false, true};
	result = result && lb_salloc_assert(buffer, blk_size, blk_free, 4);
	return result;
}

static bool salloc_alloc_not_enough(void)
{
	void *ptr;
	bool result = true;
	lb_salloc_init(buffer, salloc_header_size + salloc_header_size + salloc_header_size - 1);
	ptr = lb_salloc(buffer, salloc_header_size);
	result = result && (ptr != NULL);
	uint32_t blk_size[] = {salloc_header_size + salloc_header_size + salloc_header_size - 1};
	bool blk_free[] = {false};
	result = result && lb_salloc_assert(buffer, blk_size, blk_free, 1);
	return result;
}


static bool salloc_basic_free(void)
{
	void *ptr;
	bool result = true;
	lb_salloc_init(buffer, 1024);
	ptr = lb_salloc(buffer, 10);
	result = result && (ptr != NULL);

	lb_sfree(buffer, ptr);
	uint32_t blk_size[] = {1024};
	bool blk_free[] = {true};
	result = result && lb_salloc_assert(buffer, blk_size, blk_free, 1);
	return result;
}

static bool salloc_full_free(void)
{
	void *ptr;
	bool result = true;
	lb_salloc_init(buffer, 1024);
	ptr = lb_salloc(buffer, 1024 - salloc_header_size);
	result = result && (ptr != NULL);

	lb_sfree(buffer, ptr);
	uint32_t blk_size[] = {1024};
	bool blk_free[] = {true};
	result = result && lb_salloc_assert(buffer, blk_size, blk_free, 1);
	return result;
}

static bool salloc_multiple_free(void)
{
	void *ptr1, *ptr2, *ptr3, *ptr4;
	bool result = true;
	lb_salloc_init(buffer, 1024);
	ptr1 = lb_salloc(buffer, 10);
	ptr2 = lb_salloc(buffer, 10);
	ptr3 = lb_salloc(buffer, 10);
	ptr4 = lb_salloc(buffer, 10);
	result = result && (ptr1 != NULL) && (ptr2 != NULL) && (ptr3 != NULL) && (ptr4 != NULL);
	lb_sfree(buffer, ptr1);
	lb_sfree(buffer, ptr3);

	uint32_t blk_size[] = {10 + salloc_header_size,
	                       10 + salloc_header_size,
	                       10 + salloc_header_size,
	                       10 + salloc_header_size,
	                       1024 - 4 * (10 + salloc_header_size)};
	bool blk_free[] = {true, false, true, false, true};
	result = result && lb_salloc_assert(buffer, blk_size, blk_free, 5);
	return result;
}

static bool salloc_free_join_tail(void)
{
	void *ptr1, *ptr2, *ptr3, *ptr4;
	bool result = true;
	lb_salloc_init(buffer, 1024);
	ptr1 = lb_salloc(buffer, 10);
	ptr2 = lb_salloc(buffer, 10);
	ptr3 = lb_salloc(buffer, 10);
	ptr4 = lb_salloc(buffer, 10);
	result = result && (ptr1 != NULL) && (ptr2 != NULL) && (ptr3 != NULL) && (ptr4 != NULL);
	lb_sfree(buffer, ptr4);

	uint32_t blk_size[] = {10 + salloc_header_size,
	                       10 + salloc_header_size,
	                       10 + salloc_header_size,
	                       1024 - 3 * (10 + salloc_header_size)};
	bool blk_free[] = {false, false, false, true};
	result = result && lb_salloc_assert(buffer, blk_size, blk_free, 4);
	return result;
}

static bool salloc_free_join_head(void)
{
	void *ptr1, *ptr2, *ptr3, *ptr4;
	bool result = true;
	lb_salloc_init(buffer, 1024);
	ptr1 = lb_salloc(buffer, 10);
	ptr2 = lb_salloc(buffer, 10);
	ptr3 = lb_salloc(buffer, 10);
	ptr4 = lb_salloc(buffer, 10);
	result = result && (ptr1 != NULL) && (ptr2 != NULL) && (ptr3 != NULL) && (ptr4 != NULL);
	lb_sfree(buffer, ptr1);
	lb_sfree(buffer, ptr2);

	uint32_t blk_size[] = {2 * (10 + salloc_header_size),
	                       10 + salloc_header_size,
	                       10 + salloc_header_size,
	                       1024 - 4 * (10 + salloc_header_size)};
	bool blk_free[] = {true, false, false, true};
	result = result && lb_salloc_assert(buffer, blk_size, blk_free, 4);
	return result;
}

static bool salloc_free_join_mid(void)
{
	void *ptr1, *ptr2, *ptr3, *ptr4;
	bool result = true;
	lb_salloc_init(buffer, 1024);
	ptr1 = lb_salloc(buffer, 10);
	ptr2 = lb_salloc(buffer, 10);
	ptr3 = lb_salloc(buffer, 10);
	ptr4 = lb_salloc(buffer, 10);
	result = result && (ptr1 != NULL) && (ptr2 != NULL) && (ptr3 != NULL) && (ptr4 != NULL);
	lb_sfree(buffer, ptr2);
	lb_sfree(buffer, ptr3);

	uint32_t blk_size[] = {10 + salloc_header_size,
	                       2 * (10 + salloc_header_size),
	                       10 + salloc_header_size,
	                       1024 - 4 * (10 + salloc_header_size)};
	bool blk_free[] = {false, true, false, true};
	result = result && lb_salloc_assert(buffer, blk_size, blk_free, 4);
	return result;
}

static bool salloc_free_join_consecutive(void)
{
	void *ptr1, *ptr2, *ptr3, *ptr4, *ptr5;
	bool result = true;
	lb_salloc_init(buffer, 1024);
	ptr1 = lb_salloc(buffer, 10);
	ptr2 = lb_salloc(buffer, 10);
	ptr3 = lb_salloc(buffer, 10);
	ptr4 = lb_salloc(buffer, 10);
	ptr5 = lb_salloc(buffer, 10);
	result = result && (ptr1 != NULL) && (ptr2 != NULL) && (ptr3 != NULL) && (ptr4 != NULL) && (ptr5 != NULL);
	lb_sfree(buffer, ptr2);
	lb_sfree(buffer, ptr4);

	uint32_t blk_size[] = {10 + salloc_header_size,
	                       10 + salloc_header_size,
	                       10 + salloc_header_size,
	                       10 + salloc_header_size,
	                       10 + salloc_header_size,
	                       1024 - 5 * (10 + salloc_header_size)};
	bool blk_free[] = {false, true, false, true, false, true};
	result = result && lb_salloc_assert(buffer, blk_size, blk_free, 6);

	lb_sfree(buffer, ptr3);

	uint32_t blk_size2[] = {10 + salloc_header_size,
	                        3 * (10 + salloc_header_size),
	                        10 + salloc_header_size,
	                        1024 - 5 * (10 + salloc_header_size)};
	bool blk_free2[] = {false, true, false, true};
	result = result && lb_salloc_assert(buffer, blk_size2, blk_free2, 4);
	return result;
}

static bool salloc_free_all(void)
{
	void *ptr1, *ptr2, *ptr3, *ptr4;
	bool result = true;
	lb_salloc_init(buffer, 1024);
	ptr1 = lb_salloc(buffer, 10);
	ptr2 = lb_salloc(buffer, 10);
	ptr3 = lb_salloc(buffer, 10);
	ptr4 = lb_salloc(buffer, 10);
	result = result && (ptr1 != NULL) && (ptr2 != NULL) && (ptr3 != NULL) && (ptr4 != NULL);
	lb_sfree(buffer, ptr1);
	lb_sfree(buffer, ptr2);
	lb_sfree(buffer, ptr3);
	lb_sfree(buffer, ptr4);

	uint32_t blk_size[] = {1024};
	bool blk_free[] = {true};
	result = result && lb_salloc_assert(buffer, blk_size, blk_free, 1);
	return result;
}


void KABI salloc_test(void)
{
	test_begin("salloc test");

	run_case("salloc_init_test", salloc_init_test());

	run_case("salloc_basic_alloc", salloc_basic_alloc());
	run_case("salloc_full_alloc", salloc_full_alloc());
	run_case("salloc_overflow_alloc", salloc_overflow_alloc());
	run_case("salloc_multiple_alloc", salloc_multiple_alloc());
	run_case("salloc_alloc_not_enough", salloc_alloc_not_enough());

	run_case("salloc_basic_free", salloc_basic_free());
	run_case("salloc_full_free", salloc_full_free());
	run_case("salloc_multiple_free", salloc_multiple_free());
	run_case("salloc_free_join_tail", salloc_free_join_tail());
	run_case("salloc_free_join_head", salloc_free_join_head());
	run_case("salloc_free_join_mid", salloc_free_join_mid());
	run_case("salloc_free_join_consecutive", salloc_free_join_consecutive());
	run_case("salloc_free_all", salloc_free_all());

	test_end();
}