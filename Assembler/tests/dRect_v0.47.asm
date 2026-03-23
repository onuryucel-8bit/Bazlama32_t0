close_debug

CALL drawDots	


LOAD $ro0, 100
LOAD $ro1, 100
LOAD $ro2, 30
LOAD $ro3, 30
LOAD $ra4, 0xffff


CALL drawRect

HLT

FUNC drawRect
	
	;r0  |x0	 1'0032, 10033, 10034, 10035
	;r1  |y0	 1'0036, 10037, 10038, 10039
	;r2  |w		 1'003a, 1003b, 1003c, 1003d
	;r3  |h		 1'003e, 1003f, 10040, 10041
	;r4  |renk	 1'0042, 10043, 10044, 10045	
	
	STR @0001'0032, $ro0
	STR @0001'0036, $ro1
	STR @0001'003a, $ro2
	STR @0001'003e, $ro3
	STR @0001'0042, $ro4
	
	;
	;  x0,y0		  x0+w, y0
	;   ._____________.
	;	|			  |
	;	|			  |
	;   |			  |
	;	|			  |
	;   ._____________.
	;  x0,y0+h        x0+w, y0+h
	;
		
	
	;-------------------------------;
	;(x0, y0) => (x0 + w, y0)
	
	;r0 = x0
	;r1 = y0
	
	;r2 = x0 + w	
	ADD $ro2, $ro0
	
	;r3 = y0
	MOV $ro3, $ro0
	CALL drawLineDDA	
	
	;-------------------------------;	
	;(x0 + w, y0) => (x0 + w, y0 + h)
	
	;r0 = x0 + w
	LOAD $ro0, @0001'0032
	ADD $ro0,  @0001'003a
	
	;r1 = y0
	LOAD $ro1, @0001'0036
	
	;r2 = x0 + w
	MOV $ro2, $ro0
	
	;r3 = y0 + h
	MOV $ro3, $ro1
	ADD $ro3, @0001'003e
	
	LOAD $ro4, @0001'0042
	CALL drawLineDDA	
	
	;-------------------------------;	
	;(x0 + w, y0 + h) => (x0, y0 + h)
	
	;r0 = x0 + w
	LOAD $ro0, @0001'0032
	ADD $ro0,  @0001'003a
	
	;r1 = y0 + h
	LOAD $ro1, @0001'0036
	ADD $ro1, @0001'003e
	
	;r2 = x0
	LOAD $ro2, @0001'0032
	
	;r3 = y0 - h
	LOAD $ro3, @0001'0036
	SUB $ro3, @0001'003e
		
	LOAD $ro4, @0001'0042
	CALL drawLineDDA

	;-------------------------------;	
	;(x0, y0 + h) => (x0, y0)
	
	;r0 = x0
	LOAD $ro0, @0001'0032
	
	;r1 = y0 + h
	LOAD $ro1, @0001'0036
	ADD $ro1, @0001'003e
	
	;r2 = x0
	LOAD $ro2, @0001'0032
	
	;r3 = y0
	LOAD $ro3, @0001'0036
		
	LOAD $ro4, @0001'0042
	CALL drawLineDDA
	
	
RET

FUNC drawDots
	
	;x
	LOAD $ro0, 0
	;y
	LOAD $ro1, 0
	;renk
	LOAD $ra2, 0xffff
	;i = 0
	LOAD $ro3, 0
	;j = 0
	LOAD $ro4, 0
	
	/*
	while i < 800
		while j < 600
			DrawPixel(i,j);
			j++;
		i++;	
	*/
	   
	drawDotsLoop:
		
		;j = 0
		LOAD $ro4, 0x0000'0000
		;x = 0
		LOAD $ro0, 0x0000'0000
		drawDotsLoop_L0:
			CALL DrawPixel
			
			CMP $ro4, 0x0000'0040
			;j++
			ADD $ro4, 0x0000'0001
			;x++
			ADD $ro0, 0x0000'000a
			JNE drawDotsLoop_L0
					
		CMP $ro3,0x0000'0020
		;i++
		ADD $ro3,0x0000'0001
		;y++
		ADD $ro1,0x0000'000a
		
		JNE drawDotsLoop
	
