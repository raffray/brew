#include "frameID.h"
//#include "byte.h"

UC is_tag_char_valid(UC c)
{
  // each of the 4 char of the tag name must be composed of A-Z or 0-9
  if ( (   (c>0x29) && (c<0x3A))
       || ((c>0x40) && (c<0x5B)) )
    return true;
  return false;
}

UC is_tag_valid(U4 tag)
{
  return (is_tag_char_valid   ( (tag & 0x000000FF) )
	  && is_tag_char_valid( (tag & 0x0000FF00) >>8)
	  && is_tag_char_valid( (tag & 0x00FF0000) >>16)
	  && is_tag_char_valid( (tag & 0xFF000000) >>24) );
}



UC is_Txyz_tag(U4 t)   { return ((t!= 0x54585858) && ((t & 0xFF000000) == 0x54000000)); } //  return ( ((t & 0xFF000000) == 0x54000000) );
UC is_TXXX_tag(U4 t)   { return (t == 0x54585858); }
UC is_TCON_tag(U4 t)   { return (t == 0x54434F4E); }
UC is_COMM_tag(U4 t)   { return (t == 0x434F4D4D); }
UC is_POPM_tag(U4 t)   { return (t == 0x504F504D); }
UC is_APIC_tag(U4 t)   { return (t == 0x41504943); }

/*
UC is_PCNT_tag(UC c1, UC c2, UC c3, UC c4)   { return ((c1=='P') && (c2=='C') && (c3=='N') && (c4=='T') ); }
UC is_OWNE_tag(UC c1, UC c2, UC c3, UC c4)   { return ((c1=='O') && (c2=='W') && (c3=='N') && (c4=='E') ); }
*/
