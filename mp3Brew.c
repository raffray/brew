// OPTIONS
//
// Originally, BREW: Backup/Restore/Examine/Wipe
// ... backup and restore isn't really a viable scheme. Options removed.
//
// P --> print id3 tag info
//
// A --> Analyze the entire file structure
//       print iD3 tags + structure.
//
// S --> Silent scan
//       Only reportcorruptions/inconsistencies
//
// OPTIONS that will modify a file's content will only apply to uncorrupt files
// Corrupt meaning: a break in the data stream
//
// C --> Cleanup the iD3v2 tag
//       Remove unwanted frames and the footer and extended header if present
//       Remove compressed and/or encrypted tags
//       Unsync frames/tag
//       All flags removed (including datalength; do we want to keep that?)
//       Extract pictures (From APIC frames. Frames will be removed from tag)
//
// F --> Fix
//       like "Cleanup"
//       + adjust padding size (256 bytes)
//         fix or create the vbr frame
//         if id3v2 tag not present but id3v1 tag is, create id3v2 tag from it
//         delete all data after stream (includes tags)
//
// T --> Cut
//       create a new file including data-stream between 2 frames or timestamps
//       new file will only have a data-stream. No tag?

//#include <unistd.h>
//#include <math.h>
//#include <mcheck.h> /* Header file to include mtrace related functions */

#include "includes.h"

#include "mp3Brew.h"
#include "mp3File.h"
#include "extern.h"

#include "mio.h"
#include "err.h"
#include "fileList.h"

#include "cleanTag.h"
#include "examine.h"
#include "wipeFile.h"

#include "renameFile.h"
#include "extractPics.h"

#include "fileList.h"

//UC          verbose1 = 0;
//UC          verbose2 = 0;
FILE        *ostream;       // either stdout or a logfile

UC            option;
UC printWarnings_flag = false;
UC         hasWarning = false;


UC (*funcs[OPTION_COUNT])   (fileLink_t *link);
UC doNothing                (fileLink_t *link)   { return pExit("Invalid option [print usage]"); }


/* NEXT feature... extract APIC into filename.jpg (overwrite if exists) */
/*               */
/* During wipe??? */

// keep APIC in SELECT restore mode?
    /*
    // add option -f (fix vbr)
    //  if any issue is detected with a vbr frame...  generate new vbrFrame (if there is none)
    //
    // if createXing successful
    //
    // to add vbr... consider how much "room" we have available between end-of-tag (or start of file) and start of stream.
    // if we have enough, 0 out until what would be start of vbr frame and write vbr-frame
    // ELSE rewrite file. TAG | padding | vbr-frame | stream
    */

void preDispatch()
{ // Wipe logfile and reset ostream
  resetFile("logfile");
  ostream = stdout;
  memset(&good_fList, 0, sizeof(fileList_t));
//  memset(& bad_fList, 0, sizeof(fileList_t));
}

UC dispatch(int argc, char **argv)
{
// prog -e file [-s]                ... print the id3Tag and examines the mp3 data for frame corruptions
// prog -e dir  [-s]                ...
// prog -w file        (TESTS ONLY) ... wipe id3Tag + padding + data after last good frame
// prog -w dir                      ... wipe id3Tag + padding + "  of all mp3s found in current dirrectory
//                    [-v1]         ... extra debugging info (to make sure what retrieve is ok)
//                    [-v2]         ... more  debugging info (extra dev tests)
//
// we process the number of args we expect, ignoring possible extra ones

  if(argc<3)  return pExit("Error, expecting file/directory argument");
  option = isFlag(argv[1]);

  if(isDir(argv[2]))
    { ostream = fopen("logfile", "a");
      if(ostream == NULL)   pExit("Could not open logfile"); }

  switch (option)
    {
//    case B  : resetFile(backupFilename);                                                                         break;
//    case R  : if ( (argc>3) && (isFlag(argv[3]) == A))   { restoreMode = ALL;       printf("OOO\n");        }    break;
    case E  : if ( (argc>3) && (isFlag(argv[3]) == S))   { printMode   = SCAN;   printWarnings_flag = true; }    break;
//      /**/                                                                                                    else   break;
    case P  :                                                                                                        break;
    case W  :                                                                                                        break;
    case N  :                                                                                                        break;
    case X  :                                                                                                        break;
    case T  :                                                                                                        break;
    case C  :                                                                                                        break;
    default : option = 0;
    }

  /*
    add option F ---> fix vbr
    will remove frame if stream is vbr
    *    correctly set bytecount and framecount if they are wrong
    *    change flag 'Info' to 'XING'
    *    create a vbr frame (XING) if stream is vbr and doesn't have one
    ****  generate TOC or not? --> still need to check what header to chose
    ****  to fit the info we need... we need the frame-duration (length) to
    ****  be the same has other frames (and they all need to have the same length
    ****  for the vbr frame to be coherent.
    *     DO NOT CREATE VBR FRAME IF length is not constant

    FORMULA :   hdr->length  = (float) samplesPerFrame_idx [ hdr->versionId -1] [ hdr->layer - 1 ] / hdr->samplingRate;
    Shows that length depends on 1: Version
    //                           2: Layer
    //                           3: Sampling Rate
    //                          (4: samples per frame which actually depends on version and layer)
    // 2 headers are equivalent IF :
    // versions are equal
    // layers are equal
    // sampleRates are equal
    //(might want to drop the channel mode?)
    // So, in short, if there is a change of header specs... WE CAN'T CREATE A VBR HEADER
    // ( technically, we could have a change in specs and still have a constant length.
    // But this still should be considered a corruption!!
    // NO FIXING WILL BE DONE IF WE HAVE SPEC CHANGES (AND MAYBE BROKEN CHAINS TOO???)
    *
    *     Might want to add to -s option to warn about non constant length too...
   */

  generateFileList(argv[2]);

//  printFileList_2(&good_fList); exit(42);

  return processFiles(funcs[option]);
}

void postDispatch()
{ if (ostream != stdout)   fclose(ostream);
  clean_file_list(&good_fList);                   }


int main(int argc, char **argv)
{
  ostream = stdout;

  tests(argc, argv); // <== to remove

  funcs[0] =        doNothing;
//  funcs[B] =        doNothing;
//  funcs[R] =        doNothing;
  funcs[P] =     print_tags;
  funcs[E] =     examine_file;
  funcs[W] =        wipe_file;
  funcs[N] =      reName_file;
  funcs[X] = extractPics_file;
  funcs[T] = test_file;
  funcs[C] = cleanTag_file;

  preDispatch();
  dispatch(argc, argv);
  postDispatch();

  return SUCCESS;
}


UC test_file(fileLink_t *link)
{ mp3File_t mpFile;
  char   *filename = link->filename;
  U4        offset = link->offset;

  setMp3File  (&mpFile, filename,  offset, BSTREAM); // <== FSTREAM?
//  printf("%s\n", filename);
//  printU4(get_stream_start_offset(&mpFile), NL);
//  printU4(get_firstFrame_offset  (&mpFile), NL); exit(42);

  if (retrieve_id3v2Tag(&mpFile) == FAILURE)   { closeMp3File(&mpFile);   return SUCCESS; }

  print_id3v2Tag(&mpFile);
  closeMp3File(&mpFile);

  return SUCCESS;
}
