
# Search for SimGrid
include_directories("${SimGrid_INCLUDE_DIR}" SYSTEM)

INCLUDE_DIRECTORIES(../Elasticity)
INCLUDE_DIRECTORIES(SYSTEM /home/clem/.hunter/_Base/d45d77d/4430a64/3b7ee27/Install/include)
LINK_DIRECTORIES(
  /home/clem/.hunter/_Base/d45d77d/4430a64/3b7ee27/Install/lib
  ~/Code/framagit.org/simgridOfficiel/simgrid//build3.27/lib)

if(USE_JAEGERTRACING)
  list(APPEND EXTRA_LIBS  jaegertracing opentracing yaml-cppd)

endif()

FILE(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../Elasticity/config.yml
      DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

ADD_EXECUTABLE( graph GraphExperiment.cpp )
TARGET_LINK_LIBRARIES(graph Elasticity simgrid ${EXTRA_LIBS})

ADD_EXECUTABLE( dataSourceTest DataSourceTest.cpp )
TARGET_LINK_LIBRARIES(dataSourceTest Elasticity simgrid ${EXTRA_LIBS})

ADD_EXECUTABLE( testDoc testDoc.cpp )
TARGET_LINK_LIBRARIES(testDoc Elasticity simgrid ${EXTRA_LIBS})

ADD_EXECUTABLE( bench1 bench1.cpp )
TARGET_LINK_LIBRARIES(bench1 Elasticity simgrid ${EXTRA_LIBS})

ADD_EXECUTABLE( bench2 bench2.cpp )
TARGET_LINK_LIBRARIES(bench2 Elasticity simgrid ${EXTRA_LIBS})

ADD_EXECUTABLE( testReqTypes testReqTypes.cpp )
TARGET_LINK_LIBRARIES(testReqTypes Elasticity simgrid ${EXTRA_LIBS})

ADD_EXECUTABLE( a a.cpp )
TARGET_LINK_LIBRARIES(a Elasticity simgrid ${EXTRA_LIBS})
ADD_EXECUTABLE(b b.cpp )
TARGET_LINK_LIBRARIES(b Elasticity simgrid ${EXTRA_LIBS})
ADD_EXECUTABLE(c c.cpp )
TARGET_LINK_LIBRARIES(c Elasticity simgrid ${EXTRA_LIBS})
ADD_EXECUTABLE(testg testg.cpp )
TARGET_LINK_LIBRARIES(testg Elasticity simgrid ${EXTRA_LIBS})
ADD_EXECUTABLE(generated generated.cpp )
TARGET_LINK_LIBRARIES(generated Elasticity simgrid ${EXTRA_LIBS})
ADD_EXECUTABLE(test2req test2req.cpp )
TARGET_LINK_LIBRARIES(test2req Elasticity simgrid ${EXTRA_LIBS})
ADD_EXECUTABLE(testreunion testreunion.cpp )
TARGET_LINK_LIBRARIES(testreunion Elasticity simgrid ${EXTRA_LIBS})
ADD_EXECUTABLE(dsb_compose dsb_compose.cpp )
TARGET_LINK_LIBRARIES(dsb_compose Elasticity simgrid ${EXTRA_LIBS})
ADD_EXECUTABLE(cpuRatioExample cpuRatioExample.cpp )
TARGET_LINK_LIBRARIES(cpuRatioExample Elasticity simgrid ${EXTRA_LIBS})