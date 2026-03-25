#include <iostream>
#include <cmath>
#include <cstdint>

int main()
{   
    for(int i=0;i<=360;i++)
    {
        printf(".db ");
		
		//float s = sin(i * M_PI / 180.0f);
        float s = cos(i * M_PI / 180.0f);
		
		
        uint32_t v = *(uint32_t*)&s;

       printf("0x%02X,0x%02X,0x%02X,0x%02X ; sin(%d)\n",
        (v >> 24) & 0xFF,
        (v >> 16) & 0xFF,
        (v >> 8)  & 0xFF,
        v & 0xFF,
        i);
    }
}