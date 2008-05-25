#include "tap.h"

volatile dword *EMMA_ATA_CONTROL                 = (dword *) 0xb0012000;
volatile dword *EMMA_ATA_INT_EN_ITOP_STATUS      = (dword *) 0xb0012060;
volatile dword *EMMA_ATA_InterruptFlag1          = (dword *) 0xb0012064;
volatile dword *EMMA_ATA_INT_STATUS              = (dword *) 0xb0012064;
volatile dword *EMMA_ATA_MODE                    = (dword *) 0xb0012068;
volatile dword *EMMA_ATA_UNKNOWN                 = (dword *) 0xb0012074;
volatile dword *EMMA_ATA_ATACFG_PIO_CONTROL      = (dword *) 0xb0012078;
volatile dword *EMMA_ATA_SWAP                    = (dword *) 0xb001207c;
volatile dword *EMMA_ATA_SOFT_RESET              = (dword *) 0xb0012084;
volatile dword *EMMA_ATA_ATACFG_DMA_TIMING_MODE  = (dword *) 0xb0012090;
volatile dword *EMMA_ATA_ATACFG_UDMA_TIMING_MODE = (dword *) 0xb0012098;
volatile dword *EMMA_ATA_ATACFG_SPEC_CONTROL     = (dword *) 0xb001209c;
