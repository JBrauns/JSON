#if !defined(JSON_PLATFORM_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Joshua Brauns $
   $Notice: $
   ======================================================================== */

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

#ifdef JSON_DEV_BUILD
#define AssertMsg(exp, msg) if(!(exp)) { LogMessage("ASSERT >> %s@(%s)", (msg), __FUNCTION__); *(int *)0 = 0; }
#else
#define AssertMsg(...)
#endif

#define NotImplemented AssertMsg(false, "Not implemented!")

#define JSON_PLATFORM_H
#endif
