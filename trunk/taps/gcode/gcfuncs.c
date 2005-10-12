//------------------------------ Information --------------------------------------
//
// GCode Functions
//
// Author:	John De Angelis  email: j_de_angelis@hotmail.com
// Date:	4 September 2004
// Description:
//
//			This module contains all the gcode decoding code. The entry point 
//			is the decode function. It takes a gcode in int format and returns
//			the timer information.
//
//			There's not a lot of information on how the algorithm works but gcode
//			is the same as showview in Europe but is not quite the same as the
//			VCRplus/Video+ algorithm used in the US/UK.
//
//			For information on Showview see here: http://www.zelczak.com/showv_js.htm
//			The demo is implemented in Javascript. This implementation is based on
//			that work and that of a VCRplus C implementation.
//---------------------------------------------------------------------------------

#include <string.h>
//#include <stdio.h>
#include <math.h>

//------------------------------ TAP stuff --------------------------------------
//

//TAP stuff here
#include <tap.h>
//#include "tools.h"

#define main(a,b) TAP_Main()


//------------------------------ local stuff --------------------------------------
//
#define		KEY	(68150631)
#define		INVALID	(-987)

int	mixup(int x, int y);
int noOfDigitsIn(int num);
int sumOfDigitsIn(int num);
int func1( int code );
int func2( int code, int *rem );
void func4( int func1_out, int dya, int year, int *off, int *top );
int map_top(int day, int year, int top, int digits);
void bit_shuffle(int mtop, int func5_out, int *tval, int *cval);
void lookup(int i, int *outtime, int *outdur);
void repeat( int *startElem, int startVal, int noOfIter, int incr );

//------------------------------ Functions --------------------------------------
//



//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------

void repeat( int *startElem, int startVal, int noOfIter, int incr )
{
	int *p = startElem;
	int i;
	for (i = startVal; i < startVal+noOfIter; i+=incr)
		*p++ = i;
}


//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------

int noOfDigitsIn(int num)
{
	int pwr, digits;
	pwr = 1 ; digits = 0;
	while(num >= pwr) { digits++; pwr *= 10; }
	return digits;
}

//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------

int sumOfDigitsIn(int num)
{
	/* assume 3 digits */
	return (int)((num % 10) + ((num % 100) / 10) + (num / 100));
}

//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------

int func1( int code )
{
	int hashval, digits;
	hashval = code;
	digits = noOfDigitsIn(code);
	//printf("digits: %d\n", digits);
	do
	{
		hashval = mixup(hashval, KEY) % (int)pow(10, digits);
	} while (hashval < (int)(pow(10, digits-1)));
	return (hashval);
}

//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------

int func2( int code, int *rem )
{
	int bot3, top3, quo;
	bot3 = code % 1000;
	top3 = code / 1000;
	quo = (bot3 - 1) / 32 ;
	*rem = (bot3 - 1) % 32 ;
	return (quo + 1);
}

//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------

void func4( int code, int day, int year, int *off, int *top )
{
	int i, t, top3, digits, toff;

	top3 = (int)(code / 1000);
	digits = noOfDigitsIn(top3);
	toff = sumOfDigitsIn(top3);
	t = top3;

	do
	{
		for (i=0; i<=(year % 16); i++)
			toff = toff + (map_top(day, i, t, digits) % 10);
		t = map_top(day, year, t, digits);
	} while (t < (int)(pow(10, digits-1)));
	*off = (toff % 32);
	*top = (t);
	//return top, off


}


//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------

void bit_shuffle(int t8c5, int t2c1, int *tval, int *cval)
{
	int	tt, cc, b;

	tt = 0 ;
	cc = 0 ;

	/* for showview/gcode t8c5 is t9,c4,c3,t8,t7,t6,t5,t4,t3,c2 */
	/* for vcrplus        t8c5 is t8,c5,t7,c4,t6,t5,t4,c3,c2,t3 */
	/* t2c1 is the same for both */

	/* get t9 thru t3 -gcode */
	b = 0x00000001 & (t8c5 >> 9) ;
	tt += (b << 9);

	b = 0x00000001 & (t8c5 >> 6) ;
	tt += (b << 8);

	b = 0x00000001 & (t8c5 >> 5) ;
	tt += (b << 7);

	b = 0x00000001 & (t8c5 >> 4) ;
	tt += (b << 6);

	b = 0x00000001 & (t8c5 >> 3) ;
	tt += (b << 5);

	b = 0x00000001 & (t8c5 >> 2) ;
	tt += (b << 4);

	b = 0x00000001 & (t8c5 >> 1) ;
	tt += (b << 3);


	/* get c4 thru c2 */
	b = 0x00000001 & (t8c5 >> 8) ;
	cc += (b << 4);

	b = 0x00000001 & (t8c5 >> 7) ;
	cc += (b << 3);

	b = 0x00000001 & t8c5 ;
	cc += (b << 2);


	/* get t2 thru t0 */
	b = 0x00000001 & (t2c1 >> 4) ;
	tt += (b << 2);

	b = 0x00000001 & (t2c1 >> 2) ;
	tt += (b << 1);

	b = 0x00000001 & t2c1 ;
	tt += b;


	/* get c1 thru c0 */
	b = 0x00000001 & (t2c1 >> 3) ;
	cc += (b << 1);

	b = 0x00000001 & (t2c1 >> 1) ;
	cc += b ;

	*tval = tt;
	*cval = cc;
}

//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------

