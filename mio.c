#include <sys/stat.h>
#include <ctype.h>
#include "mio.h"
#include "err.h"
#include "prints.h"

void resetFile(char *filename)
{
  /*
  UC res = unlink(filename);

  if(( res == 0) || ( errno == ENOENT ))
    { // file does not exist now
        return SUCCESS; }

  //fprintf to get -->"Cannot reset file [%] : "
  perror("Cannot reset file : "); return FAILURE;
  */

  FILE *fp = fopen(filename, "w"); // create an empty file or empty an existing one
  if(!fp) { printf("Cannot wipe file. \"%s\" ", filename);   fflush(stdout);   perror("");   pExit("Bye!"); }
  else fclose(fp);
}

UC isNumber(char *s)
{
  UC i;

  for(i=0; i<strlen(s); i++) if(!isdigit(s[i])) return false;
  return true;
}

UC isRegFile(char *fname) // implies that node exists
{
  int status;
  struct stat st_buf;
  
  status = lstat(fname, &st_buf);
  if (status != 0)              return false; // printf ("Error, errno = %d\n", errno); 
  if (S_ISREG (st_buf.st_mode)) return  true;
  return false;
}

UC isDir(char *name) // implies that node exists
{
  int status;
  struct stat st_buf;
  
  status = lstat (name, &st_buf);
  if (status != 0)              return false; // printf ("Error, errno = %d\n", errno);
  if (S_ISDIR (st_buf.st_mode)) return  true;
  return false;
}

UC nodeExists(char *fname)
{
  /*
  FILE *file = fopen(fname, "r");
  if (file != NULL)   { fclose(file);   return true; }
  return false;
  */
  if(access(fname, F_OK)==0) return true;
  return false;
}

UC node_exists     (char * fname){  if(access(fname, F_OK)==0) return true;  return false;}
UC node_readRights (char * fname){  if(access(fname, R_OK)==0) return true;  return false;}
UC node_writeRights(char * fname){  if(access(fname, W_OK)==0) return true;  return false;}

/*
we need directory r-x to get in and read content.
   includes original directory

for files, r-- to read their content
           -w- to write to them

this should be taken care of during fopen(char *name, char *mode)
 */

UC isFlag(char *s)
{ // strcmp returns 0 if equal
//  if ( !strcmp(s, "-b" ))  return B;
//  if ( !strcmp(s, "-r" ))  return R;
  if ( !strcmp(s, "-p" ))  return P;
  if ( !strcmp(s, "-e" ))  return E;
  if ( !strcmp(s, "-w" ))  return W;
  if ( !strcmp(s, "-n" ))  return N;
  if ( !strcmp(s, "-x" ))  return X;
  if ( !strcmp(s, "-t" ))  return T;
  if ( !strcmp(s, "-c" ))  return C;

  if ( !strcmp(s, "-all")) return A;
  if ( !strcmp(s, "-s"))   return S;

  if ( !strcmp(s, "-v1"))  return V1;
  if ( !strcmp(s, "-v2"))  return V2;

  return 0; // not a flag
}
