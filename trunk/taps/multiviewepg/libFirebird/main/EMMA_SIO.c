#include "tap.h"

//SIO1 is not used by the Toppy (also called PFUR0)
volatile dword *EMMA_SIO1_Data_DivLSB  = (dword *) 0xb2001000;
volatile dword *EMMA_SIO1_IE_DivMSB    = (dword *) 0xb2001010;
volatile dword *EMMA_SIO1_IID_FIFO     = (dword *) 0xb2001020;
volatile dword *EMMA_SIO1_LineControl  = (dword *) 0xb2001030;
volatile dword *EMMA_SIO1_ModemControl = (dword *) 0xb2001040;
volatile dword *EMMA_SIO1_LineStatus   = (dword *) 0xb2001050;
volatile dword *EMMA_SIO1_ModemStatus  = (dword *) 0xb2001060;
volatile dword *EMMA_SIO1_Scratch      = (dword *) 0xb2001070;

//SIO2 is used for the serial interface (INT2 ISR17)  (also called PFUR1)
volatile dword *EMMA_SIO2_Data_DivLSB  = (dword *) 0xb2002000;
volatile dword *EMMA_SIO2_IE_DivMSB    = (dword *) 0xb2002010;
volatile dword *EMMA_SIO2_IID_FIFO     = (dword *) 0xb2002020;
volatile dword *EMMA_SIO2_LineControl  = (dword *) 0xb2002030;
volatile dword *EMMA_SIO2_ModemControl = (dword *) 0xb2002040;
volatile dword *EMMA_SIO2_LineStatus   = (dword *) 0xb2002050;
volatile dword *EMMA_SIO2_ModemStatus  = (dword *) 0xb2002060;
volatile dword *EMMA_SIO2_Scratch      = (dword *) 0xb2002070;

//SIO3 is used for the front panel communication (INT2 ISR18)  (also called PURT0)
volatile dword *EMMA_SIO3_ASIM            = (dword *) 0xb2003000;
volatile dword *EMMA_SIO3_Err             = (dword *) 0xb2003020;
volatile dword *EMMA_SIO3_BaudRateControl = (dword *) 0xb2003030;
volatile dword *EMMA_SIO3_BaudRateMode    = (dword *) 0xb2003040;
volatile dword *EMMA_SIO3_Data            = (dword *) 0xb2003050;
volatile dword *EMMA_SIO3_Int             = (dword *) 0xb2003060;
volatile dword *EMMA_SIO3_IntMask         = (dword *) 0xb2003070;

//SIO4 is not used by the Toppy (also called PURT1)
volatile dword *EMMA_SIO4_ASIM            = (dword *) 0xb2004000;
volatile dword *EMMA_SIO4_Err             = (dword *) 0xb2004020;
volatile dword *EMMA_SIO4_BaudRateControl = (dword *) 0xb2004030;
volatile dword *EMMA_SIO4_BaudRateMode    = (dword *) 0xb2004040;
volatile dword *EMMA_SIO4_Data            = (dword *) 0xb2004050;
volatile dword *EMMA_SIO4_Int             = (dword *) 0xb2004060;
volatile dword *EMMA_SIO4_IntMask         = (dword *) 0xb2004070;
