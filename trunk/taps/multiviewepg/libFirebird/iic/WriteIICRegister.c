#include "../libFireBird.h"

bool WriteIICRegister (dword Controller, dword ChipAddress, dword ChipRegister, dword NrOfRegisters, char *DestinationAddress)
{
  int                   Delay;
  dword                 i, RS;

  volatile dword        *IIC_00         = EMMA_IIC1_00;
  volatile dword        *IIC_10         = EMMA_IIC1_10;
  volatile dword        *IIC_20         = EMMA_IIC1_20;
  volatile dword        *IIC_Data       = EMMA_IIC1_Data;
  volatile dword        *IIC_Cmd_Status = EMMA_IIC1_Cmd_Status;
  volatile dword        *IIC_60         = EMMA_IIC1_60;

  switch (Controller)
  {
    case 0:
      //Already assigned as default
      break;

    case 1:
      IIC_00         = EMMA_IIC2_00;
      IIC_10         = EMMA_IIC2_10;
      IIC_20         = EMMA_IIC2_20;
      IIC_Data       = EMMA_IIC2_Data;
      IIC_Cmd_Status = EMMA_IIC2_Cmd_Status;
      IIC_60         = EMMA_IIC2_60;
      break;
  }

  RS = intLock ();

  //Reset the IIC bus and wait until it's ready
  *IIC_00 = 0x9a;
  Delay = 0x16e360;
  while (Delay > 0)
  {
    if ((*IIC_10 & 0x80) != 0) break;
    Delay--;
  }
  if (Delay == 0)
  {
    intUnlock (RS);
    return FALSE;
  }

  //Select the chip in write mode
  *IIC_Data = (ChipAddress & 0xFE);
  *IIC_Cmd_Status = 0;
  Delay = 0x493e0;
  while (Delay > 0)
  {
    if ((*IIC_Cmd_Status & 0x01) != 0) break;
    Delay--;
  }
  if (Delay == 0)
  {
    intUnlock (RS);
    return FALSE;
  }

  //Select the register
  *IIC_Data = ChipRegister;
  *IIC_Cmd_Status = 0;
  Delay = 0x493e0;
  while (Delay > 0)
  {
    if ((*IIC_Cmd_Status & 0x01) != 0) break;
    Delay--;
  }
  if (Delay == 0)
  {
    intUnlock (RS);
    return FALSE;
  }

  //Write loop if more than one register should be read
  for (i=0; i<(NrOfRegisters-1); i++)
  {
    //Write the data byte
    *IIC_Cmd_Status = 0;
    *IIC_Data = *DestinationAddress;
    DestinationAddress++;

    Delay=0x493e0;
    while (Delay > 0)
    {
      if ((*IIC_Cmd_Status & 0x01) != 0) break;
      Delay--;
    }
    if (Delay == 0)
    {
      intUnlock (RS);
      return FALSE;
    }
  }

  //Write the last data byte
  *IIC_Cmd_Status = 0;
  *IIC_Data = *DestinationAddress;
  Delay=0x493e0;
  while (Delay > 0)
  {
    if ((*IIC_Cmd_Status & 0x01) != 0) break;
    Delay--;
  }
  if (Delay == 0)
  {
    intUnlock (RS);
    return FALSE;
  }

  //Terminate the transfer
  *IIC_00 = 0x99;
  Delay=0x16e360;
  while (Delay > 0)
  {
    if ((*IIC_10 & 0x80) == 0) break;
    Delay--;
  }

  intUnlock (RS);
  if (Delay == 0) return FALSE;
  return TRUE;
}
