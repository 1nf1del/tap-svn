#include "tap.h"

volatile dword *EMMA_USB_RegAddrPtr                               = (dword *) 0xbf400000;
volatile dword *EMMA_USB_RegData                                  = (dword *) 0xbf400002;
volatile dword *EMMA_USB_IRQStat0                                 = (dword *) 0xbf400004;
volatile dword *EMMA_USB_IRQStat1                                 = (dword *) 0xbf400006;
volatile dword *EMMA_USB_PageSel                                  = (dword *) 0xbf400008;
volatile dword *EMMA_USB_EP_Data                                  = (dword *) 0xbf40000a;
volatile dword *EMMA_USB_EP_Stat0                                 = (dword *) 0xbf40000c;
volatile dword *EMMA_USB_EP_Stat1                                 = (dword *) 0xbf40000e;
volatile dword *EMMA_USB_EP_Transfer0                             = (dword *) 0xbf400010;
volatile dword *EMMA_USB_EP_Transfer1                             = (dword *) 0xbf400012;
volatile dword *EMMA_USB_EP_Transfer2                             = (dword *) 0xbf400014;
volatile dword *EMMA_USB_EP_IRQEnb                                = (dword *) 0xbf400016;
volatile dword *EMMA_USB_EP_Avail0                                = (dword *) 0xbf400018;
volatile dword *EMMA_USB_EP_Avail1                                = (dword *) 0xbf40001a;
volatile dword *EMMA_USB_EP_RspClr                                = (dword *) 0xbf40001c;
volatile dword *EMMA_USB_EP_RspSet                                = (dword *) 0xbf40001e;
volatile dword *EMMA_USB_Reserved1                                = (dword *) 0xbf400020;
volatile dword *EMMA_USB_Reserved2                                = (dword *) 0xbf400022;
volatile dword *EMMA_USB_Reserved3                                = (dword *) 0xbf400024;
volatile dword *EMMA_USB_Reserved4                                = (dword *) 0xbf400026;
volatile dword *EMMA_USB_Reserved5                                = (dword *) 0xbf400028;
volatile dword *EMMA_USB_Reserved6                                = (dword *) 0xbf40002a;
volatile dword *EMMA_USB_Reserved7                                = (dword *) 0xbf40002c;
volatile dword *EMMA_USB_Reserved8                                = (dword *) 0xbf40002e;
volatile dword *EMMA_USB_USBCtl0                                  = (dword *) 0xbf400030;
volatile dword *EMMA_USB_USBCtl1                                  = (dword *) 0xbf400032;
volatile dword *EMMA_USB_Frame0                                   = (dword *) 0xbf400034;
volatile dword *EMMA_USB_Frame1                                   = (dword *) 0xbf400036;
volatile dword *EMMA_USB_DMAReq                                   = (dword *) 0xbf400038;
volatile dword *EMMA_USB_Scratch                                  = (dword *) 0xbf40003a;
volatile dword *EMMA_USB_Reserved9                                = (dword *) 0xbf40003c;
volatile dword *EMMA_USB_Reserved10                               = (dword *) 0xbf40003e;
