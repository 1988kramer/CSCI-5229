#  Project file for the SLAM visualizer
#  Andrew Kramer
#
#  List of header files
HEADERS = viewer.h SlamViz.h airplane.h CSCIx229.h
#  List of source files
SOURCES = main.cpp viewer.cpp SlamViz.cpp airplane.cpp errcheck.cpp project.cpp fatal.cpp
#  Include OpenGL support
QT += opengl
LIBS += -lglut -lGLU
