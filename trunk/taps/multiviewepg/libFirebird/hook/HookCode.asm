00: 0x27BDFF00	addiu		$sp, $sp, 0xff00	# Reserve space on the stack
01: 0xAFB00000	sw		$s0, 0($sp)		# Save $s0 as this variable be our pointer to the TAP's variables

02: 0x3C100000	lui		$s0, 0x0000		# << Load the $gp value for the TAP and save it on the stack
03: 0x36100000	ori		$s0, $s0, 0x0000	# <<
04: 0xAFB00004	sw		$s0, 4($sp)

05: 0x3C100000	lui		$s0, 0x0000		# <<Load the address of the CPURegs structure
06: 0x36100000	ori		$s0, $s0, 0x0000	# <<
07: 0xAFB00008	sw		$s0, 8($sp)		# Save the CPURegs' address on the stack so we don't need to patch the hook code twice

08: 0xAE010000	sw		$at,   0($s0)		# Save all registers into the CPURegs structure
09: 0xAE020004	sw		$v0,   4($s0)
10: 0xAE030008	sw		$v1,   8($s0)
11: 0xAE04000C	sw		$a0,  12($s0)
12: 0xAE050010	sw		$a1,  16($s0)
13: 0xAE060014	sw		$a2,  20($s0)
14: 0xAE070018	sw		$a3,  24($s0)
15: 0xAE08001C	sw		$t0,  28($s0)
16: 0xAE090020	sw		$t1,  32($s0)
17: 0xAE0A0024	sw		$t2,  36($s0)
18: 0xAE0B0028	sw		$t3,  40($s0)
19: 0xAE0C002C	sw		$t4,  44($s0)
20: 0xAE0D0030	sw		$t5,  48($s0)
21: 0xAE0E0034	sw		$t6,  52($s0)
22: 0xAE0F0038	sw		$t7,  56($s0)
23: 0xAE110040	sw		$s1,  64($s0)
24: 0xAE120044	sw		$s2,  68($s0)
25: 0xAE130048	sw		$s3,  72($s0)
26: 0xAE14004C	sw		$s4,  76($s0)
27: 0xAE150050	sw		$s5,  80($s0)
28: 0xAE160054	sw		$s6,  84($s0)
29: 0xAE170058	sw		$s7,  88($s0)
30: 0xAE18005C	sw		$t8,  92($s0)
31: 0xAE190060	sw		$t9,  96($s0)
32: 0xAE1A0064	sw		$k0, 100($s0)
33: 0xAE1B0068	sw		$k1, 104($s0)
34: 0xAE1C006C	sw		$gp, 108($s0)

35: 0x27B10100	addiu		$s1, $sp, 256		# Save the stack pointer as $sp + 256
36: 0xAE110070	sw		$s1, 112($s0)

37: 0xAE1E0074	sw		$fp, 116($s0)
38: 0xAE1F0078	sw		$ra, 120($s0)

39: 0x26110000	addiu		$s1, $s0, 0		# Save the register $s0 which has been destroyed above
40: 0x8FB00000	lw		$s0, 0($sp)
41: 0xAE30003C	sw		$s0, 60($s1)

42: 0x3C100000	lui		$s0, 0x0000		# << Load the address of the TAP_Index entry...
43: 0x36100000	ori		$s0, $s0, 0x0000	# <<
44: 0xAFB0000C	sw		$s0, 12($sp)		# ... and save the address on the stack
45: 0x8E110000	lw		$s1, 0($s0)		# Get the current value ...
46: 0xAFB10010	sw		$s1, 16($sp)		# ... and save it on the stack
47: 0x24110000	addiu		$s1, $0, 0x0000		# << Save the index of this TAP into TAP_Index
48: 0xAE110000	sw		$s1, 0($s0)

49: 0x24040000	addiu		$a0, $0, 0x00		# << Inform the TAP about the hook index
50: 0x8FBC0004	lw		$gp, 4($sp)		# Load global variable TAPgp into $gp

51: 0x8FA50008	lw		$a1, 8($sp)		# Let the second parameter point to the CPURegs structure

52: 0x0C000000	jal		0			# << Call the HookFcn
53: 0x00000000	nop

54: 0x8FB0000C	lw		$s0, 12($sp)		# Restore the TAP_Ident value
55: 0x8FB10010	lw		$s1, 16($sp)
56: 0xAE110000	sw		$s1, 0($s0)

57: 0x8FB00008	lw		$s0, 8($sp)		# Recall the CPURegs' structure address

58: 0x8E010000	lw		$at,   0($s0)		# Load all registers from the CPURegs structure
69: 0x8E020004	lw		$v0,   4($s0)
60: 0x8E030008	lw		$v1,   8($s0)
61: 0x8E04000C	lw		$a0,  12($s0)
62: 0x8E050010	lw		$a1,  16($s0)
63: 0x8E060014	lw		$a2,  20($s0)
64: 0x8E070018	lw		$a3,  24($s0)
65: 0x8E08001C	lw		$t0,  28($s0)
66: 0x8E090020	lw		$t1,  32($s0)
67: 0x8E0A0024	lw		$t2,  36($s0)
68: 0x8E0B0028	lw		$t3,  40($s0)
69: 0x8E0C002C	lw		$t4,  44($s0)
70: 0x8E0D0030	lw		$t5,  48($s0)
71: 0x8E0E0034	lw		$t6,  52($s0)
72: 0x8E0F0038	lw		$t7,  56($s0)
73: 0x8E110040	lw		$s1,  64($s0)
74: 0x8E120044	lw		$s2,  68($s0)
75: 0x8E130048	lw		$s3,  72($s0)
76: 0x8E14004C	lw		$s4,  76($s0)
77: 0x8E150050	lw		$s5,  80($s0)
78: 0x8E160054	lw		$s6,  84($s0)
79: 0x8E170058	lw		$s7,  88($s0)
80: 0x8E18005C	lw		$t8,  92($s0)
81: 0x8E190060	lw		$t9,  96($s0)
82: 0x8E1A0064	lw		$k0, 100($s0)
83: 0x8E1B0068	lw		$k1, 104($s0)
84: 0x8E1C006C	lw		$gp, 108($s0)
85: 0x8E1E0074	lw		$fp, 116($s0)
86: 0x8E1F0078	lw		$ra, 120($s0)
87: 0x8E10003C	lw		$s0,  60($s0)		# The last one is our structure pointer

88: 0x27BD0100	addiu		$sp, $sp, 256		# Free the space on the stack
89: 0x00000000	nop					# Make sure that $sp is ready
90: 0x00000000	nop

91: 0x00000000	nop					# << Reserved for the first two commands of the hooked function
92: 0x00000000	nop					# <<

93: 0x08000000	j		0			# << Return to the original routine
94: 0x00000000	nop
