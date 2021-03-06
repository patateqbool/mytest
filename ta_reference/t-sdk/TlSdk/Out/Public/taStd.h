/*
 * Copyright (c) 2013-2014 TRUSTONIC LIMITED
 * All rights reserved
 *
 * The present software is the confidential and proprietary information of
 * TRUSTONIC LIMITED. You shall not disclose the present software and shall
 * use it only in accordance with the terms of the license agreement you
 * entered into with TRUSTONIC LIMITED. This software may be subject to
 * export or import laws in certain countries.
 */
/**
 * This is a wrapper to include standard header files.
 */

#ifndef __TASTD_H__
#define __TASTD_H__

#if TBASE_API_LEVEL >= 3

//==============================================================================
// Standard include files
//==============================================================================


#if defined(__cplusplus)

    #include <cstdint>
    #include <cstdlib>
    #include <cstdio>
    #include <cstring>
    #include <string>
    #include <cstdarg>
    #include <exception>
    using namespace std;

    extern "C" {
        // nothing here
    }

#else

    #include <stdint.h>
    #include <stdbool.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    #include <stdarg.h>

#endif // defined(__cplusplus)


//==============================================================================
// Compiler specific definitions
//==============================================================================

#if defined(__ARMCC__)

    #define _INLINE             __inline__
    #define _SECTION(x)         __attribute__((section(x)))
    #define _UNUSED             __attribute__((unused))
    #define _USED               __attribute__((used))
    #define _NORETURN           __attribute__((noreturn))
    #define _WEAK               __attribute__((weak))

    // The section attribute specifies that a variable must be placed in a
    // particular data section. The zero_init attribute specifies that a
    // variable with no initializer is placed in a ZI data section. If an
    // initializer is specified, an error is reported.
    #define _ZEROINIT           __attribute__((zero_init))
#elif defined(__GNUC__)
    #define _INLINE             __inline__
    #define _SECTION(x)         __attribute__((section(x)))
    #define _UNUSED             __attribute__((unused))
    #define _USED               __attribute__((used))
    #define _NORETURN           __attribute__((noreturn))
    #define _WEAK               __attribute__((weak))
    #define _ZEROINIT
#else
    #error "unknown compiler"
#endif


#if defined(__ARMCC__) && defined(TRUSTLET)
    #pragma import(__use_two_region_memory)
    #pragma import(__use_no_semihosting)
#endif


//==============================================================================
// C/C++ compatibility
//==============================================================================

#if defined(__cplusplus)

    #define _EXTERN_C                extern "C"
    #define _BEGIN_EXTERN_C          extern "C" {
    #define _END_EXTERN_C            }

#else

    #define _EXTERN_C
    #define _BEGIN_EXTERN_C
    #define _END_EXTERN_C

#endif // defined(__cplusplus)


// Thread entry function
#define _THREAD_ENTRY       _EXTERN_C _NORETURN


//==============================================================================
// TLAPI settings
//==============================================================================
#define _TLAPI_EXTERN_C     _EXTERN_C
#define _TLAPI_NORETURN     _NORETURN


//==============================================================================
// Basic types
//==============================================================================

#if !defined(TRUE)
    #define TRUE    (1==1)
#endif // !defined(TRUE)

#if !defined(FALSE)
    #define FALSE   (1!=1)
#endif // !defined(TRUE)


#define DUMMY_FUNCTION()       do{}while(FALSE)


typedef void        *void_ptr;  /**< a pointer to anything. */
typedef void_ptr    addr_t;     /**< an address, can be physical or virtual */
typedef uint32_t    bool_t;     /**< boolean data type. */

//==============================================================================
// Trusted application entry function
//==============================================================================

#define _TLAPI_ENTRY        _TLAPI_EXTERN_C _TLAPI_NORETURN

_TLAPI_ENTRY void tlMain(
    const addr_t    tciBuffer,
    const uint32_t  tciBufferLen
);



//==============================================================================
// Stack utilities
//==============================================================================

typedef uint32_t        stackEntry_t, *stackEntry_ptr;

// calculate the size in byte for the given number of stack entries
#define STACK_ENTRY_COUNT_TO_BYTE_SIZE(e)    (e * sizeof(stackEntry_t))
// calculate the number of stack entries require to hold given number of bytes
#define BYTE_SIZE_TO_STACK_ENTRY_COUNT(b)    ((b + sizeof(stackEntry_t) - 1) / sizeof(stackEntry_t))

// internal macros to get variable names from given name. Don't try using the
// variable names directly.
#define _STACK_ARRAY_VAR(_name_)    (__attribute__((aligned(8))) _stack_##_name_##_array)
#define _STACK_ENTRIES_VAR(_name_)  _stack_##_name_##_entries
#define _SECTION_STACK              _SECTION("stack")


