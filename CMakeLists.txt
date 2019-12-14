cmake_minimum_required(VERSION 2.8)

project(LabelDicom)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-DGLOG_NO_ABBREVIATED_SEVERITIES)
add_definitions(-DGOOGLE_GLOG_DLL_DECL=)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

find_package(VTK REQUIRED)

include(${VTK_USE_FILE})

find_package(ITK REQUIRED)

include(${ITK_USE_FILE})


# Find package Qt
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
#find_package(Qt5OpenGL REQUIRED)
#find_package(Qt5PrintSupport REQUIRED)

# Use Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(QT_USE_QTMAIN TRUE)

file (GLOB QT_RES ${PROJECT_SOURCE_DIR}/res/qdarkstyle/*.qrc)

include_directories(${PROJECT_SOURCE_DIR}/external/include)
include_directories(${PROJECT_SOURCE_DIR}/external/glm)
include_directories(${PROJECT_SOURCE_DIR}/external/Common)
include_directories(${PROJECT_SOURCE_DIR}/external/glog/include)
include_directories(${PROJECT_SOURCE_DIR}/external/opencv/include)
include_directories(${PROJECT_SOURCE_DIR}/external/ITKSnap)
include_directories(${PROJECT_SOURCE_DIR}/external/licensepp/include)


link_directories(${PROJECT_SOURCE_DIR}/external/glog/lib)
link_directories(${PROJECT_SOURCE_DIR}/external/opencv/lib)
link_directories(${PROJECT_SOURCE_DIR}/external/licensepp/lib)

add_subdirectory(${PROJECT_SOURCE_DIR}/external/Common)
add_subdirectory(${PROJECT_SOURCE_DIR}/external/ITKSnap)
add_subdirectory(src)
add_subdirectory(${PROJECT_SOURCE_DIR}/tools/encrypt)
add_subdirectory(${PROJECT_SOURCE_DIR}/tools/decrypt)