cmake_minimum_required (VERSION 3.5)

# --------------------------------------------------------
# Define sources and executable
set(EXECUTABLE_NAME "quad")
add_executable(${EXECUTABLE_NAME} src/${EXECUTABLE_NAME}.cpp)
target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES} ${ROOT_LIBRARIES} 
  ${OPENGL_LIBRARIES} -lX11)
install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)


# --------------------------------------------------------
# Define sources and executable
set(EXECUTABLE_NAME "gl3_test")
add_executable(${EXECUTABLE_NAME} src/${EXECUTABLE_NAME}.cpp)
target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES} ${ROOT_LIBRARIES} 
  ${OPENGL_LIBRARIES} -lX11)
install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)