/* the 512-entry tables of starting time and pgm duration */
void lookup(int i, int *outtime, int *outdur)
{
	static	int	anStart[1024], anLength[1024] ;
	static	int	initializer = 0;

	int	j, s;

	if(initializer == 0) {
		int nCount;
	initializer = 1;

	/*
	repeat( &anStart[0], 1900, 10, 30 );
	anStart[10] = 0;
	repeat( &anStart[11], 1830, 27, -30 );

	repeat( &anStart[37], 1900, 10, 30 );
	anStart[47] = 0;
	repeat( &anStart[48], 1830, 27, -30 );

	repeat( &anStart[74], 1900, 10, 30 );
	anStart[84] = 0;
	repeat( &anStart[85], 1830, 10, 30 );

	anStart[95] = 1500;
	anStart[96] = 1430;
	anStart[97] = 600;

	repeat( &anStart[98], 5, 5, 5 );
	repeat( &anStart[103], 1835, 5, 5 );
	repeat( &anStart[108], 1805, 5, 5 );
	repeat( &anStart[113], 1735, 5, 5 );
	repeat( &anStart[118], 1705, 5, 5 );

	repeat( &anStart[123], 1200, 5, 0 );
	repeat( &anStart[128], 1100, 5, 0 );
	repeat( &anStart[133], 1000, 5, 0 );
	repeat( &anStart[138], 900, 5, 0 );
	repeat( &anStart[143], 800, 5, 0 );
	*/

	anStart[0] = 1900;
	anStart[1] = 1930;
	anStart[2] = 2000;
	anStart[3] = 2030;
	anStart[4] = 2100;
	anStart[5] = 2130;
	anStart[6] = 2200;
	anStart[7] = 2230;
	anStart[8] = 2300;
	anStart[9] = 2330;
	anStart[10] = 0;
	anStart[11] = 1830;
	anStart[12] = 1800;
	anStart[13] = 1730;
	anStart[14] = 1700;
	anStart[15] = 1630;
	anStart[16] = 1600;
	anStart[17] = 1530;
	anStart[18] = 1500;
	anStart[19] = 1430;
	anStart[20] = 1400;
	anStart[21] = 1330;
	anStart[22] = 1300;
	anStart[23] = 1230;
	anStart[24] = 1200;
	anStart[25] = 1130;
	anStart[26] = 1100;
	anStart[27] = 1030;
	anStart[28] = 1000;
	anStart[29] = 930;
	anStart[30] = 900;
	anStart[31] = 830;
	anStart[32] = 800;
	anStart[33] = 730;
	anStart[34] = 700;
	anStart[35] = 630;
	anStart[36] = 600;
	anStart[37] = 1900;
	anStart[38] = 1930;
	anStart[39] = 2000;
	anStart[40] = 2030;
	anStart[41] = 2100;
	anStart[42] = 2130;
	anStart[43] = 2200;
	anStart[44] = 2230;
	anStart[45] = 2300;
	anStart[46] = 2330;
	anStart[47] = 0;
	anStart[48] = 1830;
	anStart[49] = 1800;
	anStart[50] = 1730;
	anStart[51] = 1700;
	anStart[52] = 1630;
	anStart[53] = 1600;
	anStart[54] = 1530;
	anStart[55] = 1500;
	anStart[56] = 1430;
	anStart[57] = 1400;
	anStart[58] = 1330;
	anStart[59] = 1300;
	anStart[60] = 1230;
	anStart[61] = 1200;
	anStart[62] = 1130;
	anStart[63] = 1100;
	anStart[64] = 1030;
	anStart[65] = 1000;
	anStart[66] = 930;
	anStart[67] = 900;
	anStart[68] = 830;
	anStart[69] = 800;
	anStart[70] = 730;
	anStart[71] = 700;
	anStart[72] = 630;
	anStart[73] = 600;
	anStart[74] = 1900;
	anStart[75] = 1930;
	anStart[76] = 2000;
	anStart[77] = 2030;
	anStart[78] = 2100;
	anStart[79] = 2130;
	anStart[80] = 2200;
	anStart[81] = 2230;
	anStart[82] = 2300;
	anStart[83] = 2330;
	anStart[84] = 0;
	anStart[85] = 1830;
	anStart[86] = 1900;
	anStart[87] = 1930;
	anStart[88] = 2000;
	anStart[89] = 2030;
	anStart[90] = 2100;
	anStart[91] = 2130;
	anStart[92] = 2200;
	anStart[93] = 2230;
	anStart[94] = 2300;
	anStart[95] = 1500;
	anStart[96] = 1430;
	anStart[97] = 600;
	anStart[98] = 5;
	anStart[99] = 10;
	anStart[100] = 15;
	anStart[101] = 20;
	anStart[102] = 25;
	anStart[103] = 1835;
	anStart[104] = 1840;
	anStart[105] = 1845;
	anStart[106] = 1850;
	anStart[107] = 1855;
	anStart[108] = 1805;
	anStart[109] = 1810;
	anStart[110] = 1815;
	anStart[111] = 1820;
	anStart[112] = 1825;
	anStart[113] = 1735;
	anStart[114] = 1740;
	anStart[115] = 1745;
	anStart[116] = 1750;
	anStart[117] = 1755;
	anStart[118] = 1705;
	anStart[119] = 1710;
	anStart[120] = 1715;
	anStart[121] = 1720;
	anStart[122] = 1725;
	anStart[123] = 1200;
	anStart[124] = 1200;
	anStart[125] = 1200;
	anStart[126] = 1200;
	anStart[127] = 1200;
	anStart[128] = 1100;
	anStart[129] = 1100;
	anStart[130] = 1100;
	anStart[131] = 1100;
	anStart[132] = 1100;
	anStart[133] = 1000;
	anStart[134] = 1000;
	anStart[135] = 1000;
	anStart[136] = 1000;
	anStart[137] = 1000;
	anStart[138] = 900;
	anStart[139] = 900;
	anStart[140] = 900;
	anStart[141] = 900;
	anStart[142] = 900;
	anStart[143] = 800;
	anStart[144] = 800;
	anStart[145] = 800;
	anStart[146] = 800;
	anStart[147] = 800;
	anStart[148] = 700;
	anStart[149] = 700;
	anStart[150] = 700;
	anStart[151] = 700;
	anStart[152] = 700;
	anStart[153] = 600;
	anStart[154] = 600;
	anStart[155] = 600;
	anStart[156] = 600;
	anStart[157] = 600;
	anStart[158] = 1700;
	anStart[159] = 1700;
	anStart[160] = 1700;
	anStart[161] = 1700;
	anStart[162] = 1700;
	anStart[163] = 1530;
	anStart[164] = 1530;
	anStart[165] = 1530;
	anStart[166] = 1530;
	anStart[167] = 1530;
	anStart[168] = 1500;
	anStart[169] = 1500;
	anStart[170] = 1500;
	anStart[171] = 1500;
	anStart[172] = 1500;
	anStart[173] = 1300;
	anStart[174] = 1300;
	anStart[175] = 1300;
	anStart[176] = 1300;
	anStart[177] = 1300;
	anStart[178] = 1200;
	anStart[179] = 1200;
	anStart[180] = 1200;
	anStart[181] = 1200;
	anStart[182] = 1200;
	anStart[183] = 5;
	anStart[184] = 10;
	anStart[185] = 15;
	anStart[186] = 20;
	anStart[187] = 25;
	anStart[188] = 1735;
	anStart[189] = 1740;
	anStart[190] = 1745;
	anStart[191] = 1750;
	anStart[192] = 1755;
	anStart[193] = 1235;
	anStart[194] = 1240;
	anStart[195] = 1245;
	anStart[196] = 1250;
	anStart[197] = 1255;
	anStart[198] = 1405;
	anStart[199] = 1410;
	anStart[200] = 1415;
	anStart[201] = 1420;
	anStart[202] = 1425;
	anStart[203] = 2015;
	anStart[204] = 2015;
	anStart[205] = 2015;
	anStart[206] = 2015;
	anStart[207] = 2015;
	anStart[208] = 1900;
	anStart[209] = 1900;
	anStart[210] = 1900;
	anStart[211] = 1900;
	anStart[212] = 1900;
	anStart[213] = 1930;
	anStart[214] = 1930;
	anStart[215] = 1930;
	anStart[216] = 1930;
	anStart[217] = 1930;
	anStart[218] = 2000;
	anStart[219] = 2000;
	anStart[220] = 2000;
	anStart[221] = 2000;
	anStart[222] = 2000;
	anStart[223] = 2030;
	anStart[224] = 2030;
	anStart[225] = 2030;
	anStart[226] = 2030;
	anStart[227] = 2030;
	anStart[228] = 2100;
	anStart[229] = 2100;
	anStart[230] = 2100;
	anStart[231] = 2100;
	anStart[232] = 2100;
	anStart[233] = 2130;
	anStart[234] = 2130;
	anStart[235] = 2130;
	anStart[236] = 2130;
	anStart[237] = 2130;
	anStart[238] = 2200;
	anStart[239] = 2200;
	anStart[240] = 2200;
	anStart[241] = 2200;
	anStart[242] = 2200;
	anStart[243] = 2230;
	anStart[244] = 2230;
	anStart[245] = 2230;
	anStart[246] = 2230;
	anStart[247] = 2230;
	anStart[248] = 2300;
	anStart[249] = 2300;
	anStart[250] = 2300;
	anStart[251] = 2300;
	anStart[252] = 2300;
	anStart[253] = 2330;
	anStart[254] = 2330;
	anStart[255] = 2330;
	anStart[256] = 2330;
	anStart[257] = 2330;
	anStart[258] = 0;
	anStart[259] = 0;
	anStart[260] = 0;
	anStart[261] = 0;
	anStart[262] = 0;
	anStart[263] = 1830;
	anStart[264] = 1830;
	anStart[265] = 1830;
	anStart[266] = 1830;
	anStart[267] = 1830;
	anStart[268] = 1800;
	anStart[269] = 1800;
	anStart[270] = 1800;
	anStart[271] = 1800;
	anStart[272] = 1800;
	anStart[273] = 1730;
	anStart[274] = 1730;
	anStart[275] = 1730;
	anStart[276] = 1730;
	anStart[277] = 1730;
	anStart[278] = 1700;
	anStart[279] = 1700;
	anStart[280] = 1700;
	anStart[281] = 1700;
	anStart[282] = 1700;
	anStart[283] = 1630;
	anStart[284] = 1630;
	anStart[285] = 1630;
	anStart[286] = 1630;
	anStart[287] = 1630;
	anStart[288] = 1600;
	anStart[289] = 1600;
	anStart[290] = 1600;
	anStart[291] = 1600;
	anStart[292] = 1600;
	anStart[293] = 1530;
	anStart[294] = 1530;
	anStart[295] = 1530;
	anStart[296] = 1530;
	anStart[297] = 1530;
	anStart[298] = 1500;
	anStart[299] = 1500;
	anStart[300] = 1500;
	anStart[301] = 1500;
	anStart[302] = 1500;
	anStart[303] = 1430;
	anStart[304] = 1430;
	anStart[305] = 1430;
	anStart[306] = 1430;
	anStart[307] = 1430;
	anStart[308] = 1400;
	anStart[309] = 1400;
	anStart[310] = 1400;
	anStart[311] = 1400;
	anStart[312] = 1400;
	anStart[313] = 1330;
	anStart[314] = 1330;
	anStart[315] = 1330;
	anStart[316] = 1330;
	anStart[317] = 1330;
	anStart[318] = 1300;
	anStart[319] = 1300;
	anStart[320] = 1300;
	anStart[321] = 1300;
	anStart[322] = 1300;
	anStart[323] = 1230;
	anStart[324] = 1230;
	anStart[325] = 1230;
	anStart[326] = 1230;
	anStart[327] = 1230;
	anStart[328] = 1900;
	anStart[329] = 1900;
	anStart[330] = 1900;
	anStart[331] = 1900;
	anStart[332] = 1900;
	anStart[333] = 1930;
	anStart[334] = 1930;
	anStart[335] = 1930;
	anStart[336] = 1930;
	anStart[337] = 1930;
	anStart[338] = 2000;
	anStart[339] = 2000;
	anStart[340] = 2000;
	anStart[341] = 2000;
	anStart[342] = 2000;
	anStart[343] = 2030;
	anStart[344] = 2030;
	anStart[345] = 2030;
	anStart[346] = 2030;
	anStart[347] = 2030;
	anStart[348] = 2100;
	anStart[349] = 2100;
	anStart[350] = 2100;
	anStart[351] = 2100;
	anStart[352] = 2100;
	anStart[353] = 2130;
	anStart[354] = 2130;
	anStart[355] = 2130;
	anStart[356] = 2130;
	anStart[357] = 2130;
	anStart[358] = 2200;
	anStart[359] = 2200;
	anStart[360] = 2200;
	anStart[361] = 2200;
	anStart[362] = 2200;
	anStart[363] = 2230;
	anStart[364] = 2230;
	anStart[365] = 2230;
	anStart[366] = 2230;
	anStart[367] = 2230;
	anStart[368] = 2300;
	anStart[369] = 2300;
	anStart[370] = 2300;
	anStart[371] = 2300;
	anStart[372] = 2300;
	anStart[373] = 2330;
	anStart[374] = 2330;
	anStart[375] = 2330;
	anStart[376] = 2330;
	anStart[377] = 2330;
	anStart[378] = 0;
	anStart[379] = 0;
	anStart[380] = 0;
	anStart[381] = 0;
	anStart[382] = 0;
	anStart[383] = 1830;
	anStart[384] = 1830;
	anStart[385] = 1830;
	anStart[386] = 1830;
	anStart[387] = 1830;
	anStart[388] = 1800;
	anStart[389] = 1800;
	anStart[390] = 1800;
	anStart[391] = 1800;
	anStart[392] = 1800;
	anStart[393] = 1800;
	anStart[394] = 1730;
	anStart[395] = 1700;
	anStart[396] = 1630;
	anStart[397] = 1600;
	anStart[398] = 1530;
	anStart[399] = 1500;
	anStart[400] = 1430;
	anStart[401] = 1400;
	anStart[402] = 1330;
	anStart[403] = 1300;
	anStart[404] = 1230;
	anStart[405] = 1200;
	anStart[406] = 1130;
	anStart[407] = 1100;
	anStart[408] = 1030;
	anStart[409] = 1000;
	anStart[410] = 930;
	anStart[411] = 900;
	anStart[412] = 2330;
	anStart[413] = 0;
	anStart[414] = 1830;
	anStart[415] = 1800;
	anStart[416] = 1730;
	anStart[417] = 1700;
	anStart[418] = 1630;
	anStart[419] = 1600;
	anStart[420] = 1530;
	anStart[421] = 1500;
	anStart[422] = 1430;
	anStart[423] = 1400;
	anStart[424] = 1330;
	anStart[425] = 1300;
	anStart[426] = 1230;
	anStart[427] = 1200;
	anStart[428] = 2005;
	anStart[429] = 2010;
	anStart[430] = 2015;
	anStart[431] = 2020;
	anStart[432] = 2025;
	anStart[433] = 1605;
	anStart[434] = 1610;
	anStart[435] = 1615;
	anStart[436] = 1620;
	anStart[437] = 1625;
	anStart[438] = 2035;
	anStart[439] = 2040;
	anStart[440] = 2045;
	anStart[441] = 2050;
	anStart[442] = 2055;
	anStart[443] = 2235;
	anStart[444] = 2240;
	anStart[445] = 2245;
	anStart[446] = 2250;
	anStart[447] = 2255;
	anStart[448] = 1505;
	anStart[449] = 1510;
	anStart[450] = 1515;
	anStart[451] = 1520;
	anStart[452] = 1525;
	anStart[453] = 2035;
	anStart[454] = 2040;
	anStart[455] = 2045;
	anStart[456] = 2050;
	anStart[457] = 2055;
	anStart[458] = 1905;
	anStart[459] = 1910;
	anStart[460] = 1915;
	anStart[461] = 1920;
	anStart[462] = 1925;
	anStart[463] = 1935;
	anStart[464] = 1940;
	anStart[465] = 1945;
	anStart[466] = 1950;
	anStart[467] = 1955;
	anStart[468] = 2005;
	anStart[469] = 2010;
	anStart[470] = 2015;
	anStart[471] = 2020;
	anStart[472] = 2025;
	anStart[473] = 2035;
	anStart[474] = 2040;
	anStart[475] = 2045;
	anStart[476] = 2050;
	anStart[477] = 2055;
	anStart[478] = 2105;
	anStart[479] = 2110;
	anStart[480] = 2115;
	anStart[481] = 2120;
	anStart[482] = 2125;
	anStart[483] = 2135;
	anStart[484] = 2140;
	anStart[485] = 2145;
	anStart[486] = 2150;
	anStart[487] = 2155;
	anStart[488] = 2205;
	anStart[489] = 2210;
	anStart[490] = 2215;
	anStart[491] = 2220;
	anStart[492] = 2225;
	anStart[493] = 1435;
	anStart[494] = 1440;
	anStart[495] = 1445;
	anStart[496] = 1450;
	anStart[497] = 1455;
	anStart[498] = 1405;
	anStart[499] = 1410;
	anStart[500] = 1415;
	anStart[501] = 1420;
	anStart[502] = 1425;
	anStart[503] = 1335;
	anStart[504] = 1340;
	anStart[505] = 1345;
	anStart[506] = 1350;
	anStart[507] = 1355;
	anStart[508] = 1305;
	anStart[509] = 1310;
	anStart[510] = 1315;
	anStart[511] = 1320;
	anStart[512] = 1325;
	anStart[513] = 1235;
	anStart[514] = 1240;
	anStart[515] = 1245;
	anStart[516] = 1250;
	anStart[517] = 1255;
	anStart[518] = 1205;
	anStart[519] = 1210;
	anStart[520] = 1215;
	anStart[521] = 1220;
	anStart[522] = 1225;
	anStart[523] = 2200;
	anStart[524] = 2200;
	anStart[525] = 2200;
	anStart[526] = 2200;
	anStart[527] = 2200;
	anStart[528] = 2230;
	anStart[529] = 2230;
	anStart[530] = 2230;
	anStart[531] = 2230;
	anStart[532] = 2230;
	anStart[533] = 2300;
	anStart[534] = 2300;
	anStart[535] = 2300;
	anStart[536] = 2300;
	anStart[537] = 2300;
	anStart[538] = 2330;
	anStart[539] = 2330;
	anStart[540] = 2330;
	anStart[541] = 2330;
	anStart[542] = 2330;
	anStart[543] = 1330;
	anStart[544] = 1330;
	anStart[545] = 1330;
	anStart[546] = 1330;
	anStart[547] = 1330;
	anStart[548] = 1900;
	anStart[549] = 1900;
	anStart[550] = 1900;
	anStart[551] = 1900;
	anStart[552] = 1900;
	anStart[553] = 2300;
	anStart[554] = 2300;
	anStart[555] = 2300;
	anStart[556] = 2300;
	anStart[557] = 2300;
	anStart[558] = 2030;
	anStart[559] = 2030;
	anStart[560] = 2030;
	anStart[561] = 2030;
	anStart[562] = 2030;
	anStart[563] = 2200;
	anStart[564] = 2200;
	anStart[565] = 2200;
	anStart[566] = 2200;
	anStart[567] = 2200;
	anStart[568] = 1800;
	anStart[569] = 1800;
	anStart[570] = 1800;
	anStart[571] = 1800;
	anStart[572] = 1800;
	anStart[573] = 1100;
	anStart[574] = 1100;
	anStart[575] = 1100;
	anStart[576] = 1100;
	anStart[577] = 1100;
	anStart[578] = 1530;
	anStart[579] = 1530;
	anStart[580] = 1530;
	anStart[581] = 1530;
	anStart[582] = 1530;
	anStart[583] = 1905;
	anStart[584] = 1910;
	anStart[585] = 1915;
	anStart[586] = 1920;
	anStart[587] = 1925;
	anStart[588] = 1935;
	anStart[589] = 1940;
	anStart[590] = 1945;
	anStart[591] = 1950;
	anStart[592] = 1955;
	anStart[593] = 2005;
	anStart[594] = 2010;
	anStart[595] = 2015;
	anStart[596] = 2020;
	anStart[597] = 2025;
	anStart[598] = 2035;
	anStart[599] = 2040;
	anStart[600] = 2045;
	anStart[601] = 2050;
	anStart[602] = 2055;
	anStart[603] = 2105;
	anStart[604] = 2110;
	anStart[605] = 2115;
	anStart[606] = 2120;
	anStart[607] = 2125;
	anStart[608] = 2135;
	anStart[609] = 2140;
	anStart[610] = 2145;
	anStart[611] = 2150;
	anStart[612] = 2155;
	anStart[613] = 2205;
	anStart[614] = 2210;
	anStart[615] = 2215;
	anStart[616] = 2220;
	anStart[617] = 2225;
	anStart[618] = 2235;
	anStart[619] = 2240;
	anStart[620] = 2245;
	anStart[621] = 2250;
	anStart[622] = 2255;
	anStart[623] = 1405;
	anStart[624] = 1410;
	anStart[625] = 1415;
	anStart[626] = 1420;
	anStart[627] = 1425;
	anStart[628] = 1335;
	anStart[629] = 1340;
	anStart[630] = 1345;
	anStart[631] = 1350;
	anStart[632] = 1355;
	anStart[633] = 1305;
	anStart[634] = 1310;
	anStart[635] = 1315;
	anStart[636] = 1320;
	anStart[637] = 1325;
	anStart[638] = 1605;
	anStart[639] = 1610;
	anStart[640] = 1615;
	anStart[641] = 1620;
	anStart[642] = 1625;
	anStart[643] = 1435;
	anStart[644] = 1440;
	anStart[645] = 1445;
	anStart[646] = 1450;
	anStart[647] = 1455;
	anStart[648] = 1335;
	anStart[649] = 1340;
	anStart[650] = 1345;
	anStart[651] = 1350;
	anStart[652] = 1355;
	anStart[653] = 1905;
	anStart[654] = 1910;
	anStart[655] = 1915;
	anStart[656] = 1920;
	anStart[657] = 1925;
	anStart[658] = 2005;
	anStart[659] = 2010;
	anStart[660] = 2015;
	anStart[661] = 2020;
	anStart[662] = 2025;
	anStart[663] = 2335;
	anStart[664] = 2340;
	anStart[665] = 2345;
	anStart[666] = 2350;
	anStart[667] = 2355;
	anStart[668] = 1835;
	anStart[669] = 1840;
	anStart[670] = 1845;
	anStart[671] = 1850;
	anStart[672] = 1855;
	anStart[673] = 1705;
	anStart[674] = 1710;
	anStart[675] = 1715;
	anStart[676] = 1720;
	anStart[677] = 1725;
	anStart[678] = 2035;
	anStart[679] = 2035;
	anStart[680] = 2035;
	anStart[681] = 2035;
	anStart[682] = 2035;
	anStart[683] = 2105;
	anStart[684] = 2105;
	anStart[685] = 2105;
	anStart[686] = 2105;
	anStart[687] = 2105;
	anStart[688] = 2035;
	anStart[689] = 2035;
	anStart[690] = 2035;
	anStart[691] = 2035;
	anStart[692] = 2035;
	anStart[693] = 1905;
	anStart[694] = 1910;
	anStart[695] = 1915;
	anStart[696] = 1920;
	anStart[697] = 1925;
	anStart[698] = 1935;
	anStart[699] = 1940;
	anStart[700] = 1945;
	anStart[701] = 1950;
	anStart[702] = 1955;
	anStart[703] = 2135;
	anStart[704] = 2140;
	anStart[705] = 2145;
	anStart[706] = 2150;
	anStart[707] = 2155;
	anStart[708] = 5;
	anStart[709] = 10;
	anStart[710] = 15;
	anStart[711] = 20;
	anStart[712] = 25;
	anStart[713] = 1835;
	anStart[714] = 1840;
	anStart[715] = 1845;
	anStart[716] = 1850;
	anStart[717] = 1855;
	anStart[718] = 1705;
	anStart[719] = 1710;
	anStart[720] = 1715;
	anStart[721] = 1720;
	anStart[722] = 1725;
	anStart[723] = 1505;
	anStart[724] = 1510;
	anStart[725] = 1515;
	anStart[726] = 1520;
	anStart[727] = 1525;
	anStart[728] = 2335;
	anStart[729] = 2340;
	anStart[730] = 2345;
	anStart[731] = 2350;
	anStart[732] = 2355;
	anStart[733] = 1705;
	anStart[734] = 1710;
	anStart[735] = 1715;
	anStart[736] = 1720;
	anStart[737] = 1725;
	anStart[738] = 2040;
	anStart[739] = 2040;
	anStart[740] = 2040;
	anStart[741] = 2040;
	anStart[742] = 2040;
	anStart[743] = 1935;
	anStart[744] = 1940;
	anStart[745] = 1945;
	anStart[746] = 1950;
	anStart[747] = 1955;
	anStart[748] = 1235;
	anStart[749] = 1240;
	anStart[750] = 1245;
	anStart[751] = 1250;
	anStart[752] = 1255;
	anStart[753] = 2305;
	anStart[754] = 2310;
	anStart[755] = 2315;
	anStart[756] = 2320;
	anStart[757] = 2325;
	anStart[758] = 2045;
	anStart[759] = 2045;
	anStart[760] = 2045;
	anStart[761] = 2045;
	anStart[762] = 2045;
	anStart[763] = 2215;
	anStart[764] = 2215;
	anStart[765] = 2215;
	anStart[766] = 2215;
	anStart[767] = 2215;
	anStart[768] = 2245;
	anStart[769] = 2245;
	anStart[770] = 2245;
	anStart[771] = 2245;
	anStart[772] = 2245;
	anStart[773] = 2045;
	anStart[774] = 2045;
	anStart[775] = 2045;
	anStart[776] = 2045;
	anStart[777] = 2045;
	anStart[778] = 2215;
	anStart[779] = 2215;
	anStart[780] = 2215;
	anStart[781] = 2215;
	anStart[782] = 2215;
	anStart[783] = 2045;
	anStart[784] = 2045;
	anStart[785] = 2045;
	anStart[786] = 2045;
	anStart[787] = 2045;
	anStart[788] = 2245;
	anStart[789] = 2245;
	anStart[790] = 2245;
	anStart[791] = 2245;
	anStart[792] = 2245;
	anStart[793] = 1835;
	anStart[794] = 1840;
	anStart[795] = 1845;
	anStart[796] = 1850;
	anStart[797] = 1855;
	anStart[798] = 1805;
	anStart[799] = 1810;
	anStart[800] = 1815;
	anStart[801] = 1820;
	anStart[802] = 1825;
	anStart[803] = 1735;
	anStart[804] = 1740;
	anStart[805] = 1745;
	anStart[806] = 1750;
	anStart[807] = 1755;
	anStart[808] = 1705;
	anStart[809] = 1710;
	anStart[810] = 1715;
	anStart[811] = 1720;
	anStart[812] = 1725;
	anStart[813] = 1635;
	anStart[814] = 1640;
	anStart[815] = 1645;
	anStart[816] = 1650;
	anStart[817] = 1655;
	anStart[818] = 1605;
	anStart[819] = 1610;
	anStart[820] = 1615;
	anStart[821] = 1620;
	anStart[822] = 1625;
	anStart[823] = 1905;
	anStart[824] = 1910;
	anStart[825] = 1915;
	anStart[826] = 1920;
	anStart[827] = 1925;
	anStart[828] = 1935;
	anStart[829] = 1940;
	anStart[830] = 1945;
	anStart[831] = 1950;
	anStart[832] = 1955;
	anStart[833] = 2005;
	anStart[834] = 2010;
	anStart[835] = 2015;
	anStart[836] = 2020;
	anStart[837] = 2025;
	anStart[838] = 2035;
	anStart[839] = 2040;
	anStart[840] = 2045;
	anStart[841] = 2050;
	anStart[842] = 2055;
	anStart[843] = 2105;
	anStart[844] = 2110;
	anStart[845] = 2115;
	anStart[846] = 2120;
	anStart[847] = 2125;
	anStart[848] = 2135;
	anStart[849] = 2140;
	anStart[850] = 2145;
	anStart[851] = 2150;
	anStart[852] = 2155;
	anStart[853] = 1905;
	anStart[854] = 1910;
	anStart[855] = 1915;
	anStart[856] = 1920;
	anStart[857] = 1925;
	anStart[858] = 1235;
	anStart[859] = 1240;
	anStart[860] = 1245;
	anStart[861] = 1250;
	anStart[862] = 1255;
	anStart[863] = 2305;
	anStart[864] = 2310;
	anStart[865] = 2315;
	anStart[866] = 2320;
	anStart[867] = 2325;
	anStart[868] = 2050;
	anStart[869] = 2050;
	anStart[870] = 2050;
	anStart[871] = 2050;
	anStart[872] = 2050;
	anStart[873] = 2250;
	anStart[874] = 2250;
	anStart[875] = 2250;
	anStart[876] = 2250;
	anStart[877] = 2250;
	anStart[878] = 2050;
	anStart[879] = 2050;
	anStart[880] = 2050;
	anStart[881] = 2050;
	anStart[882] = 2050;
	anStart[883] = 1850;
	anStart[884] = 1850;
	anStart[885] = 1850;
	anStart[886] = 1850;
	anStart[887] = 1850;
	anStart[888] = 1620;
	anStart[889] = 1620;
	anStart[890] = 1620;
	anStart[891] = 1620;
	anStart[892] = 1620;
	anStart[893] = 1805;
	anStart[894] = 1810;
	anStart[895] = 1815;
	anStart[896] = 1820;
	anStart[897] = 1825;
	anStart[898] = 1735;
	anStart[899] = 1740;
	anStart[900] = 1745;
	anStart[901] = 1750;
	anStart[902] = 1755;
	anStart[903] = 1705;
	anStart[904] = 1710;
	anStart[905] = 1715;
	anStart[906] = 1720;
	anStart[907] = 1725;
	anStart[908] = 1635;
	anStart[909] = 1640;
	anStart[910] = 1645;
	anStart[911] = 1650;
	anStart[912] = 1655;
	anStart[913] = 1605;
	anStart[914] = 1610;
	anStart[915] = 1615;
	anStart[916] = 1620;
	anStart[917] = 1625;
	anStart[918] = 1535;
	anStart[919] = 1540;
	anStart[920] = 1545;
	anStart[921] = 1550;
	anStart[922] = 1555;
	anStart[923] = 1505;
	anStart[924] = 1510;
	anStart[925] = 1515;
	anStart[926] = 1520;
	anStart[927] = 1525;
	anStart[928] = 1435;
	anStart[929] = 1440;
	anStart[930] = 1445;
	anStart[931] = 1450;
	anStart[932] = 1455;
	anStart[933] = 1405;
	anStart[934] = 1410;
	anStart[935] = 1415;
	anStart[936] = 1420;
	anStart[937] = 1425;
	anStart[938] = 1335;
	anStart[939] = 1340;
	anStart[940] = 1345;
	anStart[941] = 1350;
	anStart[942] = 1355;
	anStart[943] = 1305;
	anStart[944] = 1310;
	anStart[945] = 1315;
	anStart[946] = 1320;
	anStart[947] = 1325;
	anStart[948] = 1235;
	anStart[949] = 1240;
	anStart[950] = 1245;
	anStart[951] = 1250;
	anStart[952] = 1255;
	anStart[953] = 1205;
	anStart[954] = 1210;
	anStart[955] = 1215;
	anStart[956] = 1220;
	anStart[957] = 1225;
	anStart[958] = 1905;
	anStart[959] = 1910;
	anStart[960] = 1915;
	anStart[961] = 1920;
	anStart[962] = 1925;
	anStart[963] = 2005;
	anStart[964] = 2010;
	anStart[965] = 2015;
	anStart[966] = 2020;
	anStart[967] = 2025;
	anStart[968] = 2205;
	anStart[969] = 2210;
	anStart[970] = 2215;
	anStart[971] = 2220;
	anStart[972] = 2225;
	anStart[973] = 2255;
	anStart[974] = 2255;
	anStart[975] = 2255;
	anStart[976] = 2255;
	anStart[977] = 2255;
	anStart[978] = 1625;
	anStart[979] = 1625;
	anStart[980] = 1625;
	anStart[981] = 1625;
	anStart[982] = 1625;
	anStart[983] = 1805;
	anStart[984] = 1810;
	anStart[985] = 1815;
	anStart[986] = 1820;
	anStart[987] = 1825;
	anStart[988] = 1735;
	anStart[989] = 1740;
	anStart[990] = 1745;
	anStart[991] = 1750;
	anStart[992] = 1755;
	anStart[993] = 1705;
	anStart[994] = 1710;
	anStart[995] = 1715;
	anStart[996] = 1720;
	anStart[997] = 1725;
	anStart[998] = 1635;
	anStart[999] = 1640;
	anStart[1000] = 1645;
	anStart[1001] = 1650;
	anStart[1002] = 1655;
	anStart[1003] = 1605;
	anStart[1004] = 1610;
	anStart[1005] = 1615;
	anStart[1006] = 1620;
	anStart[1007] = 1625;
	anStart[1008] = 1535;
	anStart[1009] = 1540;
	anStart[1010] = 1545;
	anStart[1011] = 1550;
	anStart[1012] = 1555;
	anStart[1013] = 1505;
	anStart[1014] = 1510;
	anStart[1015] = 1515;
	anStart[1016] = 1520;
	anStart[1017] = 1525;
	anStart[1018] = 1435;
	anStart[1019] = 1440;
	anStart[1020] = 1445;
	anStart[1021] = 1450;
	anStart[1022] = 1455;
	anStart[1023] = 1405;
	

/*
	s = 2330;
	for(j=192; j<240; j++)
	{ start[j] = s; leng[j] = 150; if(0==(j%2)) s-=30; else s-=70; }

	s = 2330;
	for(j=240; j<288; j++)
	{ start[j] = s; leng[j] = 180; if(0==(j%2)) s-=30; else s-=70; }

	s = 2330;
	for(j=288; j<336; j++)
	{ start[j] = s; leng[j] = 210; if(0==(j%2)) s-=30; else s-=70; }

	s = 2330;
	for(j=336; j<384; j++)
	{ start[j] = s; leng[j] = 240; if(0==(j%2)) s-=30; else s-=70; }

	s = 2330;
	for(j=384; j<432; j++)
	{ start[j] = s; leng[j] = 270; if(0==(j%2)) s-=30; else s-=70; }

	s = 2330;
	for(j=432; j<480; j++)
	{ start[j] = s; leng[j] = 300; if(0==(j%2)) s-=30; else s-=70; }
*/

	for (nCount = 0; nCount < 37; nCount++)
		anLength[nCount] =  30;
	for (nCount = 37; nCount < 74; nCount++)
		anLength[nCount] =  60;
	for (nCount = 74; nCount < 86; nCount++)
		anLength[nCount] =  90;
	for (nCount = 86; nCount < 97; nCount++)
		anLength[nCount] =  120;
	anLength[97] = 150;
	for (nCount = 98; nCount < 123; nCount++)
		anLength[nCount] =  30;
	anLength[123] = 25;
	anLength[124] = 20;
	anLength[125] = 15;
	anLength[126] = 10;
	anLength[127] = 5;
	anLength[128] = 25;
	anLength[129] = 20;
	anLength[130] = 15;
	anLength[131] = 10;
	anLength[132] = 5;
	anLength[133] = 25;
	anLength[134] = 20;
	anLength[135] = 15;
	anLength[136] = 10;
	anLength[137] = 5;
	anLength[138] = 25;
	anLength[139] = 20;
	anLength[140] = 15;
	anLength[141] = 10;
	anLength[142] = 5;
	anLength[143] = 25;
	anLength[144] = 20;
	anLength[145] = 15;
	anLength[146] = 10;
	anLength[147] = 5;
	anLength[148] = 25;
	anLength[149] = 20;
	anLength[150] = 15;
	anLength[151] = 10;
	anLength[152] = 5;
	anLength[153] = 25;
	anLength[154] = 20;
	anLength[155] = 15;
	anLength[156] = 10;
	anLength[157] = 5;
	anLength[158] = 55;
	anLength[159] = 50;
	anLength[160] = 45;
	anLength[161] = 40;
	anLength[162] = 35;
	anLength[163] = 55;
	anLength[164] = 50;
	anLength[165] = 45;
	anLength[166] = 40;
	anLength[167] = 35;
	anLength[168] = 55;
	anLength[169] = 50;
	anLength[170] = 45;
	anLength[171] = 40;
	anLength[172] = 35;
	anLength[173] = 55;
	anLength[174] = 50;
	anLength[175] = 45;
	anLength[176] = 40;
	anLength[177] = 35;
	anLength[178] = 55;
	anLength[179] = 50;
	anLength[180] = 45;
	anLength[181] = 40;
	anLength[182] = 35;
	for (nCount = 183; nCount < 198; nCount++)
		anLength[nCount] =  25;
	anLength[198] = 45;
	anLength[199] = 45;
	anLength[200] = 45;
	anLength[201] = 45;
	anLength[202] = 45;
	anLength[203] = 115;
	anLength[204] = 110;
	anLength[205] = 105;
	anLength[206] = 100;
	anLength[207] = 95;
	anLength[208] = 25;
	anLength[209] = 20;
	anLength[210] = 15;
	anLength[211] = 10;
	anLength[212] = 5;
	anLength[213] = 25;
	anLength[214] = 20;
	anLength[215] = 15;
	anLength[216] = 10;
	anLength[217] = 5;
	anLength[218] = 25;
	anLength[219] = 20;
	anLength[220] = 15;
	anLength[221] = 10;
	anLength[222] = 5;
	anLength[223] = 25;
	anLength[224] = 20;
	anLength[225] = 15;
	anLength[226] = 10;
	anLength[227] = 5;
	anLength[228] = 25;
	anLength[229] = 20;
	anLength[230] = 15;
	anLength[231] = 10;
	anLength[232] = 5;
	anLength[233] = 25;
	anLength[234] = 20;
	anLength[235] = 15;
	anLength[236] = 10;
	anLength[237] = 5;
	anLength[238] = 25;
	anLength[239] = 20;
	anLength[240] = 15;
	anLength[241] = 10;
	anLength[242] = 5;
	anLength[243] = 25;
	anLength[244] = 20;
	anLength[245] = 15;
	anLength[246] = 10;
	anLength[247] = 5;
	anLength[248] = 25;
	anLength[249] = 20;
	anLength[250] = 15;
	anLength[251] = 10;
	anLength[252] = 5;
	anLength[253] = 25;
	anLength[254] = 20;
	anLength[255] = 15;
	anLength[256] = 10;
	anLength[257] = 5;
	anLength[258] = 25;
	anLength[259] = 20;
	anLength[260] = 15;
	anLength[261] = 10;
	anLength[262] = 5;
	anLength[263] = 25;
	anLength[264] = 20;
	anLength[265] = 15;
	anLength[266] = 10;
	anLength[267] = 5;
	anLength[268] = 25;
	anLength[269] = 20;
	anLength[270] = 15;
	anLength[271] = 10;
	anLength[272] = 5;
	anLength[273] = 25;
	anLength[274] = 20;
	anLength[275] = 15;
	anLength[276] = 10;
	anLength[277] = 5;
	anLength[278] = 25;
	anLength[279] = 20;
	anLength[280] = 15;
	anLength[281] = 10;
	anLength[282] = 5;
	anLength[283] = 25;
	anLength[284] = 20;
	anLength[285] = 15;
	anLength[286] = 10;
	anLength[287] = 5;
	anLength[288] = 25;
	anLength[289] = 20;
	anLength[290] = 15;
	anLength[291] = 10;
	anLength[292] = 5;
	anLength[293] = 25;
	anLength[294] = 20;
	anLength[295] = 15;
	anLength[296] = 10;
	anLength[297] = 5;
	anLength[298] = 25;
	anLength[299] = 20;
	anLength[300] = 15;
	anLength[301] = 10;
	anLength[302] = 5;
	anLength[303] = 25;
	anLength[304] = 20;
	anLength[305] = 15;
	anLength[306] = 10;
	anLength[307] = 5;
	anLength[308] = 25;
	anLength[309] = 20;
	anLength[310] = 15;
	anLength[311] = 10;
	anLength[312] = 5;
	anLength[313] = 25;
	anLength[314] = 20;
	anLength[315] = 15;
	anLength[316] = 10;
	anLength[317] = 5;
	anLength[318] = 25;
	anLength[319] = 20;
	anLength[320] = 15;
	anLength[321] = 10;
	anLength[322] = 5;
	anLength[323] = 25;
	anLength[324] = 20;
	anLength[325] = 15;
	anLength[326] = 10;
	anLength[327] = 5;
	anLength[328] = 55;
	anLength[329] = 50;
	anLength[330] = 45;
	anLength[331] = 40;
	anLength[332] = 35;
	anLength[333] = 55;
	anLength[334] = 50;
	anLength[335] = 45;
	anLength[336] = 40;
	anLength[337] = 35;
	anLength[338] = 55;
	anLength[339] = 50;
	anLength[340] = 45;
	anLength[341] = 40;
	anLength[342] = 35;
	anLength[343] = 55;
	anLength[344] = 50;
	anLength[345] = 45;
	anLength[346] = 40;
	anLength[347] = 35;
	anLength[348] = 55;
	anLength[349] = 50;
	anLength[350] = 45;
	anLength[351] = 40;
	anLength[352] = 35;
	anLength[353] = 55;
	anLength[354] = 50;
	anLength[355] = 45;
	anLength[356] = 40;
	anLength[357] = 35;
	anLength[358] = 55;
	anLength[359] = 50;
	anLength[360] = 45;
	anLength[361] = 40;
	anLength[362] = 35;
	anLength[363] = 55;
	anLength[364] = 50;
	anLength[365] = 45;
	anLength[366] = 40;
	anLength[367] = 35;
	anLength[368] = 55;
	anLength[369] = 50;
	anLength[370] = 45;
	anLength[371] = 40;
	anLength[372] = 35;
	anLength[373] = 55;
	anLength[374] = 50;
	anLength[375] = 45;
	anLength[376] = 40;
	anLength[377] = 35;
	anLength[378] = 55;
	anLength[379] = 50;
	anLength[380] = 45;
	anLength[381] = 40;
	anLength[382] = 35;
	anLength[383] = 55;
	anLength[384] = 50;
	anLength[385] = 45;
	anLength[386] = 40;
	anLength[387] = 35;
	anLength[388] = 55;
	anLength[389] = 50;
	anLength[390] = 45;
	anLength[391] = 40;
	anLength[392] = 35;
	for (nCount = 393; nCount < 412; nCount++)
		anLength[nCount] =  90;
	for (nCount = 412; nCount < 428; nCount++)
		anLength[nCount] =  120;
	for (nCount = 428; nCount < 438; nCount++)
		anLength[nCount] = 60;
	for (nCount = 438; nCount < 453; nCount++)
		anLength[nCount] = 90;
	anLength[453] = 120;
	anLength[454] = 120;
	anLength[455] = 120;
	anLength[456] = 120;
	anLength[457] = 120;
	for (nCount = 458; nCount < 523; nCount++)
		anLength[nCount] = 30;
	anLength[523] = 25;
	anLength[524] = 20;
	anLength[525] = 15;
	anLength[526] = 10;
	anLength[527] = 5;
	anLength[528] = 25;
	anLength[529] = 20;
	anLength[530] = 15;
	anLength[531] = 10;
	anLength[532] = 5;
	anLength[533] = 25;
	anLength[534] = 20;
	anLength[535] = 15;
	anLength[536] = 10;
	anLength[537] = 5;
	anLength[538] = 25;
	anLength[539] = 20;
	anLength[540] = 15;
	anLength[541] = 10;
	anLength[542] = 5;
	anLength[543] = 55;
	anLength[544] = 50;
	anLength[545] = 45;
	anLength[546] = 40;
	anLength[547] = 35;
	anLength[548] = 85;
	anLength[549] = 80;
	anLength[550] = 75;
	anLength[551] = 70;
	anLength[552] = 65;
	anLength[553] = 85;
	anLength[554] = 80;
	anLength[555] = 75;
	anLength[556] = 70;
	anLength[557] = 65;
	anLength[558] = 115;
	anLength[559] = 110;
	anLength[560] = 105;
	anLength[561] = 100;
	anLength[562] = 95;
	anLength[563] = 115;
	anLength[564] = 110;
	anLength[565] = 105;
	anLength[566] = 100;
	anLength[567] = 95;
	anLength[568] = 115;
	anLength[569] = 110;
	anLength[570] = 105;
	anLength[571] = 100;
	anLength[572] = 95;
	anLength[573] = 115;
	anLength[574] = 110;
	anLength[575] = 105;
	anLength[576] = 100;
	anLength[577] = 95;
	anLength[578] = 145;
	anLength[579] = 140;
	anLength[580] = 135;
	anLength[581] = 130;
	anLength[582] = 125;
	for (nCount = 583; nCount < 623; nCount++)
		anLength[nCount] = 25;
	for (nCount = 623; nCount < 638; nCount++)
		anLength[nCount] = 55;
	for (nCount = 638; nCount < 653; nCount++)
		anLength[nCount] = 25;
	for (nCount = 653; nCount < 678; nCount++)
		anLength[nCount] = 55;
	anLength[678] = 85;
	anLength[679] = 80;
	anLength[680] = 75;
	anLength[681] = 70;
	anLength[682] = 65;
	anLength[683] = 85;
	anLength[684] = 80;
	anLength[685] = 75;
	anLength[686] = 70;
	anLength[687] = 65;
	anLength[688] = 115;
	anLength[689] = 110;
	anLength[690] = 105;
	anLength[691] = 100;
	anLength[692] = 95;
	for (nCount = 693; nCount < 728; nCount++)
		anLength[nCount] = 20;
	for (nCount = 728; nCount < 738; nCount++)
		anLength[nCount] = 50;
	anLength[738] = 115;
	anLength[739] = 110;
	anLength[740] = 105;
	anLength[741] = 100;
	anLength[742] = 95;
	for (nCount = 743; nCount < 753; nCount++)
		anLength[nCount] = 15;
	anLength[753] = 45;
	anLength[754] = 45;
	anLength[755] = 45;
	anLength[756] = 45;
	anLength[757] = 45;
	anLength[758] = 85;
	anLength[759] = 80;
	anLength[760] = 75;
	anLength[761] = 70;
	anLength[762] = 65;
	anLength[763] = 85;
	anLength[764] = 80;
	anLength[765] = 75;
	anLength[766] = 70;
	anLength[767] = 65;
	anLength[768] = 85;
	anLength[769] = 80;
	anLength[770] = 75;
	anLength[771] = 70;
	anLength[772] = 65;
	anLength[773] = 115;
	anLength[774] = 110;
	anLength[775] = 105;
	anLength[776] = 100;
	anLength[777] = 95;
	anLength[778] = 115;
	anLength[779] = 110;
	anLength[780] = 105;
	anLength[781] = 100;
	anLength[782] = 95;
	anLength[783] = 145;
	anLength[784] = 140;
	anLength[785] = 135;
	anLength[786] = 130;
	anLength[787] = 125;
	anLength[788] = 145;
	anLength[789] = 140;
	anLength[790] = 135;
	anLength[791] = 130;
	anLength[792] = 125;
	for (nCount = 793; nCount < 823; nCount++)
		anLength[nCount] = 15;
	for (nCount = 823; nCount < 853; nCount++)
		anLength[nCount] = 45;
	for (nCount = 853; nCount < 863; nCount++)
		anLength[nCount] = 10;
	anLength[863] = 40;
	anLength[864] = 40;
	anLength[865] = 40;
	anLength[866] = 40;
	anLength[867] = 40;
	anLength[868] = 85;
	anLength[869] = 80;
	anLength[870] = 75;
	anLength[871] = 70;
	anLength[872] = 65;
	anLength[873] = 85;
	anLength[874] = 80;
	anLength[875] = 75;
	anLength[876] = 70;
	anLength[877] = 65;
	anLength[878] = 115;
	anLength[879] = 110;
	anLength[880] = 105;
	anLength[881] = 100;
	anLength[882] = 95;
	anLength[883] = 115;
	anLength[884] = 110;
	anLength[885] = 105;
	anLength[886] = 100;
	anLength[887] = 95;
	anLength[888] = 115;
	anLength[889] = 110;
	anLength[890] = 105;
	anLength[891] = 100;
	anLength[892] = 95;
	for (nCount = 893; nCount < 958; nCount++)
		anLength[nCount] = 40;
	for (nCount = 958; nCount < 973; nCount++)
		anLength[nCount] = 35;
	anLength[973] = 115;
	anLength[974] = 110;
	anLength[975] = 105;
	anLength[976] = 100;
	anLength[977] = 95;
	anLength[978] = 115;
	anLength[979] = 110;
	anLength[980] = 105;
	anLength[981] = 100;
	anLength[982] = 95;
	for (nCount = 983; nCount < 1024; nCount++)
		anLength[nCount] = 35;


	} /* end of initializer block */

	if((i >= 1024) || (i < 0))
	{
		char err[50];
		sprintf(err, "PANIC, Illegal table index %d\r\n", i );
		ShowMessage(err, 300);
	}

	*outtime = (anStart[i]) ;
	*outdur = (anLength[i]) ;

}


