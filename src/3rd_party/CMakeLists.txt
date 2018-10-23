cmake_minimum_required (VERSION 3.5)

# --------------------------------------------------------
#
set(IMGUI_SOURCES
  imgui/examples/libs/gl3w/GL/gl3w.c
  imgui/imgui.cpp
  imgui/imgui_demo.cpp
  imgui/imgui_draw.cpp
  imgui/imgui_widgets.cpp
  #imgui/examples/imgui_impl_opengl3.cpp
  imgui/examples/imgui_impl_opengl2.cpp
  imgui/examples/imgui_impl_glfw.cpp
  #imgui/examples/opengl2_example/imgui_impl_glfw_gl2.cpp
  #imgui/dock/imgui_dock.cpp  
  #imgui/dock/imgui_widgets.cpp  
  )

set(IMGUI_HEADERS
  imgui/examples/libs/gl3w/GL/gl3w.h
  imgui/examples/libs/gl3w/GL/glcorearb.h
  imgui/imconfig.h
  imgui/imgui.h
  imgui/imgui_internal.h
  imgui/imstb_rectpack.h
  imgui/imstb_textedit.h
  imgui/imstb_truetype.h
  imgui/examples/imgui_impl_opengl3.h
  imgui/examples/imgui_impl_opengl2.h
  imgui/examples/imgui_impl_glfw.h
  #imgui/dock/imgui_dock.h  
  #imgui/dock/imgui_widgets.h
  #imgui/ImWindow/ImwConfig.h       
  #imgui/ImWindow/ImwPlatformWindow.h  
  #imgui/ImWindow/ImwWindow.h           
  #imgui/ImWindow/ImwMenu.h              
  #imgui/ImWindow/ImwToolBar.h    
  #imgui/ImWindow/JsonValue.h
  #imgui/ImWindow/ImwContainer.h    
  #imgui/ImWindow/ImwStatusBar.h       
  #imgui/ImWindow/ImwWindowManager.h
  #imgui/ImWindowGLFW/ImwPlatformWindowGLFW.h
  #imgui/ImWindowGLFW/ImwWindowManagerGLFW.h
  )

add_library(ImGui ${IMGUI_SOURCES} ${IMGUI_HEADERS})
target_link_libraries(ImGui ${OPENGL_LIBRARIES} glfw -lrt -lm -ldl )
target_include_directories(ImGui
  PUBLIC imgui
  PUBLIC imgui/examples/libs/gl3w
  PUBLIC imgui/examples/example_glfw_opengl3
  PUBLIC imgui/examples/example_glfw_opengl2
  #PUBLIC imgui/dock
  #PUBLIC imgui/ImWindow
  #PUBLIC imgui/ImWindowGLFW
  )
target_include_directories(ImGui 
  PUBLIC
  $<BUILD_INTERFACE:${ImGui_BINARY_DIR}/include>
  $<BUILD_INTERFACE:${ImGui_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/glfw/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/glfw/include>
  $<INSTALL_INTERFACE:include>
   
)

# --------------------------------------------------------
# container.cpp  rootcontainer.cpp  widgets.cpp
#add_executable(test_container 
#  tests/container.cpp
#  )
#target_link_libraries(test_container ImGui ${SFML_LIBRARIES} ${ROOT_LIBRARIES} ${OPENGL_LIBRARIES})
#target_include_directories(test_container
#  PUBLIC imgui
#  PUBLIC imgui/gl3w
#  PUBLIC imgui/dock
#  #PUBLIC imgui/ImWindow
#  #PUBLIC imgui/ImWindowGLFW
#  )
