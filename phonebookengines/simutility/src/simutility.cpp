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
#include <startupdomainpskeys.h>
#include <e32property.h>

SimUtility::SimUtility(StoreType type, int& error, QObject *parent)
    : QObject(parent),
    m_etelStoreInfoPckg(m_etelStoreInfo),
    m_serviceTablePckg(m_serviceTable),
    m_activeRequest(ENoActiveRequest)
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
    
    if (error == KErrNone) {
        TRAP(error, 
                m_asyncWorker = new (ELeave)AsyncWorker(this);
                TSecUi::InitializeLibL();
                m_securitySettings = CSecuritySettings::NewL();
            );
    }
}

SimUtility::~SimUtility()
{
    m_etelStore.Close();
    m_etelPhone.Close();
    m_etelServer.Close();

    delete m_asyncWorker;
    delete m_securitySettings;
    TSecUi::UnInitializeLib();
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

SimUtility::AvailableStores SimUtility::getAvailableStores(int& error)
{
    AvailableStores availableStores;
    if (!isSimInserted()) {
        error = KErrNone;
        return availableStores;
    }
    
    m_serviceTableType = RMobilePhone::ESIMServiceTable;
    unsigned long int iccCaps;
    TRequestStatus requestStatus;
    
    int result = m_etelPhone.GetIccAccessCaps(iccCaps);
    if (result == KErrNone && (iccCaps & RMobilePhone::KCapsUSimAccessSupported)) {
        m_serviceTableType = RMobilePhone::EUSIMServiceTable;
    }

    m_etelPhone.GetServiceTable(requestStatus, m_serviceTableType, m_serviceTablePckg);
    User::WaitForRequest(requestStatus);
    error = requestStatus.Int();
    
    if (error == KErrNone) {
        //parse service table to find what stores are supported
         ParseServiceTable(&availableStores);
    }
    return availableStores;
}

bool SimUtility::verifyPin2Code()
{
    bool verified = false;
    TRAP_IGNORE(verified = m_securitySettings->AskPin2L(););
    return verified;
}

bool SimUtility::isFdnActive()
{
    RMobilePhone::TMobilePhoneFdnStatus fdnStatus;
    (void)m_etelPhone.GetFdnStatus(fdnStatus);
    if (fdnStatus == RMobilePhone::EFdnActive ||
        fdnStatus == RMobilePhone::EFdnPermanentlyActive) {
        return true;
        }
    return false;
}

int SimUtility::setFdnStatus(bool activated)
{
    RMobilePhone::TMobilePhoneFdnSetting fdnStatus = RMobilePhone::EFdnSetOff;
    if (activated) {
        fdnStatus = RMobilePhone::EFdnSetOn;
    }
    TRequestStatus status;
    m_etelPhone.SetFdnSetting(status, fdnStatus);
    User::WaitForRequest(status);
    return status.Int();
}

bool SimUtility::startGetSimInfo()
{
    if(m_asyncWorker->IsActive()) {
        return false;
    }
    
    m_etelStore.GetInfo(m_asyncWorker->iStatus, (TDes8&)m_etelStoreInfoPckg);
    m_asyncWorker->SetActive();
    m_activeRequest = EGetInfo;
    return true;
}

bool SimUtility::startGetAvailableStores()
{
    if(m_asyncWorker->IsActive()) {
        return false;
    }
    
    if (!isSimInserted()) {
        AvailableStores availableStores;
        emit availableStoresReady(availableStores, KErrNone);
        return true;
    }
    
    m_serviceTableType = RMobilePhone::ESIMServiceTable;
    unsigned long int iccCaps;
    TRequestStatus requestStatus;
    
    int result = m_etelPhone.GetIccAccessCaps(iccCaps);
    if (result == KErrNone && (iccCaps & RMobilePhone::KCapsUSimAccessSupported)) {
        m_serviceTableType = RMobilePhone::EUSIMServiceTable;
    }
    m_etelPhone.GetServiceTable(m_asyncWorker->iStatus, m_serviceTableType, m_serviceTablePckg);
    m_asyncWorker->SetActive();
    m_activeRequest = EGetAvailableStores;
    return true;
}

void SimUtility::RequestCompleted(int error)
{
    if (m_activeRequest == EGetInfo) {
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
        emit simInfoReady(results, error);
    }//EGetInfo
    else if (m_activeRequest == EGetAvailableStores) {
        AvailableStores availableStores;
        if (error == KErrNone) {
            //parse service table to find what stores are supported
             ParseServiceTable(&availableStores);
        }
        emit availableStoresReady(availableStores, error);
    }//EGetAvailableStores
    
    m_activeRequest = ENoActiveRequest;
}

void SimUtility::ParseServiceTable(AvailableStores* availableStores) const
{
    if (m_serviceTableType == RMobilePhone::EUSIMServiceTable) {
        if (m_serviceTable.iServices1To8 & RMobilePhone::KUstLocalPhBk ) {
            availableStores->AdnStorePresent = true;
        }
        else {
            availableStores->AdnStorePresent = false;
        }
        
        if (m_serviceTable.iServices1To8 & RMobilePhone::KUstSDN ) {
            availableStores->SdnStorePresent = true;
        }
        else {
            availableStores->SdnStorePresent = false;
        }
        
        if (m_serviceTable.iServices1To8 & RMobilePhone::KUstFDN) {
            availableStores->FdnStorePresent = true;
        }
        else {
            availableStores->FdnStorePresent = false;
        }
    }
    else if (m_serviceTableType == RMobilePhone::ESIMServiceTable) {
        if (m_serviceTable.iServices1To8 & RMobilePhone::KSstADN ) {
            availableStores->AdnStorePresent = true;
        }
        else {
            availableStores->AdnStorePresent = false;
        }
        
        if (m_serviceTable.iServices17To24 & RMobilePhone::KSstSDN) {
            availableStores->SdnStorePresent = true;
        }
        else {
            availableStores->SdnStorePresent = false;
        }
        
        if (m_serviceTable.iServices1To8 & RMobilePhone::KSstFDN) {
            availableStores->FdnStorePresent = true;
        }
        else {
            availableStores->FdnStorePresent = false;
        }
    }
}
    
bool SimUtility::isSimInserted() const
{
    bool result = false;
    
    RProperty property;   
    int simStatus = KErrNotFound;
    
    int ret = property.Get(KPSUidStartup, KPSSimStatus, simStatus);
    if (simStatus == ESimUsable && ret == KErrNone) {
        result = true;
        }
    
    property.Close();
    return result;
}
