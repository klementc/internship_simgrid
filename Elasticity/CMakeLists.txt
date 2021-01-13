
INCLUDE_DIRECTORIES(
  ~/Code/framagit.org/simgridOfficiel/simgrid//build/include
  ~/Code/framagit.org/simgridOfficiel/simgrid//include
  ~/Code/framagit.org/simgridOfficiel/simgrid/
  ~/Code/framagit.org/simgridOfficiel/simgrid//build/)
INCLUDE_DIRECTORIES(SYSTEM /home/clem/.hunter/_Base/d45d77d/4430a64/3b7ee27/Install/include)
LINK_DIRECTORIES(
  /home/clem/.hunter/_Base/d45d77d/4430a64/3b7ee27/Install/lib
  ~/Code/framagit.org/simgridOfficiel/simgrid//build/lib)

if(USE_JAEGERTRACING)
  list(APPEND EXTRA_LIBS  jaegertracing opentracing yaml-cppd)
  # list(APPEND EXTRA_INCLUDES "${PROJECT_SOURCE_DIR}/MathFunctions")
endif()


add_library(Elasticity ElasticTask.cpp ElasticPolicyCPUThreshold.cpp ElasticPolicyHybrid1.cpp ElasticPolicyReactive1.cpp TaskInstance.cpp)

TARGET_LINK_LIBRARIES(Elasticity ${EXTRA_LIBS})
