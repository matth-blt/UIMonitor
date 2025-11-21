cmake_minimum_required(VERSION 3.10)
project(UIMonitor VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

# Fichiers sources
file(GLOB SOURCES "src/*.cpp")

# Intégration de FTXUI via vcpkg
find_package(ftxui CONFIG REQUIRED)

add_executable(UIMonitor ${SOURCES})

# Link libraries
# Windows specific for system stats
if(WIN32)
    target_link_libraries(UIMonitor PRIVATE pdh psapi)
endif()

# Link FTXUI
target_link_libraries(UIMonitor PRIVATE ftxui::screen ftxui::dom ftxui::component)
