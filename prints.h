#ifndef PRINTS_
#define PRINTS_

#include "includes.h"
#include "brew_types.h"

void printWarning(mp3File_t *);
void printNspace(int);
void printDuration (float, UC);
void print_int_commas(U4);
void print_byteCount   (    const char *, U4, UC);
void printSectionTitle (    const char *);
void printSection_short(UC, const char *, U4, U4);
void printSection_long (    const char *, U4, U4, U4, mp3Header_t *, float, float);
void printSection_long2(const char *str, marker_t *m1, marker_t *m2);

#endif // PRINTS_
