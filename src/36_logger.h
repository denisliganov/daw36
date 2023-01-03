#ifndef _CHAOTIC_LOGGER_
#define _CHAOTIC_LOGGER_

// Module Name:  36_logger.h
//
// General Description: Logging utility functions are declared here.
//
// Revision _MHistory:
//
// Author                       Date             Changes
// ----------------------   ------------      -------------
// Alexander Veprik          09/01/2008        Initial version

#if (ENABLE_LOGGING == 1)

extern void AwfulLogData(const char* filename, unsigned int line, const char* text);

#define LogData(text)   AwfulLogData(__FILE__, __LINE__, text)
#define DBG1(text)       AwfulLogData(NULL, -1, text)

#else

#define LogData(text) NULL
#define DBG1(text) NULL

#endif

#endif /* _CHAOTIC_LOGGER_ */
