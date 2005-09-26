
/******************************************************
TAP (Topfield¡¯s customizing API) Ld script
*******************************************************/

/* Steve Bennett <steveb@workware.net.au>
 * This is a modified version of the Topfield tap.ld
 * with the SEARCH paths removed and rel.dyn sections discarded
 */

/* DO NOT EDIT */

OUTPUT_FORMAT(elf32-bigmips)
ENTRY( TAP_Main )

SECTIONS
{
	._tap_init_data .	: 
	{
		*(._tap_header)
		. = 0x00000020;
		*(._tap_id)
		*(._tap_ext_header)	
		. = 0x00000040;
		*(._tap_program_name)
		. = 0x00000080;
		*(._tap_author_name)
		. = 0x000000C0;
		*(._tap_description)
		. = 0x00000140;
		*(._tap_etcinfo)
		. = 0x00000200;
		*(._tap_init_data)
	}

	.text 	. : 
	{ 
	
		*(.text) 
	}
	.text_end 	.	: {}

	.rodata  .  :
	{
		*(.rodata)
		*(.rodata.*)
		*(.gnu.linkonce.r*)
	}

	.data  .	:
	{
		_fdata = . ;
		*(.data)
		*(.data.*)
		*(.gnu.linkonce.d*)
	}

	.ctors   : 
	{
		KEEP (*(SORT(.ctors.*)))
		KEEP (*(.ctors))
	}

	.dtors         :
	{
		KEEP (*(SORT(.dtors.*)))
		KEEP (*(.dtors))
	}

	_gp = .;
   	_GLOBAL_OFFSET_TABLE_ = .;
	.got	.	:
  	{
   		*(.ref_start)
   		*(*.got*)
	   	*(*.xdata*)
	   	*(.ctors)
	   	*(.dtors)
	   	*(.fixup_start)
	   	*(.fixup)
	   	*(.dynamic)
	   	*(.fixup_end)
	   	*(.ref_end)
	}

  	.rosdata . :
  	{
		*(.rosdata)
  	}

	.sdata     : 
  	{
    	*(.sdata) 
    	*(.sdata.*)
    	*(.gnu.linkonce.s.*)
  	}

  	_edata = .;
  	PROVIDE (edata = .);
  	__bss_start = .;
  	_fbss = .;
  
  	.sbss      :
  	{
    	*(.dynsbss)
    	*(.sbss)
    	*(.sbss.*)
    	*(.scommon)
  	}
  
	.bss       :
  	{
   		*(.dynbss)
   		*(.bss)
   		*(.bss.*)
   		*(COMMON)
   		. = ALIGN(32 / 8);
  	}
  
  	.stab 	 .	  : { *(.stab) }
  	.debug	 .	  : { *(.debug) }

  	_end = . ;
  	PROVIDE (end = .);

	/DISCARD/ : { *(.rel.dyn) }
 }
