
cmake_minimum_required(VERSION 2.8)
project("unit_conversion")

find_package(SymEngine CONFIG)
set(CMAKE_CXX_FLAGS_RELEASE ${SYMENGINE_CXX_FLAGS_RELEASE})
include_directories(${SYMENGINE_INCLUDE_DIRS})

add_executable("${PROJECT_NAME}" "main.cpp")
target_link_libraries("${PROJECT_NAME}" ${SYMENGINE_LIBRARIES})

install(TARGETS "${PROJECT_NAME}" DESTINATION bin) #${CMAKE_INSTALL_PREFIX}/bin
install(FILES "main.cpp" DESTINATION src)#${CMAKE_INSTALL_PREFIX}/src
