#include "FBLib_flash.h"

void FlashDeleteFavourites (void)
{
  dword favs;

  if (!LibInitialized) InitTAPex();
  if (!LibInitialized || (FlashOffset == 0)) return;

  favs = FlashBlockOffset[Favourites] + 4;

  memset((byte *) favs, 0, MAXFAVS * sizeof(TYPE_Fav));
}
