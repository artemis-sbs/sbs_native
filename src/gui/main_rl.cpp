/*******************************************************************************************
*
*   raylib-extras [ImGui] example - Simple Integration
*
*	This is a simple ImGui Integration
*	It is done using C++ but with C style code
*	It can be done in C as well if you use the C ImGui wrapper
*	https://github.com/cimgui/cimgui
*
*   Copyright (c) 2021 Jeffery Myers
*
********************************************************************************************/

#include "raylib.h"
#include "raymath.h"

#include "imgui.h"
#include "rlImGui.h"
#include "star_system.h"

int pixels_per_int = 50;
float max_radius = 50.0/40.0/2.0;
ImVec2 galaxy_offset {0,0};
ImVec2 galaxy_speed {1,1};
ImVec2 selection{-1,-1};
bool show_another_window = true;
    

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


void draw() {
	{
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Solar system");
ImGui::SetWindowFontScale(2.0);
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

      
        float star_date = 1.0f;
		int width=GetScreenWidth();
		int height = GetScreenHeight();
        draw_galaxy(galaxy_offset.x, galaxy_offset.y, 
            width, height, 
            selection.x, selection.y,
            star_date);
        
    }


bool Quit= false;
bool ImGuiDemoOpen = false;

void DoMainMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		ImGui::SetWindowFontScale(2.0);
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
				Quit = true;

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			ImGui::MenuItem("ImGui Demo", nullptr, &show_another_window);
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

int main(int argc, char* argv[])
{
	// Initialization
	//--------------------------------------------------------------------------------------
	int screenWidth = 1920;
	int screenHeight = 1080;

	//SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
	InitWindow(screenWidth, screenHeight, "raylib-Extras [ImGui] example - simple ImGui Demo");
	SetTargetFPS(144);
	rlImGuiSetup(true);
	
	ImGuiIO& io = ImGui::GetIO(); 
	//ImGuiStyle::ScaleAllSizes(2.0);
	ImGui::GetStyle().ScaleAllSizes(2.0);
	//ImGui::SetWindowFontScale(2.0);
	//ImGuiStyle::ScaleAllSizes(2.0f);
	
	// Main game loop
	while (!WindowShouldClose())    // Detect window close button or ESC key
	{

		if (!io.WantCaptureKeyboard) {

			if (IsKeyDown(KEY_RIGHT)) galaxy_offset.x += galaxy_speed.x;
			if (IsKeyDown(KEY_LEFT)) galaxy_offset.x -= galaxy_speed.x;
			if (IsKeyDown(KEY_UP)) galaxy_offset.y -= galaxy_speed.y;
			if (IsKeyDown(KEY_DOWN)) galaxy_offset.y += galaxy_speed.y;
		}

  		if (!io.WantCaptureMouse) {
			auto pos =  GetMousePosition();
			if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
					selection.x = galaxy_offset.x+((pos.x+25)/pixels_per_int);
					selection.y =galaxy_offset.y+((pos.y+25)/pixels_per_int);
				}
		}
	

        
        //------------------------------------------------

		BeginDrawing();
		ClearBackground(BLACK);

		// start ImGui Conent
		rlImGuiBegin();
		
		DoMainMenu();

		// show ImGui Content
		bool open = true;
		//ImGui::ShowDemoWindow(&open);
		//io.DisplaySize = ImVec2(GetScreenWidth(), GetScreenHeight());
        draw();
	  // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::SetWindowFontScale(2.0);
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }
		

        

		// end ImGui Content
		rlImGuiEnd();

		EndDrawing();
		//----------------------------------------------------------------------------------
	}
	rlImGuiShutdown();

	// De-Initialization
	//--------------------------------------------------------------------------------------   
	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}