#include "FBLib_av.h"
#include "../libFireBird.h"

// ------------------------------ BMP_WriteHeader ------------------------------------
//
void BMP_WriteHeader (TYPE_File *pFile, int width, int height )
{
  static struct BMP_HEAD head;
  static struct BMP_INFO info;
  dword                  i, size;

  size = sizeof( head ) + sizeof( info ) + (width * height) * 3;

  head.id[0]          = 'B';
  head.id[1]          = 'M';
  head.size_l         = LE16( (size & 0xffff) );
  head.size_h         = LE16( (size >> 16) );
  i                   = (sizeof( head ) + sizeof( info ));
  head.offset_l       = LE16( i & 0xffff );
  head.offset_h       = LE16( i >> 16 );

  info.info_size      = LE32( 40 );
  info.width          = LE32( width );
  info.height         = LE32( height );
  info.plane          = LE16( 1 );
  info.bits_per_pixel = LE16( 24 );
  info.compress       = LE32( 0 );
  info.img_size       = LE32( (width * height) * 3 );
  info.x_res          = 0;
  info.y_res          = 0;
  info.color          = 0;
  info.icolor         = 0;

  TAP_Hdd_Fwrite( &head, sizeof( head ), 1, pFile );
  TAP_Hdd_Fwrite( &info, sizeof( info ), 1, pFile );
}
