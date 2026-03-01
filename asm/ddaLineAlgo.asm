/*
void Graphics::ddaLineAlgo(int x0, int y0, int x1, int y1, Color_t color)
{
	int deltaX = x1 - x0;
	int deltaY = y1 - y0;

	/*		
		if |deltaX| >= |deltaY|
			sideLength = |deltaX|

		else
			sideLength = |deltaY|		
	*/
	int sideLength = abs(deltaX) >= abs(deltaY) ? abs(deltaX) : abs(deltaY);

	float incX = deltaX / (float)sideLength;
	float incY = deltaY / (float)sideLength;

	float currentX = x0;
	float currentY = y0;

	for (size_t i = 0; i <= sideLength; i++)
	{
		drawPixel(round(currentX), round(currentY), color);
		currentX += incX;
		currentY += incY;
	}
}
*/

;x0
load $ro0, 0x0
;x1
load $ro1, 0x0
;deltaX
sub $ro0, $ro1

;y0
load $ro1, 0x0
;y1
load $ro2, 0x0
;deltaY
sub $ro1, $ro2

;call abs

;float incX = deltaX / (float)sideLength;
;float incY = deltaY / (float)sideLength;
fdiv $ro0, sideLength
fdiv $ro1, sideLength

;i = 0
load $ro2, 0x0
loop_dda:

	call drawPixel
	add $ro3, $ro0
	add $ro4, $ro1
	
	
	;for (size_t i = 0; i <= sideLength; i++)
	;i++
	add $ro2, 0x1
	;i <= sideLength
	cmp $ro2, sideLength
	jl loop_dda
	