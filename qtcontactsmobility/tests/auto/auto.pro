TEMPLATE = subdirs

include(../../pathconfig.pri)
include($$QT_MOBILITY_BUILD_TREE/config.pri)

contains(mobility_modules,serviceframework) {
    SUBDIRS += databasemanager \                #service framework
           servicemetadata \
           qserviceinterfacedescriptor \
           qservicefilter \
           qservicemanager \
           qabstractsecuritysession \
           qservicecontext \
           icheck

# servicedatabase is not compiled into the serviceframework library on symbian,
# special handling is needed
    !symbian:SUBDIRS+=servicedatabase
}

contains(mobility_modules,bearer) {
    SUBDIRS += qnetworkconfigmanager \          #Bearer management
           qnetworkconfiguration \
           qnetworksession
}

contains(mobility_modules,location) {
    SUBDIRS += qgeocoordinate \                 #Location
          qgeopositioninfo \
          qgeosatelliteinfo \
          qgeosatelliteinfosource \
          qgeopositioninfosource \
          qgeoareamonitor \
          qlocationutils \
          qnmeapositioninfosource

    wince* {
        SUBDIRS += qgeoinfosources_wince
    }
}


contains(mobility_modules,publishsubscribe) {
    SUBDIRS += qvaluespace \                           #Publish and Subscribe
           qvaluespacepublisher \
           qvaluespacesubscriber \
	   qcrmlparser

    unix|win32 {
        !symbian:!maemo6: SUBDIRS+= \
            qsystemreadwritelock \
            qsystemreadwritelock_oop
    }

    unix:!symbian:!maemo6: {
        SUBDIRS+= \
               qpacket \
               qmallocpool \
               qpacketprotocol
    }
}

contains(mobility_modules,systeminfo) {
    SUBDIRS += qsysteminfo \                    #SystemInformation
          qsystemdeviceinfo \
          qsystemdisplayinfo \
          qsystemstorageinfo \
          qsystemnetworkinfo \
          qsystemscreensaver
}

contains(mobility_modules,contacts) {
    #Contacts
    SUBDIRS +=  qcontact \
            qcontactactions \
            qcontactasync \
            qcontactdetail \
            qcontactdetaildefinition \
            qcontactdetails \
            qcontactfilter \
            qcontactmanager \
            qcontactmanagerplugins \
            qcontactmanagerfiltering \
            qcontactrelationship

    SUBDIRS +=  qcontact_deprecated \
            qcontactactions_deprecated \
            qcontactasync_deprecated \
            qcontactdetail_deprecated \
            qcontactdetaildefinition_deprecated \
            qcontactdetails_deprecated \
            qcontactfilter_deprecated \
            qcontactmanager_deprecated \
            qcontactmanagerplugins_deprecated \
            qcontactmanagerfiltering_deprecated \
            qcontactrelationship_deprecated

}

contains(mobility_modules,versit) {
    # Versit module
    SUBDIRS += \
            qvcard21writer \
            qvcard30writer \
            qversit \
            qversitcontactexporter \
            qversitcontactimporter \
            qversitdocument \
            qversitproperty \
            qversitreader \
            qversitutils \
            qversitwriter
}

contains(mobility_modules,multimedia) {
    SUBDIRS += \             #Multimedia
        qaudiocapturesource \
        qcamera \
        qmediaimageviewer \
        qmediaobject \
        qmediaplayer \
        qmediaplaylist \
        qmediaplaylistnavigator \
        qmediapluginloader \
        qmediarecorder \
        qmediaresource \
        qmediaservice \
        qmediaserviceprovider \
        qmediacontent \
        qradiotuner \
        qvideowidget \
        qmediatimerange

    contains(QT_CONFIG, multimedia) {
        SUBDIRS += \
                qgraphicsvideoitem \
                qpaintervideosurface

    }

    symbian: {
        #symbian spesific autotests
        SUBDIRS += symbian 
        SUBDIRS -= \
                qcamera \
                qmediaplayer \
                qradiotuner \
                qmediaobject
    }
}
#Messaging
contains(mobility_modules,messaging) {
    contains(qmf_enabled,yes)|wince*|win32|symbian|maemo6 {
    !win32-g++:SUBDIRS += \
        qmessagestore \
        qmessagestorekeys \
        qmessage \
        qmessageservice
    }
}