#ifndef EXTERN_
#define EXTERN_
#include "includes.h"
#include "brew_types.h"

//extern UC       verbose1;
//extern UC       verbose2;

extern UC     restoreMode;
extern UC       printMode;
extern UC option;
extern fileList_t good_fList;
extern char *backupFilename;

extern UC CPU_ENDIANNESS;
extern FILE     *ostream;
extern UC     hasWarning;
extern UC printWarnings_flag;
extern mp3File_t     buFile;

extern U2 samplingRate_idx[3][3];
extern U2 samplesPerFrame_idx[3][3];

UC tconArray[256][22];

#endif //EXTERN_
