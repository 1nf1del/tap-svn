#include "FBLib_av.h"
#include "../libFireBird.h"

// ------------------------------ SaveBitmap ------------------------------------
//
bool SaveBitmap (char *strName, int width, int height, byte* pBuffer )
{
  TYPE_File             *pFile;

  if( !pBuffer || !strName ) return FALSE;

  TAP_Hdd_Create( strName, ATTR_NORMAL );
  pFile = TAP_Hdd_Fopen( strName );
  if ( !pFile ) return FALSE;

  // Write Header
  BMP_WriteHeader( pFile, width, height );

  // write bitmap data
  TAP_Hdd_Fwrite (pBuffer, width*3, height, pFile);
  HDD_TouchFile  (pFile);
  TAP_Hdd_Fclose (pFile);

  return TRUE;
}
