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
#include "simutility.h"
#include <mmtsy_names.h>

SimUtility::SimUtility(StoreType type, int& error, QObject *parent)
    : QObject(parent),
    m_etelStoreInfoPckg( m_etelStoreInfo )
{
    error = m_etelServer.Connect();
    if (error == KErrNone) {
        error = m_etelServer.LoadPhoneModule(KMmTsyModuleName);
    }
    if (error == KErrNone) {
        RTelServer::TPhoneInfo info;
        error = m_etelServer.GetPhoneInfo(0, info);
        if (error == KErrNone) {
            error = m_etelPhone.Open(m_etelServer, info.iName);
        }
    }
    if (error == KErrNone) {
        // open specified Etel store
        if (type == AdnStore) {
            error = m_etelStore.Open(m_etelPhone, KETelIccAdnPhoneBook);
        }
        else if (type == SdnStore) {
            error = m_etelStore.Open(m_etelPhone, KETelIccSdnPhoneBook);
        }
        else if (type == FdnStore) {
            error = m_etelStore.Open(m_etelPhone, KETelIccFdnPhoneBook);
        }
        else {
            error = KErrNotSupported;
        }
    }
}

SimUtility::~SimUtility()
{
    m_etelStore.Close();
    m_etelPhone.Close();
    m_etelServer.Close();
}

SimUtility::SimInfo SimUtility::getSimInfo(int& error)
{
    TRequestStatus requestStatus;
    m_etelStore.GetInfo(requestStatus, (TDes8&)m_etelStoreInfoPckg);
    User::WaitForRequest(requestStatus);
    error = requestStatus.Int();
    SimInfo results;
    
    if (error == KErrNone) {
        results.totalEntries = m_etelStoreInfo.iTotalEntries;
        results.usedEntries = m_etelStoreInfo.iUsedEntries;
        results.maxNumLength = m_etelStoreInfo.iMaxNumLength;
        results.maxTextLength = m_etelStoreInfo.iMaxTextLength;
        results.maxSecondNames = m_etelStoreInfo.iMaxSecondNames;
        results.maxTextLengthSecondName = m_etelStoreInfo.iMaxTextLengthSecondName;
        results.maxAdditionalNumbers = m_etelStoreInfo.iMaxAdditionalNumbers;
        results.maxNumLengthAdditionalNumber = m_etelStoreInfo.iMaxNumLengthAdditionalNumber;
        results.maxTextLengthAdditionalNumber = m_etelStoreInfo.iMaxTextLengthAdditionalNumber;
        results.maxGroupNames = m_etelStoreInfo.iMaxGroupNames;
        results.maxTextLengthGroupName = m_etelStoreInfo.iMaxTextLengthGroupName;
        results.maxEmailAddr = m_etelStoreInfo.iMaxEmailAddr;
        results.maxTextLengthEmailAddr = m_etelStoreInfo.iMaxTextLengthEmailAddr;
    }
    return results;
}
