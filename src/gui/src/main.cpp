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

#include "ImGuiDM.h"
#include "ImGuiDMConfig.h"
#include "clipp.h"
#include "imguivariouscontrols.h"

#include "TH1F.h"

static const char* vertex_shader_text =
"#version 330 core\n"
"layout(location = 0) in vec3 vertexPosition_modelspace;\n"
"void main(){\n"
"    gl_Position.xyz = vertexPosition_modelspace;\n"
"    gl_Position.w = 1.0;\n"
"}\n";
//"uniform mat4 MVP;\n"
//"attribute vec3 vCol;\n"
//"attribute vec2 vPos;\n"
//"varying vec3 color;\n"
//"void main()\n"
//"{\n"
//"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
//"    color = vCol;\n"
//"}\n";
static const char* fragment_shader_text =
"#version 330 core\n"
"out vec4 FragColor;\n"
"void main(){\n"
"  FragColor = vec4(1.0f, 0.5f, 0.2f, 0.4f);"
"}\n";

//GLuint LoadShaders(){
//
//  // Create the shaders
//  GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
//  GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
//
//  GLint Result = GL_FALSE;
//  int InfoLogLength;
//
//  // Compile Vertex Shader
//  //printf("Compiling shader : %s\n", vertex_file_path);
//  //char const * VertexSourcePointer = VertexShaderCode.c_str();
//  glShaderSource(VertexShaderID, 1, &vertex_shader_text , NULL);
//  glCompileShader(VertexShaderID);
//
//  // Check Vertex Shader
//  glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
//  glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
//  if ( InfoLogLength > 0 ){
//    std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
//    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
//    printf("%s\n", &VertexShaderErrorMessage[0]);
//  }
//
//  // Compile Fragment Shader
//  //printf("Compiling shader : %s\n", fragment_file_path);
//  //char const * FragmentSourcePointer = FragmentShaderCode.c_str();
//  glShaderSource(FragmentShaderID, 1, &fragment_shader_text , NULL);
//  glCompileShader(FragmentShaderID);
//
//  // Check Fragment Shader
//  glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
//  glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
//  if ( InfoLogLength > 0 ){
//    std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
//    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
//    printf("%s\n", &FragmentShaderErrorMessage[0]);
//  }
//
//  // Link the program
//  printf("Linking program\n");
//  GLuint ProgramID = glCreateProgram();
//  glAttachShader(ProgramID, VertexShaderID);
//  glAttachShader(ProgramID, FragmentShaderID);
//  glLinkProgram(ProgramID);
//
//  // Check the program
//  glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
//  glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
//  if ( InfoLogLength > 0 ){
//    std::vector<char> ProgramErrorMessage(InfoLogLength+1);
//    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
//    printf("%s\n", &ProgramErrorMessage[0]);
//  }
//
//
//  glDetachShader(ProgramID, VertexShaderID);
//  glDetachShader(ProgramID, FragmentShaderID);
//
//  glDeleteShader(VertexShaderID);
//  glDeleteShader(FragmentShaderID);
//
//  return ProgramID;
//}




template<class C>
void print_help(C cli)
{
  std::cout << make_man_page(cli, "bubble_chamber") << "\n";
}
//______________________________________________________________________________

bool fexists(const std::string& filename) {
   std::ifstream ifile(filename.c_str());
   if( ifile ) return true;
   return false;
}
//______________________________________________________________________________

std::string exec(const char* cmd) {
   std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
   if (!pipe) return "ERROR";
   char buffer[128];
   std::string result = "";
   while (!feof(pipe.get())) {
      if (fgets(buffer, 128, pipe.get()) != NULL)
         result += buffer;
   }
   return result;
}
//______________________________________________________________________________

void copy_files(const std::vector<std::string>& files)
{
  for(const auto& f: files) {
    std::string cmd = std::string("cp ") + f + " .";
    if( fexists(f) ) {
      std::cout << "copying " << f << std::endl;
      exec(cmd.c_str());
    }
  }
}
//______________________________________________________________________________

template<typename T>
void print_usage(T cli, const char* argv0 )
{
  //used default formatting
  std::cout << "Usage:\n" << usage_lines(cli, argv0)
            << "\nOptions:\n" << documentation(cli) << '\n';
}
//______________________________________________________________________________

