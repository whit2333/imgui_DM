#ifndef ImGuiDM_HH
#define ImGuiDM_HH

#include "ImGuiDMConfig.h"
#include <map>
#include <array>
#include <vector>
#include <string>
#include <iostream>
#include "imconfig.h"
#include "imgui.h"
#include "imgui_internal.h"

class GLFWwindow;

namespace ImGuiDM {

  struct MainMenu {
    bool show_main_menu          = true;
    bool show_demo_window        = true;
    bool show_default_window     = true;
    bool show_another_window     = false;
    bool show_yet_another_window = true;
  };
  //______________________________________________________________________________

  struct Settings {
    enum class Mode { none, help, copy, list, run };
    enum class CopyMode { none, vis, v0, v1, v2 };
    enum class GenMode  { none, gps, gps1, gps2 };
    Mode         selected = Mode::none; 
    CopyMode     copy_sel = CopyMode::none;
    GenMode      gen_mode = GenMode::none;
    int          run_number        = 0;
    int          number_of_events  = -1;
    std::string  output_file_name  = "";
    std::string  output_tree_name  = "";
    std::string  theRest           = "";
    std::string  macroFile         = "";
    bool         run_manager_init  = false;
    bool         use_gui           = true;
    bool         use_vis           = true;
    bool         is_interactive    = true;
    bool         has_macro_file    = false;
    bool         help              = false;
    bool         is_command        = false;
    bool         is_copy_command   = false;
    bool         use_gps           = false;
    bool         vertex_set        = false;
    std::array<double,3> vertex = {{0, 0, 0}};

    std::map<CopyMode, std::vector<std::string>> copymode_file_names = {
      {CopyMode::vis, { IMGUIDM_MACRO_DIR "/examples/vis.mac",
                        IMGUIDM_MACRO_DIR "/examples/vis2.mac"} },
      {CopyMode::v1,  { IMGUIDM_MACRO_DIR "/examples/vis.mac"} },
      {CopyMode::v2,  { IMGUIDM_MACRO_DIR "/examples/vis2.mac"} }
    };
  };
  //______________________________________________________________________________

  struct GPSConfig {
    enum class EnergyType  { Mono, Lin, Exp, Pow, Gauss, Brem, Bbody, Cdg };
    enum class AngDistType { iso , cos, planar, beam1d, beam2d, focused, user };
    enum class PosDistType { Point, Plane, Beam, Surface, Volume };
    EnergyType   energy_dist = EnergyType ::Mono; 
    AngDistType  ang_dist = AngDistType::iso;
    PosDistType  pos_dist = PosDistType::Point;
    // Sets the angular distribution type () to either isotropic, cosine-law or user-defined.
    double energy = 8.0;
    std::vector<double> energy_params;
    std::vector<double> angle_params;
    std::vector<double> position_params;
  };
  //______________________________________________________________________________

  class Application {
  public:
    Settings    settings;
    MainMenu    menu;
    GPSConfig   gps_conf;
    std::vector<GLFWwindow*> windows;

  public:

    Application();
    ~Application();
    
    GLFWwindow* CreateWindow(int w = 1280, int h = 720);
    void Init(GLFWwindow* window);

    void Render(){ }

    void Quit(){ }

  };
  //______________________________________________________________________________

    // Plot value over time
    // Pass FLT_MAX value to draw without adding a new value
    void	PlotVar(const char* label, float value, float scale_min = FLT_MAX, float scale_max = FLT_MAX, size_t buffer_size = 120);

    // Call this periodically to discard old/unused data
    void	PlotVarFlushOldEntries();


  struct CustomWindow {
  };
  //______________________________________________________________________________

  //void PrintSettings(const Settings& X);

}

#endif

