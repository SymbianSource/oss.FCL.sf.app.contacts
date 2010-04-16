/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef SIMUTILITY_H
#define SIMUTILITY_H

#include <qglobal.h>
#include <QObject>
#include <etelmm.h>
#include "simutilityglobal.h"

/*!
 * SimUtility provides additional functionality for SIM contacts
 * handling which QContactManager doesn't support.
 */
class SIMUTILITYLIB_EXPORT SimUtility : public QObject
{
    Q_OBJECT

public:
    enum StoreType {
        AdnStore = 0,
        SdnStore,
        FdnStore
    };
    
    struct SimInfo {
        int totalEntries;
        int usedEntries;
        int maxNumLength;
        int maxTextLength;
        int maxSecondNames;
        int maxTextLengthSecondName;
        int maxAdditionalNumbers;
        int maxNumLengthAdditionalNumber;
        int maxTextLengthAdditionalNumber;
        int maxGroupNames;
        int maxTextLengthGroupName;
        int maxEmailAddr;
        int maxTextLengthEmailAddr;
        
        SimInfo() {
            totalEntries = 0;
            usedEntries = 0;
            maxNumLength = 0;
            maxTextLength = 0;
            maxSecondNames = 0;
            maxTextLengthSecondName = 0;
            maxAdditionalNumbers = 0;
            maxNumLengthAdditionalNumber = 0;
            maxTextLengthAdditionalNumber = 0;
            maxGroupNames = 0;
            maxTextLengthGroupName = 0;
            maxEmailAddr = 0;
            maxTextLengthEmailAddr = 0;
        };
    };
    
public:
	SimUtility(StoreType type, int& error, QObject *parent = 0);
	~SimUtility();
	
	SimInfo getSimInfo(int& error);

private:
    RTelServer m_etelServer;
    RMobilePhone m_etelPhone;
    RMobilePhoneBookStore m_etelStore;
    RMobilePhoneBookStore::TMobilePhoneBookInfoV5 m_etelStoreInfo;
    RMobilePhoneBookStore::TMobilePhoneBookInfoV5Pckg m_etelStoreInfoPckg;
};

#endif
