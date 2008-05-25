#include "FBLib_av.h"
#include "../libFireBird.h"

// ------------------------------ YUV2RGB2 ------------------------------------
//
void YUV2RGB2 ( word yy, word uu, word vv, int *r, int *g, int *b )
{
  signed int            _r,_g,_b;
  signed int            y, u, v;

  y = yy; // - 12;
  u = (int)uu - 128;
  v = (int)vv - 128;

  _r = YUVR(y,u,v);
  _g = YUVG(y,u,v);
  _b = YUVB(y,u,v);

  *r = _Clip(_r);
  *g = _Clip(_g);
  *b = _Clip(_b);
}
