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



;(100,100) => (10,10)
;LOAD $ra0, 0x01f4
;LOAD $ra1, 0x01f3
;LOAD $ra2, 0x0000
;LOAD $ra3, 0x0000


;LOAD $ra0, 0x0000
;LOAD $ra1, 0x000a
;LOAD $ra2, 0xffff


;(100,500) => (400,100)
LOAD $ra0, 0x0064	;x0
LOAD $ra1, 0x01f4	;y0
LOAD $ra2, 0x0190	;x1
LOAD $ra3, 0x0064	;y1
LOAD $ra4, 0xffff	;renk

CALL drawLineDDA

;(400,100) => (700,500)
LOAD $ra0, 0x0190	;x0
LOAD $ra1, 0x0064	;y0
LOAD $ra2, 0x02bc	;x1
LOAD $ra3, 0x01f4	;y1
LOAD $ra4, 0xffff	;renk

CALL drawLineDDA

;(700,500) => (100,500)
LOAD $ra2, 0x02bc	;x0
LOAD $ra3, 0x01f4	;y0
LOAD $ra0, 0x0064	;x1
LOAD $ra1, 0x01f4	;y1
LOAD $ra4, 0xffff	;renk

CALL drawLineDDA


HLT

FUNC drawLineDDA
	
	;r0,r1,r2,r3
	;x0,y0,x1,y1
	
	;r4
	;color
	
	;color    0'ffff,!10000,10001,10002
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
		
		;round() ~~
		FTOI $ro0
		FTOI $ro1
		
		;r0,r1,r2
		;x0,y0,color
		CALL DrawPixel
		
		ITOF $ro0  
		ITOF $ro1
		
		
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
	;r0 x(float)
	;r1 y(float)
	;r2 argb_4444
	
	PUSH $ro0
	PUSH $ro1
		;r1 = y0
		;r0 = x0
		
		;frameBuffer[r0] = r2
		
		;round() ~~
		;FTOI $ro0
		;FTOI $ro1
		
		;HATA: 1 bayt kayma var
		;x++
		;ADD $rs0, 0x01
		;y * width
		MUL $ro1, 0x0000'0320
		;y * width + x
		ADD $ro0, $rs1
		
		STR @0311'5A00+r0, $ra2
	
	POP $ro1
	POP $ro0
RET