cmake_minimum_required(VERSION 3.10.0)

# change project name depending on assignament
project(MissileSim)
# set sources to be compiled
set (CODE main.cpp)

# change this variables only if running on windows
set(VULKAN_PATH $ENV{VULKAN_SDK})
set(GLFW_PATH "windows/glfw")

# ------------------------------------------------

# build to main directory to find resources
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

# set c++17 that is required
set (CMAKE_CXX_STANDARD 17)

# ------------------------------------------------
# linux

if (UNIX)
	#include vulkan
	find_package(Vulkan REQUIRED)
	if (Vulkan_FOUND)
	  message(STATUS "Vulkan found")
	  include_directories(${Vulkan_INCLUDE_DIRS})
	  link_libraries (${Vulkan_LIBRARIES})
	endif (Vulkan_FOUND)

	# include glfw
	find_package(glfw3 REQUIRED)
	if (glfw3_FOUND)
	  message(STATUS "GLFW found")
	  include_directories(${glfw3_INCLUDE_DIRS})
	  link_libraries (${glfw3_LIBRARIES})
	endif (glfw3_FOUND)


endif()

# ------------------------------------------------
# windows
if(WIN32) 	
	# vulkan
	include_directories("${VULKAN_PATH}/Include")
	file(GLOB VULKAN_LIBS  "${VULKAN_PATH}/Lib/*.lib")

	link_libraries(${VULKAN_LIBS})
	
	# glfw	
	include_directories("${GLFW_PATH}/include")
	link_directories("${GLFW_PATH}/lib")

endif()

# ------------------------------------------------
# header only libs

# add header only libraries shipped with the assignment
include_directories("${PROJECT_SOURCE_DIR}/headers")

# ------------------------------------------------
# exe

# create executable
add_executable(${PROJECT_NAME} ${CODE}) 

# add required libraries for linux
if (UNIX)
	target_link_libraries(${PROJECT_NAME} dl pthread X11 Xrandr glfw)
endif()

# add required libraries for windows
if(WIN32)
	target_link_libraries(${PROJECT_NAME} glfw3)
endif()

