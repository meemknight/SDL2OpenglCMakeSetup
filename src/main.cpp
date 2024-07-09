#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <iostream>
#include <gl2d/gl2d.h> //my 2d library, just to try opengl
#undef main


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
	//apple moment...
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#endif



	// Create a window
	SDL_Window *window = SDL_CreateWindow("Hello SDL", 100, 100, 680, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE 
	| SDL_WINDOW_OPENGL);
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


	// Create a renderer
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr)
	{
		SDL_DestroyWindow(window);
		std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	//my 2D library, just to check that opengl works! you can remove it later
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
			if (event.type == SDL_QUIT)
			{
				running = false;
			}
		}

		// either draw with openl or their renderer
		if(0)
		{
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderClear(renderer);

			SDL_Rect square = {};
			square.x = 350;  // x position
			square.y = 250;  // y position
			square.w = 100;  // width
			square.h = 100;  // height

			// Render the square
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderFillRect(renderer, &square);
			
			// Present the backbuffer
			SDL_RenderPresent(renderer);
		}
		else
		{
			glClear(GL_COLOR_BUFFER_BIT);

			renderer2d.renderRectangle({100,100,100,100}, Colors_Orange);
			renderer2d.flush();
			SDL_GL_SwapWindow(window);

		}

	


	}

	// Cleanup
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}