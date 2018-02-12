// ImGui - standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)
// (GL3W is a helper library to access OpenGL functions since there is no standard header to access modern OpenGL functions easily. Alternatives are GLEW, Glad, etc.)

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include <stdio.h>
#include <math.h>
#include <thread>
#include <GL/gl3w.h>// This example is using gl3w to access OpenGL functions (because it is small). You may use glew/glad/glLoadGen/etc. whatever already works for you.
#include <GLFW/glfw3.h>
#include "PVMonitor.h"
#include "PVGetList.h"

int test_getme(const char* pvname);
static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

struct MainMenu {
    bool show_main_menu         = true;
    bool show_demo_window        = true;
    bool show_default_window        = true;
    bool show_another_window     = false;
    bool show_yet_another_window = true;
};

int main(int, char**)
{

  std::vector<std::string> pvs = {
    "whit:circle:angle",
    "whit:circle:period",
    "whit:circle:x",
    "whit:circle:y"
  };
  imguiDM::PVGetList get_list1(pvs);

  bool quit_polling = false;

  std::thread thread_1(
    [&](){
      while(!quit_polling) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        get_list1.Poll();
      }
    });

  //auto monitors_vec = imguiDM::test_pvmonitor2();
  //test_getme("whit:circle:angle");
  //pvac::ClientProvider provider("ca");
  //pvac::ClientChannel channel(provider.connect("whit:circle:angle"));
  //std::cout << channel.name() << " : " << channel.get() << "\n";

    // Setup window
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui OpenGL3 example", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    gl3wInit();

    // Setup ImGui binding
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfwGL3_Init(window, true);
    //io.NavFlags |= ImGuiNavFlags_EnableKeyboard;  // Enable Keyboard Controls
    //io.NavFlags |= ImGuiNavFlags_EnableGamepad;   // Enable Gamepad Controls

    // Setup style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them. 
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple. 
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'misc/fonts/README.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    MainMenu menu;

    menu.show_demo_window = true;
    menu.show_another_window = false;
    menu.show_yet_another_window = true;

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        ImGui_ImplGlfwGL3_NewFrame();

        // 1. Show a simple window.
        // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
        if( menu.show_default_window )
        {
            ImGui::Begin("Default Window", &menu.show_another_window);
            static float f = 0.0f;
            static int counter = 0;
            ImGui::Text("Hello, world!");                           // Display some text (you can use a format string too)
            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            ImGui::Checkbox("Demo Window", &menu.show_demo_window);      // Edit bools storing our windows open/close state
            ImGui::Checkbox("Another Window", &menu.show_another_window);

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        if(menu.show_main_menu)
        {
            ImGui::Begin("Main Menu Window", &menu.show_main_menu);

            ImGui::Checkbox("Demo Window",    &menu.show_demo_window);      // Edit bools storing our windows open/close state
            ImGui::Checkbox("Another Window", &menu.show_another_window);
            ImGui::Checkbox("Yet Another Window", &menu.show_yet_another_window);
            ImGui::Checkbox("default window", &menu.show_default_window);
            //ImGui::SameLine();
            ImGui::End();
        }
        // 2. Show another simple window. In most cases you will use an explicit Begin/End pair to name your windows.
        if (menu.show_another_window)
        {
            ImGui::Begin("Another Window", &menu.show_another_window);
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                menu.show_another_window = false;
            ImGui::End();
        }

        if (menu.show_yet_another_window)
        {
            ImGui::Begin("YET Another Window", &menu.show_another_window);

    //ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.65f);    // 2/3 of the space for widget and 1/3 for labels
    ImGui::PushItemWidth(-140);                                 // Right align, keep 140 pixels for labels
            //static float f = 0.0f;
            //static int counter = 0;
            ImGui::Text("DERP DERP!");                           // Display some text (you can use a format string too)

            static bool animate = true;
            ImGui::Checkbox("Animate", &animate);

            static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
            ImGui::PlotLines("Frame Times", arr, IM_ARRAYSIZE(arr));

            // Create a dummy array of contiguous float values to plot
            // Tip: If your float aren't contiguous but part of a structure, you can pass a pointer to your first float and the sizeof() of your structure in the Stride parameter.
            static float values[90] = { 0 };
            static int values_offset = 0;
            static float refresh_time = 0.0f;
            if (!animate || refresh_time == 0.0f)
                refresh_time = ImGui::GetTime();
            while (refresh_time < ImGui::GetTime()) // Create dummy data at fixed 60 hz rate for the demo
            {
                static float phase = 0.0f;
                values[values_offset] = cosf(phase);
                values_offset = (values_offset+1) % IM_ARRAYSIZE(values);
                phase += 0.10f*values_offset;
                refresh_time += 1.0f/10.0f;
                get_list1.PrintAll();
            }
            
            ImGui::PlotLines("Lines", values, IM_ARRAYSIZE(values), values_offset, "avg 0.0", -1.0f, 1.0f, ImVec2(0,80));
            ImGui::PlotHistogram("Histogram", arr, IM_ARRAYSIZE(arr), 0, NULL, 0.0f, 1.0f, ImVec2(0,80));

            // Use functions to generate output
            // FIXME: This is rather awkward because current plot API only pass in indices. We probably want an API passing floats and user provide sample rate/count.
            struct Funcs
            {
                static float Sin(void*, int i) { return sinf(i * 0.1f); }
                static float Saw(void*, int i) { return (i & 1) ? 1.0f : -1.0f; }
            };
            static int func_type = 0, display_count = 70;
            ImGui::Separator();
            ImGui::PushItemWidth(100); ImGui::Combo("func", &func_type, "Sin\0Saw\0"); ImGui::PopItemWidth();
            ImGui::SameLine();
            ImGui::SliderInt("Sample count", &display_count, 1, 400);
            float (*func)(void*, int) = (func_type == 0) ? Funcs::Sin : Funcs::Saw;
            ImGui::PlotLines("Lines", func, NULL, display_count, 0, NULL, -1.0f, 1.0f, ImVec2(0,80));
            ImGui::PlotHistogram("Histogram", func, NULL, display_count, 0, NULL, -1.0f, 1.0f, ImVec2(0,80));
            ImGui::Separator();
            ImGui::PlotLines(get_list1.GetName(0).c_str(), get_list1.GetBuffer(0).data(), get_list1.GetBuffer(0).size(), 
                             0, "A", -1.0f, 300.0f, ImVec2(0,80));
            ImGui::PlotLines(get_list1.GetName(1).c_str(), get_list1.GetBuffer(1).data(), get_list1.GetBuffer(1).size(), 
                             0, "B", -1.0f, 1.0f, ImVec2(0,80));
            ImGui::PlotLines(get_list1.GetName(2).c_str(), get_list1.GetBuffer(2).data(), get_list1.GetBuffer(2).size(), 
                             0, "C", -1.0f, 1.0f, ImVec2(0,80));
            ImGui::PlotLines(get_list1.GetName(3).c_str(), get_list1.GetBuffer(3).data(), get_list1.GetBuffer(3).size(), 
                             0, "C", -1.0f, 1.0f, ImVec2(0,80));
            ImGui::Separator();

            //ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
            //ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            //ImGui::Checkbox("Demo Window", &menu.show_demo_window);      // Edit bools storing our windows open/close state
            //ImGui::Checkbox("Another Window", &show_another_window);

            //if (ImGui::Button("Button"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
            //    counter++;
            //ImGui::SameLine();
            //ImGui::Text("counter = %d", counter);

            //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }


        // 3. Show the ImGui demo window. Most of the sample code is in ImGui::ShowDemoWindow(). Read its code to learn more about Dear ImGui!
        if (menu.show_demo_window)
        {
            ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
            ImGui::ShowDemoWindow(&menu.show_demo_window);
        }

        // Rendering
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();

  thread_1.join();

    return 0;
}


int test_getme(const char* pvname)
{
//  epics::RefMonitor refmon;
//  double waitTime = 1.0;
//  std::string providerName("ca");
//  typedef std::vector<std::string> pvs_t;
//  pvs_t pvs;
//  pvs.push_back(std::string(pvname));
//
//#ifdef USE_SIGNAL
//  signal(SIGINT, alldone);
//  signal(SIGTERM, alldone);
//  signal(SIGQUIT, alldone);
//#endif
//
//  // build "pvRequest" which asks for all fields
//  pvd::PVStructure::shared_pointer pvReq(pvd::createRequest("field()"));
//
//  // explicitly select configuration from process environment
//  pva::Configuration::shared_pointer conf(pva::ConfigurationBuilder().push_env().build());
//
//  // "pva" provider automatically in registry
//  // add "ca" provider to registry
//  pva::ca::CAClientFactory::start();
//
//  std::cout << "Use provider: " << providerName << "\n";
//  pvac::ClientProvider provider(providerName, conf);
//
//  // need to store references to keep get (and channel) from being closed
//  typedef std::set<Getter::shared_pointer> gets_t;
//  gets_t gets;
//
//  for(const auto& pv : pvs) {
//  //for(pvs_t::const_iterator it=pvs.begin(); it!=pvs.end(); ++it) {
//    //const std::string& pv = *it;
//
//    Getter::shared_pointer get(new Getter(provider, pv));
//    // addConnectListener() always invokes connectEvent() with current state
//
//    gets.insert(get);
//  }
//
//  if(waitTime<0.0)
//    done.wait();
//  else
//    done.wait(waitTime);
//
//  //if(refmon.running()) {
//  //  refmon.stop();
//  //  // drop refs to operations, but keep ref to ClientProvider
//  //  gets.clear();
//  //  // show final counts
//  //  refmon.current();
//  //}

  return 0;
}

