
cmake_minimum_required(VERSION 2.8)

PROJECT(PolyDataToImageData)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_executable(PolyDataToImageData MACOSX_BUNDLE PolyDataToImageData)

if(VTK_LIBRARIES)
  target_link_libraries(PolyDataToImageData ${VTK_LIBRARIES})
else()
  target_link_libraries(PolyDataToImageData vtkHybrid)
endif()
