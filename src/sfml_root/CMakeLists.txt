cmake_minimum_required (VERSION 3.5)

# --------------------------------------------------------
# Define sources and executable
set(EXECUTABLE_NAME "sfml_root_test")
add_executable(${EXECUTABLE_NAME} src/main.cpp)
target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES} ${ROOT_LIBRARIES} ${OPENGL_LIBRARIES})
install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)

# --------------------------------------------------------
# Define sources and executable
set(EXECUTABLE_NAME "sfml_root_test2")
add_executable(${EXECUTABLE_NAME} src/test.cpp)
target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES} ${ROOT_LIBRARIES} ${OPENGL_LIBRARIES})
install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)



