include_directories(${PROJECT_SOURCE_DIR}/src)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/x64)

add_executable(Decrypt
main.cpp
${PROJECT_SOURCE_DIR}/src/CodingHelper.cpp
${PROJECT_SOURCE_DIR}/src/GlobalVal.cpp
)

target_link_libraries(Decrypt ${VTK_LIBRARIES})
target_link_libraries(Decrypt ${ITK_LIBRARIES})
target_link_libraries(Decrypt ITKSnap)

qt5_use_modules(Decrypt Core Gui Widgets)