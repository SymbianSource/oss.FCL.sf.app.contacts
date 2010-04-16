#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: 
#

TEMPLATE = app
QT += testlib

INCLUDEPATH += inc
INCLUDEPATH += ../inc

symbian:
{
   :BLD_INF_RULES.prj_exports += "preferredaddressmap.png /epoc32/winscw/c/maptiletest/preferredaddressmap.png"
   :BLD_INF_RULES.prj_exports += "workaddressmap.png /epoc32/winscw/c/maptiletest/workaddressmap.png"
   :BLD_INF_RULES.prj_exports += "homeaddressmap.png /epoc32/winscw/c/maptiletest/homeaddressmap.png"
   SYSTEMINCLUDEPATH += \epoc32\include\stdapis
}

SOURCES += ut_maptileservice.cpp


LIBS += -lcntmaptileservice \
        -lQtContacts 

TARGET.CAPABILITY = ALL -TCB    