template<typename T>
void print_man_page(T cli, const char* argv0 ){
  //all formatting options (with their default values)
  auto fmt = clipp::doc_formatting{}
  .start_column(8)                           //column where usage lines and documentation starts
  .doc_column(30)                            //parameter docstring start col
  .indent_size(4)                            //indent of documentation lines for children of a documented group
  .line_spacing(0)                           //number of empty lines after single documentation lines
  .paragraph_spacing(1)                      //number of empty lines before and after paragraphs
  .flag_separator(", ")                      //between flags of the same parameter
  .param_separator(" ")                      //between parameters 
  .group_separator(" ")                      //between groups (in usage)
  .alternative_param_separator("|")          //between alternative flags 
  .alternative_group_separator(" | ")        //between alternative groups 
  .surround_group("(", ")")                  //surround groups with these 
  .surround_alternatives("(", ")")           //surround group of alternatives with these
  .surround_alternative_flags("", "")        //surround alternative flags with these
  .surround_joinable("(", ")")               //surround group of joinable flags with these
  .surround_optional("[", "]")               //surround optional parameters with these
  .surround_repeat("", "...");                //surround repeatable parameters with these
  //.surround_value("<", ">")                  //surround values with these
  //.empty_label("")                           //used if parameter has no flags and no label
  //.max_alternative_flags_in_usage(1)         //max. # of flags per parameter in usage
  //.max_alternative_flags_in_doc(2)           //max. # of flags per parameter in detailed documentation
  //.split_alternatives(true)                  //split usage into several lines for large alternatives
  //.alternatives_min_split_size(3)            //min. # of parameters for separate usage line
  //.merge_alternative_flags_with_common_prefix(false)  //-ab(cdxy|xy) instead of -abcdxy|-abxy
  //.merge_joinable_flags_with_common_prefix(true);    //-abc instead of -a -b -c

  auto mp = make_man_page(cli, argv0, fmt);
  mp.prepend_section("DESCRIPTION", "Bubble chamber simulation");
  mp.append_section("EXAMPLES", " $ bubble_chamber -h ");
  std::cout << mp << "\n";
}
//______________________________________________________________________________

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

  ImGuiDM::ImGuiDMApplication app;
  auto& menu = app.menu;
  auto& S    = app.settings;
  auto& gps_conf = app.gps_conf;

  TH1F *h1 = new TH1F("h1", "h11", 10, -1, 1);
  h1->FillRandom("gaus");
  h1->SetFillColor(2);

  auto cli_basics = (
    ( option("-r", "--run") & value("run_number",S.run_number) )      % "set the run number",
    //( option("-E", "--energy") & number("energy",gps_conf.energy) )      % "set beam energy [MeV]",
    //(option("-v","--vertex").set(S.vertex_set) & value("x", S.vertex[0]) & value("y",  S.vertex[1]) & value("z", S.vertex[2])) % "vertex position [mm]",
    //( option("-m", "--macro") & value("macro",S.macroFile).set(S.has_macro_file,true) )      % "set the run number",
    //option("-b", "--batch")([&](){S.use_gui=false; S.use_vis=false; S.is_interactive=false;}) % "run in batch mode (no gui or vis)",
    //(option("-g", "--gui") & integer("gui")([&](auto v){S.use_gui=v;}))                 % "use GUI ",
    //option("-i", "--interactive")([&](){S.is_interactive=true;}) % "run in interactivemode ",
    //(option("-v", "--vis") & integer("vis")([&](auto v){S.use_vis=v;})) % "use vis ",
    option("-h", "--help").set(S.selected,Mode::help)
    );
  
  auto help_mode = command("help").set(S.selected, Mode::help);
  //auto copy_mode = (command("copy").set(S.selected, Mode::copy) % "Copy example/stock files to the current directory.",
  //                  (command("macro") % "selec macro to copy",
  //                   option("-v","--vis").set(S.copy_sel,CopyMode::vis)  % "all vis macro.",
  //                   option("-v1","--vis1").set(S.copy_sel,CopyMode::v1) % "only vis.mac is copied."
  //                  "This macro visualizes the detector using the qt gui and runs 1000",
  //                   option("-v2","--vis2").set(S.copy_sel,CopyMode::v2) % "vis2 mac only")
  //                 );
  //auto gps_build_energy = (
  //  "Mono energetic GPS distributated like I∝δ(E−E0) with one parameter" % (
  //    command("Mono")                   % "type name"  &
  //    value("E0")                       % "E0 fixed energy parameter" & 
  //    ( option("MeV")|option("GeV") ) % "Units [default:MeV]" )
  //  |
  //  "Gaussian distributed  distribution with two parameters. I=(2πσ)−12exp[−(E/E0)2/σ2] Mean energy E0, std deviation σ" % (
  //    command("Gauss")                  % "type name"  & 
  //    value("E0")                       % "mean energy" & 
  //    ( option("MeV")|option("GeV") ) % "Units [default:MeV]"  &
  //    value("sigma")                    % "sigma: std deviation " & 
  //    ( option("MeV")|option("GeV") ) % "Units [default:MeV]" )
  //  |
  //  "Exp  - exponential    I∝exp(−E/E0) Energy scale-height E0" % (
  //    command("Exp")                    % " typename " & 
  //    value("E0")                       % "E0 fixed energy parameter" & 
  //    ( option("MeV")|option("GeV") ) % "Units [default:MeV]" )
  //  |
  //  "Lin  - linear         I∝I0+m×E Intercept I0 , slope m" % ( 
  //    command("Lin")                    % "type name" & 
  //    value("I0")                       % "I0 intercept" & 
  //    value("m")                        % "slope" & 
  //    ( option("MeV")|option("GeV") ) % "inverse Units [default:MeV]" )
  //  |
  //  "Pow  - power-law      I∝Eα Spectral index α" % (
  //    command("Pow")                    % " typename" &
  //    value("alpha")                    % "alpha parameter" & 
  //    ( option("MeV")|option("GeV") ) % "Units [default:MeV]")
  //  //command("brem")        % "Brem - bremsstrahlung I=∫2E2[h2c2(exp(−E/kT)−1)]−1 Temperature T",
  //  //command("Bbody")       % "Bbody- black body     I∝(kT)12Eexp(−E/kT) Temperature T",
  //  //command("cdg")         % "Cdg  - cosmic diffuse gamma ray I∝[(E/Eb)α1+(E/Eb)α2]−1 Energy range Emin to Emax; Eb and indices α1 and α2 are fixed"
  //  ) ;
  //
  //auto gps_mode = "gps mode:" % (
  //  command("gps") & 
  //  "GPS energy confugration" % (
  //    command("energy") % "Energy Dist Type. See G4 docs for details (https://tinyurl.com/ydgsc5qj)" & gps_build_energy 
  //    )
  //    //|
  //    //command("angle") % "Angular Dist Type " &
  //    //"List of Angular Distributions:" % (command("iso") % "iso"| command("flat") % "flat" )
  //  );
  auto cli = ( //help_mode % "print help this help"
              cli_basics % "typical style commands");
              //|copy_mode % "Copy mode"
              //|gps_mode  

  //assert( cli.flags_are_prefix_free() );
  auto result = parse(argc, argv, cli);

  auto doc_label = [](const parameter& p) {
    if(!p.flags().empty()) return p.flags().front();
    if(!p.label().empty()) return p.label();
    return doc_string{"<?>"};
  };

  cout << "args -> parameter mapping:\n";
  ;
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

  std::cout << " the rest of the arguments: " << S.theRest << std::endl;
  std::cout << "output : " << S.output_file_name << std::endl;
  std::cout << "  tree : " << S.output_tree_name << std::endl;

  std::vector<std::string> pvs = {
    "whit:circle:angle",
    "whit:circle:period",
    "whit:circle:x",
    "whit:circle:y"
  };

  ImGuiDM::PVGetList get_list1(pvs);


  bool quit_polling = false;

  std::thread thread_1(
    [&](){
      while(!quit_polling) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        get_list1.Poll();
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
  glfwSetErrorCallback(error_callback);
  if (!glfwInit())
    return 1;
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  int w = 1280;
  int h = 720;
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
  io.Fonts->AddFontDefault();
  io.Fonts->AddFontFromFileTTF("../share/fonts/Cousine-Regular.ttf", 15.0f);
  io.Fonts->AddFontFromFileTTF("../share/fonts/DroidSans.ttf", 16.0f);
  io.Fonts->AddFontFromFileTTF("../share/fonts/ProggyTiny.ttf", 10.0f);
  io.Fonts->AddFontFromFileTTF("../share/fonts/Roboto-Medium.ttf", 16.0f);
  //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
  //IM_ASSERT(font != NULL);


  menu.show_demo_window = true;
  menu.show_another_window = false;
  menu.show_yet_another_window = true;

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

    // My Game has a different viewport than the editor's one:
    //const int W = 1080 / 3;
    //const int H = 1920 / 3;
    //// We set the same viewport size (plus margin) to the next window (if first use)
    //ImGui::SetNextWindowSize(ImVec2(W + 10, H + 10),
    //                         ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Game rendering");
    {
      auto cur_w = ImGui::GetWindowWidth();
      auto cur_h = ImGui::GetWindowHeight();
      auto wpos = ImGui::GetWindowPos();
      auto wmin = ImGui::GetWindowContentRegionMin();
      auto wmax = ImGui::GetWindowContentRegionMax();
      // Get the current cursor position (where your window is)
      ImVec2 pos = ImGui::GetCursorScreenPos();
      ImGui::GetWindowDrawList()->AddLine(ImVec2(wpos.x+wmin.x, wpos.y+wmin.y), 
                                          ImVec2(wpos.x+wmin.x, wpos.y+wmax.y) ,
                                          IM_COL32(255,0,0,255), 
                                          3.0f );
      ImGui::GetWindowDrawList()->AddLine(ImVec2(wpos.x+wmin.x, wpos.y+wmax.y), 
                                          ImVec2(wpos.x+wmax.x, wpos.y+wmax.y) ,
                                          IM_COL32(255,0,0,255), 
                                          3.0f );
    }
    ImGui::End();



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

      ImGui::PlotLines(get_list1.GetName(0).c_str(), get_list1.GetBuffer(0).data()+get_list1.GetBufferOffset(0), get_list1.GetBufferSize(0), 
                       0, "A", -1.0f, 300.0f, ImVec2(0,80));

      ImGui::PlotLines(get_list1.GetName(1).c_str(), get_list1.GetBuffer(1).data()+get_list1.GetBufferOffset(1), get_list1.GetBufferSize(1),
                       0, "B", -1.0f, 1.0f, ImVec2(0,80));

      ImGui::PlotLines(get_list1.GetName(2).c_str(), get_list1.GetBuffer(2).data()+get_list1.GetBufferOffset(2), get_list1.GetBufferSize(2), 
                       0, "C", -1.0f, 1.0f, ImVec2(0,80));

      ImGui::PlotLines(get_list1.GetName(3).c_str(), get_list1.GetBuffer(3).data()+get_list1.GetBufferOffset(3), get_list1.GetBufferSize(3), 
                       0, "C", -1.0f, 1.0f, ImVec2(0,80));

      ImGui::Separator();
      //std::cout << " angle : " << get_list1.GetValue(0) << std::endl;


      // Call once a frame with current value
      ImGuiDM::PlotVar("Speed", get_list1.GetValue(2));
      ImGuiDM::PlotVar("Speed2", get_list1.GetValue(3));

      // inside a ImGui::Window:
      ImGui::TestProgressBar();
      ImGui::TestPopupMenuSimple();

      //
      std::vector<std::string> names = {get_list1.GetName(0),get_list1.GetName(1),get_list1.GetName(2)};
      std::vector<ImColor> colors =  {ImColor(255,255,0,100), ImColor(0,255,0,100),ImColor(255,0,0,200)};//const ImColor* colors,
      get_list1.GetBufferCopy(0);
      get_list1.GetBufferCopy(1);
      get_list1.GetBufferCopy(2);

      ImGui::PlotMultiHistograms(
        "DERP",
        names,
        colors,
        get_list1.m_buffer_copy,
        -1.0, 1.0, ImVec2(0,80));


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

