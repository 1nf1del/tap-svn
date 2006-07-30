/*
	Copyright (C) 2005-2006 Simon Capewell

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

#include <tap.h>
#include "model.h"

#ifdef WIN32
int _appl_version = 0x1225;
word id = 456;
word* sysID = &id;
#else
word* sysID = (word*)0x83fffffa;
#endif 

// Return the Topfield model number
Model GetModel()
{
	switch ( *sysID )
	{
	case 406:	return TF5000_5500;
	case 416:	return TF5000t;
	case 436:	return TF5010_5510;
	case 446:	return TF5000_BP_WP;	// Black Panther / White Polar
	case 456:	return TF5800t;
	case 466:	return TF5000t_BP_WP;	// Black Panther / White Polar 
	case 486:	return TF5010_SE;		// Black Panther / White Polar
	case 501:	return TF5000CI_ME;
	case 1416:	return TF5000_MP;		// Masterpiece
	case 1426:	return TF5000t_MP;		// Masterpiece (AUS)
	case 1456:	return TF5010_MP;		// Masterpiece
	case 1466:	return TF5100t_MP;		// Masterpiece
	case 1486:	return TF5100c_MP;		// Masterpiece
	case 1501:	return TF5000CI_EUR;
	case 10416:	return PC5101c_5102c;	// Procaster
	case 10426:	return PC5101t_5102t;	// Procaster
	case 10446:	return TF5200c;
	case 12406:	return TF5100c;
	case 13406:	return TF5100;
	}
	return TFOther;
}
