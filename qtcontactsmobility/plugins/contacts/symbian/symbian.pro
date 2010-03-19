TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(mobapicontactspluginsymbian)
PLUGIN_TYPE=contacts

include(../../../common.pri)
symbian: { 
    load(data_caging_paths)
    
    defFiles = \
        "$${LITERAL_HASH}ifdef WINSCW" \
        "DEFFILE bwins/$${TARGET}.def" \
        "$${LITERAL_HASH}elif defined EABI" \
        "DEFFILE eabi/$${TARGET}.def" \
        "$${LITERAL_HASH}endif "
    MMP_RULES += defFiles

    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.UID3 = 0x2002AC7B
    
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
  
    INCLUDEPATH += inc
    INCLUDEPATH += rss
    INCLUDEPATH += $$SOURCE_DIR/contacts
    INCLUDEPATH += $$SOURCE_DIR/contacts/details
    INCLUDEPATH += $$SOURCE_DIR/contacts/filters
    INCLUDEPATH += $$SOURCE_DIR/contacts/requests

  HEADERS += \
        $$PUBLIC_HEADERS \
        inc/cntsymbianengine.h \
        inc/cnttransformcontact.h \
        inc/cnttransformcontactdata.h \
        inc/cnttransformname.h \
        inc/cnttransformnickname.h \
        inc/cnttransformphonenumber.h \
        inc/cnttransformemail.h \
        inc/cnttransformaddress.h \
        inc/cnttransformurl.h \
        inc/cnttransformbirthday.h \
        inc/cnttransformonlineaccount.h \
        inc/cnttransformorganisation.h \
        inc/cnttransformavatar.h \
        inc/cnttransformavatarsimple.h \
        inc/cntthumbnailcreator.h \
        inc/cnttransformsynctarget.h \
        inc/cnttransformgender.h \
        inc/cnttransformanniversary.h \
        inc/cnttransformanniversarysimple.h \
        inc/cnttransformgeolocation.h \
        inc/cnttransformnote.h \
        inc/cnttransformfamily.h \  
        inc/cnttransformempty.h \  
        inc/cntabstractcontactfilter.h \
        inc/cntsymbianfilterdbms.h \
        inc/cntsymbianfiltersql.h \
        inc/cntabstractcontactsorter.h \
        inc/cntsymbiansorterdbms.h \
        inc/cntrelationship.h \
        inc/cntabstractrelationship.h \
        inc/cntrelationshipgroup.h \
        inc/cntsymbianfiltersqlhelper.h \
        inc/cntsymbiansrvconnection.h \
        inc/cntsymbiantransformerror.h \
        inc/cntsymbiandatabase.h \
        inc/cntdisplaylabel.h \
        inc/cntdisplaylabelsqlfilter.h \
	inc/cntsqlsearch.h
      
    SOURCES += \
        src/cntsymbianengine.cpp \
        src/cnttransformcontact.cpp \
        src/cnttransformcontactdata.cpp \
        src/cnttransformname.cpp \
        src/cnttransformnickname.cpp \
        src/cnttransformphonenumber.cpp \
        src/cnttransformemail.cpp \
        src/cnttransformaddress.cpp \
        src/cnttransformurl.cpp \
        src/cnttransformbirthday.cpp \
        src/cnttransformonlineaccount.cpp \
        src/cnttransformorganisation.cpp \
        src/cnttransformavatar.cpp \
        src/cnttransformavatarsimple.cpp \
        src/cntthumbnailcreator.cpp\
        src/cnttransformsynctarget.cpp \
        src/cnttransformgender.cpp \
        src/cnttransformanniversary.cpp \
        src/cnttransformanniversarysimple.cpp \
        src/cnttransformgeolocation.cpp \
        src/cnttransformnote.cpp \
        src/cnttransformfamily.cpp \  
        src/cnttransformempty.cpp \
        src/cntsymbianfilterdbms.cpp \
        src/cntsymbianfiltersql.cpp \
        src/cntsymbiansorterdbms.cpp \
        src/cntrelationship.cpp \
        src/cntabstractrelationship.cpp \
        src/cntrelationshipgroup.cpp \
        src/cntsymbianfiltersqlhelper.cpp \
        src/cntsymbiansrvconnection.cpp \
        src/cntsymbiantransformerror.cpp \
        src/cntsymbiandatabase.cpp \
        src/cntdisplaylabel.cpp \
        src/cntdisplaylabelsqlfilter.cpp \
	src/cntsqlsearch.cpp 


      
    CONFIG += mobility
    MOBILITY = contacts

    qtAddLibrary(QtContacts)
    
    LIBS += \
        -lcntmodel \
        -lcentralrepository \
        -lestor \
        -lflogger \
        -lefsrv \
        -lfbscli \
        -limageconversion \
        -lbitmaptransforms \
        -lbafl

    target.path = /sys/bin
    INSTALLS += target

    exists($${EPOCROOT}epoc32/data/z/system/install/Series60v5.2.sis) {
        exists($${EPOCROOT}epoc32/release/winscw/udeb/VPbkEng.dll) \
        | exists($${EPOCROOT}epoc32/release/armv5/urel/VPbkEng.dll) {
            message("TB 9.2 platform")
        } else {
            message("TB 10.1 or later platform")
            DEFINES += SYMBIAN_BACKEND_USE_SQLITE
            cntmodelResourceFile = \
                "START RESOURCE ../rss/cntmodel.rss" \
                "TARGETPATH $${CONTACTS_RESOURCE_DIR}" \
                "END"
            MMP_RULES += cntmodelResourceFile
        }
    }
    
    contains(S60_VERSION, 3.2) {
    	DEFINES += SYMBIAN_BACKEND_S60_VERSION_32
    }
    


    symbianplugin.sources = $${TARGET}.dll
    symbianplugin.path = /resource/qt/plugins/contacts
    DEPLOYMENT += symbianplugin
}

target.path=$$QT_MOBILITY_PREFIX/plugins/contacts
INSTALLS+=target
