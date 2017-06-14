#include "err.h"
#include "extern.h"

UC pExit          (const char *s) { fprintf(ostream, "Something went wrong... %s\n", s); exit(42); }
//UC pReturn_silent (const char *s) { fprintf(ostream,          "%s\n", s);          return FAILURE; }
//UC pReturn_warning(const char *s) { fprintf(ostream, "WARNING: %s\n", s);          return FAILURE; }

UC pReturn(mp3File_t *file, const char *s) { printWarning(file);   fprintf(ostream, "%s\n", s);   return FAILURE; }

/*
void printWarning(mp3File_t *file)
{ if ((printWarnings_flag == true) && (hasWarning == false))   { fprintf(ostream, "----> %s\n", get_filename(file));   hasWarning = true; }
  fprintf(ostream, "WARNING: "); }
*/

