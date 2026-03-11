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



FUNC drawLineDDA:
	
	;r0,r1,r2,r3
	;x0,y0,x1,y1
	
	;r4
	;renk
	
	STR @color, r4
	
	PUSH r0
	PUSH r1

	SUB r0, r2	;deltax
	SUB r1, r3	;deltay
	
	STR @deltax, r0
	STR @deltay, r1
	
	;r0 deltax
	;r1 deltay
	
	;r0 => abs => r1
	CALL abs
	;r2 = abs(deltax)
	MOV r2,r1
	LOAD r0, @deltay
	CALL abs
	
	
	;r2 abs(deltax)
	;r1 abs(deltay)
	
	CMP r2,r1
	
	JL L0
	JMP L1
	L0:
		;SideLenght = deltay
		MOV r5,r1
		JMP L2
	L1:
		;SideLenght = deltax
		MOV r5,r2
	L2:
	
	;float(SideLenght)
	ITOF r5
	LOAD r4, @deltax
	;float(deltax)
	ITOF r4
	;incx = deltax / (float)sideLength
	FDIV r4, r5
	
	LOAD r3, @deltay
	;float(deltay)
	ITOF r3
	;incy = deltay / (float)sideLength
	FDIV r3, r5
	
	POP r1
	POP r0
	;r5 sideLength
	;r4 incx
	;r3 incY
	;r2 color
	;r1 y0 currentY
	;r0 x0 currentX
	
	;i
	STR @.., 0x0
	drawLoop:
		
		
		;r0,r1,r2
		;x0,y0,color
		CALL DrawPixel
		
		;currentX += incX
		FADD r0, r4
		;currentY += incY
		FADD r1, r3
				
		PUSH r0
		;r0 = i
		LOAD r0, @...
		; i, sideLength
		CMP r0, r5
		POP r0
		JL drawLoop
	
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
	
	
FUNC abs:
	;int mask = x >> 31;
	;return (x ^ mask) - mask
	
	;r0 = x
	;r1 = mask
	MOV r1, r0	;r1 = x
	SHR r0, 31	;mask = x >> 31
	XOR r1, r0	;x ^ mask
	SUB r1, r0  ;(x ^ mask) - mask
	
RET

FUNC DrawPixel:
	;r0 x(float)
	;r1 y(float)
	;r2 argb_4444
	
	PUSH r0
	PUSH r1
	;r1 = y0
	;r0 = x0
	
	;round() ~~
	FTOI r0
	FTOI r1
	
	
	;y * width
	MUL r1, 800
	;y * width + x
	ADD r0, r1
	STR @0031'5A00+r0, r2
	
	POP r1
	POP r0
RET