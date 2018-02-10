cmake_minimum_required (VERSION 3.5)

# --------------------------------------------------------
#
set(IMGUI_SOURCES
  imgui/gl3w/GL/gl3w.c
  imgui/imgui.cpp
  imgui/imgui_demo.cpp
  imgui/imgui_draw.cpp
  imgui/imgui_impl_glfw_gl3.cpp
  imgui/dock/imgui_dock.cpp  
  imgui/dock/imgui_widgets.cpp  

  #imgui/ImWindowGLFW/ImwPlatformWindowGLFW.cpp
  #imgui/ImWindowGLFW/ImwWindowManagerGLFW.cpp
  #imgui/ImWindow/ImwMenu.cpp            
  #imgui/ImWindow/ImwToolBar.cpp  
  #imgui/ImWindow/JsonValue.cpp
  #imgui/ImWindow/ImwContainer.cpp  
  #imgui/ImWindow/ImwStatusBar.cpp     
  #imgui/ImWindow/ImwWindowManager.cpp  
  #imgui/ImWindow/ImwPlatformWindow.cpp  
  #imgui/ImWindow/ImwWindow.cpp   
  )

set(IMGUI_HEADERS
  imgui/gl3w/GL/gl3w.h
  imgui/gl3w/GL/glcorearb.h
  imgui/imconfig.h
  imgui/imgui.h
  imgui/imgui_internal.h
  imgui/stb_rect_pack.h
  imgui/stb_textedit.h
  imgui/stb_truetype.h
  imgui/imgui_impl_glfw_gl3.h
  imgui/dock/imgui_dock.h  
  imgui/dock/imgui_widgets.h
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
target_link_libraries(ImGui ${OPENGL_LIBRARIES} -lglfw -lrt -lm -ldl )
target_include_directories(ImGui
  PUBLIC imgui
  PUBLIC imgui/gl3w
  PUBLIC imgui/dock
  #PUBLIC imgui/ImWindow
  #PUBLIC imgui/ImWindowGLFW
  )
target_include_directories(ImGui PUBLIC
  $<BUILD_INTERFACE:${ImGui_BINARY_DIR}/include>
  $<BUILD_INTERFACE:${ImGui_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# --------------------------------------------------------
# container.cpp  rootcontainer.cpp  widgets.cpp
add_executable(test_container 
  tests/container.cpp
  )
target_link_libraries(test_container ImGui ${SFML_LIBRARIES} ${ROOT_LIBRARIES} ${OPENGL_LIBRARIES})
target_include_directories(test_container
  PUBLIC imgui
  PUBLIC imgui/gl3w
  PUBLIC imgui/dock
  #PUBLIC imgui/ImWindow
  #PUBLIC imgui/ImWindowGLFW
  )
