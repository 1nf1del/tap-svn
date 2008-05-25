#include "FBLib_flash.h"

//--------------------------------------- FlashReindexFavourites --------------------------------
//
void FlashReindexFavourites (word ServiceIndex, int ServiceType, ReindexType itype)
{
  TYPE_Fav              *Favs;
  byte                  i;

  if (itype != delete && itype != insert) return;

  Favs = (TYPE_Fav *) (FlashBlockOffset[Favourites] + 4);

  while (*Favs->FavName)
  {
    for (i = 0; i < Favs->NrOfEntries; i++)
    {
      if (delete)
      {
        if (Favs->ServiceIndex[i] > ServiceIndex && Favs->Flags[i] == ServiceType) Favs->ServiceIndex[i]--;
      }
      else // insert
      {
        if (Favs->ServiceIndex[i] >= ServiceIndex && Favs->Flags[i] == ServiceType) Favs->ServiceIndex[i]++;
      }
    }

    Favs++;
  }
}
