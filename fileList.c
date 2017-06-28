#include "fileList.h"
#include "includes.h"
#include "str.h"
#include "err.h"
#include "mio.h"
#include "mp3Stream.h"
#include "mp3File.h"
#include "byte.h"
#include "extern.h"

fileList_t good_fList; // <== change name. put in mp3Brew.c ???
//fileList_t  bad_fList;
//U4         fileCount = 0;
U4         rank      = 1;
U4         fileNb    = 0;

/*
1> check if file is mp3
2> get offset of mp3 data <--- offset

may create link during dir processing ( having checked it's an mp3 )
and addlink link to list
*/

char progressStr[9][6] = { ".    ",
			   "..   ",
			   "...  ",
			   ".... ",
			   ".....",
			   " ....",
			   "  ...",
			   "   ..",
			   "    ." };
			   /*
			   "  ...",
			   "  ...",
			   "  ...",
			   "  ...",
			   */

fileLink_t *create_file_link(char *filename, U4 offset)
{
  fileLink_t *newLink = malloc(sizeof(fileLink_t));
  scopy(&(newLink->filename), filename);
  newLink->offset = offset;
  newLink->next = NULL;

  return newLink;
}

void push_file_link(fileList_t *list, char *filename, U4 offset)
{
  fileLink_t *newLink = create_file_link(filename, offset);

  list->fileCount++;

  if(list->first == NULL)
    {
      list->first = newLink;
      list-> last = newLink;
    }
  else
    {
      (list->last)->next = newLink;
       list->last        = newLink;
    }
}

fileLink_t *get_fileLink(fileList_t *list, U4 index)   { return list->ptrList[index]; }

void create_file_ptrList(fileList_t *list)
{
  U4 i;
  fileLink_t *link = list->first;

  list->ptrList = malloc(list->fileCount * sizeof(fileLink_t *));

  for(i=0; i< list->fileCount; i++)
    {
      list->ptrList[i] = link;
      link = link->next;
    }
}

/*void cleanList2(fileList_t *list)
{
  fileLink_t *link = list->first;
  fileLink_t *next;

  while ( link != NULL )
    { next = link->next;
      free(link->filename);
      free(link);
      link = next;
//      list->fileCount--;  //DEBUG
		}
  free(list->ptrList);

  // DEBUG
//  if(list->fileCount !=0) pExit("Error in cleanList");
}*/

void clean_file_list(fileList_t *list)
{
  U4 i;

  for(i=0; i< list->fileCount; i++)
    { free(list->ptrList[i]->filename);
      free(list->ptrList[i]); }

  free(list->ptrList);
}
/*
void printFileList_1(fileList_t *list)
{
  fileLink_t *link = list->first;
  fileLink_t *next;

  while ( link != NULL )
    {
      next = link->next;
      fprintf(ostream, "--> %s\n", link->filename);
      link = next;
    }
}

void printFileList_2(fileList_t *list)
{
  U4 i;

  for(i=0; i< list->fileCount; i++)
    fprintf(ostream, "%s [ ] %d\n", list->ptrList[i]->filename, list->ptrList[i]->offset);
}
*/
int filenameCmp(const void * a, const void * b)   { return strcmp( (*(fileLink_t **)a)->filename, (*(fileLink_t **)b)->filename ); }


UC test_and_addFileToList(fileList_t *list, char *filename, UC print)
{
  mp3File_t         mpFile;
  // <=== in general, in case of real mp3 files, FSTREAM is preferable since the start of the stream is very near the beginning of the file
  // in case a file is not an mp3 file, BSTREAM is faster, but we don't expect to encounter .mp? files to not contain an mpeg stream

  TRY(open_mpFile(&mpFile, filename,  FSTREAM) ,   fprintf(ostream, "Could not open file \"%s\"\n", filename);)
  if(   isMp3File(&mpFile) )
    push_file_link(list, filename, get_stream_start_offset(&mpFile));
  closeMp3File   (&mpFile);

  return SUCCESS;

  // else push_fileLink(&bad_fList, entry_string, get_stream_start_offset(&mpFile));
  // bad list for wrong mp? files and one for non mp? files????
}

UC testFileExtension(char *filename)
{
	U4 len = strlen(filename)-4;
	char *extension = filename + len;

	if( (len>0) &&
			(   (strcmp(extension, ".mp1")==0)		|| (strcmp(extension, ".mp2")==0)   || (strcmp(extension, ".mp3")==0)
		   || (strcmp(extension, ".MP1")==0)		|| (strcmp(extension, ".MP2")==0)		|| (strcmp(extension, ".MP3")==0) ) )
		return SUCCESS;

	return FAILURE;
}



