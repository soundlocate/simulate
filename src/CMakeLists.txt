FILE (GLOB_RECURSE project_SRCS *.cpp *.cxx *.cc *.C *.c *.h *.hpp)
SET (project_LIBS ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${SFML_LIBRARIES})
SET (project_BIN ${PROJECT_NAME})

ADD_EXECUTABLE(${project_BIN} ${project_SRCS} glyphs.o)
TARGET_LINK_LIBRARIES(${project_BIN} ${project_LIBS})

SET_TARGET_PROPERTIES(${project_BIN} PROPERTIES VERSION "${APPLICATION_VERSION_MAJOR}.${APPLICATION_VERSION_MINOR}" OUTPUT_NAME ${project_BIN} CLEAN_DIRECT_OUTPUT 1 INTERPROCEDURAL_OPTIMIZATION 1)

#TARGET_COMPILE_FEATURES(${PROJECT_NAME} PRIVATE cxx_constexpr)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp -xHOST -O3 -ipo -no-prec-div -fp-model fast=2 -std=c++14 -static-intel -parallel")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -std=gnu++14")

# -fopt-info-vec-missed  -xHOST -ipo -no-prec-div -fp-model fast=2 -static-intel -parallel

INSTALL(TARGETS ${project_BIN} DESTINATION bin)
