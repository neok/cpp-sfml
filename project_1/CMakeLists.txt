cmake_minimum_required(VERSION 3.16)
project(ownage)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)

# SFML location
# Set the SFML directory where SFMLConfig.cmake is located
set(SFML_DIR "C:/SFML/lib/cmake/SFML")

# ImGui location
set(IMGUI_DIR "C:/imgui")

# ImGui-SFML location
set(IMGUI_SFML_DIR "C:/imgui-sfml")

# Find SFML package
set(SFML_STATIC_LIBRARIES TRUE)  # Use static linking
find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)

# Add the executable
add_executable(${PROJECT_NAME}
        WIN32
        src/main.cpp
)

# Include directories for ImGui and SFML
#target_include_directories(${PROJECT_NAME}
#        PRIVATE
#        ${IMGUI_DIR}
#        ${IMGUI_DIR}/backends
#)

# Link SFML libraries
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system)
target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
        ${IMGUI_SFML_DIR}  # Add ImGui-SFML include directory
)

# Link ImGui and SFML dependencies for static linking (if necessary)
if(WIN32)
    target_link_libraries(${PROJECT_NAME} opengl32 winmm gdi32)
endif()

if(MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
endif()

if (WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++ -mconsole")
endif()
