cmake_minimum_required(VERSION 3.17)

project(ElasticTask)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g")

add_subdirectory(Elasticity)
add_subdirectory(examples)

option(USE_JAEGERTRACING "Use jaeger traces through opentracing" ON)
configure_file(Elasticity/ElasticConfig.hpp.in ${PROJECT_SOURCE_DIR}/Elasticity/ElasticConfig.hpp)
# set(SIMULATOR_SOURCES main.cpp ElasticTask.hpp ElasticTask.cpp)
# add_executable(my_simulator ${SIMULATOR_SOURCES})
# target_link_libraries(my_simulator simgrid)
