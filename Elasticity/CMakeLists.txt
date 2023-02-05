

# Search for SimGrid
include_directories("${SimGrid_INCLUDE_DIR}" SYSTEM)

INCLUDE_DIRECTORIES(SYSTEM /home/clem/.hunter/_Base/d45d77d/4430a64/3b7ee27/Install/include)
LINK_DIRECTORIES(
  /home/clem/.hunter/_Base/d45d77d/4430a64/3b7ee27/Install/lib)




if(USE_JAEGERTRACING)
# Subdirectories
  find_package(opentelemetry-cpp CONFIG REQUIRED)
  #list(APPEND EXTRA_LIBS  opentelemetry_trace jaeger_trace_exporter )
endif()


add_library(Elasticity ElasticTask.cpp DataSource.cpp ElasticPolicyCPUThreshold.cpp ElasticPolicyHybrid1.cpp ElasticPolicyReactive1.cpp TaskInstance.cpp)
#target_include_directories(Elasticity PRIVATE ${OPENTELEMETRY_CPP_INCLUDE_DIRS})
#target_link_libraries(Elasticity PRIVATE ${OPENTELEMETRY_CPP_LIBRARIES})
target_link_libraries(Elasticity SimGrid::SimGrid)
TARGET_LINK_LIBRARIES(Elasticity ${EXTRA_LIBS})
