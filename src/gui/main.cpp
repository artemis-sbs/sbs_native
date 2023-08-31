// Dear ImGui: standalone example application for SDL2 + SDL_Renderer
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// Important to understand: SDL_Renderer is an _optional_ component of SDL2.
// For a multi-platform app consider using e.g. SDL+DirectX on Windows and SDL+OpenGL on Linux/OSX.
#define SDL_MAIN_HANDLED
#include <imgui.h>
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"
#include <stdio.h>
#include <SDL.h>

#include "star_system.h"

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

int pixels_per_int = 50;
float max_radius = 50.0/40.0/2.0;

void draw_galaxy(int gx, int gy, 
                int width, int height, 
                int sel_x, int sel_y,
                float star_date) {
    // Layer is Galaxy, Star, Planet
    auto drawlist = ImGui::GetBackgroundDrawList();
    for(int y=0;y<height/pixels_per_int; y+= 1) {
        for(int x = 0; x< width/pixels_per_int; x+=1) {
            StarSystem system(gx+x,gy+y, false);
            if (system.exists()) {
                ImColor color = ImColor::HSV(system.star_color()/8.0,1.0f, 1.0f);
	            drawlist->AddCircleFilled(ImVec2(x*pixels_per_int,y*pixels_per_int), 
                    system.star_radius()*max_radius, color, 32);
                if (sel_x == gx+x && sel_y == gy+y) {
                    drawlist->AddCircle(ImVec2(x*pixels_per_int,y*pixels_per_int), 
                        50, IM_COL32(255,255,0,255), 32);
                }
	            //drawlist->AddText(ImVec2(128,128+70), IM_COL32(255,0,0,255), "Hello", NULL);
            }
        }
        
    }
    
}
// Main code
int main(int, char**)
{
    ImVec2 galaxy_offset {0,0};
    ImVec2 galaxy_speed {1,1};
    ImVec2 selection{-1,-1};
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    // Create window with SDL_Renderer graphics context
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("Dear ImGui SDL2+SDL_Renderer example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        SDL_Log("Error creating SDL_Renderer!");
        return 0;
    }
    //SDL_RendererInfo info;
    //SDL_GetRendererInfo(renderer, &info);
    //SDL_Log("Current SDL_Renderer: %s", info.name);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    //ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);

            
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;

            if (!io.WantCaptureKeyboard && event.type == SDL_KEYUP) {
                 switch( event.key.keysym.sym )
                        {
                            case SDLK_UP:
                                galaxy_offset.y -= galaxy_speed.y;
                            break;

                            case SDLK_DOWN:
                                galaxy_offset.y += galaxy_speed.y;
                            break;

                            case SDLK_LEFT:
                                galaxy_offset.x -= galaxy_speed.x;
                            break;

                            case SDLK_RIGHT:
                                galaxy_offset.x += galaxy_speed.x;
                            break;
                        }
            }

            if (!io.WantCaptureMouse) {
                if (event.type == SDL_MOUSEBUTTONDOWN && 
                    event.button.button == SDL_BUTTON_LEFT) {
                        int x,y;
                        SDL_GetMouseState(&x,&y);
                        selection.x = galaxy_offset.x+((x+25)/pixels_per_int);
                        selection.y =galaxy_offset.y+((y+25)/pixels_per_int);
                    }
            }

        }
        int width=0, height = 0;
        SDL_GetWindowSize(window,&width,&height);
        
        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        // if (show_demo_window)
        //     ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Solar system"); 

            StarSystem system(selection.x,selection.y, true);
            ImVec2 canvas_pos_min = ImGui::GetItemRectMin();
            ImVec2 canvas_pos = ImGui::GetItemRectMax();
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            for(int p=0; p< system.planets.size();p++) {
                auto planet = system.planets[p];
                ImColor color = ImColor::HSV((planet.temperature+200)/28.0,1.0f, 1.0f);
                //ImColor color = ImColor::HSV(1.0f,1.0f, 1.0f);
                int x = canvas_pos_min.x +  p*70+ 35;
                int y = canvas_pos.y + 35;
                draw_list->AddCircleFilled(ImVec2(x,y), planet.radius, color,32);
                y+=35;
                //draw_list->AddCircleFilled(ImVec2(x,y), 50, color,32);
                for (int s=0;s<planet.stations.size();s++) {
                    y += 20;
                    auto station = planet.stations[s];
                    ImColor color = ImColor::HSV(station.type/5.0f,1.0f, 1.0f);
                    draw_list->AddCircleFilled(ImVec2(x,y), 10, color,6);
                }
                for (int s=0;s<planet.moons.size();s++) {
                    y += 30;
                    auto moon = planet.moons[s];
                    ImColor color = ImColor::HSV((moon.gases),1.0f, 1.0f);
                    draw_list->AddCircleFilled(ImVec2(x,y), moon.radius*5, color,24);
                }
            }

            
            ImGui::InvisibleButton("canvas", ImVec2(1000,400));

           // ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

           /// ImGui::Checkbox("Another Window", &show_another_window);

            // ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            // ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            // if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            //     counter++;
            // ImGui::SameLine();
            // ImGui::Text("counter = %d", counter);

            // ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }
        float star_date = 1.0f;
        draw_galaxy(galaxy_offset.x, galaxy_offset.y, 
            width, height, 
            selection.x, selection.y,
            star_date);
        
        
        
		

        // Rendering
        ImGui::Render();
        SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
