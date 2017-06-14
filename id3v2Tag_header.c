#include "id3v2Tag_header.h"
#include "id3v2Tag.h"
#include "fileList.h"
#include "byte.h"
#include "mfile.h"
#include "str.h"
#include "err.h"
#include "mp3File.h"
#include "extern.h"
#include "prints.h"
#include "brew_types.h"

id3v2Tag_header_t *get_id3v2Tag_header(mp3File_t *file)            { return &(file->id3v2Tag).header; }

UC   get_id3v2Tag_presenceFlag(mp3File_t *file)          { return get_id3v2Tag_header(file)->presenceFlag; }
UC   get_id3v2Tag_version     (mp3File_t *file)          { return get_id3v2Tag_header(file)->version;        }
UC   get_id3v2Tag_revision  (mp3File_t *file)          { return get_id3v2Tag_header(file)->revision; }
UC   get_id3v2Tag_unsync_presenceFlag (mp3File_t *file)          { return get_id3v2Tag_header(file)->unsync_presenceFlag;  }
UC   get_id3v2Tag_xtdHdr_presenceFlag (mp3File_t *file)          { return get_id3v2Tag_header(file)->xtdHdr_presenceFlag;  }
U4   get_id3v2Tag_xtdHdr_size  (mp3File_t *file)          { return get_id3v2Tag_header(file)->xtdHdr_size;  }
//UC   get_id3v2Tag_exp_presenceFlag  (mp3File_t *file)          { return get_id3v2Tag_header(file)->exp_presenceFlag;  }
//UC   get_id3v2Tag_footer_presenceFlag (mp3File_t *file)          { return get_id3v2Tag_header(file)->footer_presenceFlag;  }

void set_id3v2Tag_info(mp3File_t *file, UC version, UC revision, UC unsync, UC xtdHdr, U4 xtdHdr_size, UC exp, UC footer)
{ id3v2Tag_header_t *header = get_id3v2Tag_header(file);

  header->presenceFlag        = true;
  header->version             = version;
  header->revision            = revision;
  header->unsync_presenceFlag = unsync;
  header->xtdHdr_presenceFlag = xtdHdr;
  header->xtdHdr_size         = xtdHdr_size;
  header->exp_presenceFlag    = exp;
  header->footer_presenceFlag = footer;
}

UC check_id3v2Tag_presence(mp3File_t *file)
{ U4 sig;
  UC version;
  UC revision;
  UC flags;
  UC unsync;
  UC xtdHdr;  U4 xtdHdr_size = 0;
  UC exp;
  UC footer;

  mseek(file, 0, SEEK_SET);
  mreadU4(file, &sig);
  if ((sig & 0xFFFFFF00) != 0x49443300)   return FAILURE;//return SUCCESS; // not a Tag, but not an issue

  version = sig; //(UC) (sig & 0xFF)
  mreadUC(file, &revision);

  if ((version!=3) && (version!=4))  { return pReturn(file, "We only handle id3 Tag version 2.3 and 2.4"); }
  if (revision==0xFF)                  {  printWarning(file);   fprintf(ostream, "WARNING, revision number invalid; ignored.\n"); }

  mreadUC(file, &flags);

  unsync = ((flags & 0x80) == 0x80);
  xtdHdr = ((flags & 0x40) == 0x40);
  exp    = ((flags & 0x20) == 0x20);
  footer = ((flags & 0x10) == 0x10);

  if (version == 4)
    {  if (footer)                 {  printWarning(file);   fprintf(ostream, "WARNING, Presence of a 10-byte footer at end of Tag; ignored.\n"); }
       if ((flags & 0x0F) != 0x00) {  printWarning(file);   fprintf(ostream, "WARNING, Extra flags set; ignored.\n"); } }
  else if ((flags & 0x1F) != 0x00) {  printWarning(file);   fprintf(ostream, "WARNING, Extra flags set; ignored.\n"); }

  if (exp) {  printWarning(file);   fprintf(ostream, "Experimental Indicator flag set; ignored.\n"); }

  if (xtdHdr)
    { mseek(file, 4, SEEK_CUR);    // pass the tagSize field
      if(mreadU4(file, &xtdHdr_size) == FAILURE)   return pReturn(file, "Invalid id3 Tag"); // <== Wouldn't even be an mp3 file, test probably useless
      switch(version)
	{ case 3: xtdHdr_size += 4; break;
  	  case 4: xtdHdr_size = decodeU4(xtdHdr_size); break;
	  default: ; //can't happen
	}
    }

//   fprintf(ostream, "Id3v2.%d.%d Tag found\n", version, revision);
   set_id3v2Tag_info(file, version, revision, unsync, xtdHdr, xtdHdr_size, exp, footer);

  return SUCCESS;
}
