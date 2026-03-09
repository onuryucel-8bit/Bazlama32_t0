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
	
	STR @renk, r4
	
	PUSH r0
	PUSH r1

	SUB r0, r2	;deltax
	SUB r1, r3	;deltay
	
	STR @deltax, r0
	STR @deltay, r1
	
	;r0 => abs => r1
	CALL abs
	MOV r2,r1
	LOAD r0, @deltax
	CALL abs
	
	;r2 abs(deltay)
	;r1 abs(deltax)
	
	CMP r1,r2
	
	JL L0
	JMP L1
	L0:
		MOV r5,r2
		JMP L2
	L1:
		MOV r5,r1
	L2:
	
	ITOF r5
	LOAD r4, @deltax
	ITOF r4
	FDIV r4, r5 ;incX
	
	LOAD r3, @deltay
	ITOF r3
	FDIV r3, r5 ;incY
	
	;sideLength => stack
	PUSH r5
	
	;r4 incx
	;r3 incY
		
	drawLoop:
		;r0 x(float)
		;r1 y(float)
		;r2 argb_4444
		
		
		CALL DrawPixel
	
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
	
	;round() ~~
	FTOI r0
	FTOI r1
	
	MUL r0, 800
	ADD r0, r1
	STR @grafikKarti+r0, r2
RET