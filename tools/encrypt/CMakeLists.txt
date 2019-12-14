include_directories(${PROJECT_SOURCE_DIR}/src)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/x64)

add_executable(Encrypt
main.cpp
${PROJECT_SOURCE_DIR}/src/CodingHelper.cpp
${PROJECT_SOURCE_DIR}/src/GlobalVal.cpp
)

target_link_libraries(Encrypt ${VTK_LIBRARIES})
target_link_libraries(Encrypt ${ITK_LIBRARIES})
target_link_libraries(Encrypt ITKSnap)

qt5_use_modules(Encrypt Core Gui Widgets)