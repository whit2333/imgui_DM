#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>
#include <stdio.h>
//#include <GL/glew.h>
//#include <GL/glfw3.h>
#include <GL/gl3w.h>//This example is using gl3w to access OpenGL functions (because it is small). You may use glew/glad/glLoadGen/etc. whatever already works for you.
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#define IM_ARRAYGroesse(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

static void error_callback(int error, const char* description)
{
  fprintf(stderr, "Error %d: %s\n", error, description);
}

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

GLuint LoadShaders(){

  // Create the shaders
  GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

  GLint Result = GL_FALSE;
  int InfoLogLength;

  // Compile Vertex Shader
  //printf("Compiling shader : %s\n", vertex_file_path);
  //char const * VertexSourcePointer = VertexShaderCode.c_str();
  glShaderSource(VertexShaderID, 1, &vertex_shader_text , NULL);
  glCompileShader(VertexShaderID);

  // Check Vertex Shader
  glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if ( InfoLogLength > 0 ){
    std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    printf("%s\n", &VertexShaderErrorMessage[0]);
  }

  // Compile Fragment Shader
  //printf("Compiling shader : %s\n", fragment_file_path);
  //char const * FragmentSourcePointer = FragmentShaderCode.c_str();
  glShaderSource(FragmentShaderID, 1, &fragment_shader_text , NULL);
  glCompileShader(FragmentShaderID);

  // Check Fragment Shader
  glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if ( InfoLogLength > 0 ){
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    printf("%s\n", &FragmentShaderErrorMessage[0]);
  }

  // Link the program
  printf("Linking program\n");
  GLuint ProgramID = glCreateProgram();
  glAttachShader(ProgramID, VertexShaderID);
  glAttachShader(ProgramID, FragmentShaderID);
  glLinkProgram(ProgramID);

  // Check the program
  glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
  glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if ( InfoLogLength > 0 ){
    std::vector<char> ProgramErrorMessage(InfoLogLength+1);
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    printf("%s\n", &ProgramErrorMessage[0]);
  }


  glDetachShader(ProgramID, VertexShaderID);
  glDetachShader(ProgramID, FragmentShaderID);

  glDeleteShader(VertexShaderID);
  glDeleteShader(FragmentShaderID);

  return ProgramID;
}

static void draw_callback(const ImDrawList* parent_list, const ImDrawCmd* cmd)
{
  //printf("hello!\n");
  static GLuint programID = LoadShaders();
  glViewport(0, 0, 300, 300);

  glUseProgram(programID);

  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  static const GLfloat g_vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
  };

  GLuint vertexbuffer;
  glGenBuffers(1, &vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glVertexAttribPointer(
    0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void *) 0            // array buffer offset
    );
  glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
  glDisableVertexAttribArray(0);
}

void Chart(GLFWwindow *window)
{
  // An array of 3 vectors which represents 3 vertices
  static const GLfloat g_vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f,  1.0f, 0.0f,
  };
  GLfloat lineVertices[] =
  {
    100, 300, 0,
    800, 500, 0
  };

  GLfloat colour[] =
  {
    255,0,0,
    0,255,0
  };

  // Render OpenGL here
  //glEnable(GL_LINE_SMOOTH);
  //glEnable(GL_LINE_STIPPLE);
  //glPushAttrib(GL_LINE_BIT);
  //glLineWidth(10);
  //glLineStipple(1, 0x00FF);
  //glEnableClientState(GL_VERTEX_ARRAY);
  //glEnableClientState(GL_COLOR_ARRAY);
  //glVertexPointer(3, GL_FLOAT, 0, lineVertices);
  //glColorPointer(3, GL_FLOAT, 0, colour);
  //glDrawArrays(GL_LINES, 0, 2);
  //glDisableClientState(GL_VERTEX_ARRAY);
  //glDisableClientState(GL_COLOR_ARRAY);
  //glPopAttrib();
  //glDisable(GL_LINE_STIPPLE);
  //glDisable(GL_LINE_SMOOTH);
  // This will identify our vertex buffer
  GLuint vertexbuffer;
  // Generate 1 buffer, put the resulting identifier in vertexbuffer
  glGenBuffers(1, &vertexbuffer);
  // The following commands will talk about our 'vertexbuffer' buffer
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  // Give our vertices to OpenGL.
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
  // 1rst attribute buffer : vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glVertexAttribPointer(
    0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void*)0            // array buffer offset
    );
  // Draw the triangle !
  glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
  glDisableVertexAttribArray(0);
}