// declare a stack. Basically, this is an array in RAM. However, no assumptions
// must be make about how a stack is defined internally. Access functions and
// macros must be used. The actual implementation may change anytime
// Note about the "extern const" declaration, that in C++ variables declared
// "const" have internal linkage. To enforce external linkage, "extern" must be
// given, too. Actually, it is given by _EXTERN_C, but we keep the additional
// "extern" here to remember why we need it.
// _STACK_ARRAY_VAR() has "_USED", because compiler/liker shall never remove
// it. If a stack is declared, it must exist, even if no code references it. It
// may still be needed by a wrapper that set up our binary externally.
// _STACK_ENTRIES_VAR() has "_UNSED" because it my not be referenced at all. It
// is just an internal helper variable.
#if defined(__ARMCC__)
#define DECLARE_STACK(_name_,_size_) \
            _BEGIN_EXTERN_C \
                _USED stackEntry_t             _STACK_ARRAY_VAR(_name_)[BYTE_SIZE_TO_STACK_ENTRY_COUNT(_size_)] _SECTION_STACK _ZEROINIT; \
                _UNUSED extern const uint32_t  _STACK_ENTRIES_VAR(_name_) = BYTE_SIZE_TO_STACK_ENTRY_COUNT(_size_); \
            _END_EXTERN_C

#elif defined(__GNUC__)
#define DECLARE_STACK(_name_,_size_) \
            _BEGIN_EXTERN_C \
                _USED stackEntry_t             _STACK_ARRAY_VAR(_name_)[BYTE_SIZE_TO_STACK_ENTRY_COUNT(_size_)]; \
                _UNUSED const uint32_t  _STACK_ENTRIES_VAR(_name_) = BYTE_SIZE_TO_STACK_ENTRY_COUNT(_size_); \
            _END_EXTERN_C

#else
    #error "unknown compiler"
#endif


// import the references from a stack declares somewhere else
#define EXTERNAL_STACK(_name_) \
            _BEGIN_EXTERN_C \
                extern stackEntry_t    _STACK_ARRAY_VAR(_name_)[]; \
                extern const uint32_t  _STACK_ENTRIES_VAR(_name_); \
            _END_EXTERN_C

#define TL_MAIN_STACK_NAME                            tlMain
#define DECLARE_TRUSTLET_MAIN_STACK(_mainStackSize_)  DECLARE_STACK(TL_MAIN_STACK_NAME, _mainStackSize_)
#define DECLARE_TRUSTED_APPLICATION_MAIN_STACK        DECLARE_TRUSTLET_MAIN_STACK

// get a pointer to the top of a given stack.
#define GET_STACK_TOP(_name_) \
            ( &_STACK_ARRAY_VAR(_name_)[_STACK_ENTRIES_VAR(_name_)] )
#define getStackTop(_name_) GET_STACK_TOP(_name_)

// get a pointer to the bottom of a given stack.
#define GET_STACK_BOTTOM(_name_) \
            ( &_STACK_ARRAY_VAR(_name_)[0] )
#define getStackBottom(_name_) GET_STACK_BOTTOM(_name_)

// get the size of a given stack. cannot use sizeof() here, as only the symbol
// name are know for external stacks.
#define GET_STACK_SIZE(_name_) \
            ( _STACK_ENTRIES_VAR(_name_) * sizeof(stackEntry_t) )
#define getStackSize(_name_) GET_STACK_SIZE(_name_)

//==============================================================================
// Heap utilities
//==============================================================================

typedef uint32_t        heapEntry_t, *heapEntry_ptr;

#if ((TBASE_API_LEVEL == 3) || (TBASE_API_LEVEL == 4))

// calculate the size in byte for the given number of heap entries
#define HEAP_ENTRY_COUNT_TO_BYTE_SIZE(e)    (e * sizeof(heapEntry_t))
// calculate the number of heap entries require to hold given number of bytes
#define BYTE_SIZE_TO_HEAP_ENTRY_COUNT(b)    ((b + sizeof(heapEntry_t) - 1) / sizeof(heapEntry_t))

// internal macros to get variable names from given name. Don't try using the
// variable names directly.
#define _HEAP_ARRAY_VAR(_name_)    (__attribute__((aligned(8))) _heap_##_name_##_array)
#define _HEAP_ENTRIES_VAR(_name_)  _heap_##_name_##_entries
#define _SECTION_HEAP              _SECTION("heap")

