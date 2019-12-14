cmake_minimum_required(VERSION 2.8)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/x64)
set(ROOT_DIR ${PROJECT_SOURCE_DIR}/src)

file (GLOB Resources resources/*.qrc)
file (GLOB UIs gui/*.ui)

# Src group

file(GLOB GUI_DIR_SRC gui/*.cpp gui/*.h)
file(GLOB MODEL_DIR_SRC model/*.cpp model/*.h)
file(GLOB TOOLS_DIR_SRC tools/*.cpp tools/*.h)
file(GLOB RENDERER_DIR_SRC renderer/*.cpp renderer/*.h)
file(GLOB LOGIC_DIR_SRC logic/*.cpp logic/*.h)
file(GLOB SRC *.cpp *.h)

source_group("Gui" FILES ${GUI_DIR_SRC})
source_group("Model" FILES ${MODEL_DIR_SRC})
source_group("Tools" FILES ${TOOLS_DIR_SRC})
source_group("Src" FILES ${SRC})
source_group("Renderer" FILES ${RENDERER_DIR_SRC})
source_group("Logic" FILES ${LOGIC_DIR_SRC})

qt5_add_resources(Resources ${Resources} ${QT_RES})
qt5_wrap_ui(UI_HEADERS ${UIs})

source_group("UI" FILES ${UIs} ${UI_HEADERS})

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${ROOT_DIR}/gui)
include_directories(${ROOT_DIR}/model)
include_directories(${ROOT_DIR}/tools)
include_directories(${ROOT_DIR}/renderer)

# Build executable


add_executable(${PROJECT_NAME} WIN32
${UIs}
${UI_HEADERS}
${GUI_DIR_SRC}
${MODEL_DIR_SRC}
${TOOLS_DIR_SRC}
${RENDERER_DIR_SRC}
#${LOGIC_DIR_SRC}
${SRC}
${Resources}
)

# Link libraries
qt5_use_modules(${PROJECT_NAME}  Core Gui Widgets OpenGL)
target_link_libraries(${PROJECT_NAME} Qt5::WinMain)
target_link_libraries(${PROJECT_NAME} ${VTK_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${ITK_LIBRARIES})
target_link_libraries(${PROJECT_NAME} Common)
target_link_libraries(${PROJECT_NAME} ITKSnap)
target_link_libraries(${PROJECT_NAME} glog.lib)
target_link_libraries(${PROJECT_NAME} opencv_world411.lib)
target_link_libraries(${PROJECT_NAME} licensepp_static.lib os.lib base.lib tools_base.lib)


