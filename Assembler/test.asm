
/*

;LOAD
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

;STR
	str @0f00'20a0,0x4020'ffff
	str @0f00'20a0,@r2
	str @0f00'20a0,$ro2
	str @0f00'20a0+r1, @r2


;MOV
	MOV $ro1, $rs2
	

;ADD
	add $rs1, 0xff
	add $ra1, 0xffaa
	add $ro1, 0xffaa01cc

	add $rs1,@r2
	
	add $rs1, @0f02'04cc

	add $rs1, $rs2
	
;SUB

	sub $rs1, 0xff
	sub $ra1, 0xffaa
	sub $ro1, 0xffaa01cc

	sub $rs1, @r2

	sub $rs1, @0f02'04cc

	sub $rs1, $rs2

;MUL
	
	mul $rs1, 0xff
	mul $ra1, 0xffaa
	mul $ro1, 0xffaa01cc
	
	mul $rs1, @r2
	
	mul $rs1, @0f02'04cc
	
	mul $rs1, $rs2

;DIV 
	
	div $rs1, 0xff
	div $ra1, 0xffaa
	div $ro1, 0xffaa01cc
	
	div $rs1, @r2
	
	div $rs1, @0f02'04cc
	
	div $rs1, $rs2

;CMP
	cmp $rs1,0xf2
	cmp $rs1,@r2
	cmp $rs1,@ff02'0c0d
	cmp $rs1,$rs2

;SHL
	shl $rs1, 0xff
	shl $rs1, $rs2

;SHR
	shr $rs1, 0xff
	shr $rs1, $rs2

;AND
	and $rs1, $rs2

;OR
	or $rs1, $rs2

;XOR
	xor $rs1, $rs2

;NOT
	not $rs1

;PUSH
	push $rs1
	push $ra1
	push $ro1

;POP
	pop $rs1
	pop $ra1
	pop $ro1

;CALL
	call @r1
	call @ff00'22cc

	;------------;
	FUNC func_t0
	
	RET	

	call func_t0
	;------------;

	call func_t1

	FUNC func_t1

	RET
	;------------;

;JMP

	add $rs1, $rs2
	l_t1:
		sub $rs1, $rs2

		jmp l_t1

	add $rs1, $rs2

	jmp l_t2

	l_t2:
		add $rs1,$rs2
		add $rs1,$rs2
*/

