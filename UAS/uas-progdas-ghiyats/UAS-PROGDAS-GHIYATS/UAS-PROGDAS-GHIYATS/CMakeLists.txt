cmake_minimum_required(VERSION 3.10)
project(GhiyatInventory)

set(CMAKE_CXX_STANDARD 17)

set(wxWidgets_ROOT_DIR "C:/wxWidgets-3.2.8")
set(wxWidgets_LIB_DIR "C:/wxWidgets-3.2.8/lib/gcc_lib")
set(wxWidgets_CONFIGURATION mswud)

find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})

include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(GhiyatInventory
    main.cpp
    src/AUDIT_PANEL.cpp
    src/DATA_PANEL.cpp
)

target_link_libraries(GhiyatInventory ${wxWidgets_LIBRARIES})
