/*-------------------------------------------------------
 |
 |  bifrost_api.h
 |
 |  Bifrost API declarations
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2011-2013  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#ifndef _BIFROST_API_H
#define _BIFROST_API_H

#include <bifrost_mem.h>

//
// Print functions
//
EXTERN_C void hw_printf( HW_PRINT_TYPE print_type, const char* format, ... );
EXTERN_C void hw_printfWithPrefix( const char* prefix, HW_PRINT_TYPE print_type, const char* format, ... );
EXTERN_C void hw_vprintf( const char* fmt, va_list args );

EXTERN_C int
hw_vsnprintf( char* outstring,
              UINT32 bufsize,
              const char* fmt,
              va_list args );
EXTERN_C int hw_snprintf( char* outstring, UINT32 size, const char* fmt, ... );

EXTERN_C void hw_set_verbosity( UINT32 verbosity );
EXTERN_C void hw_set_print_synchronization(BOOL value);

//
// printf macros for printing by type.
// Should match the HW_PRINT_TYPE enum in bifrost_types_constants.h
//
#define hw_critical(...)          hw_printf(HW_CRITICAL, __VA_ARGS__)
#define hw_status(...)            hw_printf(HW_STATUS, __VA_ARGS__)
#define hw_debug(...)             hw_printf(HW_DEBUG, __VA_ARGS__)
#define hw_verbose(...)           hw_printf(HW_VERBOSE, __VA_ARGS__)

// hw_errmsg is a special case which prints
// an error and also updates the test's error
// count, if applicable
#define hw_errmsg(...)            do {                                                            \
                                      hw_incrementErrorCnt();                                     \
                                      hw_printfWithPrefix("ERROR: ", HW_CRITICAL, __VA_ARGS__);   \
                                  } while(0)


//
// Flush a testslave's outgoing mailbox
//
EXTERN_C void hw_flushMailbox();

//
// Test timeout
//
EXTERN_C void hw_setTimeout( UINT64 value );

//
// Sleep for a given number of nanoseconds
//
EXTERN_C int hw_sleep(UINT64 nanoseconds);

//
// Suspend test driver for a given number of cycles
//
EXTERN_C BOOL hw_suspend_td(UINT32 cycles);

//
// Critical Section Protection
//
EXTERN_C HW_INT_VECTOR_MASK hw_criticalSectionBegin();
EXTERN_C VOID hw_criticalSectionEnd(HW_INT_VECTOR_MASK intMask);

//
// Interrupt support
//
#ifdef __cplusplus
EXTERN_C HW_RESULT hw_int_registerHandler(HW_INT_VECTOR interrupt, HW_INT_HANDLER_FUNC* handler, VOID* buffer = NULL, BOOL preemptive = TRUE, UINT32 core = HW_CORE_DEFAULT);
EXTERN_C HW_RESULT hw_int_deregisterHandler(HW_INT_VECTOR interrupt, UINT32 core = HW_CORE_DEFAULT);
EXTERN_C VOID hw_int_deregisterAllHandlers();
EXTERN_C HW_INT_VECTOR_MASK hw_int_enable(HW_INT_VECTOR_MASK mask);
EXTERN_C HW_INT_VECTOR_MASK hw_int_disable(HW_INT_VECTOR_MASK mask);
EXTERN_C HW_RESULT hw_int_set(HW_INT_VECTOR_MASK mask);
EXTERN_C HW_RESULT hw_int_clear(HW_INT_VECTOR_MASK mask);
EXTERN_C HW_INT_VECTOR_MASK hw_int_queryInterrupt();
EXTERN_C HW_INT_VECTOR_MASK hw_int_queryEnable();
EXTERN_C HW_RESULT hw_int_timerSetTimeout(HW_INT_VECTOR timer, UINT32 timeout);
EXTERN_C HW_RESULT hw_int_timerSetCompare(HW_INT_VECTOR timer, UINT32 compare);
#endif

//
// Exception support
//
#ifdef __cplusplus
EXTERN_C HW_RESULT hw_exc_registerHandlerGeneric(HW_EXC_VECTOR exception, HW_EXC_HANDLER_FUNC* handler, BOOL preemptive);
#define hw_exc_registerHandler(exception, handler) hw_exc_registerHandlerGeneric(exception, handler, TRUE)
EXTERN_C HW_RESULT hw_exc_deregisterHandler(HW_EXC_VECTOR exception);
EXTERN_C VOID hw_exc_deregisterAllHandlers();
#endif // __cplusplus

//
// Poll support
//
#ifdef __cplusplus
EXTERN_C HW_RESULT hw_register_poll_handler(HW_POLL_CB_FUNC* poll_cb, HW_POLL_HANDLER_CB_FUNC* handler_cb, VOID* cb_buffer = NULL, UINT32 core = HW_CORE_DEFAULT);
EXTERN_C HW_RESULT hw_deregister_poll_handler(HW_POLL_CB_FUNC* poll_cb, VOID* cb_buffer = NULL, UINT32 core = HW_CORE_DEFAULT);
#endif // __cplusplus

//
// Memory functions
//
EXTERN_C void hw_memset( void* ptr, UINT8 value, SIZE_T len );
EXTERN_C INT32 hw_memcmp( const void* ptr1, const void* ptr2, const SIZE_T len );
EXTERN_C void hw_memcpy( void* destination, const void* source, SIZE_T n );
EXTERN_C void hw_memcpyAligned32( void* destination, const void* source, SIZE_T n );
EXTERN_C void hw_memcpytormem( void* destination, const void* source, SIZE_T n );
EXTERN_C void hw_memcpyfromrmem( void* destination, const void* source, SIZE_T n );
EXTERN_C void hw_memcpyfromrmemtormem( void* destination, const void* source, SIZE_T n );

//
// Heap allocation functions
//
EXTERN_C void* hw_allocAlignedWithType( HW_HEAP_TYPE type, SIZE_FIELD size, PTR_FIELD pattern, PTR_FIELD mask);
#define hw_allocAligned(size, pattern, mask) hw_allocAlignedWithType(HW_HEAP_TYPE_DEFAULT, size, pattern, mask)
#define hw_allocWithType(type, size) hw_allocAlignedWithType(type, roundUpToMultipleUnsigned(size,HW_CACHELINE_SIZE), 0, HW_CACHELINE_SIZE - 1)
#define hw_alloc(size) hw_allocWithType(HW_HEAP_TYPE_DEFAULT, size)

EXTERN_C void hw_freeWithType(HW_HEAP_TYPE type, void* addr);
EXTERN_C void hw_freeAllWithType(HW_HEAP_TYPE type);
#define hw_free(addr) hw_freeWithType(HW_HEAP_TYPE_DEFAULT, addr)
#define hw_freeAll() hw_freeAllWithType(HW_HEAP_TYPE_DEFAULT)

EXTERN_C void hw_setAllocFailuresNonFatal();


//
// Utility functions
//
EXTERN_C UINT32 hw_swap_endian32(UINT32 inword);
EXTERN_C UINT16 hw_swap_endian16(UINT16 inshort);
EXTERN_C BOOL hw_am_i_big_endian( void );

//
// Test status
//
#ifdef __cplusplus
EXTERN_C void hw_setTestState(HW_TEST_RUN_STATE state, UINT32 core = HW_CORE_DEFAULT, UINT32 testIdx = HW_NO_TEST_INDEX);
EXTERN_C void hw_doneBackend(HW_TEST_RUN_STATE dwCondition, const char* func, UINT32 runEndConditions = 0);
#define hw_done(code, ...) hw_doneBackend(code, __func__, ##__VA_ARGS__)
#endif // __cplusplus
EXTERN_C void hw_setIterationCount(UINT32 count);
EXTERN_C void hw_fatalBackend(const char* func);
#define hw_fatal() hw_fatalBackend(__func__)
EXTERN_C void hw_incrementErrorCnt();
EXTERN_C UINT32 hw_getErrorCnt();

//
// String operations
//
EXTERN_C SIZE_T hw_strnlen( const char* s, SIZE_T maxlen );
EXTERN_C char* hw_strncpy( char* s1, const char* s2, SIZE_T n );
EXTERN_C char* hw_strncpytomem( char* s1, const char* s2, SIZE_T n );
EXTERN_C char* hw_strncpyfrommem( char* s1, const char* s2, SIZE_T n );
EXTERN_C char* hw_strncat( char* s1, const char* s2, SIZE_T n );
EXTERN_C INT32 hw_strcmp( const char* str1, const char* str2 );
EXTERN_C UINT64 hw_strhash(const char* s);

//
// String conversion functions
//
EXTERN_C UINT32 hw_strtoul( const char* nsptr, char** endptr, int base );
EXTERN_C INT32 hw_strtol( const char* nsptr, char** endptr, int base );
EXTERN_C INT32 hw_atoi( const char* nsptr );
EXTERN_C FLOAT32 hw_strtof( const char* str, char** endptr );

//
// Char set tests
//
EXTERN_C BOOL hw_isupper(UCHAR ch);
EXTERN_C BOOL hw_islower(UCHAR ch);
EXTERN_C BOOL hw_isdigit(UCHAR ch);
EXTERN_C BOOL hw_isspace(UCHAR ch);

//
// Messaging
//
EXTERN_C void hw_sendMessage(HW_MESSAGE* message, HW_TESTID destinationID);
EXTERN_C void hw_broadcastMessage(HW_MESSAGE* message);
EXTERN_C BOOL hw_receiveMessage(HW_MESSAGE* ret);
EXTERN_C UINT32 hw_getMailboxNumberOfMessages();

//
// Test index function
//
EXTERN_C HW_TESTID hw_getMyInstanceID();

//
// Buffer retrieval functions
//
EXTERN_C UINT32 hw_getBufferIdx();
#ifdef __cplusplus
template <typename T> T* hw_getBufferPtr(T arr[])
{
    UINT32 bufferIdx = hw_getBufferIdx();
    return &arr[bufferIdx];
}
#endif

//
// Global index function
//
EXTERN_C UINT32 hw_getGlobalIdx();

//
// Random number generator operations
//
EXTERN_C INT32 hw_rand();
EXTERN_C INT32 hw_randrange(INT32 min, INT32 max);
EXTERN_C FLOAT32 hw_randF();
EXTERN_C FLOAT32 hw_distUniform(FLOAT32 low, FLOAT32 high);

EXTERN_C void hw_srandWithIteration(UINT32 seed, UINT32 iteration);
#define hw_srand(seed) hw_srandWithIteration(seed, 0)
EXTERN_C UINT32 hw_getRandCurrentSeed();
EXTERN_C UINT32 hw_getRandCurrentIteration();

//
// Array shuffle functions
//

#ifdef __cplusplus

// Swap two inputs
template<typename T>
void hw_swap( T& a, T& b)
{
    T temp = a;
    a = b;
    b = temp;
}

// The modern version of the Fisher-Yates shuffle
template<typename T>
void hw_shuffle( T arr[], UINT32 n )
{
    UINT32 i;
    UINT32 j;
    for (i = n-1; i > 0; i--)
    {
        // Pick a random index such that 0 <= j <= i
        j = hw_randrange(0, i);

        // Swap arr[i] with the element at random index
        hw_swap(arr[i], arr[j]);
    }
}
#endif

//
// Parameter retrieval functions
//

// C++ functions, not usable if compiled in C.
#ifdef __cplusplus
extern BOOL hw_getParam(UINT32 key, UINT32* val);
extern BOOL hw_getParam(UINT32 key, FLOAT32* val);
extern BOOL hw_getParam(UINT32 key, char* val);
extern BOOL hw_getParam(UINT32 key, UINT32* val, UINT32 def_val);
extern BOOL hw_getParam(UINT32 key, FLOAT32* val, FLOAT32 def_val);
extern BOOL hw_getParam(UINT32 key, char* val, char* def_val);

// V3 parameters
extern BifrostTestParams* hw_initTestParams(BifrostTestParams* bfTp_array);
extern BifrostTestVars*   hw_initTestVars(BifrostTestVars* bfTv_array);

#endif
// Non-C++ functions
EXTERN_C UINT32 hw_getParamCount();
EXTERN_C UINT32 hw_getKeyAtIdx(UINT32 index);

//
// Private variable save/restore functions
//
EXTERN_C void hw_setPrivateVars(void* privateVars);
EXTERN_C void* hw_getPrivateVars();

//
// Assert
//
#define hw_assert(X) if(!(X)){ hw_errmsg("hw_assert: Assertion " #X  " failed at %s, line %d\n", __FILE__, __LINE__); if(hw_getCoreNum() == bifrostCachedGlobals.tpid) { hw_terminate(); } else { hw_fatal(); } while(1);}

//
// Core termination
//
EXTERN_C void hw_terminate();

//
// Cache flush function
//
EXTERN_C void hw_cacheFlushAddr(void* addr);
EXTERN_C void hw_cacheFlushBuffer(void* addr, SIZE_T bytes_size);
EXTERN_C void hw_cacheInvalidateAddr(void* addr);
EXTERN_C void hw_cacheInvalidateBuffer(void* addr, SIZE_T bytes_size);

//
// Time functions
//
EXTERN_C UINT64 hw_getCycleCount();
EXTERN_C UINT64 hw_getTime();

//
// CRC
//
EXTERN_C UINT32 hw_crc32(UINT8* data, SIZE_T sizeInBytes, UINT32 prevRemainder);

//
// Open-source IEEE754 math function implementations
//
EXTERN_C double hw_exp  (double x);
EXTERN_C double hw_pow  (double x, double y);
EXTERN_C double hw_sqrt (double x);
EXTERN_C double hw_fabs (double x);
EXTERN_C double hw_scalbn (double x,int n);
EXTERN_C double hw_copysign (double x, double y);

//
// Environment query functions
//
EXTERN_C HW_PLATFORM hw_get_platform();
EXTERN_C const char* hw_get_config();
EXTERN_C HW_ARCHTYPE hw_get_archType();
EXTERN_C BOOL hw_is_node_rtl(UINT32 node_number);
EXTERN_C BOOL hw_is_dram_rtl();

//
// POST code function
//
EXTERN_C void hw_post(HW_STATUSCODE code);

//
// VITAG code function
//
EXTERN_C VOID hw_vitag(UINT32 code);

//
// File access
//
EXTERN_C HW_FILE* hw_fopen(const char* filename, const char* mode);
EXTERN_C INT32 hw_fclose(HW_FILE* file);
EXTERN_C UCHAR* hw_fgetbuf(HW_FILE* file);
EXTERN_C SIZE_T hw_fgetsize(HW_FILE* file);

//
// Memory consistency
//
EXTERN_C void hw_serialize();

//
// Memory access prototypes.
//

static inline UINT32 hw_read32(volatile UINT32* addr)
{
    return *addr;
}
static inline UINT8 hw_read8(volatile UINT8* addr)
{
    return *addr;
}
static inline UINT16 hw_read16(volatile UINT16* addr)
{
    return *addr;
}

static inline UINT64 hw_read64(volatile UINT64* addr)
{
    return *addr;
}

//
// 32-bit environment
//
#ifdef ENV_32BIT
static inline UINTPTR hw_readptr(volatile PTR_FIELD* addr)
{
    PTR_FIELD tempData = *addr;
    hw_assert(((PTR_FIELD)((UINTPTR)tempData)) == tempData);
    return (UINTPTR) tempData;
}
#else
static inline UINTPTR hw_readptr(volatile PTR_FIELD* addr)
{
    return (UINTPTR) *addr;
}
#endif

static inline void hw_write32(volatile UINT32* addr, UINT32 data)
{
    *addr = data;
}

static inline void hw_write8(volatile UINT8* addr, UINT8 data)
{
    *addr = data;
}

static inline void hw_write16(volatile UINT16* addr, UINT16 data)
{
    *addr = data;
}

static inline void hw_write64(volatile UINT64* addr, UINT64 data)
{
    *addr = data;
}

static inline void hw_writeptr(volatile PTR_FIELD* addr, UINTPTR data)
{
    *addr = (PTR_FIELD) data;
}

//
// End memory access prototypes.
//



//
// Read-after-write functions
// Implemented as a macro to reduce code duplication
//
#define __READ_WRITE_CHK_DEF(size, datatype) \
    static inline bool CONCAT(CONCAT(hw_write,size),_chk)(datatype* addr , datatype exp) \
    { \
        CONCAT(hw_write,size)(addr, exp); \
        datatype data = CONCAT(hw_read,size)(addr); \
        bool ret = (data != exp); \
        if (ret) { \
            hw_errmsg("%s: at address: 0x%x, read data: 0x%x , expected: 0x%x\n", __func__, addr, data, exp); \
        } else { \
            hw_debug("%s: at address: 0x%x, read data: 0x%x\n", __func__, addr, data); \
        } \
        return ret; \
    } \
    \
    static inline bool CONCAT(CONCAT(hw_read,size),_chk)(datatype* addr , datatype exp) \
    { \
        datatype data = CONCAT(hw_read,size)(addr); \
        bool ret = (data != exp); \
        if (ret) { \
            hw_errmsg("%s: at address: 0x%x, read data: 0x%x , expected: 0x%x\n", __func__, addr, data, exp); \
        } else { \
            hw_debug("%s: at address: 0x%x, read data: 0x%x\n", __func__, addr, data); \
        } \
        return ret; \
    }

__READ_WRITE_CHK_DEF(32, UINT32)
__READ_WRITE_CHK_DEF(16, UINT16)
__READ_WRITE_CHK_DEF(8, UINT8)

//
// Register manipulation functions
// Implemented as a macro to reduce code duplication
//
#define __READ_WRITE_REG_DEF(size, datatype) \
    static inline datatype CONCAT(hw_reg_readBits,size)(UINT32 base, UINT32 offset, datatype mask) \
    { \
        volatile datatype* addr = (volatile datatype*)(UINTPTR)(base + offset); \
        return BITS_GET(*addr, mask); \
    } \
    static inline datatype CONCAT(hw_reg_read,size)(UINT32 base, UINT32 offset) \
    { \
        volatile datatype* addr = (volatile datatype*)(UINTPTR)(base + offset); \
        return *addr; \
    } \
    \
    static inline void CONCAT(hw_reg_writeBits,size)(UINT32 base, UINT32 offset, datatype data, datatype mask) \
    { \
        volatile datatype* addr = (volatile datatype*)(UINTPTR)(base + offset); \
        datatype old_value = *addr; \
        datatype new_value = (BITS_CLEAR(old_value, mask)) | (BITS_GET(data, mask)); \
        *addr = new_value; \
    } \
    static inline void CONCAT(hw_reg_write,size)(UINT32 base, UINT32 offset, datatype data) \
    { \
        volatile datatype* addr = (volatile datatype*)(UINTPTR)(base + offset); \
        *addr = data; \
    }

__READ_WRITE_REG_DEF(32, UINT32)
__READ_WRITE_REG_DEF(16, UINT16)
__READ_WRITE_REG_DEF(8, UINT8)



//
// Atomic Operations
//

//
// Uses an atomic operation to test for *addr==cond, and if true, does *addr=data
// Note: There is no timeout on this function! It is possible to spin forever
//
static inline void hw_storeConditionalBlocking(UINT32* addr, UINT32 cond, UINT32 data)
{
    while(!__sync_bool_compare_and_swap(addr, cond, data));
}

//
// Returns what the value of *addr was before attempting the S32C1I
// If the return value matches cond, the operation was successful
//
static inline UINT32 hw_storeConditional(UINT32* addr, UINT32 cond, UINT32 data)
{
    return __sync_val_compare_and_swap(addr, cond, data);
}

//
// Mark core to be active or not. An active core can run test after it is waken up
//
EXTERN_C void hw_markActiveCore(HW_CORE core, BOOL active);

//
// Node wakeup/shutdown
//
// bits[0-6] SFP Nodes[0-6]
// bits[7-11] VFP Nodes[0-4]
// bits[12-15] pala[0-3]
// bits[16-17] kula[0-1]
// bits[18] hana[0]
EXTERN_C void hw_wakeup_node(UINT32 mask);
EXTERN_C void hw_shutdown_node(UINT32 mask);
EXTERN_C void hw_clockon_node(UINT32 mask);
EXTERN_C void hw_clockoff_node(UINT32 mask);

//
// Fabric timeout
//
EXTERN_C void hw_configureFabricTimeout(BOOL enable);


//
// Host command
//
EXTERN_C BOOL hw_hostcmd(UINT16 cmd, UINT8* cmd_data, UINT32 cmd_data_size, BOOL need_resp, UINT16* resp, UINT8* resp_data, UINT32 resp_data_size);
EXTERN_C BOOL hw_host_memcmp( const void* ptr1, const void* ptr2, const SIZE_T len, INT32* result);
EXTERN_C BOOL hw_host_crc32( UINT8* data, const SIZE_T len, UINT32 prevRemainder, UINT32* result);
EXTERN_C BOOL hw_host_cdma_crc32( UINT8* data, const SIZE_T len, UINT32 prevRemainder, UINT32 frag_en, UINT32 frag_threshold_enc, UINT32 xdim, UINT32 ydim, UINT32* result);
EXTERN_C BOOL hw_host_file_read( const CHAR* fileName, const SIZE_T fileOffset, const CHAR* buffer, const SIZE_T size, UINT32* len);
EXTERN_C BOOL hw_host_file_write( const CHAR* fileName, const SIZE_T fileOffset, CHAR* buffer, const SIZE_T size, UINT32* len);
EXTERN_C BOOL hw_host_pcie_read(const void* ptr, const SIZE_T len, UINT32 checksum, HW_HOSTCMD_PCIE_BURST_MODE burstMode, UINT8* data, BOOL32* result);
EXTERN_C BOOL hw_host_pcie_write(void* ptr, const SIZE_T len, HW_HOSTCMD_PCIE_BURST_MODE burstMode, HW_HOSTCMD_PCIE_DATA_PATTERN_TYPE patternType, UINT32* patternData, UINT32* checksum, BOOL32* result);
EXTERN_C BOOL hw_host_pcie_memcpy(void* dest, const void* src, const SIZE_T len, HW_HOSTCMD_PCIE_BURST_MODE burstMode, BOOL32* result);
EXTERN_C BOOL hw_host_pcie_suspend(UINT32 timeout, BOOL32* result);
EXTERN_C BOOL hw_host_pcie_resume(BOOL32* result);
EXTERN_C BOOL hw_host_notify_standby(BOOL32* result);
EXTERN_C BOOL hw_host_query_msi(UINT8 vector, BOOL32* result);
EXTERN_C BOOL hw_host_notify_fir(BOOL32* result);
EXTERN_C BOOL hw_host_notify_hostmem_alloc(UINT64* hostmemPhyAddrBase, BOOL32* result);
EXTERN_C BOOL hw_force_release_signal(BOOL fr_n, const CHAR* sig_name, UINT64 value);
EXTERN_C void hw_hostcmd_post(HW_STATUSCODE code, UINT32 testID);
EXTERN_C void hw_hostcmd_puts( const char* buffer, UINT32 testID );

//
// Delay
//
static inline void hw_delay_cycles(UINT64 cycles)
{
    UINT64 start = hw_getCycleCount();
    while((hw_getCycleCount() - start) < cycles);
}

#define hw_delay_ns(ns) hw_delay_cycles(ns / 2)
#define hw_delay_us(us) hw_delay_cycles(us * 1000 / 2)
#define hw_delay_ms(ms) hw_delay_cycles(ms * 1000000 / 2)

//
// Fabric error diag
//
EXTERN_C UINT32 hw_fabric_error_diag();


EXTERN_C BOOL hw_raise_msi(UINT8 vector);

EXTERN_C UINT32 hw_get_jtm_interval();
EXTERN_C INT32 hw_get_jtm_slope();
EXTERN_C INT32 hw_get_jtm_offset();

EXTERN_C UINT32 hw_get_ddr_frequency();

EXTERN_C BOOL hw_is_silicon();

//PCIE

EXTERN_C void   hw_pcie_clear_correctable_error();
EXTERN_C void   hw_pcie_clear_uncorrectable_error();
EXTERN_C void   hw_pcie_clear_error_status ();

EXTERN_C UINT32 hw_pcie_check_correctable_error(UINT32* counter);
EXTERN_C UINT32 hw_pcie_check_uncorrectable_error(UINT32* counter);
EXTERN_C void   hw_pcie_parse_correctable_error(UINT32 data);
EXTERN_C void   hw_pcie_parse_uncorrectable_error(UINT32 data);
EXTERN_C void   hw_pcie_parse_correctable_error_counter(UINT32* counter);
EXTERN_C void   hw_pcie_parse_uncorrectable_error_counter(UINT32* counter);
EXTERN_C UINT32 hw_pcie_check_error();

EXTERN_C UINT32 hw_pcie_check_speed();
EXTERN_C void   hw_pcie_parse_speed(UINT32 data);

EXTERN_C BOOL32 hw_pcie_check_training();

EXTERN_C UINT32 hw_pcie_init();

// Boot diag
EXTERN_C HW_BOOT_DIAG_DRAMC_INIT_TIME* hw_get_dramc_init_time();

//FPGA
EXTERN_C BOOL hw_i2c_init_port(UINT8 i2cInstance, UINT8 busSpeed);
EXTERN_C BOOL hw_i2c_write_fpga(UINT8 i2cInstance, UINT16 slvAddr, UINT8 regAddr, UINT8 wrData);
EXTERN_C BOOL hw_i2c_read_fpga(UINT8 i2cInstance, UINT16 slvAddr, UINT8 regAddr, UINT8* result);
EXTERN_C BOOL hw_i2c_write_verify_fpga(UINT8 i2cInstance, UINT16 slvAddr, UINT8 regAddr, UINT8 wrData);
EXTERN_C BOOL hw_i2c_read_verify_fpga(UINT8 i2cInstance, UINT16 slvAddr, UINT8 regAddr, UINT8 wrData);
EXTERN_C BOOL hw_i2c_fpga_set_led(UINT32 mask);
EXTERN_C BOOL hw_i2c_fpga_set_seg7(UINT16 code);
EXTERN_C BOOL hw_i2c_fpga_power_cplb();


//Standby
EXTERN_C BOOL32 hw_request_standby();
EXTERN_C BOOL32 hw_boot_outof_standby();

#endif