RET


FUNC drawTriangle
	;r0,r1,r2,r3,r4,stack,r5
	;x0,y0,x1,y1,x2,y2   ,color
		
	;x0		1'0017,10018,10019,1001a
	;y0		1'001b,1001c,1001d,1001e
	;x1		1'001f,10020,10021,10022
	;y1		1'0023,10024,10025,10026
	;x2		1'0027,10028,10029,1002a
	;y2		1'002b,1002c,1002d,1002e
	;renk	1'002f,10030,10031,10032
	
	
	STR @0001'0017, $ro0	;x0
	STR @0001'001b, $ro1	;y0
	STR @0001'001f, $ro2	;x1
	STR @0001'0023, $ro3	;y1
	STR @0001'0027, $ro4	;x2
	
	;color => r4
	MOV $ro4, $ro5	

	;stackPointer => r5
	MOV $ro3, $ro7

	;	|		 |
	;	|		 | <= stackPointer [2]
	;	.--------.
	;	|		 |	[3]
	;	|		 |	[4]
	;	|		 |	[5]
	;	| retAdr | 	[6]
	;	.--------.
	;	|		 |	[7]
	;	|		 |	[8]
	;	|		 |	[9]
	;	|  renk	 |  [10]
	;	.--------.
	;stackPointer + 2 => renk_adr

	;stackPointer + sizeof(data)
	ADD $ro3, 0x0000'0005
	LOAD $ro5, @r3
	
	;y2 => ram
	STR @0001'002b, $ro5	;y2
	;renk => ram
	STR @0001'002f, $ro4	;renk
	
	
	;x0		1'0017,10018,10019,1001a
	;y0		1'001b,1001c,1001d,1001e
	;x1		1'001f,10020,10021,10022
	;y1		1'0023,10024,10025,10026
	;x2		1'0027,10028,10029,1002a
	;y2		1'002b,1002c,1002d,1002e
	;renk	1'002f,10030,10031,10032				
	
	;x2,y2 => x0,y0
	LOAD $ro0, @0001'0027
	LOAD $ro1, @0001'002b
	LOAD $ro2, @0001'0017
	LOAD $ro3, @0001'001b
	LOAD $ro4, @0001'002f
	CALL drawLineDDA
	
	;x0,y0 => x1,y1
	LOAD $ro0, @0001'0017
	LOAD $ro1, @0001'001b
	LOAD $ro2, @0001'001f
	LOAD $ro3, @0001'0023
	LOAD $ro4, @0001'002f
	CALL drawLineDDA
	
	;x1,y1 => x2,y2
	LOAD $ro0, @0001'001f
	LOAD $ro1, @0001'0023
	LOAD $ro2, @0001'0027
	LOAD $ro3, @0001'002b
	LOAD $ro4, @0001'002f
	CALL drawLineDDA
	
	
	
RET