UC processFiles(UC (*func)(fileLink_t *))
{
  U4 i;
  U4 fileCount = good_fList.fileCount;

  if(fileCount == 0) { fprintf(ostream, "No file to process\n"); return FAILURE; }
  if(fileCount==1)
    {      TRY(func(good_fList.ptrList[0]) , {})    }
  else
    {
      qsort(good_fList.ptrList, fileCount, sizeof(fileLink_t *), filenameCmp);
      fprintf(ostream, "Processing %"PRId32" file(s)\n", fileCount); // <== printf too??

      for(i=0; i<fileCount; i++)
	{ if( printWarnings_flag == false ) //!( (option==E) && (printMode==SCAN)) )
	    fprintf(ostream, "====> --- Filename: %s\n", good_fList.ptrList[i]->filename);
	  printf("\rprogress: %"PRId32"/%"PRId32"", ++fileNb, good_fList.fileCount);   fflush(stdout);



	  // <=== IN COMMON
	  // will open file, call setMp3_offsets() on file with OFFSET retrieved at list creation
	  //
	  // check in all "funcs"
	  //   TRY(setMp3File       (&mpFile, filename, offset, FSTREAM),     fprintf(ostream, "Could not open file \"%s\"\n", filename);)
	  // pass mpFile as arg to func(...)
	  // or just insert   setMp3_offsets()   in all the functions <===============================

 //	  TRY(func(good_fList.ptrList[i]), {})
	  TRY(func(get_fileLink(&good_fList, i)) , {})
	  //
	  // <===



	    /**/hasWarning = false;

	  if( !( (option==E) && (printMode==SCAN)) )   fprintf(ostream, "\n");
	}

      printf("\n");
      fprintf(ostream, "%"PRId32" file(s) processed\n", fileCount); // <== printf too?
    }

  return SUCCESS;
}

UC generateFileList(char *entry)
{
  struct stat entry_stat;

	if(lstat(entry, &entry_stat)==0) // else, we may want to check the value of errno
	  if( !S_ISLNK(entry_stat.st_mode) ) // WE DO NOT WANT TO FOLLOW LINKS
		{
		  if      (S_ISREG(entry_stat.st_mode)) 	test_and_addFileToList(&good_fList, entry, true);
		  else if (S_ISDIR(entry_stat.st_mode))   addFilesFromDir(entry);
	  }

	create_file_ptrList(&good_fList);

  return SUCCESS;
}

UC addFilesFromDir(char *path)
{
  // This function will be used to create a list of files to be processed. (and maybe one of files not to process -- or we simply print the filenames?)
  // Files will be added IF they have "mp?" extension (AND mp3Data is found)
  //
  // then functions will be passed the list and process every file on the list.


  // we recursively find every .mp? file
  DIR        *currentDir;
  struct dirent   *entry;  // Possibly not POSIX | entry->d_name is the only field you can count on in all POSIX systems.
  char     *entry_string;
  struct stat entry_stat;
  int lstat_res;

  if ( (currentDir  = opendir(      path)) != NULL) // Directory CAN be opened
    while(   (entry = readdir(currentDir)) != NULL)
      {
				entry_string = malloc(strlen  (path) + 1 + strlen(entry->d_name) + 1);
				sprintf(entry_string, "%s/%s", path,              entry->d_name);
				lstat_res = lstat(entry_string, &entry_stat);
				// <=== should check for errors such as
				// [ELOOP] : A loop exists in symbolic links encountered during resolution of the path argument.
				// [ENAMETOOLONG] : The length of the path argument exceeds {PATH_MAX} or a pathname component is longer than {NAME_MAX}.

				if(lstat_res==0) // else, we may want to check the value of errno
	  			if( !S_ISLNK(entry_stat.st_mode) ) // WE DO NOT WANT TO FOLLOW LINKS
	    		{
	      		if (S_ISDIR(entry_stat.st_mode)) // it's a directory
						{ if( (strcmp(entry->d_name, ".")!=0) && (strcmp(entry->d_name, "..")!=0) )
		    			{ if( (entry_stat.st_mode & S_IXUSR) && (entry_stat.st_mode & S_IRUSR) )  // READ and EXECUTE rights
								addFilesFromDir(entry_string); } }
	      		else if (S_ISREG(entry_stat.st_mode))// it's a file
						{
		  				//printf("\r%s %d files checked", progressStr[rank%10], rank); fflush(stdout);
		  				printf("\r%5d files checked", rank); fflush(stdout);
		  				rank++;
		  				// default, we only process likely-to-be-ok files
		  				if(testFileExtension(entry_string) == SUCCESS)
		    				test_and_addFileToList(&good_fList, entry_string, true);
						}
	      	// else
	      	// add to not_ok_list
	      	//// increase total file count and/or total good-file count
	    	}
			free(entry_string);
    }
  closedir(currentDir);

	return SUCCESS;
}
