#ifndef OPTSREADER_H
#define OPTSREADER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef bool (*OptionCallback)(const char* const * params, void* userdata);

#define OptionHelpCallback ((OptionCallback)NULL)

typedef struct OptionHandler {
    const char* opt;
    const char* description;
    int paramCount;
    OptionCallback callback;
    bool isLast;
} OptionHandler;

typedef struct OptionsSpecification {
    const char* extraHelpText;
    const OptionHandler handlers[];
} OptionsSpecification;

typedef struct OptionsReader
{
    int argsLeft;
    const char* const * nextArg;
    const char* curOpt;
    const char* const * curFirstParam;
    int paramCount;
    const OptionsSpecification* const spec;
} OptionsReader;

void readOptions(int argc, char* const* argv, const OptionsSpecification* spec, void* userdata);

#ifdef OPTSREADER_IMPLEMENTATION
static bool prv_optsReader_isOption(const char* arg)
{
    return *arg == '-';
}

static bool prv_optsReader_next(OptionsReader* reader)
{
    if (reader->argsLeft == 0)
        return false;

    reader->curOpt = NULL;
    reader->paramCount = 0;
    if (prv_optsReader_isOption(*reader->nextArg))
    {
        reader->curOpt = *(reader->nextArg++);
        reader->argsLeft--;
    }

    reader->curFirstParam = reader->nextArg;
    while (reader->argsLeft > 0 && !prv_optsReader_isOption(*reader->nextArg))
    {
        reader->paramCount++;
        reader->nextArg++;
        reader->argsLeft--;
    }
    return true;
}

static bool prv_optsReader_handlesOpt(const OptionHandler* handler, const char* opt)
{
    static const char SEPARATOR = '|';
    if (handler->opt == NULL)
        return opt == NULL;

    const char* start = handler->opt;
    const char* nextEnd = strchr(start, SEPARATOR);
    while (nextEnd != NULL)
    {
        int length = nextEnd - start;
        if (strncmp(start, opt, length) == 0)
            return true;
        start = nextEnd + 1;
        nextEnd = strchr(start, SEPARATOR);
    }
    return strcmp(start, opt) == 0;
}

static const OptionHandler* prv_optsReader_findCurrentHandler(const OptionsReader* reader)
{
    const OptionHandler* curHandler = reader->spec->handlers;
    while (!curHandler->isLast && !prv_optsReader_handlesOpt(curHandler, reader->curOpt))
        curHandler++;
    return curHandler->isLast ? NULL : curHandler;
}

static void prv_optsReader_printHelpAndExit(const OptionsReader* reader)
{
    const OptionHandler* curHandler = reader->spec->handlers;
    puts("options:");
    while (!curHandler->isLast)
    {
        printf("  %s", curHandler->opt);
        if (curHandler->description != NULL)
            printf(" - %s", curHandler->description);
        putchar('\n');
        curHandler++;
    }
    if (reader->spec->extraHelpText != NULL)
        printf("\n%s\n", reader->spec->extraHelpText);
    exit(0);
}

void readOptions(int argc, char* const* argv, const OptionsSpecification* spec, void* userdata)
{
    OptionsReader reader = {
        .argsLeft = argc - 1, // first arg is executable file
        .nextArg = (const char* const *)argv + 1,
        .curOpt = NULL,
        .curFirstParam = NULL,
        .paramCount = 0,
        .spec = spec
    };

    if (reader.argsLeft == 0)
        prv_optsReader_printHelpAndExit(&reader);

    while(prv_optsReader_next(&reader))
    {
        const OptionHandler* handler = prv_optsReader_findCurrentHandler(&reader);
        if (reader.curOpt != NULL)
        {
            if (handler == NULL)
                printf("unknown option: %s\n", reader.curOpt);
            if (handler->paramCount != reader.paramCount)
            {
                printf("expected %d parameter(s) for %s\n", handler->paramCount, reader.curOpt);
                prv_optsReader_printHelpAndExit(&reader);
            }
        }
        if (handler == NULL && reader.curOpt != NULL)
            printf("unknown option: %s\n", reader.curOpt);
        if (handler == NULL ||
            handler->callback == OptionHelpCallback ||
            !handler->callback(reader.curFirstParam, userdata))
            prv_optsReader_printHelpAndExit(&reader);
    }
}
#endif // OPTSREADER_IMPLEMENTATION

#endif