//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------

/* function that performs initial scrambling */
int	mixup(x, y)
	int	x, y;
{
	int	i, j, k, sum;

	int	a[12], b[12], out[12] ;

	/* get the digits of x into a[] */
	j = x ;
	for(i=0; i<9; i++)
	{
		k = j % 10;
		a[i] = k;
		j = (j - k) / 10 ;
	}

	/* get the digits of y into b[] */
	j = y ;
	for(i=0; i<9; i++)
	{
		k = j % 10;
		b[i] = k;
		j = (j - k) / 10 ;
		out[i] = 0;
	}


	for(i=0; i<=8; i++)
	{
		for(j=0; j<=8; j++)
		{
			out[i+j] += (b[j] * a[i]) ;
		}
	}

	j = 1;
	sum = 0;
	for(i=0; i<=8; i++)
	{
		sum += j * (out[i] % 10);
		j = j * 10 ;
	}
	return( sum ) ;
}

/*
long length_loop(int value, int i, int Mi)
{
	//(value = input value, i = number of digits, Mi = mapping on i digits.)
	do
		value = Mi(value);
	while (value < 10ˆ(i-1));
	return value;
}

int key_mult(int value)
{
	int digits = noOfDigitsIn(value);
	do
		value = (value (´) 68150631) mod 10ˆdigits;
	while (value < 10ˆ(digits-1));
	return value;
}
*/

