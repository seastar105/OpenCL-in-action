cmake_minimum_required(VERSION 3.19)
project(OpenCL_in_action)

set(CMAKE_CXX_STANDARD 14)

include_directories(sogang/HW1)
include_directories(sogang/HW1/Util)
include_directories(sogang/HW1/inc)
link_directories(/opt/homebrew/lib)

add_executable(OpenCL_in_action
        sogang/HW1/Util/my_OpenCL_util_2_2.cpp
        sogang/HW1/Util/my_OpenCL_util_2_2.h
        sogang/HW1/Config_SoA_AoS.h
        sogang/HW1/Context_SoA_AoS.h
        sogang/HW1/Image_IO.cpp
        sogang/HW1/main_SoA_AoS.cpp
        sogang/HW1/My_Image_Filtering_Codes.cpp)

target_link_libraries(OpenCL_in_action
        "-framework OpenCL"
        "-lfreeimage"
        )