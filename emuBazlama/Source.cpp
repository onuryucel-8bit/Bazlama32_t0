#include "pch.h"

#include "Emu.h"

#include <bitset>

uint32_t decToIEEE754_32(float decf)
{
	uint32_t result;
	std::memcpy(&result, &decf, sizeof(result));
	return result;
}

uint32_t decToIEEE754_32(std::string decf)
{
	uint32_t result;

	float value = std::stof(decf);

	std::memcpy(&result, &value, sizeof(result));
	return result;
}

//uint32_t decToIEEE754_32(std::string dec)
//{
//	size_t dotPos = dec.find('.');
//
//	std::string valbin = rdx::decToBin(std::stoi(dec.substr(0, dotPos)));
//
//	std::cout << "valbin " << valbin << "\n";
//
//	float fractPart = std::stof(dec.substr(dotPos, dec.size()));
//
//	std::string fractStr = "";
//	for (int i = 0; i < 22 - dec.substr(dotPos, dec.size()).size(); i++)
//	{
//		fractPart *= 2;
//
//		if (fractPart >= 1.0f)
//		{
//			fractStr += "1";
//			fractPart = fractPart - 1.0f;
//		}
//		else
//		{
//			fractStr += "0";
//		}		
//	}	
//
//	std::string result_FractPart = valbin + "." + fractStr;
//	std::string mantissa = valbin.substr(1, valbin.size()) + fractStr;
//
//	dotPos = result_FractPart.find('.');	
//
//	std::string exponent = rdx::decToBin(dotPos - 1 + 127);
//
//	std::cout << "result " << exponent << " " << result_FractPart << "\n";
//	
//
//	std::cout << "0" << exponent << mantissa << "\n";
//
//	return 0;
//}

#include "SDL3/SDL.h"

baz::Emu emu("emu.txt");

uint16_t getBytes_t0(uint8_t uz, uint32_t adr)
{
	uint16_t retval = 0;
	uint16_t temp;

	for (size_t i = 0; i < uz; i++)
	{
		temp = emu.m_ram[adr];
		adr++;
		retval = (retval << 8) | temp;
	}

	return retval;
}

int main()
{	
	
	emu.run();

	int windowWidth = 800;
	int windowHeight = 600;

	SDL_Window* window = SDL_CreateWindow("bazlama", windowWidth, windowHeight, SDL_WINDOW_FULLSCREEN);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

	using Color_t = uint16_t;

	Color_t* colorBuffer = new Color_t[windowWidth * windowHeight];

	for (size_t i = 0; i < windowWidth*windowHeight; i++)
	{
		colorBuffer[i] = getBytes_t0(baz::UzTip::REG_16, 0x0000'0020 + (i * 2) );
	}	

	SDL_Texture* canvas = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB4444, SDL_TEXTUREACCESS_STREAMING, windowWidth, windowHeight);
	
	bool f_running = true;
	while (f_running)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_EVENT_QUIT)
			{
				f_running = false;
			}

			switch (event.key.key)
			{
			case SDLK_ESCAPE:
				f_running = false;
				break;
			}
		}
		SDL_RenderClear(renderer);

		//load colorbuffer
		SDL_UpdateTexture(canvas, NULL, colorBuffer, (int)(windowWidth * sizeof(Color_t)));

		//make it pixaled
		SDL_SetTextureScaleMode(canvas, SDL_SCALEMODE_NEAREST);

		//render canvas
		SDL_RenderTexture(renderer, canvas, NULL, NULL);

		//swap buffers
		SDL_RenderPresent(renderer);
	}

	/*int a = 5;
	int b = -5;
	std::bitset<8> bit_a(a);
	std::bitset<8> bit_b(b);

	float afl = 10.75;
	float bfl = 0.75;
	
	std::bitset<32> bit_afl(decToIEEE754_32(afl));
	std::bitset<32> bit_bfl(decToIEEE754_32(bfl));

	std::cout
		<< " 5 | " << bit_a.to_string() << "\n"
		<< "-5 | " << bit_b.to_string() << "\n"
		<< "======================\n"
		<< "10.75 | " << bit_afl.to_string().substr(0, 1) << " " << bit_afl.to_string().substr(1, 8) << " " << bit_afl.to_string().substr(9, 23) << "\n"
		<< "0.75  | " << bit_bfl.to_string().substr(0, 1) << " " << bit_bfl.to_string().substr(1, 8) << " " << bit_bfl.to_string().substr(9, 23) << "\n";*/

	/*std::string val = "18.75";

	

	std::cout << std::bitset<32>(decToIEEE754_32(18.75f)).to_string() << "\n";
	std::cout << std::bitset<32>(decToIEEE754_32("18.75")).to_string();*/

	/*std::string valbin = rdx::decToBin(std::stoi(val.substr(0, 2)));

	std::cout << valbin << "\n";

	float fractPart = std::stof(val.substr(2, 3));

	std::cout << "::" << fractPart << "\n";
	std::string fractStr = "";
	for(int i = 0; i < 23; i++)
	{
		fractPart *= 2;

		if (fractPart >= 1.0f)
		{
			fractStr += "1";
		}
		else
		{
			fractStr += "0";
		}

		fractPart = fractPart - 1.0;		
	}

	std::cout << "fracpart = " << fractStr << "\n";		

	std::string result_FractPart = valbin + "." + fractStr;

	size_t dotPos = result_FractPart.find('.');

	std::cout << "result " << result_FractPart << "\n";

	std::cout << dotPos - 1<< "\n";

	std::cout << "expo" << rdx::decToBin(dotPos - 1 + 127) << "\n";*/
}