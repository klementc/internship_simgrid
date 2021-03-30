
INCLUDE_DIRECTORIES(
  ../Elasticity
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

FILE(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../Elasticity/config.yml
      DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

ADD_EXECUTABLE( test test.cpp )
TARGET_LINK_LIBRARIES(test Elasticity simgrid ${EXTRA_LIBS})


ADD_EXECUTABLE( fifa FifaExperiment.cpp )
TARGET_LINK_LIBRARIES(fifa Elasticity simgrid ${EXTRA_LIBS})

ADD_EXECUTABLE( graph GraphExperiment.cpp )
TARGET_LINK_LIBRARIES(graph Elasticity simgrid ${EXTRA_LIBS})

ADD_EXECUTABLE( main main.cpp )
TARGET_LINK_LIBRARIES(main Elasticity simgrid ${EXTRA_LIBS})

ADD_EXECUTABLE( dataSourceTest DataSourceTest.cpp )
TARGET_LINK_LIBRARIES(dataSourceTest Elasticity simgrid ${EXTRA_LIBS})

ADD_EXECUTABLE( testDoc testDoc.cpp )
TARGET_LINK_LIBRARIES(testDoc Elasticity simgrid ${EXTRA_LIBS})

ADD_EXECUTABLE( bench1 bench1.cpp )
TARGET_LINK_LIBRARIES(bench1 Elasticity simgrid ${EXTRA_LIBS})

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