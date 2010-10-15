TEMPLATE=app
TARGET=ut_cntsimutility
QT += testlib 


CONFIG += mobility

DEFINES += SIMUTILITY_NO_EXPORT

symbian: {
    TARGET.CAPABILITY =  CAP_APPLICATION NetworkControl
    load(data_caging_paths)
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    SOURCES  += ut_cntsimutility.cpp \
                ../../src/cntsimutility.cpp \
                ../../src/asyncworker.cpp \
                start.cpp \
                ut_mock_implementations.cpp
    HEADERS +=  ../../inc/cntsimutility.h \
                ../../inc/simutilityglobal.h \
                ../../inc/asyncworker.h \
                ut_cntsimutility.h 
                
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData
    
    
}
