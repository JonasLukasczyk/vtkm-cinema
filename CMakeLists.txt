cmake_minimum_required(VERSION 3.10)
project(vtkm_cinema)

find_package(VTKm REQUIRED)

add_executable(vtkm_cinema CinemaExporter.cpp VTKDataSetWriter2.cpp main.cpp)
target_link_libraries(vtkm_cinema PUBLIC vtkm_filter vtkm_rendering vtkm_io)
