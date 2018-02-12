cmake_minimum_required (VERSION 3.5)

# --------------------------------------------------------
#
add_executable(demo 
  src/main.cpp
  )
target_link_libraries(demo 
  ImGui 
  epics 
  ${EPICS_LIBRARIES}
  ${SFML_LIBRARIES} 
  ${ROOT_LIBRARIES} 
  ${OPENGL_LIBRARIES}
  )
target_include_directories(demo
  PUBLIC include
  PUBLIC src/3rd_party/imgui
  PUBLIC ${EPICS_INCLUDE_DIRS}
  )

# --------------------------------------------------------
#
add_executable(demo2
  src/demo.cpp
  src/imgui-SFML.cpp
  )
target_link_libraries(demo2 
  ImGui 
  ${SFML_LIBRARIES} 
  ${ROOT_LIBRARIES} 
  ${OPENGL_LIBRARIES}
  )
target_include_directories(demo2
  PUBLIC include
  PUBLIC src/3rd_party/imgui
  )


# ----------------------------------------------
# Tests

# test_line
set(test_name test_line)
add_executable(${test_name} tests/${test_name}.cpp)
target_include_directories(${test_name} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
  PUBLIC src/3rd_party/imgui
  )
target_link_libraries(${test_name} 
  PRIVATE ImGui  ${SFML_LIBRARIES}  ${ROOT_LIBRARIES} ${OPENGL_LIBRARIES} -lGLEW -lGLU -lGL )
target_link_libraries(${test_name}
  PRIVATE Catch)
add_test(NAME ${test_name} COMMAND ${test_name})

