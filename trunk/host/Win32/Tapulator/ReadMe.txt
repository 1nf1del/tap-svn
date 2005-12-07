Tapulator v0.10
Robin Glover
robin_tap at rattus dot homelinux dot com

INTRODUCTION

Tapulator is a Topfield TF5800 emulator for Windows to facilitate TAP development. Emulation is achieved by compiling the TAP code using an x86 compiler rather than trying to emulate the MIPS instruction set native to the Topfield. Tapulator emulates the majority of the Topfield TAP API (v1.22), but only a small fraction of the functionality provided by the Topfield firmware. The main advantages of using Tapulator are

*) Quick round trip time to see if something works
*) Ability to debug TAP code using an interactive debugger
*) Memory usage and other diagnostics immediately available to the programmer

Note that Tapulator does implement both the drawing of GD (Topfield Pack) graphics and the memory mapped access to the OSD display.

BUILDING TAPULATOR

The source code for Tapulator was written using Microsoft Visual Studio .NET 2003. If you have VS.NET 2003 or 2005 you should be able to open the solution file and just build. For VS.NET 2002 users I believe there is a tool on the web which allows you to convert from VS.NET 2003 solution and project files to VS.NET 2002 versions. 

Users of other compilers who have the ability to compile MFC based programs will probably be able to hack together some way of getting the code built. In practice this shouldn't be necessary - get a binary version of Tapulator.exe and libtapulator.lib, then compile your TAPs using your preferred compiler - see below.

BUILDING TAPS

Tapulator expects to find a TAP as a Win32 DLL which exports the following 3 functions

TAP_EventHandler	// dword TAP_EventHandler(word, dword, dword)
TAP_Main			// int TAP_Main(void)
SetFramework		// void SetFramework(IFramework*)

The first 2 must be implemented in code in your TAP, just as normal, the third is implemented in libtapulator.lib, which you must link into your dll. In MS VS.NET land the exports are best defined by setting Tap.def as the export definition file for your dll project. Note that these functions are loaded by name by Tapulator.exe and the names are *NOT* C++ mangled.

In creating a new TAP project you will need to change the following options 
* turn off pre-compiled headers
* add the folder containing your Topfield includes as an additional include directory
* change the runtime library option to 'Multithreaded Debug DLL' or 'Multithreaded DLL'

Create a tap.h *in* your project directory with this contents

---------------------- cut here ---------------
#pragma once
#ifndef __cplusplus
typedef int bool;
#endif

#include "path\to\real\tap.h"

#define __attribute__(x)
#define section(x)
---------------------- cut here ---------------

Note that if you include standard headers like <stdlib> you should do so *before* the real tap.h, otherwise you will get errors like "'TAP_MemAlloc' : redefinition; previous definition was a 'data variable'"

Note also that you may need to change the code of your TAP slightly to get compliance with your win32 compiler - for instance some of the demo TAPs do pointer arithmetic on void pointers, which is illegal in MSVC even when compiling C code and apparently illegal in later versions of gcc.

RUNNING TAPS

Just run Tapulator.exe with either the full path of your TAP DLL on the command line (in quotes if it has spaces in it) or with no parameters and choose file->open. Of course, most likely you'll want to start your TAP under the debugger. The best course for this in MS VS.NET is to set (for your TAP DLL project) the Debugging Command to the filepath of Tapulator.exe and the command arguments to "$(TargetPath)" (including the quotes). Then make this project the start up project and pressing 'F5' will start Tapulator under the debugger and auto-load your TAP.

OPTIONS

Check out the File->Options menu item to set the location of the virtual Topfield hard disk and the JAG EPG data file (note 100 channels * 256 events format only). You can also filter logging messages here.

TO-DO:

* More support for simulating firmware actions (menus, channel change via number keys, info box, etc)
* Ability to have more than one TAP loaded
* Description Extender emulation
* TAP Commander emulation
* Remote buttons have no auto-repeat and are slow to respond to a second press
* Does not exit cleanly if closed with a Non-TSR Tap loaded
* May crash (stack overflow) if a TAP calls TAP_SystemProc from its TAP_EventHandler
* Can consume all CPU resources if a TAP redraws in response to WM_IDLE
* Return values of some implemented functions may be wrong (who checks em anyway?)
* Some functions implemented but untested :-(
* Sizing of the main window is rather hacked.
* OSD Regions only support 1555 colour depth
* TAP_OSD_Ctrl 
* TAP_OSD_SetTransparency 
* TAP_OSD_DrawPixmap only implemented for OSD_256 and OSD_1555
* Transparency probably wrongly implemented in some cases (trying to draw an area transparent after having had OSD on it?)
* All RS232 functionality 
* All USB functionality 
* TAP_Set_Bk undocumented and unimplemented
* TAP_SetInfoBoxTime 
* PCM and MP3 functions 
* TAP_CaptureScreen 
* TAP_Hdd_GetID 
* TAP_Hdd_ChangePlaybackPos 
* Tap_Hdd_[Set|Goto]Bookmark
* Tap_OSD_DrawString
* Radio channel support
* Customization of which TV channels appear
* TAP_PutEvent, Control_EIT, UpdateEvent, DeleteEvent, EPG_GetExtInfo
* TAP_GetEvent wrongly trims out programs that have finished from the returned data
* VFD Functions
* Window drawing is rough & reading
* Window_SetActive and Window_SetDrawItemFunc not implemented

LICENCE

