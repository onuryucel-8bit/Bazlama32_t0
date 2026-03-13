;
;			.B
;		   /|
;		  / |
;		 /  |
;		/   | dy
;	   /    |
;	  /     |
;	 /      |
;  A.-------.
;	    dx
;
;	A(x0,y0)
;	B(x1,y1)
;

;------------------------;

close_debug

LOAD $ra0, 0x0010
LOAD $ra1, 0x01F4
LOAD $ra2, 0x01F4
LOAD $ra3, 0x0064
LOAD $ra4, 0xf00f


CALL drawLineDDA
HLT

FUNC drawLineDDA
	
	;r0,r1,r2,r3
	;x0,y0,x1,y1
	
	;r4
	;color
	
	STR @0000'ffff, $rs4
	
	PUSH $rs0
	PUSH $rs1

		SUB $rs0, $rs2	;deltax
		SUB $rs1, $rs3	;deltay
		
		;STR @deltax, $rs0
		STR @0001'0000, $rs0
		;STR @deltay, $rs1
		STR @0001'0001, $rs1
		
		;r0 deltax
		;r1 deltay
		
		;r0 => abs => r1
		CALL abs
		;r2 = abs(deltax)
		MOV $rs2,$rs1
		;LOAD $rs0, @deltay
		LOAD $rs0, @0001'0001
		CALL abs
		
		/*
		;r2 abs(deltax)
		;r1 abs(deltay)
		
		CMP $rs2,$rs1
		
		JL L0
		JMP L1
		L0:
			;SideLenght = deltay
			MOV $rs5, $rs1
			JMP L2
		L1:
			;SideLenght = deltax
			MOV $rs5, $rs2
		L2:
		
		;float(SideLenght)
		ITOF $rs5
		;LOAD $rs4, @deltax
		LOAD $rs4, @0001'0000
		;float(deltax)
		ITOF $rs4
		;incx = deltax / (float)sideLength
		FDIV $rs4, $rs5
		
		;LOAD $rs3, @deltay
		LOAD $rs3, @0001'0001
		;float(deltay)
		ITOF $rs3
		;incy = deltay / (float)sideLength
		FDIV $rs3, $rs5
	
	POP $rs1
	POP $rs0
	;r5 sideLength
	;r4 incx
	;r3 incY
	;r2 color
	;r1 y0 currentY
	;r0 x0 currentX
	
	ITOF $rs0
	ITOF $rs1
	
	;i
	;STR @.., 0x0
	STR @0001'0002, 0x0000'0000
	
	;(int)SideLength
	FTOI $rs5 ;HATA VAR ASSEMBLER GORMUYOR
	drawLoop:
		
		
		;r0,r1,r2
		;x0,y0,color
		;CALL DrawPixel
		
		;currentX += incX
		FADD $rs0, $rs4
		;currentY += incY
		FADD $rs1, $rs3
				
		PUSH $rs0
			;r0 = i
			;LOAD $rs0, @...
			LOAD $rs0, @0001'0002
			; i, sideLength
			CMP $rs0, $rs5
			;i++
			ADD $rs0, 0x01
		POP $rs0
		JL drawLoop
	*/
RET

/*
float currentX = x0;
	float currentY = y0;

	for (size_t i = 0; i <= sideLength; i++)
	{
		drawPixel(round(currentX), round(currentY), color);
		currentX += incX;
		currentY += incY;
	}

	*/
	
	
FUNC abs
	;int mask = x >> 31;
	;return (x ^ mask) - mask
	
	;r0 = x
	;r1 = mask
	MOV $rs1, $rs0	;r1 = x
	SHR $rs0, 31	;mask = x >> 31
	XOR $rs1, $rs0	;x ^ mask
	SUB $rs1, $rs0  ;(x ^ mask) - mask
	
RET

FUNC DrawPixel
	;r0 x(float)
	;r1 y(float)
	;r2 argb_4444
	
	PUSH $rs0
	PUSH $rs1
		;r1 = y0
		;r0 = x0
		
		;round() ~~
		FTOI $rs0
		FTOI $rs1
		
		
		;y * width
		MUL $rs1, 800
		;y * width + x
		ADD $rs0, $rs1
		STR @0031'5A00+r0, $ro2
	
	POP $rs1
	POP $rs0
RET