######################################################################
# Automatically generated by qmake (2.00a) Thu Aug 17 15:33:51 2006
######################################################################
CONFIG -= app_bundle
CONFIG += qt
QT += xml
TEMPLATE = app
TARGET = maketree
INCLUDEPATH += . include ../c++adt/include ../stl-lignum/include ../Firmament/include ../stl-voxelspace/include  ../Pine ../XMLTree 
INCLUDEPATH += ../LEngine/include ../DigitalTree/include
DEPENDPATH += $$INCLUDEPATH
LIBS += -L../c++adt/lib -L../stl-lignum/lib -L../Firmament/lib -L../LEngine/lib -L../stl-voxelspace/lib -lsky -lL -lvoxel -lLGM  -lcxxadt 
     
macx:LIBS +=  -L../Graphics -lVisual -F/usr/local/Trolltech/Qt-4.1.4/lib -framework GLUT -framework OpenGL
win32:CONFIG += console
HEADERS += include/QSMreader.h

SOURCES += make_lignumtree.cc
