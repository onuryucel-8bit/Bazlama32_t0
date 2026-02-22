/*

LOAD
	load $rs1, 0x55
	load $ra1, 0x0203
	load $ro1, 0x0203'2420

	load $rs1, @r2
	load $ra1, @r2
	load $ro1, @r2

	load $rs1, @0f00'2f00
	load $ra1, @0f00'2f00
	load $ro1, @0f00'2f00

	load $rs1, @0f00'2f00+r2
	load $ra1, @0f00'2f00+r2
	load $ro1, @0f00'2f00+r2

STR
	str @0f00'20a0,0x4020'ffff
	str @0f00'20a0,$ro2
	str @0f00'20a0, @r2
	str @0f00'20a0+r1, @r2
*/