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

internal void
LogMessage(char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    printf("Message: ");
    vprintf(fmt, args);
    printf("\n");

    va_end(args);
}

#include "tokenizer.h"
#include "json_parser.h"

#define JSON_TEST_H
#endif
