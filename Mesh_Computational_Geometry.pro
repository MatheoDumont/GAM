#-------------------------------------------------
#
# Project created by QtCreator 2018-08-28T10:55:17
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Mesh_Computational_Geometry
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    gldisplaywidget.cpp \
    mesh.cpp

    HEADERS  += mainwindow.h \
        gldisplaywidget.h \
        mesh.h

FORMS    += mainwindow.ui

#---- Comment the following line on MacOS
LIBS = -lGLU

#---- Uncomment the following line on Windows
#LIBS += -lglu32
#LIBS += -lOpengl32


copy_OFF.commands = $(COPY_DIR) $$PWD/data $$OUT_PWD
first.depends = $(first) copy_OFF
export(first.depends)
export(copy_OFF.commands)
QMAKE_EXTRA_TARGETS += first copy_OFF

