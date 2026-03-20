#include "pch.h"

#include <bitset>
#include <filesystem>

#include "imgui-docking/imgui/imgui.h"
#include "imgui-docking/imgui/backends/imgui_impl_sdl3.h"
#include "imgui-docking/imgui/backends/imgui_impl_sdlrenderer3.h"

#include "SDL3/SDL.h"
#include "tiny-process/process.hpp"
#include "utils/Radix.h"
#include "utils/rang.hpp"
#include "utils/cxxtimer.hpp"

#include "Emu.h"


baz::Emu emu(cmake_PROJECT_TESTS "emuHex.txt");
SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* canvas;
ImGuiIO* io;
cxxtimer::Timer timer;

using Color_t = uint16_t;

int emuWindowWidth = 800;
int emuWindowHeight = 600;

int windowWidth = 800;
int windowHeight = 800;

bool f_running = true;
bool f_runEmu = false;
bool f_runStep = false;

Color_t* colorBuffer = new Color_t[emuWindowWidth * emuWindowHeight];
void* ptrBuffer = &colorBuffer;
int bufferPitch = (int)(emuWindowWidth * sizeof(Color_t));

SDL_FRect emuRect = { 0,0, emuWindowWidth + 2, emuWindowHeight + 2 };

uint32_t copyRegisterFile[8] = {};

std::shared_ptr<spdlog::logger> TEST_logger = spdlog::basic_logger_mt("getBytes()_t0", cmake_PROJECT_LOGS "basic-log.txt");

uint16_t getBytes_t0(uint8_t uz, uint32_t adr)
{
	uint16_t retval = 0;
	uint16_t temp;
	/*
	for (size_t i = 0; i < uz; i++)
	{
		temp = emu.m_frameBuffer[adr];
		adr++;
		retval = (retval << 8) | temp;
	}
	*/
	/*if (retval != 0)
	{

		std::stringstream ss;
		ss << std::hex << "adr= " << adr - 1 << ", adr+1=" << adr;
		TEST_logger.get()->info(ss.str());
	}*/	

	return retval;
}

void runAssembler()
{
	std::string assemblerPath = cmake_ASSEMBLER_PATH "Assembler.exe";
	TinyProcessLib::Process process(
		assemblerPath,
		""
	);

	if (!std::filesystem::exists(assemblerPath))
	{
		std::cout << rang::fg::red << "Error:: EXE NOT FOUND: " << assemblerPath << rang::style::reset << "\n";
	}

	std::cout << "exitcode" << process.get_exit_status() << "\n";
}