// declare a heap. Basically, this is an array in RAM. However, no assumptions
// must be make about how a heap is defined internally. Access functions and
// macros must be used. The actual implementation may change anytime
// Note about the "extern const" declaration, that in C++ variables declared
// "const" have internal linkage. To enforce external linkage, "extern" must be
// given, too. Actually, it is given by _EXTERN_C, but we keep the additional
// "extern" here to remember why we need it.
// _HEAP_ARRAY_VAR() has "_USED", because compiler/liker shall never remove
// it. If a heap is declared, it must exist, even if no code references it. It
// may still be needed by a wrapper that set up our binary externally.
// _HEAP_ENTRIES_VAR() has "_UNSED" because it my not be referenced at all. It
// is just an internal helper variable.
#if defined(__ARMCC__)
#define DECLARE_HEAP(_name_,_size_) \
            _BEGIN_EXTERN_C \
                _USED heapEntry_t             _HEAP_ARRAY_VAR(_name_)[BYTE_SIZE_TO_HEAP_ENTRY_COUNT(_size_)] _SECTION_HEAP _ZEROINIT; \
                _USED extern const uint32_t   _HEAP_ENTRIES_VAR(_name_) = BYTE_SIZE_TO_HEAP_ENTRY_COUNT(_size_); \
            _END_EXTERN_C

#elif defined(__GNUC__)
#define DECLARE_HEAP(_name_,_size_) \
            _BEGIN_EXTERN_C \
                _USED heapEntry_t             _HEAP_ARRAY_VAR(_name_)[BYTE_SIZE_TO_HEAP_ENTRY_COUNT(_size_)]; \
                _USED const uint32_t          _HEAP_ENTRIES_VAR(_name_) = BYTE_SIZE_TO_HEAP_ENTRY_COUNT(_size_); \
            _END_EXTERN_C

#else
    #error "unknown compiler"
#endif

#define TL_MAIN_HEAP_NAME                            tlMain
#define DECLARE_TRUSTLET_MAIN_HEAP(_mainHeapSize_)  DECLARE_HEAP(TL_MAIN_HEAP_NAME, _mainHeapSize_)
#define DECLARE_TRUSTED_APPLICATION_MAIN_HEAP       DECLARE_TRUSTLET_MAIN_HEAP

#endif /* #if ((TBASE_API_LEVEL == 3) || (TBASE_API_LEVEL == 4)) */

//==============================================================================
// Debugging output
//==============================================================================

#ifndef EOL
    #define EOL "\n"
#endif


// enable debugging output if DEBUG is set. We use __INCLUDE_DEBUG so avoid
// having debug output directly glued to DEBUG. This allows some hack to have
// debug output for release builds.
#if defined(DEBUG)

    #define __INCLUDE_DEBUG

#endif

#if defined(__INCLUDE_DEBUG)

    //------------------------------------------------------------------------
    // debug a da data blob
    _EXTERN_C void __dbgBlob(
                        const char    *szDescriptor,
                        const void    *blob,
                        uint32_t      sizeOfBlob);

    //------------------------------------------------------------------------
    // assert message function.
    _EXTERN_C void __dbgAssertMsg(
        const char      *exp,
        const char      *file,
        const uint32_t  line);

    _TLAPI_EXTERN_C void DbgPrintf(
                const char *fmt,
                ...);

    #define dbgBlob(...)        __dbgBlob(__VA_ARGS__)
    #define dbgAssertMsg(...)   __dbgAssertMsg(__VA_ARGS__)
#else


    #define dbgBlob(...)        DUMMY_FUNCTION()
    #define dbgAssertMsg(...)   DUMMY_FUNCTION()

    #define DbgPrintf(...)      DUMMY_FUNCTION()
#endif // __INCLUDE_DEBUG


//==============================================================================
// Assert
//==============================================================================

#if defined(DEBUG)

    #define __INCLUDE_ASSERT

#elif defined(NDEBUG) && defined(NDEBUG_WITH_ASSERT)

    #define __INCLUDE_ASSERT

#endif


#if defined(__INCLUDE_ASSERT)

    _EXTERN_C _NORETURN void _doAssert(
        const char      *expr,
        const char      *file,
        const uint32_t  line
    );


    // we don't use assert() from RVCT, because to take too much space
    // when all assert strings are used.
    // #define ASSERT_FROM_RVCT

    #if defined(ASSERT_FROM_RVCT)

        // if using the ARM Library's assert.h, "__ASSERT_MSG" must be defined that
        // __aeabi_assert() gets called, which we implement to call doAssert().
        // see ARM library documentation for further information.
        #define __ASSERT_MSG
        #include <assert.h>

    #else // use our assert implementation

        #define assert(cond) \
            do{if (!(cond)){_doAssert(NULL,__FILE__, __LINE__);}} while(FALSE)

    #endif

#else // not __INCLUDE_ASSERT

    #define assert(cond)                DUMMY_FUNCTION()

#endif // [not] __INCLUDE_ASSERT

#endif /* TBASE_API_LEVEL */

void _init(void);
extern const uint32_t _tlEntryVersion;

#endif //__TASTD_H__