FUNC drawLineDDA
	
	;r0,r1,r2,r3
	;x0,y0,x1,y1
	
	;r4
	;color
	
	;color    0'ffff,10000,10001,10002
	;deltax   1'0003,10004,10005,10006
	;deltay   1'0007,10008,10009,1000a
	;i        1'000b,1000c,1000d,1000e
	;x0		  1'000f,10010,10011,10012
	;y0		  1'0013,10014,10015,10016
	
	
	;color
	STR @0000'ffff, $ra4
	;x0
	STR @0001'000f, $ro0
	;y0
	STR @0001'0013, $ro1
		
	;x1-x0
	SUB $rs2, $rs0	;deltax
	;y1-y0
	SUB $rs3, $rs1	;deltay

	;STR @deltax, $rs0
	STR @0001'0003, $ro2
	;STR @deltay, $rs1
	STR @0001'0007, $ro3
		
	;r0 deltax
	MOV $ra0, $ra2
		
	;r0 => abs => r1
	CALL abs
	;r2 = abs(deltax)
	MOV $rs2,$rs1
	;LOAD $rs0, @deltay
	LOAD $ro0, @0001'0007
	CALL abs
		
		
	;r2 abs(deltax)
	;r1 abs(deltay)
	;		
	;	if |deltaX| >= |deltaY|
	;		sideLength = |deltaX|
	;
	;	else
	;		SideLength = |deltaY|		
	;
	;int SideLength = abs(deltaX) >= abs(deltaY) ? abs(deltaX) : abs(deltaY);
	;
	;if (r2 > r1)
	;  r5 = r2
	;
	;else
	;  r5 = r1
		
	CMP $rs2,$rs1
	
	JL L0
	JMP L1
	L0:	
		;else(|deltaX| < |deltay|)
		;SideLength = deltay
		MOV $rs5, $rs1
		JMP L2
	L1:
		;SideLenght = deltax
		MOV $rs5, $rs2
	L2:
	
	;r5 = SideLength
		
	;float incX = deltaX / (float)sideLength; 
	;float incY = deltaY / (float)sideLength;
	
	;---------------------------;	
	;float(SideLength)
	ITOF $rs5
	
	;LOAD $rs4, @deltax
	LOAD $ro4, @0001'0003
	
	;float(deltax)
	ITOF $rs4
	
	;incx = (float)deltax / (float)SideLength
	FDIV $rs4, $rs5
	
	;---------------------------;
	
	;LOAD $rs3, @deltay
	LOAD $ro3, @0001'0007
	
	;float(deltay)
	ITOF $rs3
	
	;incy = (float)deltay / (float)SideLength	
	FDIV $rs3, $rs5	
	;---------------------------;
	
	;r5 SideLength(int)
	;r4 incx(float)
	;r3 incY(float)
	;r2 color
	;r1 y0 currentY(int -> float -> int...)
	;r0 x0 currentX(int -> float -> int...)
	
	
	;x0		 1000f
	;y0		 10013
	LOAD $ro0, @0001'000f
	LOAD $ro1, @0001'0013
	
	ITOF $rs0
	ITOF $rs1
	
	;i
	;STR @.., 0x0
	STR @0001'000B, 0x0000'0000
	
	;(int)SideLength
	FTOI $rs5 
	drawLoop:
		
		
			
		;load color
		LOAD $ra2, @0000'ffff
		
		PUSH $ro0
		PUSH $ro1
		;round() ~~
		FTOI $ro0
		FTOI $ro1
		
		;r0,r1,r2
		;x0,y0,color
		CALL DrawPixel
		
		POP $ro1
		POP $ro0
		
		
		;currentX += incX 
		FADD $ro0, $ro4
		;currentY += incY
		FADD $ro1, $ro3
		
		;PUSH currentX
		PUSH $ro0
			;r0 = i
			;LOAD i
			LOAD $ro0, @0001'000B
			; i, sideLength
			CMP $ro0, $ro5
			;i++
			ADD $ra0, 0x0001
			;STR i
			STR @0001'000B, $ro0
			
		;POP currentX
		POP $ro0
		JL drawLoop
	
RET

	
FUNC abs
	;r0 = x 
	;r1 = abs(x)
	
	;if(r0 > 0)
	;	return x
	;else
	;	x = x * -1
	;	return x
	CMP $ro0, 0x0000'0000
	JL absL0
	JMP absL1
	
	absL0:
		;x = x * -1
		MUL $ro0, 0xffff'ffff		
	absL1:
	
	;r1 = abs(x)
	MOV $ro1,$ro0
	
RET

/*

FUNC abs
	;int mask = x >> 31;
	;return (x ^ mask) - mask
	
	;r0 = x
	;r1 = mask
	MOV $rs1, $rs0	;r1 = x
	SAR $ro0, 0x0000'001F	;mask = x >> 31
	XOR $ro1, $ro0	;x ^ mask
	SUB $ro1, $ro0  ;(x ^ mask) - mask
	
RET


*/

FUNC DrawPixel
	;r0 x
	;r1 y
	;r2 argb_4444
	
	PUSH $ro0
	PUSH $ro1
			
		;y * width
		MUL $ro1, 0x0000'0320
		;y * width + x
		ADD $ro0, $rs1
		
		STR @0311'5A00+r0, $ra2
	
	POP $ro1
	POP $ro0
RET