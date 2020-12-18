cmake_minimum_required(VERSION 2.8)
project(pcTest)
add_compile_options(-std=c++11)
set(sources main.cpp
	CommandQueue.cpp
	)
add_executable(${PROJECT_NAME} ${sources})
target_link_libraries(${PROJECT_NAME} pthread)
