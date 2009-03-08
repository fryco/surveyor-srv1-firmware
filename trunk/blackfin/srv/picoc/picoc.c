#include "picoc.h"

/* initialise everything */
void Initialise()
{
    HeapInit();
    TableInit();
    VariableInit();
    LexInit();
    TypeInit();
    LibraryInit(&GlobalTable, "c library", &CLibrary);
    LibraryInit(&GlobalTable, "platform library", &PlatformLibrary);
    PlatformLibraryInit();
}

/* platform-dependent code for running programs is in this file */
#ifdef UNIX_HOST
int main(int argc, char **argv)
{
    if (argc < 2)
    {
        PlatformPrintf("Format: picoc <program.c> - run a program\n        picoc -i          - interactive mode\n");
        exit(1);
    }
    
    Initialise();
    
    if (strcmp(argv[1], "-i") == 0)
        ParseInteractive();
    else
    {
        if (PlatformSetExitPoint())
            return 1;
        
        PlatformScanFile(argv[1]);
    }
        
    return 0;
}
#else
# ifdef SURVEYOR_HOST
int errjmp[41];

int picoc(char *SourceStr)
{    
    int ix;
    
    Initialise();
    printf("starting picoC\n\r");
    if (SourceStr) {
        for (ix=0; ix<strlen(SourceStr); ix++)  // clear out ctrl-z from XMODEM transfer
            if (SourceStr[ix] == 0x1A)
                SourceStr[ix] = 0x20;
        printf("%s\n\r", SourceStr);  // display program source
        printf("=====================\n");
    }
    errjmp[40] = 0;
    setjmp(errjmp);
    if (errjmp[40]) {
        printf("leaving picoC\n\r");
        return 1;
    }
        
    if (SourceStr)    
        Parse("test.c", SourceStr, strlen(SourceStr), TRUE);
    ParseInteractive();
    return 0;
}
# endif
#endif
