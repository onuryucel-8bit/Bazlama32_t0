#include "pch.h"

#include "Emu.h"

#include "imgui-docking/imgui/imgui.h"
#include "imgui-docking/imgui/backends/imgui_impl_sdl3.h"
#include "imgui-docking/imgui/backends/imgui_impl_sdlrenderer3.h"
#include <bitset>


//uint32_t decToIEEE754_32(float decf)
//{
//	uint32_t result;
//	std::memcpy(&result, &decf, sizeof(result));
//	return result;
//}
//
//uint32_t decToIEEE754_32(std::string decf)
//{
//	uint32_t result;
//
//	float value = std::stof(decf);
//
//	std::memcpy(&result, &value, sizeof(result));
//	return result;
//}

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
#include "utils/Radix.h"

baz::Emu emu(cmake_PROJECT_TESTS "emuHex.txt");
SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* canvas;
ImGuiIO* io;

using Color_t = uint16_t;

int emuWindowWidth = 800;
int emuWindowHeight = 600;

int windowWidth = 1600;
int windowHeight = 800;

bool f_running = true;
bool f_runEmu = false;
bool f_runStep = false;

Color_t* colorBuffer = new Color_t[emuWindowWidth * emuWindowHeight];

SDL_FRect emuRect = { 0,0, emuWindowWidth + 2, emuWindowHeight + 2 };

uint16_t getBytes_t0(uint8_t uz, uint32_t adr)
{
	uint16_t retval = 0;
	uint16_t temp;

	for (size_t i = 0; i < uz; i++)
	{
		temp = emu.m_frameBuffer[adr];
		adr++;
		retval = (retval << 8) | temp;
	}

	return retval;
}

void renderImgui() 
{
	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
	ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), dockspace_flags);	

	//=====================================================//
	//=====================================================//
	//=====================================================//

	ImGui::Begin("RegisterFile");
	
	static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

	if (ImGui::BeginTable("table1", 4, flags))
	{
		ImGui::TableSetupColumn("Reg", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Hex", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Dec(uint32_t)", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Ieee32", ImGuiTableColumnFlags_WidthStretch);
	
		ImGui::TableHeadersRow();
		
		ImGui::TableNextRow();
		for (int row = 0; row < 6; row++)
		{
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("R%d", row);
		}
		
		ImGui::Text("FlagReg");
		ImGui::Text("StackReg");
		ImGui::Text("PC");


		for (int row = 0; row < 8; row++)
		{
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("0x%04X'%04X", (emu.m_registerFile[row] >> 16) & 0xFFFF, emu.m_registerFile[row] & 0xFFFF);
		}

		ImGui::Text("0x%04X'%04X", (emu.pc >> 16) & 0xFFFF, emu.pc & 0xFFFF);

		for (int row = 0; row < 8; row++)
		{
			ImGui::TableSetColumnIndex(2);
			ImGui::Text("%d", emu.m_registerFile[row]);
		}

		ImGui::Text("%d", emu.pc);

		for (int row = 0; row < 8; row++)
		{
			ImGui::TableSetColumnIndex(3);
			ImGui::Text("%f", rdx::IEEE754_toFloat(emu.m_registerFile[row]));
		}
		ImGui::Text("----");

		ImGui::EndTable();
	}


	//--------------------------------------------------//

	bool pressedStep = ImGui::Button("Step");
	bool pressedRun = ImGui::Button("Run");

	if (pressedStep || (ImGui::IsItemHovered() && ImGui::IsKeyPressed(ImGuiKey_Space)))
	{
		f_runStep = true;
	}

	if (pressedRun)
	{
		f_runEmu = !f_runEmu;
	}

	//--------------------------------------------------//

	ImGui::NewLine();	
	ImGui::SliderFloat("slider", &io->FontGlobalScale, 0, 5);


	//=====================================================//
	//=====================================================//
	//=====================================================//
	ImGui::End();
	ImGui::Render();
	
	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);

}

void draw()
{
	////load colorbuffer
	SDL_UpdateTexture(canvas, NULL, colorBuffer, (int)(emuWindowWidth * sizeof(Color_t)));

	////make it pixaled
	SDL_SetTextureScaleMode(canvas, SDL_SCALEMODE_NEAREST);

	////render canvas
	SDL_FRect dest = { 1.0f, 1.0f, emuWindowWidth + 1, emuWindowHeight + 1};
	SDL_RenderTexture(renderer, canvas, NULL, &dest);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderRect(renderer, &emuRect);

	SDL_SetRenderDrawColor(renderer, 0,0,0,0);
}

void initImgui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	io = &ImGui::GetIO();

	// Enable Docking
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io->FontGlobalScale = 1;

	ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer3_Init(renderer);
}

void initSDL()
{
	window = SDL_CreateWindow("bazlama", windowWidth, windowHeight, SDL_WINDOW_FULLSCREEN);

	renderer = SDL_CreateRenderer(window, nullptr);
}

void processEvent()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL3_ProcessEvent(&event);

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
}

int main()
{
		
	initSDL();
	
	initImgui();

	canvas = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB4444, SDL_TEXTUREACCESS_STREAMING, emuWindowWidth, emuWindowHeight);
	
	
	while (f_running)
	{
		processEvent();

		if (f_runEmu == true || f_runStep == true)
		{
			emu.step();
			f_runStep = false;
		}

		for (size_t i = 0; i < emuWindowWidth * emuWindowHeight; i++)
		{
			colorBuffer[i] = getBytes_t0(baz::UzTip::REG_16, 0 + (i * 2));
		}

		SDL_RenderClear(renderer);

		renderImgui();

		draw();

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