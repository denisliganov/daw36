
/*==================================================================================================

    Module Name:  Awful_logger.cpp

    General Description: Logging utility functionality is defined here.

====================================================================================================

Revision MHistory:
                         Modification
Author                       Date          Major Changes
----------------------   ------------      -------------
Anonymous                09/01/2008        Initial version

==================================================================================================*/

//=================================================================================================
//                             Include Section
//=================================================================================================

#include <iostream>
#include <stdio.h>

//=================================================================================================
//                             Global/Static Variable Section
//=================================================================================================

#define LOGGER_FILE_NAME "36_logs.txt"

//=================================================================================================
//                             Global function declaration
//=================================================================================================

void AwfulLogData(const char* filename, unsigned int line, const char* text)
{
    FILE *fhandle = NULL;
    char *buf = NULL;
    unsigned int len = 0;

    fhandle = fopen(LOGGER_FILE_NAME, "a+");

    if (NULL != fhandle)
    {
        len = strlen(text);
        if (filename != NULL)
        {
            len += strlen(filename);
        }
        if (line != -1)
        {
            len += 16; // dunno why...
        }

        buf = (char *) malloc(len * sizeof(char));

        if (buf != NULL)
        {

            if (filename != NULL)
            {
                sprintf(buf, "%s - %u: %s\n", filename, line, text);
            }
            else
            {
                sprintf(buf, "%s\n", text);
            }

            fwrite(buf, strlen(buf), 1, fhandle);
            free(buf);
        }

        fclose(fhandle);
    }
}
