# Example usage:
#  mkdir build
#  cd build
#  cmake -g "Visual Studio 14 2015" ..

cmake_minimum_required(VERSION 3.16)
project(calc_normal C CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVK_PROTOTYPES")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_PROTOTYPES")

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

set(externRepos glfw utfcpp)

foreach(repo IN LISTS externRepos)
	if(NOT EXISTS "${PROJECT_SOURCE_DIR}/external/${repo}/CMakeLists.txt")
		message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
	endif()
endforeach()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/external/imgui/imgui.h")
	message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

# GLFW
set(GLFW_DIR external/glfw) # Set this to point to an up-to-date GLFW repo
option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)
add_subdirectory(${GLFW_DIR} binary_dir EXCLUDE_FROM_ALL)
include_directories(${GLFW_DIR}/include)

# Dear ImGui
set(IMGUI_DIR external/imgui)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends ..)

# utf8 Helper
set(UTF8_DIR external/utfcpp)
add_subdirectory(${UTF8_DIR})

# Libraries
find_package(OpenGL REQUIRED)

set(LIBRARIES "glfw;utf8cpp")

include_directories(${GLFW_DIR}/deps ${OPENGL_INCLUDE_DIRS})

file(GLOB sources src/*.cpp)

add_executable(CalculadoraFN ${sources} ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp ${IMGUI_DIR}/imgui.cpp ${IMGUI_DIR}/imgui_draw.cpp ${IMGUI_DIR}/imgui_demo.cpp ${IMGUI_DIR}/imgui_tables.cpp ${IMGUI_DIR}/imgui_widgets.cpp)
target_link_libraries(CalculadoraFN ${LIBRARIES} ${OPENGL_LIBRARIES})

target_compile_definitions(CalculadoraFN PUBLIC -DImTextureID=ImU64)

target_compile_features(CalculadoraFN PUBLIC cxx_std_17)