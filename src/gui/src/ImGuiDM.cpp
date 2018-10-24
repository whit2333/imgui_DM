#include "ImGuiDM.h"
#include <map>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#define OLDER_OGL 1
#ifdef OLDER_OGL
#include "imgui_impl_opengl2.h"
#else
#include "imgui_impl_opengl3.h"
#endif
#include <GL/gl3w.h>// This example is using gl3w to access OpenGL functions (because it is small). You may use glew/glad/glLoadGen/etc. whatever already works for you.
#include <GLFW/glfw3.h>
#include "ImGuiDMConfig.h"

static void error_callback(int error, const char* description)
{
  fprintf(stderr, "Error %d: %s\n", error, description);
}
//______________________________________________________________________________

namespace ImGuiDM {

  Application::Application()
  {
  }
  //______________________________________________________________________________
    
  Application::~Application(){
    ImGui::DestroyContext();
  }

  GLFWwindow* Application::CreateWindow(int w, int h)
  {
    // ---------------------------------------------------------------------------
    // Setup window
    // ---------------------------------------------------------------------------
    glfwSetErrorCallback(error_callback);
    // Setup window
    if (!glfwInit())
        return nullptr;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL2 example", NULL, NULL);
    if (window == nullptr)
        return nullptr;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    //glfwSetErrorCallback(glfw_error_callback);
    //if (!glfwInit())
    //  return nullptr;
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    ////glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui OpenGL3 example", NULL, NULL);
    windows.push_back(window);
    return window;
  }
  //______________________________________________________________________________

  void Application::Init(GLFWwindow* window)
  {
    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
    ImGui_ImplGlfw_InitForOpenGL(window, true);

#ifdef OLDER_OGL
    ImGui_ImplOpenGL2_Init();//window, true);
#else
    ImGui_ImplOpenGL3_Init();//window, true);
#endif
    //io.NavFlags |= ImGuiNavFlags_EnableKeyboard;  // Enable Keyboard Controls
    //io.NavFlags |= ImGuiNavFlags_EnableGamepad;   // Enable Gamepad Controls

    // Setup style
    ImGui::StyleColorsLight();
    //ImGui::StyleColorsClassic();
    //ImGui::StyleColorsDark();

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them. 
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple. 
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'misc/fonts/README.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    io.Fonts->AddFontFromFileTTF(IMGUIDM_FONT_DIR "/Roboto-Medium.ttf", 16.0f);
    io.Fonts->AddFontFromFileTTF(IMGUIDM_FONT_DIR "/Cousine-Regular.ttf", 15.0f);
    io.Fonts->AddFontFromFileTTF(IMGUIDM_FONT_DIR "/DroidSans.ttf", 16.0f);
    io.Fonts->AddFontFromFileTTF(IMGUIDM_FONT_DIR "/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    menu.show_demo_window = true;
    menu.show_another_window = false;
    menu.show_yet_another_window = true;
  }

  struct PlotVarData
  {
    ImGuiID          ID;
    ImVector<float>  Data;
    int              DataInsertIdx;
    int              LastFrame;
    PlotVarData() : ID(0), DataInsertIdx(0), LastFrame(-1) {}
  };
  typedef std::map<ImGuiID, PlotVarData> PlotVarsMap;

  static PlotVarsMap	g_PlotVarsMap;

  // Plot value over time
  // Call with 'value == FLT_MAX' to draw without adding new value to the buffer
  void PlotVar(const char* label, float value, float scale_min, float scale_max, size_t buffer_size)
  {
    using namespace ImGui;
    assert(label);
    if (buffer_size == 0)
      buffer_size = 620;

    ImGui::PushID(label);
    ImGuiID id = ImGui::GetID("");

    // Lookup O(log N)
    PlotVarData& pvd = g_PlotVarsMap[id];

    // Setup
    if (pvd.Data.capacity() != buffer_size)
    {
      pvd.Data.resize(buffer_size);
      memset(&pvd.Data[0], 0, sizeof(float) * buffer_size);
      pvd.DataInsertIdx = 0;
      pvd.LastFrame = -1;
    }

    // Insert (avoid unnecessary modulo operator)
    if (pvd.DataInsertIdx == buffer_size)
      pvd.DataInsertIdx = 0;
    int display_idx = pvd.DataInsertIdx;
    if (value != FLT_MAX)
      pvd.Data[pvd.DataInsertIdx++] = value;

    // Draw
    int current_frame = ImGui::GetFrameCount();
    if (pvd.LastFrame != current_frame)
    {
      ImGui::PlotLines("##plot", &pvd.Data[0], buffer_size, pvd.DataInsertIdx, NULL, scale_min, scale_max, ImVec2(0, 40));
      ImGui::SameLine();
      ImGui::Text("%s\n%-3.4f", label, pvd.Data[display_idx]);	// Display last value in buffer
      pvd.LastFrame = current_frame;
    }

    ImGui::PopID();
  }
  //______________________________________________________________________________

  void PlotVarFlushOldEntries()
  {
    using namespace ImGui;
    int current_frame = ImGui::GetFrameCount();
    for (PlotVarsMap::iterator it = g_PlotVarsMap.begin(); it != g_PlotVarsMap.end(); )
    {
      PlotVarData& pvd = it->second;
      if (pvd.LastFrame < current_frame - std::max(400,(int)pvd.Data.size()))
        it = g_PlotVarsMap.erase(it);
      else
        ++it;
    }
  }
  //______________________________________________________________________________


}

