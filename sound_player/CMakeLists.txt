cmake_minimum_required(VERSION 2.8)
project(sound_player)
set(CMAKE_CXX_FLAGS "-std=c++11")
set(sources main.cpp 
	)
add_executable(${PROJECT_NAME} ${sources})
target_link_libraries(${PROJECT_NAME} asound)
