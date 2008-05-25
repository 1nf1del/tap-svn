#include <string.h>
#include "FBLib_flash.h"

bool FlashAddFavourite (char *FavName, word Idx, bool TVService)
{
  TYPE_Fav              *Favs;
  byte                  i;

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized || (FlashOffset == 0)) return FALSE;

  if (!FavName || !*FavName || !FlashGetServiceByIndex(Idx, TVService)) return FALSE;

  Favs = (TYPE_Fav *) (FlashBlockOffset[Favourites] + 4);

  for (i = 0; i < MAXFAVS; i++, Favs++)
  {
    if (strncmp(FavName, Favs->FavName, sizeof(Favs->FavName)) == 0)
    {
      if (Favs->Flags[0] != (TVService ? SVC_TYPE_Tv : SVC_TYPE_Radio)) return FALSE;
      if (Favs->NrOfEntries >= sizeof(Favs->ServiceIndex) / sizeof(word)) return FALSE;

      Favs->ServiceIndex[Favs->NrOfEntries] = Idx;
      Favs->Flags[Favs->NrOfEntries] = (TVService ? SVC_TYPE_Tv : SVC_TYPE_Radio);
      Favs->NrOfEntries++;

      return TRUE;
    }
    else if (*Favs->FavName == '\0')
    {
      strncpy(Favs->FavName, FavName, sizeof(Favs->FavName));
      Favs->ServiceIndex[0] = Idx;
      Favs->Flags[0] = (TVService ? SVC_TYPE_Tv : SVC_TYPE_Radio);
      Favs->NrOfEntries = 1;

      return TRUE;
    }
  }

  return FALSE;
}
