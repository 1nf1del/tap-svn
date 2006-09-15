;	Copyright (C) 2006 Mike Morrin
;
;	This file is part of the TAPs for Topfield PVRs project.
;		http://tap.berlios.de/
;
;	This library is free software; you can redistribute it and/or
;	modify it under the terms of the GNU Lesser General Public
;	License as published by the Free Software Foundation; either
;	version 2 of the License, or (at your option) any later version.
;
;	This library is distributed in the hope that it will be useful,
;	but WITHOUT ANY WARRANTY; without even the implied warranty of
;	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;	Lesser General Public License for more details.
;
;	You should have received a copy of the GNU Lesser General Public
;	License along with this library; if not, write to the Free Software
;	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA


; This code is derived from code found at:
; http://www.tutorialsall.com/MFC/Using-ship/
; and reproduced with permission of Alan Klietz

;If you want to publish a Windows application that runs on Windows 2000 or 
;Windows XP, using the latest compiler (Visual Studio 2005), you can do so. 
;Without requiring that you ship megabytes of code from the VS2005 runtime 
;libraries (MSVCRT80.DLL and MFC80.DLL).

;Follow these steps:

;1) Install the Windows Server 2003 SP1 Platform SDK (April 2005 Edition). 
;The April 2005 PSDK bundles the VS 2005 compiler (version 14.0) and its 
;associated include headers and libraries.  With VC6 installed it allows you 
;to link VC6 compatible apps for 32-bit applications.
;2) You won't be able to use the 64-bit compiler bundled with the April 2005 
;PSDK on your 32-bit apps.  But (and this is the key trick) you can use 
;Visual Studio 2005's compiler with the April 2005 PSDK.  Just point Visual 
;Studio 2005's INCLUDE folders at the PSSDK folders: include\mfc, 
;include\atl, and include\crt.   Point the LIB folder to your original VC6 
;installation.  Now you can compile with Visual Studio 2005 and link 
;correctly with MFC42.LIB and MSVCRT.LIB while enjoying all the new IDE 
;goodies and security checking of Visual Studio 2005.



;You still need the VS 2005 version of RunTmChk.lib and sehprolg.obj in order 
;to write the __security_check_cookie stub.  (See Matt Pietrek's postings for 
;more info on this topic.)



;To fix the remaining link errors with CxxFrameHandler3 and alloca use the following:



   .386
   .model flat
   .code
   ;
   ; vc8-to-vc6 bridge assembly code.
   ;
   ; This module patches up gaps in order to a build a bridge to connect our code
   ; compiled with the VC8 compiler and with the code in the VC6 runtime library.
   ;
   ; Trampoline from ___CxxFrameHandler3 --> ___CxxFrameHandler
   ;
   ; ___CxxFrameHandler[n] is called for try/catch blocks (C++)
   ;
   ; See "How a C++ compiler implements exception handling"
   ; http://www.codeproject.com/cpp/exceptionhandler.asp

   ;

   ; MSVCRT.DLL
   extrn __imp____CxxFrameHandler:dword  ; external memory addr

public  ___CxxFrameHandler3 
___CxxFrameHandler3 proc near

   ; Jump indirect: Jumps to (*__imp__CxxFrameHandler)
   jmp  __imp____CxxFrameHandler  ; Trampoline bounce 
___CxxFrameHandler3 endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

   extrn __chkstk:abs   ; Absolute external constant

   public __alloca_probe_16 
__alloca_probe_16 proc near

arg_0  = dword ptr  8
   push    ecx
   lea     ecx, [esp+arg_0]
   sub     ecx, eax
   and     ecx, 0Fh
   add     eax, ecx
   sbb     ecx, ecx
   or      eax, ecx
   pop     ecx
   jmp     near ptr __chkstk

__alloca_probe_16 endp

   public __alloca_probe_8
__alloca_probe_8 proc near
arg_0           = dword ptr  8
   push    ecx
   lea     ecx, [esp+arg_0]
   sub     ecx, eax
   and     ecx, 7
   add     eax, ecx
   sbb     ecx, ecx
   or      eax, ecx
   pop     ecx
   jmp     near ptr __chkstk

__alloca_probe_8 endp



;-- Alan Klietz (alank at NOSPAM algintech dot com)
end