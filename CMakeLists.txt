cmake_minimum_required(VERSION "3.21")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glm-1.0.1")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glfw-3.4")

project("GrassGL")
set(CMAKE_CXX_STANDARD 17)

# TODO: add static C++ runtime libs
#set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
#set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

file(GLOB_RECURSE MY_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE GLAD_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glad/*.c")

file(GLOB IMGUI_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui-1.91.9b/*.cpp")
find_package(OpenGL REQUIRED)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
	${MY_SRC}
	${GLAD_SRC}
	${IMGUI_SRC}
	"${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui-1.91.9b/backends/imgui_impl_opengl3.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui-1.91.9b/backends/imgui_impl_glfw.cpp"
)

target_include_directories(${PROJECT_NAME} PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include/"
	"${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glad/include/"
	"${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui-1.91.9b/"
)

target_compile_definitions(${PROJECT_NAME} PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/")

target_link_libraries(${PROJECT_NAME} PRIVATE glfw glm OpenGL::GL)


