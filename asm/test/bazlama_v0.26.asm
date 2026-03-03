;ekranin ust kismini mavi yapan program
;i = 0
load $rs0, 0x00
;renk
load $ra1, 0xf00f

loop:
	;0x200000 
	str @0000'0020+r0, $ra1

	add $rs0, 0x02
	cmp $ro0, 0x0003'A980
	jne loop