int main(int, char**)
{
  // Setup window
  glfwSetErrorCallback(error_callback);

  if (!glfwInit())
    return 1;

  //glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  //GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello World", NULL, NULL);

  GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui OpenGL3 example", NULL, NULL);
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable vsync
  //glewInit();
  gl3wInit();

  // Setup ImGui binding
  //ImGuiIO& io = ImGui::GetIO(); (void)io;
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  ImGui_ImplGlfwGL3_Init(window, true);
  //io.NavFlags |= ImGuiNavFlags_EnableKeyboard;  // Enable Keyboard Controls
  //io.NavFlags |= ImGuiNavFlags_EnableGamepad;   // Enable Gamepad Controls

  // Setup style
  //ImGui::StyleColorsDark();
  ImGui::StyleColorsClassic();
  //ImGui_ImplGlfwGL3_Init(window, true);
  ImVec4 clear_color = ImColor(24, 24, 24);

  ////glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); // specifies the part of the window to which OpenGL will draw (in pixels), convert from normalised to pixels
  //glMatrixMode(GL_PROJECTION); // projection matrix defines the properties of the camera that views the objects in the world coordinate frame. Here you typically set the zoom factor, aspect ratio and the near and far clipping planes
  //glLoadIdentity(); // replace the current matrix with the identity matrix and starts us a fresh because matrix transforms such as glOrpho and glRotate cumulate, basically puts us at (0, 0, 0)
  //glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0, 1); // essentially set coordinate system
  //glMatrixMode(GL_MODELVIEW); // (default matrix mode) modelview matrix defines how your objects are transformed (meaning translation, rotation and scaling) in your world
  //glLoadIdentity(); // same as above comment

  bool show_another_window = true;
  std::cout << "derp \n";
  // Main loop --------------------------------------------------------------------------------------------------------------------------
  //
  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();
    ImGui_ImplGlfwGL3_NewFrame();

    //Fenster --------------------------------------

    //ImGuiWindowFlags Fenster_flags = 0;
    //Fenster_flags |= ImGuiWindowFlags_NoTitleBar;
    //Fenster_flags |= ImGuiWindowFlags_NoResize;;
    //Fenster_flags |= ImGuiWindowFlags_NoMove;
    //Fenster_flags |= ImGuiWindowFlags_NoCollapse;

    //int w, h;
    //glfwGetWindowSize(window, &w, &h);
    //{
    //  ImGui::Begin("Window");//, Fenster_flags);
    //  //ImGui::SetWindowPos(ImVec2(0, 0));
    //  //ImGui::SetWindowSize(ImVec2(250, h), 0);

    //  ImGui::Text("Guten Tag!");
    //  //if (ImGui::CollapsingHeader("Header"))
    //  //{
    //  //  static bool test1 = true;
    //  //  ImGui::Checkbox("Test", &test1);
    //  //  static bool test2 = true;
    //  //  ImGui::Checkbox("Test2", &test2);
    //  //  ImGui::Checkbox("show window", &show_another_window);
    //  //  static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
    //  //  ImGui::PlotLines("Plot", arr, IM_ARRAYGroesse(arr),0,"Test");
    //  //  ImGui::GetWindowDrawList()->AddCallback(draw_callback, NULL);
    //  //}
    //  ImGui::End();
    //}

    if (show_another_window) {
      ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
      ImGui::Begin("Another Window", &show_another_window);

      ImGui::GetWindowDrawList()->AddCallback(draw_callback, NULL);

      ImGui::End();
    }

    {
      // My Game has a different viewport than the editor's one:
      const int W = 1080 / 3;
      const int H = 1920 / 3;

      //window.Clear(0.5f, 0.5f, 0.5f,true,true);

      //switch to our custom FBO
      //window.bindFBO is the same as writing glBindFramebuffer(GL_FRAMEBUFFER,FBO);
      //window.bindFBO();

      ////we need to call clear twice
      ////window.Clear(1.0f, 1.0f, 1.0f, true, false);
      ////Draw our simple triangle
      //shader.Use();
      //glBindVertexArray(Tvao);
      //glDrawArrays(GL_TRIANGLES, 0, 3);

      ////switch to the default FBO
      ////again, unbindFBO() is the same as glBindFramebuffer(GL_FRAMEBUFFER,0)
      //window.unbindFBO();

      // We set the same viewport size (plus margin) to the next window (if first use)
      ImGui::SetNextWindowSize(ImVec2(W + 10, H + 10),
                               ImGuiSetCond_FirstUseEver);
      // Get the current cursor position (where your window is)
      ImVec2 pos = ImGui::GetCursorScreenPos();

      //create our ImGui window
      ImGui::Begin("Scene Window");

      //pass the texture of the FBO
      //window.getRenderTexture() is the texture of the FBO
      //the next parameter is the upper left corner for the uvs to be applied at
      //the third parameter is the lower right corner
      //the last two parameters are the UVs
      //they have to be flipped (normally they would be (0,0);(1,1) 
      //ImGui::GetWindowDrawList()->AddImage(
      //  (void *)window.getRenderTexture(), 
      //  ImVec2(ImGui::GetCursorScreenPos()),
      //  ImVec2(ImGui::GetCursorScreenPos().x + window.getWidth()/2, 
      //         ImGui::GetCursorScreenPos().y + window.getHeight()/2), ImVec2(0, 1), ImVec2(1, 0));

      // A boolean to allow me to stop the game rendering
      ////if (runApp) {
      //glViewport(0, 0, W, H);
      //// Render the scene into an FBO
      ////game->render(time);
      //// Restore previous viewport
      //glViewport(0, 0, w, h);
      ////}
      // Get the texture associated to the FBO
      //auto tex = game->getRendered();

      // Ask ImGui to draw it as an image:
      // Under OpenGL the ImGUI image type is GLuint
      // So make sure to use "(void *)tex" but not "&tex"
      //ImGui::GetWindowDrawList()->AddImage(
      //  (void *)tex, ImVec2(ImGui::GetItemRectMin().x + pos.x,
      //                      ImGui::GetItemRectMin().y + pos.y),
      //  ImVec2(pos.x + h / 2, pos.y + w / 2), ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();
    }


    // Rendering
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui::Render();
    //Chart(window);


    glfwSwapBuffers(window);
  }

  // Cleanup
  ImGui_ImplGlfwGL3_Shutdown();
  glfwTerminate();
  return 0;
}

