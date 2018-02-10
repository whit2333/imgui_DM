cmake_minimum_required (VERSION 3.5)

# --------------------------------------------------------
#
add_executable(demo 
  src/main.cpp
  )
target_link_libraries(demo ImGui ${SFML_LIBRARIES} ${ROOT_LIBRARIES} ${OPENGL_LIBRARIES})
target_include_directories(demo
  PUBLIC src/3rd_party/imgui
  )

add_executable(demo2
  src/demo.cpp
  src/imgui-SFML.cpp
  )
target_link_libraries(demo2 ImGui ${SFML_LIBRARIES} ${ROOT_LIBRARIES} ${OPENGL_LIBRARIES})
target_include_directories(demo2
  PUBLIC include
  PUBLIC src/3rd_party/imgui
  )

