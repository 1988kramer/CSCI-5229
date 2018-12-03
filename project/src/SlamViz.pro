#  Project file for the SLAM visualizer
#  Andrew Kramer
#
#  List of header files
HEADERS = viewer.h SlamViz.h airplane.h Star.h SmokeBB.h CSCIx229.h
#  List of source files
SOURCES = main.cpp viewer.cpp SlamViz.cpp airplane.cpp Star.cpp SmokeBB.cpp errcheck.cpp fatal.cpp
#  Include OpenGL support
QT += opengl
# LIBS += -lGLU -lglut
CONFIG += c++11
