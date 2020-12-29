cmake_minimum_required(VERSION 3.10)

SET(GCC_COVERAGE_COMPILE_FLAGS "-Wall -g -O3")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

# set(CMAKE_C_COMPILER clang)
# set(CMAKE_CXX_COMPILER clang++)

project(GAM)

set(OpenGL_GL_PREFERENCE "GLVND")
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS OpenGL REQUIRED)
find_package(GLU REQUIRED)

# https://stackoverflow.com/questions/13703647/how-to-properly-add-include-directories-with-cmake
include_directories(
  include
)

set(INCLUDES
  include/mainwindow.h 
  include/gldisplaywidget.h 
  include/mesh.h
  include/algorithm.h
  include/point.h
  include/topography.h
  include/color.h
)

set(SOURCES
  src/main.cpp
  src/mainwindow.cpp 
  src/gldisplaywidget.cpp 
  src/mesh.cpp
  src/algorithm.cpp
  src/point.cpp
  src/topography.cpp
  src/mesh_triangulation.cpp
  src/mesh_reconstruction.cpp
  src/mesh_refinement.cpp
  ${INCLUDES}
)

qt5_wrap_cpp(
  SOURCES
)

add_executable(
  ${PROJECT_NAME}
  ${SOURCES}
  )


qt5_use_modules(${PROJECT_NAME} Widgets OpenGL)

# https://bugreports.qt.io/browse/QTBUG-42999 HELPED !!
target_link_libraries(
  ${PROJECT_NAME} 
  GLU 
  Qt5::Core 
  Qt5::Gui 
  # ${Qt5Gui_EGL_LIBRARIES} 
  ${Qt5Gui_OPENGL_LIBRARIES}
  )

file(COPY "data" DESTINATION ${PROJECT_BINARY_DIR})

# configure_file(
#   data/cube.off ${CMAKE_CURRENT_BINARY_DIR}/data/cube.off COPYONLY
# )

# configure_file(
#   data/queen.off ${CMAKE_CURRENT_BINARY_DIR}/data/queen.off COPYONLY
# )

# configure_file(
#   data/franke4.off ${CMAKE_CURRENT_BINARY_DIR}/data/franke4.off COPYONLY
# )


