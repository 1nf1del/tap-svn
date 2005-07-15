# $Id$
#
# Copyright (c) 2004-2005 Peter Urbanec. All Rights Reserved.
#
#  This file is part of the TAPs for Topfield PVRs project.
#      http://developer.berlios.de/projects/tap/
#
#  This is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  The software is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with puppy; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
.PHONY: all clean

TAPS+=usbmon
TAPS+=timer_extend
TAPS+=freespacebar

# $(call make-dirs, dirs, target, opts)
define make-dirs
$(foreach d, $1, $(MAKE) -C $d $3 $2 &&) echo Done
endef

all:
	$(call make-dirs, $(TAPS), all)

clean:
	$(call make-dirs, $(TAPS), clean)