void drawImgui() 
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

			ImVec4 color = (copyRegisterFile[row] != emu.m_registerFile[row])
				? ImVec4(1, 0, 0, 1)						//true
				: ImGui::GetStyleColorVec4(ImGuiCol_Text);	//false

			ImGui::PushStyleColor(ImGuiCol_Text, color);
			ImGui::Text("R%d", row);
			ImGui::PopStyleColor();
		}
		
		ImGui::Text("FlagReg");
		ImGui::Text("StackReg");

			
		ImGui::Text("PC");
		

		for (int row = 0; row < 8; row++)
		{
			ImGui::TableSetColumnIndex(1);

			ImVec4 color = (copyRegisterFile[row] != emu.m_registerFile[row])
				? ImVec4(1, 0, 0, 1)						//true
				: ImGui::GetStyleColorVec4(ImGuiCol_Text);	//false


			ImGui::PushStyleColor(ImGuiCol_Text, color);
			ImGui::Text("0x%04X'%04X", (emu.m_registerFile[row] >> 16) & 0xFFFF, emu.m_registerFile[row] & 0xFFFF);
			ImGui::PopStyleColor();

			
		}

		ImGui::Text("0x%04X'%04X", (emu.pc >> 16) & 0xFFFF, emu.pc & 0xFFFF);

		for (int row = 0; row < 8; row++)
		{
			ImGui::TableSetColumnIndex(2);

			ImVec4 color = (copyRegisterFile[row] != emu.m_registerFile[row])
				? ImVec4(1, 0, 0, 1)						//true
				: ImGui::GetStyleColorVec4(ImGuiCol_Text);	//false


			ImGui::PushStyleColor(ImGuiCol_Text, color);
			ImGui::Text("%d", emu.m_registerFile[row]);
			ImGui::PopStyleColor();
		}

		ImGui::Text("%d", emu.pc);

		for (int row = 0; row < 8; row++)
		{
			ImGui::TableSetColumnIndex(3);

			ImVec4 color = (copyRegisterFile[row] != emu.m_registerFile[row])
				? ImVec4(1, 0, 0, 1)						//true
				: ImGui::GetStyleColorVec4(ImGuiCol_Text);	//false


			ImGui::PushStyleColor(ImGuiCol_Text, color);
			ImGui::Text("%f", rdx::IEEE754_toFloat(emu.m_registerFile[row]));
			ImGui::PopStyleColor();
		}
		ImGui::Text("----");

		ImGui::EndTable();
	}

	//--------------------------------------------------//

	bool pressedStep = ImGui::Button("Step");
	ImGui::SameLine();
	
	
	if (pressedStep || (ImGui::IsItemHovered() && ImGui::IsKeyPressed(ImGuiKey_Space)))
	{
		f_runStep = true;
		for (size_t i = 0; i < 8; i++)
		{
			copyRegisterFile[i] = emu.m_registerFile[i];
		}
		
	}

	if (f_runEmu)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f });
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.3f, 0.4f, 1.0f });
	}

	bool pressedRun = ImGui::Button("Run");

	ImGui::PopStyleColor(1);

	if (pressedRun)
	{
		f_runEmu = !f_runEmu;

		//TODO ? ternary
		/*if (f_runEmu == true)
		{
			timer.start();
		}
		else
		{
			timer.printElapsedTime();
			timer.reset();			
		}*/
	}



	ImGui::SameLine();
	if (ImGui::Button("RunAssembler"))
	{
		runAssembler();
	}
	ImGui::SameLine();
	if (ImGui::Button("LoadProgram"))
	{
		emu.loadProgram();
	}
	ImGui::SameLine();
	if (ImGui::Button("restart"))
	{
		emu.restart();
		if (f_runEmu == true)
		{
			f_runEmu = false;
		}
	}

	//--------------------------------------------------//
	auto enmKmt = magic_enum::enum_cast<baz::Komut>(emu.m_komut);
	if (enmKmt.has_value())
	{

		ImGui::Text("command -> : {%s}", magic_enum::enum_name(*enmKmt).data());

		baz::RegisterPart regPart = emu.getRegisterPart(emu.pc);

		ImGui::Text("RegPart  :rU {%s} : rA {%01x} : rB{%01x}", magic_enum::enum_name(regPart.m_reguz).data(), regPart.m_rega, regPart.m_regb);
	
	}
	//--------------------------------------------------//

	ImGui::NewLine();	
	ImGui::SliderFloat("slider", &io->FontGlobalScale, 0, 5);

	ImGui::End();
	//=====================================================//
	//=====================================================//
	//=====================================================//	
	ImGui::Begin("Stack");

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
	/*if (disable_mouse_wheel)
		window_flags |= ImGuiWindowFlags_NoScrollWithMouse;*/

	ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 260), ImGuiChildFlags_None, window_flags);

	for (int i = 0; i < 100; i++)
	{		
		ImGui::Text("0x%04X'%04X: %0x%04X'%04X", (baz_StackBaseADR - i )>> 16, baz_StackBaseADR - i, (emu.m_ram[baz_StackBaseADR - i] >> 16) & 0xFFFF, emu.m_ram[baz_StackBaseADR - i] & 0xFFFF);
	}

	ImGui::EndChild();

	ImGui::End();
	//=====================================================//
	//=====================================================//
	//=====================================================//

	ImGui::Render();
	
	ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
	
}

