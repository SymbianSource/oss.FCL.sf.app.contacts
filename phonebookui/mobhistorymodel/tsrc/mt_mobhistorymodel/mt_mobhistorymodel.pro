######################################################################
# Automatically generated by qmake (2.01a) pe 27. helmi 08:49:52 2009
######################################################################

TEMPLATE = app
TARGET = 

QT += sql
QT += testlib
CONFIG  += qtestlib hb
DEFINES += PBK_UNIT_TEST \
           MOBHISTORYMODEL_NO_EXPORT

DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ../inc

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
	
# Input
HEADERS += mt_mobhistorymodel.h \
           stub_classes.h \
           ../../inc/mobhistorymodelglobal.h \
           ../../inc/mobhistorymodel_p.h \
           ../../inc/mobhistorymodel.h
           
SOURCES += mt_mobhistorymodel.cpp \
           stub_classes.cpp \
           ../../src/mobhistorymodel.cpp
           
LIBS += -lQtContacts

TARGET.CAPABILITY = ALL -TCB