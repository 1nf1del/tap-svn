/*
	Copyright (C) 2005 Simon Capewell

	This file is part of the TAPs for Topfield PVRs project.
		http://tap.berlios.de/

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef __OPCODES_H
#define __OPCODES_H


#define NOP_CMD			0x00000000

#define JAL_CMD			0x0c000000
#define J_CMD			0x08000000
#define JR_CMD			0x03e00008
#define JAL_MASK		0xfc000000

#define LUI_T0_CMD		0x3c080000
#define OR_T0_CMD		0x35080000
#define LUI_T1_CMD		0x3c090000
#define OR_T1_CMD		0x35290000
#define LUI_FP_CMD		0x3c1e0000
#define ADDIU_FP_CMD	0x27de0000
#define LUI_GP_CMD		0x3c1c0000
#define OR_GP_CMD		0x379c0000
#define LUI_S6_CMD		0x3c160000
#define ADDIU_S6_CMD	0x26d60000

#define JAL(addr) (JAL_CMD | (((dword)(addr) & 0x03ffffff) >> 2))
#define J(addr) (J_CMD | (((dword)(addr) & 0x03ffffff) >> 2))


#endif
