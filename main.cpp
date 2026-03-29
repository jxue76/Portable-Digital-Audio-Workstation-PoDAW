#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

// UI State Machine
enum AppState { SEQUENCER, SETTINGS };
AppState current_state = SETTINGS;

// App Variables
int selected_menu = 2; // Default to "Options"
int current_track = 0;
int tempo = 180;

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main(int, char**) {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create a window sized for a standard embedded display (e.g., Raspberry Pi touchscreen)
    GLFWwindow* window = glfwCreateWindow(480, 320, "PoDAW Interface", nullptr, nullptr);
    if (window == nullptr) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    // Enable Keyboard/Gamepad Navigation
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      

    // Setup ImGui style to match your mockup (Dark/Gray/Blue)
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f; // Blocky, hardware feel
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f); // Gray background
    style.Colors[ImGuiCol_Header] = ImVec4(0.4f, 0.6f, 0.6f, 1.0f);   // Muted teal for selection

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Check for our "Back/Menu" button toggle (Escape key for testing)
        if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
            current_state = (current_state == SEQUENCER) ? SETTINGS : SEQUENCER;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Lock the main window to the display size, remove title bar and borders
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(480, 320));
        ImGui::Begin("MainApp", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);

        if (current_state == SETTINGS) {
            // --- SETTINGS PAGE MOCKUP ---
            // Left Menu Pane (30% width)
            ImGui::BeginChild("LeftMenu", ImVec2(140, 280), ImGuiChildFlags_Borders | ImGuiChildFlags_NavFlattened);
            if (ImGui::Selectable("Playback", selected_menu == 0)) selected_menu = 0;
            if (ImGui::Selectable("Recording", selected_menu == 1)) selected_menu = 1;
            if (ImGui::Selectable("Options", selected_menu == 2)) selected_menu = 2;
            if (ImGui::Selectable("misc.", selected_menu == 3)) selected_menu = 3;
            ImGui::EndChild();

            ImGui::SameLine();

            // Right Content Pane
            ImGui::BeginChild("RightContent", ImVec2(0, 280), ImGuiChildFlags_Borders);
            if (selected_menu == 2) {
                // Layout for Options
                ImGui::SetCursorPosY(50); // Push down a bit
                ImGui::SetCursorPosX(50);
                ImGui::Text("Tempo:");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(100);
                // InputInt automatically maps to left/right or up/down when highlighted
                ImGui::InputInt("##tempo", &tempo, 1, 10); 
                
                ImGui::SetCursorPosY(120);
                ImGui::SetCursorPosX(50);
                ImGui::Text("Track:       1");

                ImGui::SetCursorPosY(180);
                ImGui::SetCursorPosX(50);
                ImGui::Text("Time Sig:  4/4");
            }
            ImGui::EndChild();
        } 
        else if (current_state == SEQUENCER) {
            // --- SEQUENCER PAGE MOCKUP ---
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            ImVec2 p = ImGui::GetCursorScreenPos();
            
            // Draw Left Track Header (Blue)
            draw_list->AddRectFilled(p, ImVec2(p.x + 80, p.y + 280), IM_COL32(70, 130, 180, 255));
            ImGui::SetCursorPos(ImVec2(10, 10));
            ImGui::Text("1\nPNO");

            // Draw generic notes on the timeline for visual testing
            draw_list->AddRectFilled(ImVec2(p.x + 100, p.y + 150), ImVec2(p.x + 130, p.y + 165), IM_COL32(100, 150, 250, 255));
            draw_list->AddRectFilled(ImVec2(p.x + 130, p.y + 180), ImVec2(p.x + 190, p.y + 195), IM_COL32(100, 150, 250, 255));
            draw_list->AddRectFilled(ImVec2(p.x + 280, p.y + 120), ImVec2(p.x + 400, p.y + 135), IM_COL32(100, 150, 250, 255));
            draw_list->AddRectFilled(ImVec2(p.x + 280, p.y + 150), ImVec2(p.x + 400, p.y + 165), IM_COL32(100, 150, 250, 255));
            draw_list->AddRectFilled(ImVec2(p.x + 280, p.y + 180), ImVec2(p.x + 400, p.y + 195), IM_COL32(100, 150, 250, 255));
        }

        // Draw Bottom Transport Bar (Yellow/Brownish)
        ImVec2 win_pos = ImGui::GetWindowPos();
        ImGui::GetWindowDrawList()->AddRectFilled(
            ImVec2(win_pos.x, win_pos.y + 280), 
            ImVec2(win_pos.x + 480, win_pos.y + 320), 
            IM_COL32(100, 80, 30, 255)
        );

        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}