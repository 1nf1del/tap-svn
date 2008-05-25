#include "../libFireBird.h"

//--------------------------------------- GetCurrentEvent --------------------------------
//
TYPE_TapEvent *GetCurrentEvent (int *nCurEvent)
{
  TYPE_TapEvent         *evtInfo;
  unsigned int          eventNum, i;
  int                   svcType, svcNum;

  TAP_Channel_GetCurrent( &svcType, &svcNum );          // get current channel
  if ( svcType != SVC_TYPE_Tv ) return NULL;            // cancel if not in TV mode
  evtInfo = TAP_GetEvent( svcType, svcNum, &eventNum ); // get events

  if(evtInfo)
  {
    for( i = 0; i < eventNum; i++ )                     // find current event
    {
      if( evtInfo[i].runningStatus == 4 )               // current event found
      break;
    }
  }

  if( evtInfo && evtInfo[i].runningStatus == 4)
  {
    if ( nCurEvent ) *nCurEvent = i;
    return evtInfo;
  }

  if ( evtInfo ) TAP_MemFree( evtInfo );
  return NULL;
}
