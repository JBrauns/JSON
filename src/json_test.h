#if !defined(JSON_TEST_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Joshua Brauns $
   $Notice: $
   ======================================================================== */

#include <stdio.h>
#include <stdarg.h>

#include <string>
#include <vector>

#define internal static

#include "json_platform.h"
#include "tokenizer.h"
#include "json_object.h"
#include "json_parser.h"

#define TEST_EXPRESSION(expr, testName)                     \
    {                                                       \
        ++testCount;                                        \
        if(expr)                                            \
        {                                                   \
            ++failCount;                                    \
            LogMessage("Test failed >> '%s'", testName);    \
        }                                                   \
        else                                                \
        {                                                   \
            LogMessage("Test done   >> '%s'", testName);    \
        }                                                   \
    }

#define JSON_TEST_H
#endif
