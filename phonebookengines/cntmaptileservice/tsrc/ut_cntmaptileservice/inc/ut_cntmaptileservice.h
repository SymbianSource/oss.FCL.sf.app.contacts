/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include <QtTest/QtTest>
#include <QtGui>
#include <QString>
#include <qtcontacts.h>
#include <qcontactmanager.h>

#include "cntmaptileservice.h"

//Maptile test interface class
class T_MaptileServiceTest: public QObject
{
    Q_OBJECT
    
private slots:
    void init();
    void cleanup();
    
private slots:
    
    void checkLocationFeature();
    void getPreferredAddressMapTilePath();
    void getWorkAddressMapTilePath();
    void getHomeAddressMapTilePath();
    void checkInvalidContactId();
    void publishForMaptileFetching();
    void readInvalidContactInfotmation();
    
private:
    CntMapTileService* maptileService;
};
// End of File
