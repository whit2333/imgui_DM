// ImGui - standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)
// (GL3W is a helper library to access OpenGL functions since there is no standard header to access modern OpenGL functions easily. Alternatives are GLEW, Glad, etc.)

#include "imgui.h"
#include "imgui_impl_glfw.h"
#define OLDER_OGL 1
#ifdef OLDER_OGL
#include "imgui_impl_opengl2.h"
#else
#include "imgui_impl_opengl3.h"
#endif
#include <stdio.h>
#include <math.h>
#include <thread>
//#include <GL/gl3w.h>// This example is using gl3w to access OpenGL functions (because it is small). You may use glew/glad/glLoadGen/etc. whatever already works for you.
#include <GLFW/glfw3.h>
#include "PVMonitor.h"
#include "PVGetList.h"

#include "ImGuiDM.h"
#include "ImGuiDMConfig.h"
#include "clipp.h"
#include "imguivariouscontrols.h"

//#include "TH1F.h"
#include <iomanip>
#include <sstream>

#include "helpers.h"


static void error_callback(int error, const char* description)
{
  fprintf(stderr, "Error %d: %s\n", error, description);
}
//______________________________________________________________________________

int main(int argc, char** argv)
{
  using namespace clipp;
  using std::cout;
  using CopyMode = ImGuiDM::Settings::CopyMode;
  using Mode     = ImGuiDM::Settings::Mode;

  ImGuiDM::Application app;
  auto& menu     = app.menu;
  auto& S        = app.settings;
  auto& gps_conf = app.gps_conf;

  auto cli_basics = (
    ( option("-r", "--run") & value("run_number",S.run_number) )      % "set the run number",
    option("-h", "--help").set(S.selected,Mode::help)
    );
  
  auto help_mode = command("help").set(S.selected, Mode::help);
  auto cli = ( //help_mode % "print help this help"
              cli_basics % "typical style commands");

  //assert( cli.flags_are_prefix_free() );
  auto result = parse(argc, argv, cli);

  auto doc_label = [](const parameter& p) {
    if(!p.flags().empty()) return p.flags().front();
    if(!p.label().empty()) return p.label();
    return doc_string{"<?>"};
  };

  cout << "args -> parameter mapping:\n";
  for(const auto& m0 : result) {
    std::cout << "#" << m0.index() << " " << m0.arg() << " -> ";
    auto p = m0.param();
    if(p) {
      std::cout << doc_label(*p) << " \t";
      if(m0.repeat() > 0) {
        std::cout << (m0.bad_repeat() ? "[bad repeat " : "[repeat ")
        <<  m0.repeat() << "]";
      }
      if(m0.blocked())  std::cout << " [blocked]";
      if(m0.conflict()) std::cout << " [conflict]";
      std::cout << '\n';
    }
    else {
      std::cout << " [unmapped]\n";
    }
  }
  //________________________________________________________________

  cout << "missing parameters:\n";
  for(const auto& m0 : result.missing()) {
    auto p = m0.param();
    if(p) {
      std::cout << doc_label(*p) << " \t";
      std::cout << " [missing after " << m0.after_index() << "]\n";
    }
  }
  if(!result) {
    print_man_page(cli, argv[0]);
    return 0;
  }
  //________________________________________________________________

  switch(S.selected) {
    case Mode::help : 
      print_man_page<decltype(cli)>(cli,argv[0]);
      return 0;
      break;
    case Mode::copy :
      if(S.copy_sel == CopyMode::vis) {
        copy_files( S.copymode_file_names[S.copy_sel] );
      }
      return 0;
      break;
    case Mode::none : 
      break;
  }
  //---------------------------------------------------------------------------

  // here we assume the last argument is a macro file 
  if( optind < argc ) {
    S.has_macro_file = true;
  }
  for (int i = optind; i < argc; i++) {
    S.theRest        += argv[i];
  }

  // Get the piped commands
  std::vector<std::string> piped_commands;
  if(!isatty(STDIN_FILENO)) {
    std::cout << "Reading piped commands...\n";
    std::string lineInput;
    while(std::getline(std::cin,lineInput)) {
      piped_commands.push_back(lineInput);
    }
  }

  //std::cout << " the rest of the arguments: " << S.theRest << std::endl;
  //std::cout << "output : " << S.output_file_name << std::endl;
  //std::cout << "  tree : " << S.output_tree_name << std::endl;
  //________________________________________________________________

  const char* env_user = std::getenv("USER");
  std::vector<std::string> pvs = {
    "IBC3H00CRCUR4",
    "IPM3C00.XPOS",
    "IPM3C00.YPOS",
    "CPI671SC",
    "CFI6711C"
  };
  for(const auto& pv : pvs) {
    std::cout << pv << "\n";
  }

  ImGuiDM::PVGetList get_list1(pvs);

  //TH1F *h1 = new TH1F("h1", "h11", 100, -1, 1);
  //h1->FillRandom("gaus", 10000);
  //h1->SetFillColor(2);

  bool quit_polling = false;

  std::thread thread_1(
    [&](){
      while(!quit_polling) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        get_list1.Poll();
        if(get_list1.GetN()>=3){
        //h1->Fill(get_list1.GetValue(2));
        }
      }
    });

  //auto monitors_vec = imguiDM::test_pvmonitor2();
  //test_getme("whit:circle:angle");
  //pvac::ClientProvider provider("ca");
  //pvac::ClientChannel channel(provider.connect("whit:circle:angle"));
  //std::cout << channel.name() << " : " << channel.get() << "\n";


  // ---------------------------------------------------------------------------
  // Setup window
  // ---------------------------------------------------------------------------
  GLFWwindow* window = app.CreateWindow(1280, 720);

  app.Init(window);
  //// Setup style
  //ImGui::StyleColorsDark();
  ////ImGui::StyleColorsClassic();

  //// Load Fonts
  //// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them. 
  //// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple. 
  //// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
  //// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
  //// - Read 'misc/fonts/README.txt' for more instructions and details.
  //// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
  ////io.Fonts->AddFontDefault();
  //io.Fonts->AddFontFromFileTTF("../share/fonts/Roboto-Medium.ttf", 16.0f);
  //io.Fonts->AddFontFromFileTTF("../share/fonts/Cousine-Regular.ttf", 15.0f);
  //io.Fonts->AddFontFromFileTTF("../share/fonts/DroidSans.ttf", 16.0f);
  //io.Fonts->AddFontFromFileTTF("../share/fonts/ProggyTiny.ttf", 10.0f);



  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  // ---------------------------------------------------------------------------
  // Main loop
  while (!glfwWindowShouldClose(window))
  {
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    glfwPollEvents();

#ifdef OLDER_OGL
    //ImGui_ImplGlfwGL2_Init(window, true);
    //ImGui_ImplGlfwGL2_NewFrame();
    ImGui_ImplOpenGL2_NewFrame();
#else
    //ImGui_ImplGlfwGL3_Init(window, true);
    //ImGui_ImplGlfwGL3_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
#endif
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    // 1. Show a simple window.
    // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
    if( menu.show_default_window )
    {
      ImGui::Begin("Default Window", &menu.show_another_window);
      static float f = 0.0f;
      static int counter = 0;
      ImGui::Text("Hello, world!");                           // Display some text (you can use a format string too)
      static char str0[200] = "some PV";
      static int i0 = 150;
      ImGui::InputText("input text", str0, IM_ARRAYSIZE(str0));

      //  bool reclaim_focus = false;
      //  if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), ImGuiInputTextFlags_EnterReturnsTrue|ImGuiInputTextFlags_CallbackCompletion|ImGuiInputTextFlags_CallbackHistory, &TextEditCallbackStub, (void*)this))
      //  {
      //      Strtrim(InputBuf);
      //      if (InputBuf[0])
      //          ExecCommand(InputBuf);
      //      strcpy(InputBuf, "");
      //      reclaim_focus = true;
      //  }

        // Demonstrate keeping focus on the input box

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

    // My Game has a different viewport than the editor's one:
    //const int W = 1080 / 3;
    //const int H = 1920 / 3;
    //// We set the same viewport size (plus margin) to the next window (if first use)
    //ImGui::SetNextWindowSize(ImVec2(W + 10, H + 10),
    //                         ImGuiSetCond_FirstUseEver);
    
    //ImGui::Begin("Graph Rendering");
    //{
    //  auto cur_w = ImGui::GetWindowWidth();
    //  auto cur_h = ImGui::GetWindowHeight();
    //  auto wpos  = ImGui::GetWindowPos();
    //  auto wmin  = ImGui::GetWindowContentRegionMin();
    //  auto wmax  = ImGui::GetWindowContentRegionMax();
    //  // Get the current cursor position (where your window is)
    //  //std::cout << " wpos " << wpos.x << ", " << wpos.y  << "\n";
    //  //std::cout << " wmin " << wmin.x << ", " << wmin.y  << "\n";
    //  //std::cout << " wmax " << wmax.x << ", " << wmax.y  << "\n";
    //  float x_margin = 0.05*wmax.x;
    //  float y_margin = 0.05*wmax.y;
    //  float x_plot_range  = wmax.x - wmin.x - 2.0*x_margin;
    //  float y_plot_range  = wmax.y - wmin.y - 2.0*y_margin;

    //  ImVec2 pos = ImGui::GetCursorScreenPos();

    //  ImVec2 origin(wpos.x+wmin.x+x_margin,wpos.y+wmax.y-y_margin);
    //  int nbins = h1->GetNbinsX();
    //  float bin_width = x_plot_range/float(nbins);
    //  float maximum_value = h1->GetMaximum();
    //  float bin_scale = y_plot_range/maximum_value;
    //  // axis
    //  // y axis
    //  ImGui::GetWindowDrawList()->AddLine(ImVec2(origin.x, origin.y), 
    //                                      ImVec2(origin.x, origin.y-y_plot_range),
    //                                      IM_COL32(255,0,0,255), 
    //                                      3.0f );
    //  // x axis
    //  ImGui::GetWindowDrawList()->AddLine(ImVec2(origin.x, origin.y), 
    //                                      ImVec2(origin.x+x_plot_range, origin.y) ,
    //                                      IM_COL32(200,100,0,255), 
    //                                      3.0f );
    //  float x_min = h1->GetBinLowEdge(1);
    //  float x_max = h1->GetBinLowEdge(nbins+1);
    //  float y_min = 0.0;//h1->GetMinimum();
    //  float y_max = h1->GetMaximum();

    //  //std::stringstream stream;
    //  //stream << std::fixed << std::setprecision(2) << x_min;
    //  //std::string xaxis_label1 = stream.str();
    //  //std::stringstream stream2;
    //  //stream2 << std::fixed << std::setprecision(2) << x_max;
    //  //std::string xaxis_label2 = stream2.str();
    //  float x_label_shift = 12.0;

    //  //ImGui::GetWindowDrawList()->AddText(ImVec2(origin.x -x_label_shift, origin.y+y_margin/3), IM_COL32(255,255,255,255), xaxis_label1.c_str());
    //  //ImGui::GetWindowDrawList()->AddText(ImVec2(origin.x+x_plot_range-x_label_shift, origin.y+y_margin/3), IM_COL32(255,255,255,255), xaxis_label2.c_str());

    //  int N_y_label = 10;
    //  float  delta_y = y_plot_range/float(N_y_label-1);
    //  float  delta_yval = (y_max-y_min)/float(N_y_label-1);
    //  float y_label_shift = 5;
    //  float y_label_offset = 34;
    //  for(int iy =0; iy<=N_y_label;iy++){
    //    float y_val = y_min +float(iy)*delta_yval;
    //    std::stringstream stream;
    //    stream << std::fixed << std::setprecision(0) << y_val;
    //    ImGui::GetWindowDrawList()->AddText(ImVec2(origin.x-y_label_offset , origin.y -iy*delta_y-y_label_shift), IM_COL32(255,255,255,255), stream.str().c_str());
    //  }
    //  //AddText(const ImVec2& pos, ImU32 col, const char* text_begin
    //  for(int ib = 1; ib<=nbins+1; ib++ )
    //  {
    //    if((ib-1)%5==0){
    //      //periodically label the bins
    //      std::stringstream stream;
    //      stream << std::fixed << std::setprecision(2) << h1->GetBinLowEdge(ib);
    //      ImGui::GetWindowDrawList()->AddText(ImVec2(origin.x -x_label_shift+ib*bin_width-bin_width/2.0, origin.y+y_margin/3), IM_COL32(255,255,255,255), stream.str().c_str());
    //    }
    //  }
    //  for(int ib = 1; ib<=nbins; ib++ )
    //  {
    //    ImGui::GetWindowDrawList()->AddLine(ImVec2(origin.x + ib*bin_width-bin_width/2.0, origin.y), 
    //                                        ImVec2(origin.x + ib*bin_width-bin_width/2.0, origin.y - bin_scale*h1->GetBinContent(ib)),
    //                                        IM_COL32(0,0,255,255), 
    //                                        2.0f );

    //  }


    //}
    //ImGui::End();



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
        //get_list1.PrintAll();
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


      //std::cout << "buffer(3) size : " << get_list1.GetBufferSize(3) << ", buffer_offset : " << get_list1.GetBufferOffset(3) << "\n";
      //for(int i = 0 ; i<get_list1.GetBufferSize(3); i++){
      //  std::cout << get_list1.GetBuffer(0).at(i) << ", ";
      //}
      //std::cout << "\n";

      if(get_list1.GetN()>=4){
      ImGui::PlotLines(get_list1.GetName(0).c_str(), get_list1.GetBufferCopy(0).data(), get_list1.GetBufferSize(0), 
                       0, "A", -1.0f, 300.0f, ImVec2(0,80));

      ImGui::PlotLines(get_list1.GetName(1).c_str(), get_list1.GetBufferCopy(1).data(), get_list1.GetBufferSize(1),
                       0, "B", -1.0f, 1.0f, ImVec2(0,80));

      ImGui::PlotLines(get_list1.GetName(2).c_str(), get_list1.GetBufferCopy(2).data(), get_list1.GetBufferSize(2), 
                       0, "C", -1.0f, 1.0f, ImVec2(0,80));

      ImGui::PlotLines(get_list1.GetName(3).c_str(), get_list1.GetBufferCopy(3).data(), get_list1.GetBufferSize(3), 
                       0, "C", -1.0f, 1.0f, ImVec2(0,80));

      }
      ImGui::Separator();
      //std::cout << " angle : " << get_list1.GetValue(0) << std::endl;


      // Call once a frame with current value
     
      
      if(get_list1.GetN()>=4){
        ImGuiDM::PlotVar("Speed", get_list1.GetValue(2));
        ImGuiDM::PlotVar("Speed2", get_list1.GetValue(3));
      }

      // inside a ImGui::Window:
      //ImGui::TestProgressBar();
      //ImGui::TestPopupMenuSimple();

      if(get_list1.GetN()>=4){
      //
      std::vector<std::string> names = {get_list1.GetName(0),get_list1.GetName(1),get_list1.GetName(2)};
      std::vector<ImColor> colors =  {ImColor(255,255,0,100), ImColor(0,255,0,100),ImColor(255,0,0,200)};//const ImColor* colors,
      //std::vector<std::vector<float>> buff_copies = {
      //  get_list1.GetBufferCopy(0),
      //  get_list1.GetBufferCopy(1),
      //  get_list1.GetBufferCopy(2)
      //};

      //ImGui::PlotMultiHistograms(
      //  "DERP",
      //  names,
      //  colors,
      //  //get_list1.m_buffer_copy,
      //  buff_copies
      //  -1.0, 1.0, ImVec2(0,80));

      }

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
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    // Rendering
    //int display_w, display_h;
    //glfwGetFramebufferSize(window, &display_w, &display_h);
    //glViewport(0, 0, display_w, display_h);
    //glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    //glClear(GL_COLOR_BUFFER_BIT);
#ifdef OLDER_OGL
    //ImGui_ImplGlfwGL2_RenderDrawData(ImGui::GetDrawData());
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
#else
    //ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
    glfwMakeContextCurrent(window);
    glfwSwapBuffers(window);

  }

  // Cleanup
  quit_polling = true;
#ifdef OLDER_OGL
  //ImGui_ImplGlfwGL2_Shutdown();
  ImGui_ImplOpenGL2_Shutdown();
#else
  //ImGui_ImplGlfwGL3_Shutdown();
  ImGui_ImplOpenGL3_Shutdown();
#endif
  glfwTerminate();

  thread_1.join();

  return 0;
}