//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------

int map_top(int day, int year, int top, int digits)
{
	int	d2, d1, d0, y;
	int	n2, n1, n0, f3, f2, f1, f0;

	y = year % 16;
	d0 = top % 10;
	d1 = (top % 100) / 10;
	d2 = top / 100;
	//d2,d1,d0 = digits of top;

	f0 = 1;
	f1 = (year+1) % 10;
	f2 = ((year+1)*(year+2)/2) % 10;
	f3 = ((year+1)*(year+2)*(year+3)/6) % 10;

	if (digits == 1)
	{
		n0 = (d0*f0 + day*f1) % 10;
		n1 = 0;
		n2 = 0;
	} else 
		if (digits == 2)
		{
			n0 = (d0*f0 + d1*f1 + day*f2) % 10;
			n1 = (d1*f0 + day*f1) % 10;
			n2 = 0;
		} else 
			if (digits == 3)
			{
				n0 = (d0*f0 + d1*f1 + d2*f2 + day*f3) % 10;
				n1 = (d1*f0 + d2*f1 + day*f2) % 10;
				n2 = (d2*f0 + day*f1) % 10;
			}
	return (int)(n2*100 + n1*10 + n0);
}

//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------

int offset(int day, int year, int top, int *offout, int *topout)
{
	int digits = noOfDigitsIn(top);
	int off = sumOfDigitsIn(top);
	int i, t = top;
	//off = sum of the digits in top
	do
	{
		for (i=0; i<=(year % 16); i++)
			off = off + (map_top(day, i, t, digits) % 10);
		t = map_top(day, year, t, digits);
	} while (t < (int)(pow(10, digits-1)));
	*offout = (off % 32);
	*topout = (t);
	//return top, off
	return 0;
}

