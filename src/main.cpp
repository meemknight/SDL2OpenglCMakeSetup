#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <iostream>
#include <gl2d/gl2d.h> //my 2d library, just to try OpenGL
#include <openglErrorReporting.h>
#undef main

#pragma region imgui
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"
#include "imguiThemes.h"
#pragma endregion

int main(int argc, char *argv[])
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

#ifdef __APPLE__
	// apple moment...
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#endif

	// Create a window
	SDL_Window *window = SDL_CreateWindow("Hello SDL", 100, 100, 680, 480,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	if (window == nullptr)
	{
		std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	// Create OpenGL context
	SDL_GLContext glContext = SDL_GL_CreateContext(window);
	if (glContext == nullptr)
	{
		std::cerr << "SDL_GL_CreateContext Error: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	// Load OpenGL functions using glad
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		SDL_GL_DeleteContext(glContext);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	// Enable OpenGL error reporting
	enableReportGlErrors();

#pragma region imgui
	ImGui::CreateContext();

	//you can use whatever imgui theme you like!
	imguiThemes::yellow();
	//ImGui::StyleColorsDark();
	//imguiThemes::gray();
	//imguiThemes::green();
	//imguiThemes::red();
	//imguiThemes::embraceTheDarkness();


	ImGuiIO &io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	ImGuiStyle &style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		//style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 0.f;
		style.Colors[ImGuiCol_DockingEmptyBg].w = 0.f;
	}

	ImGui_ImplSDL2_InitForOpenGL(window, glContext);
	ImGui_ImplOpenGL3_Init("#version 330");
#pragma endregion

	//I am using my own renderer here to test, but you can remove it and use OpenGL normally!
	gl2d::init();
	gl2d::Renderer2D renderer2d;
	renderer2d.create();

	// Main event loop
	bool running = true;
	while (running)
	{
		int w = 0, h = 0;
		SDL_GetWindowSize(window, &w, &h);

		renderer2d.updateWindowMetrics(w, h);
		glViewport(0, 0, w, h);

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
			{
				running = false;
			}

			if (event.type == SDL_WINDOWEVENT)
			{
				if (event.window.event == SDL_WINDOWEVENT_CLOSE && 
					event.window.windowID == SDL_GetWindowID(window))
				{
					running = false;
				}
			}
		}

	#pragma region imgui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(window);
		ImGui::NewFrame();
		// Create a docking space
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
	#pragma endregion

		glClear(GL_COLOR_BUFFER_BIT);


		// Example ImGui window
		ImGui::Begin("Test Window");
		
		ImGui::Text("Hello, world!");
		ImGui::Button("I am Pressy the button!");
		float values[5] = {0.5, 0.4, 0.3, 0.56, 0.46};
		ImGui::PlotHistogram("I am a plot!", values, 5);
		
		ImGui::End();


		// testing opengl
		renderer2d.renderRectangle({100, 100, 100, 100}, Colors_Orange);
		renderer2d.flush();


	#pragma region imgui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		//view port stuff
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			SDL_Window *backup_current_window = SDL_GL_GetCurrentWindow();
			SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
		}
	#pragma endregion

		SDL_GL_SwapWindow(window);
	}

	// Cleanup ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	// Cleanup SDL
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
