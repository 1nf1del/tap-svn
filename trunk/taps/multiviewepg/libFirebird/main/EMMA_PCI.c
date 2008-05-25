#include "tap.h"

//PCI = PCI INTERFACE
volatile dword *EMMA_PCI_RST     = (dword *) 0xb0013000;
volatile dword *EMMA_PCI_ARBEN   = (dword *) 0xb0013030;
volatile dword *EMMA_PCI_TENDIAN = (dword *) 0xb0013070;
volatile dword *EMMA_PCI_DMACTRL = (dword *) 0xb0013080;
volatile dword *EMMA_PCI_ERR     = (dword *) 0xb02000b8;
volatile dword *EMMA_PCI_CTRL_L  = (dword *) 0xb02000e0;
volatile dword *EMMA_PCI_CTRL_H  = (dword *) 0xb02000e4;
volatile dword *EMMA_PCI_INIT0   = (dword *) 0xb02000f0;
volatile dword *EMMA_PCI_INIT1   = (dword *) 0xb02000f8;
volatile dword *EMMA_PCI_CMD_STS = (dword *) 0xb0200204;
volatile dword *EMMA_PCI_BARC    = (dword *) 0xb0200210;
volatile dword *EMMA_PCI_BAR0    = (dword *) 0xb0200240;
