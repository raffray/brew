#ifndef FILE_LIST_
#define FILE_LIST_

#include "includes.h"
#include "brew_types.h"

typedef struct fileLink
{
  char *filename;
  U4 offset;
  // filesize;
  fileLink_t *next;
} fileLink_t;

typedef struct fileList
{
  U4 fileCount;
  fileLink_t *first;
  fileLink_t * last;
  fileLink_t **ptrList;
} fileList_t;

fileLink_t *create_file_link (char *, U4);

void      push_file_link(fileList_t *, char *, U4);
void create_file_ptrList(fileList_t *);
void     clean_file_list(fileList_t *);

//void printFileList_1(fileList_t *);
//void printFileList_2(fileList_t *);

int filenameCmp(const void *, const void *);
UC test_and_addFileToList(fileList_t *, char *, UC);
UC      processFiles(UC (*)(fileLink_t *));
UC generateFileList(char *);
UC addFilesFromDir(char *);

#endif // FILE_LIST_
