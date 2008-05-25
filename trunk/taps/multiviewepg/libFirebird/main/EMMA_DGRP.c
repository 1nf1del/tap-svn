#include "tap.h"

//DGRP = Graphics processor control - OSD and cursor planes
volatile dword *EMMA_DGRP_RELCTR   = (dword *) 0xb0004100;

volatile dword *EMMA_DGRP_HWCCTR   = (dword *) 0xb000410c;
volatile dword *EMMA_DGRP_HWCSTART = (dword *) 0xb0004110;
volatile dword *EMMA_DGRP_b0004120 = (dword *) 0xb0004120;
volatile dword *EMMA_DGRP_b0004138 = (dword *) 0xb0004138;
volatile dword *EMMA_DGRP_b0004148 = (dword *) 0xb0004148;
volatile dword *EMMA_DGRP_b000414c = (dword *) 0xb000414c;
volatile dword *EMMA_DGRP_b0004150 = (dword *) 0xb0004150;
volatile dword *EMMA_DGRP_b0004154 = (dword *) 0xb0004154;
