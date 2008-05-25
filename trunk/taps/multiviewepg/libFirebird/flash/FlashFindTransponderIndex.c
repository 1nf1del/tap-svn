#include "FBLib_flash.h"

//--------------------------------------- FlashFindTransponderIndex --------------------------------
//
int FlashFindTransponderIndex (dword SatIndex, dword NetworkID, dword TSID)
{
  dword                 TrCount, i;
  int                   Ret = -1;

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized || (FlashOffset == 0)) return 0;

  switch (SystemType)
  {
    case ST_DVBS:
    {
      TYPE_TpInfoS  *Transponder;

#ifdef DEBUG_FIREBIRDLIB
      TAP_Print ("FireBirdLib: FindTransponderIndex: Sat=%d, TSID=%d ", SatIndex, TSID);
#endif

      TrCount = *((dword *) (FlashBlockOffset [Transponders] + 4));
      Transponder = (TYPE_TpInfoS *) (FlashBlockOffset [Transponders] + 4 + 4);
      i=-1;
      while (TrCount > 0)
      {
        if (Transponder->SatIdx == SatIndex) i++;
        if ((Transponder->SatIdx == SatIndex) && (Transponder->TSID == TSID)) break;
        Transponder++;
        TrCount--;
      }
      if (TrCount != 0)
      {

#ifdef DEBUG_FIREBIRDLIB
        TAP_Print ("@ 0x%8.8x (Index %d)\n", (dword) Transponder, i);
#endif

        Ret = i;
      }
      else
      {

#ifdef DEBUG_FIREBIRDLIB
        TAP_Print ("not found\n");
#endif

        Ret = -1;
      }
      break;
    }

    case ST_DVBT:
    case ST_DVBT5700:
    {
      byte *Transponder;

#ifdef DEBUG_FIREBIRDLIB
      TAP_Print ("FireBirdLib: FindTransponderIndex: NetworkID=%d, TSID=%d ", NetworkID, TSID);
#endif

      TrCount = *((dword *) (FlashBlockOffset [Transponders] + 4));
      Transponder = (byte *) (FlashBlockOffset [Transponders] + 4 + 4);
      i=-1;

      while (TrCount > 0)
      {
        i++;
        // fortunately, the structures are identical until NetworkID (and TSID)
        if ((((TYPE_TpInfoT *) Transponder)->NetworkID == NetworkID) && (((TYPE_TpInfoT *) Transponder)->TSID == TSID)) break;
        Transponder += (SystemType == ST_DVBT ? sizeof(TYPE_TpInfoT) : sizeof(TYPE_TpInfoT5700));
        TrCount--;
      }
      if (TrCount != 0)
      {
#ifdef DEBUG_FIREBIRDLIB
        TAP_Print ("@ 0x%8.8x (Index %d)\n", (dword) Transponder, i);
#endif
        Ret = i;
      }
      else
      {
#ifdef DEBUG_FIREBIRDLIB
        TAP_Print ("not found\n");
#endif
        Ret = -1;
      }
      break;
    }

    case ST_DVBC:
    {
      TYPE_TpInfoC  *Transponder;

#ifdef DEBUG_FIREBIRDLIB
      TAP_Print ("FireBirdLib: FindTransponderIndex: NetworkID=%d, TSID=%d ", NetworkID, TSID);
#endif

      //2005-15-11 Mod: Run from the last entry towards the first entry. This should prevent to hit a dummy 47MHz Premiere transponder
      TrCount = *((dword *) (FlashBlockOffset [Transponders] + 4));
      Transponder = (TYPE_TpInfoC *) (FlashBlockOffset [Transponders] + 4 + 4 + sizeof(TYPE_TpInfoC) * (TrCount -1));

      while (TrCount > 0)
      {
        if ((Transponder->NetworkID == NetworkID) && (Transponder->TSID == TSID)) break;
        Transponder--;
        TrCount--;
      }
      if (TrCount != 0)
      {
#ifdef DEBUG_FIREBIRDLIB
        TAP_Print ("@ 0x%8.8x (Index %d)\n", (dword) Transponder, TrCount-1);
#endif
        Ret = (TrCount-1);
      }
      else
      {
#ifdef DEBUG_FIREBIRDLIB
        TAP_Print ("not found\n");
#endif
        Ret = -1;
      }
      break;
    }

    case ST_UNKNOWN:
    {
      break;
    }
  }
  return Ret;
}
