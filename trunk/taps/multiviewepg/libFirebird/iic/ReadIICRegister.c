#include "../libFireBird.h"

bool ReadIICRegister (dword Controller, dword ChipAddress, dword ChipRegister, dword NrOfRegisters, char *DestinationAddress)
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

  //Stop the bus cycle as we would like to read
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

  //Select the chip in read mode
  *IIC_Data = (ChipAddress | 0x01);
  *IIC_Cmd_Status = 0;
  Delay=0x493e0;
  do
  {
    if ((*IIC_Cmd_Status & 0x01) != 0) break;
    Delay--;
  } while (Delay > 0);
  if (Delay == 0)
  {
    intUnlock (RS);
    return FALSE;
  }

  //Read loop if more than one register should be read
  for (i=0; i<(NrOfRegisters-1); i++)
  {
    //Clear the ACK bit and read the data byte
    *IIC_00 = 0xbc;
    *IIC_Cmd_Status = 0;
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
    *DestinationAddress = *IIC_Data;
    DestinationAddress++;
  }

  //Clear the ACK bit and read the last data byte
  *IIC_00 = 0xb8;
  *IIC_Cmd_Status = 0;
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

  *DestinationAddress = *IIC_Data;

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
