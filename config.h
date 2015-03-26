#ifndef CONFIG_H
#define CONFIG_H

// standard headers
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef _MSC_VER
#include <intrin.h>
#endif


#define DEBUG_THISFILE_ON 1
#define DEBUG_THISFILE_OFF 0

#define DEBUG_ON 1
#define DEBUG_OFF 0

// M$ does not care about the rest of the world
#ifdef _MSC_VER
#define __func__ __FUNCTION__
#endif

#ifndef STRINGIFY
#   define STRINGIFY(s) STRINGIFY_HELPER(s)
#   define STRINGIFY_HELPER(s) #s
#endif


#ifndef NULL
#   define NULL ((void*)0)
#endif


#ifndef VAR_UNUSED
#   define VAR_UNUSED(expr) do { (void)(expr); } while (0)
#endif


//! debug break point
#ifdef PILESGUI_DEBUG
#   ifdef _MSC_VER
#       define YT_BREAKPOINT  __debugbreak()
#   else
#       define YT_BREAKPOINT  __builtin_trap()
#   endif
#else
#   define YT_BREAKPOINT
#endif


//! our assert
#ifdef PILESGUI_DEBUG
#  define DBG_ASSERT(a) if (!(a)) {\
        printf("\nDBG_ASSERT Failed: " STRINGIFY(a) "\n"); \
        printf("%s[%d]: %s\n\n",__FILE__, __LINE__, __func__); \
        YT_BREAKPOINT; }
#else
#  define DBG_ASSERT(a)
#endif

//! assert and return
#ifdef PILESGUI_DEBUG
#  define DBG_ASSERT_RET(a) \
    if (!(a)) {\
        printf("\nDBG_ASSERT Failed: " STRINGIFY(a) "\n"); \
        printf("%s[%d]: %s\n\n",__FILE__, __LINE__, __func__); \
        YT_BREAKPOINT; \
        return; }
#else
#  define DBG_ASSERT_RET(a) if (!(a)) return;
#endif

#ifndef HUGE_VALD
#define HUGE_VALD (INFINITY)
#endif

#define DOUBLE_IS_NAN(__d__) \
    (__d__ != __d__)
#define DOUBLE_IS_INF(__d__) ( \
    (__d__ == HUGE_VALD) || \
    (__d__ == -HUGE_VALD))

#define DOUBLE_IS_VALID(__d__) \
    (!DOUBLE_IS_NAN (__d__) && !DOUBLE_IS_INF (__d__))
#define POINT_IS_VALID(__p__) \
    (DOUBLE_IS_VALID(__p__.x()) && DOUBLE_IS_VALID(__p__.y()))
#define RECT_IS_VALID(__r__) (\
    POINT_IS_VALID(__r__.topLeft()) && \
    POINT_IS_VALID(__r__.topRight()) && \
    POINT_IS_VALID(__r__.bottomRight()) && \
    POINT_IS_VALID(__r__.bottomLeft()))
#define TRANSFORM_IS_VALID(__t__) (\
    DOUBLE_IS_VALID(__t__.m11()) && \
    DOUBLE_IS_VALID(__t__.m12()) && \
    DOUBLE_IS_VALID(__t__.m13()) && \
    DOUBLE_IS_VALID(__t__.m21()) && \
    DOUBLE_IS_VALID(__t__.m22()) && \
    DOUBLE_IS_VALID(__t__.m23()) && \
    DOUBLE_IS_VALID(__t__.m32()) && \
    DOUBLE_IS_VALID(__t__.m33()))


#define DBG_ASSERT_DOUBLE(__d__) \
    DBG_ASSERT(DOUBLE_IS_VALID(__d__))
#define DBG_ASSERT_POINT(__p__) \
    DBG_ASSERT(POINT_IS_VALID(__p__))
#define DBG_ASSERT_RECT(__r__) \
    DBG_ASSERT(RECT_IS_VALID(__r__))
#define DBG_ASSERT_TRANSF(__t__) \
    DBG_ASSERT(TRANSFORM_IS_VALID(__t__))

//! print debug message
#ifdef PILESGUI_DEBUG
#  define DBG_MESSAGE(__m__) \
    printf("%s[%d]: %s\n%s",__FILE__, __LINE__, __func__, __m__);
#else
#  define DBG_MESSAGE(a) if (!(a)) return;
#endif

//! print debug message
#ifdef PILESGUI_DEBUG
#  define QDBG_MESSAGE(__m__) \
    qDebug() << "DEBUG: " << __m__ << " > " << __func__ << " > " << __FILE__ << "[" << __LINE__ << "] ";
#else
#  define QDBG_MESSAGE(__m__)
#endif

#ifdef PILESGUI_DEBUG
#  define DBG_PMESSAGE printf
#else
#  define DBG_PMESSAGE black_hole
#endif

#ifdef PILESGUI_DEBUG
#  define DBG_TRACE_ENTRY printf("ENTRY %s in %s[%d]\n", __func__, __FILE__, __LINE__)
#else
#  define DBG_TRACE_ENTRY
#endif

#ifdef PILESGUI_DEBUG
#  define DBG_TRACE_EXIT printf("EXIT %s in %s[%d]\n", __func__, __FILE__, __LINE__)
#else
#  define DBG_TRACE_ENTRY
#endif

#ifdef PILESGUI_DEBUG
#  define DBG_PQSTRING(__s__) printf("%s\n", TMP_A(__s__))
#else
#  define DBG_PQSTRING(__s__)
#endif

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable: 4127) // conditional expression is constant
//#pragma warning(disable: 9025) // conditional expression is constant
#endif


#define	STG_LANGUAGE		"ui-language"
#define	DATA_FLD_TRANSL		"translations"
#define	LANG_FILE_PREFIX	"pilesgui_"
#define	LANG_FILE_SUFFIX	".qm"
#define	LANG_FILE(loc)		(LANG_FILE_PREFIX + loc + LANG_FILE_SUFFIX)


#define TMP_A(__s__) __s__.toLatin1().constData()


//! describes the result of a function
///
enum	OUTCOME			{
    OUTCOME_WARNING = -1,
    OUTCOME_OK = 0,
    OUTCOME_ERROR = 1
};

//! was the result a succes (includes warnings)?
///
#define		OUT_SUCCESS( o )	( o != OUTCOME_ERROR )

//! was the result a succes (excludes warnings)?
///
#define		OUT_FULL_SUCCESS( o )	( o == OUTCOME_OK )


static inline void black_hole (...)
{}

#define SEC_TO_MSEC(__val__) (__val__*1000)

#define SIMPLE_CRYPT_KEY Q_UINT64_C(0x0c4dac5683f56123)

#endif // CONFIG_H