//--------------------------------------------------------------------------------
// This is the function that is called by external routines to decode a g-code
//--------------------------------------------------------------------------------

int decode(int gcode, int *day_ret, int *month_ret, int *year_ret, int *channel_ret, int *starttime_ret, int *duration_ret)
{
	int off, mtop, func1_out, func5_out, day_out, rem;
	int	channel_out, starttime_out, duration_out;
	int cval, tval;
	word mjd, wyear;
	byte hour, min, sec, bmonth, bday, bwkday;

	//int month_today=9, year_today=4;
	int month_today, year_today;

	//Get the current month and year
	TAP_GetTime(&mjd, &hour, &min, &sec);
	TAP_ExtractMjd(mjd, &wyear, &bmonth, &bday, &bwkday);
	month_today = (int)bmonth;
	year_today = (int)(wyear%100);
	//TAP_Print("month: %d, year: %d\r\n", month_today, year_today);

	//do the "Function 1" g-code process
	func1_out = func1(gcode);
	//TAP_Print("function 1: %d\r\n", func1_out);

	//do the "Function 2" g-code process
	day_out = func2(func1_out, &rem);
	//TAP_Print("Day: %d\r\n", day_out);

	//if we only have 3 digits don't need to worry about "top 3" processing in "Function 4"
	if (gcode >= 1000)
	{
		func4( func1_out, day_out, year_today, &off, &mtop );
	} else {
		off = 0; mtop = 0;
	}

	//do the "Function 5" g-code process
	func5_out = (rem + (day_out*(month_today+1)) + off) % 32 ;
	//TAP_Print("top: %d, off: %d, func5_out: %d, rem: %d\r\n", mtop, off, func5_out, rem);

	//re-arrange the bits from each of the outputs
	bit_shuffle(mtop, func5_out, &tval, &cval);
	channel_out = cval + 1;
	//TAP_Print("channel: %d, tval: %d \r\n", channel_out, tval);

	//using the timer value do a lookup in our table for the start time/duration
	lookup(tval, (&starttime_out), (&duration_out));

	//TAP_Print("Code= %6d    %2d  %2d  %2d   Mapped_channel= %2d  Start_time= %4d  Length= %3d\r\n",
	//	gcode, day_out, month_today, year_today, channel_out, starttime_out, duration_out);

	//return values back to caller
	*day_ret = day_out;
	*month_ret = month_today;
	*year_ret = (int)wyear;
	*channel_ret = channel_out;
	*starttime_ret = starttime_out;
	*duration_ret = duration_out;

	return 1;  //non zero is ok
}