void draw()
{
	/*SDL_LockTexture(canvas, NULL, &ptrBuffer, &bufferPitch);

	uint16_t* fb = (uint16_t*)emu.m_frameBuffer.data();

	for (int y = 0; y < emuWindowHeight; y++)
	{
		uint16_t* row = (uint16_t*)((uint8_t*)ptrBuffer + y * bufferPitch);

		memcpy(row, fb + y * emuWindowWidth, emuWindowWidth * sizeof(uint16_t));
	}

	SDL_UnlockTexture(canvas);*/

	//load colorbuffer
	SDL_UpdateTexture(canvas, NULL, colorBuffer, (int)(emuWindowWidth * sizeof(Color_t)));

	

	//render canvas
	SDL_FRect dest = { 1.0f, 1.0f, emuWindowWidth + 1, emuWindowHeight + 1};
	SDL_RenderTexture(renderer, canvas, NULL, &dest);

	SDL_SetRenderDrawColor(renderer, 255, 0, 0,255);
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
	window = SDL_CreateWindow("bazlama", windowWidth, windowHeight, SDL_WINDOW_RESIZABLE);

	renderer = SDL_CreateRenderer(window, nullptr);

	//make it pixaled
	SDL_SetTextureScaleMode(canvas, SDL_SCALEMODE_NEAREST);
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

void drawPixel(int x, int y, Color_t color)
{
	if (x >= 0 && x < emuWindowWidth && y >= 0 && y < emuWindowHeight)
	{
		uint8_t parta = (color & 0xff00) >> 8;
		uint8_t partb = color & 0x00ff;

		emu.m_frameBuffer[y * emuWindowWidth + x] = parta;
		emu.m_frameBuffer[y * emuWindowWidth + x + 1] = partb;

		//colorBuffer[y * emuWindowWidth + x] = color;
	}
}

void ddaLineAlgo(int x0, int y0, int x1, int y1, Color_t color)
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

int main()
{

	//emu.run();

	initSDL();
	
	initImgui();

	canvas = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB4444, SDL_TEXTUREACCESS_STREAMING, emuWindowWidth, emuWindowHeight);
	
	for (size_t i = 0; i < emuWindowWidth * emuWindowHeight; i++)
	{
		colorBuffer[i] = 0;
	}

	for (size_t i = 0; i < 8; i++)
	{
		copyRegisterFile[i] = emu.m_registerFile[i];
	}
	
	//ddaLineAlgo(0, 0, 100, 100, 0xffff);
	
	

	while (f_running)
	{
		processEvent();

		if (f_runEmu == true || f_runStep == true)
		{		
			
			emu.step();
			
			//std::cout << "REGISTER 0:" << emu.m_registerFile[0] << "\n";

			f_runStep = false;

			if (emu.TEST_counter == 4)
			{
          		std::cout << "NE BOK YIYORSUN SOYLE, SOYLE ULAN!!!\n";
			}

			/*for (size_t i = 0; i < emuWindowWidth * emuWindowHeight; i++)
			{
				colorBuffer[i] = 0xf0f0;
			}*/

			for (size_t i = 0; i < emuWindowWidth * emuWindowHeight; i++)
			{
				colorBuffer[i] = (emu.m_frameBuffer[i] << 8) | emu.m_frameBuffer[i + 1];
			}

			//for (size_t i = 0; i < emuWindowWidth * emuWindowHeight; i++)
			//{
			//	//colorBuffer[i] = getBytes_t0(baz::UzTip::REG_16, 0 + (i * 2));

			//	/*if (colorBuffer[i] != 0)
			//	{
			//		std::stringstream ss;
			//		ss << std::hex << "i= " << i;
			//		TEST_logger.get()->info(ss.str());
			//	}*/
			//}			
		}
		
		/*for (size_t i = 0; i < emuWindowWidth * emuWindowHeight; i++)
		{
			uint16_t val = (emu.m_frameBuffer[i * 2] << 8) | emu.m_frameBuffer[i * 2 + 1];

			colorBuffer[i] = val;
		}*/

		SDL_RenderClear(renderer);

		drawImgui();

		draw();

		SDL_RenderPresent(renderer);

		
	}
}