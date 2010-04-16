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

#ifndef CNTSTRINGMAPPER_H
#define CNTSTRINGMAPPER_H

#include <qtcontacts.h>
#include <hbglobal.h>

QTM_USE_NAMESPACE

class CntStringMapper
{

public:

    CntStringMapper() { initMappings(); }
    ~CntStringMapper() {}

public:
    void initMappings()
    {
        // init stringmap for string localization
        mStringMap.insert(QContactPhoneNumber::DefinitionName, hbTrId("Phone number"));
        mStringMap.insert(QContactPhoneNumber::SubTypeAssistant, hbTrId("Assistant number"));
        mStringMap.insert(QContactPhoneNumber::SubTypeCar, hbTrId("Car phone"));
        mStringMap.insert(QContactPhoneNumber::SubTypeMobile, hbTrId("Mobile"));
        mStringMap.insert(QContactPhoneNumber::SubTypeLandline, hbTrId("Phone"));
        mStringMap.insert(QContactPhoneNumber::SubTypeDtmfMenu, hbTrId("DTMF"));
        mStringMap.insert(QContactPhoneNumber::SubTypeFacsimile, hbTrId("Fax"));
        mStringMap.insert(QContactPhoneNumber::SubTypePager, hbTrId("Pager number"));

        mStringMap.insert(QContactEmailAddress::DefinitionName, hbTrId("Email"));

        mStringMap.insert(QContactAddress::DefinitionName, hbTrId("Address"));

        mStringMap.insert(QContactOnlineAccount::DefinitionName, hbTrId("Chat"));
        mStringMap.insert(QContactOnlineAccount::SubTypeSipVoip, hbTrId("Internet call"));
        mStringMap.insert(QContactOnlineAccount::SubTypeSip, hbTrId("SIP"));
        mStringMap.insert(QContactOnlineAccount::SubTypeVideoShare, hbTrId("Share Video"));

        mStringMap.insert(QContactUrl::DefinitionName, hbTrId("URL"));

        mStringMap.insert(QContactOrganization::DefinitionName, hbTrId("Company details"));
        mStringMap.insert(QContactOrganization::FieldAssistantName, hbTrId("Assistant name"));

        mStringMap.insert(QContactBirthday::DefinitionName, hbTrId("Birthday"));
        mStringMap.insert(QContactAnniversary::DefinitionName, hbTrId("Anniversary"));

        mStringMap.insert(QContactAvatar::SubTypeAudioRingtone, hbTrId("Ringtone"));

        mStringMap.insert(QContactNote::DefinitionName, hbTrId("Note"));

        mStringMap.insert(QContactFamily::FieldSpouse, hbTrId("Spouse"));
        mStringMap.insert(QContactFamily::FieldChildren, hbTrId("Children"));

        mStringMap.insert(QContactDetail::ContextHome, hbTrId("(home)"));
        mStringMap.insert(QContactDetail::ContextWork, hbTrId("(work)"));

        // init mapping of editor icons
        mEditorIconMap.insert(QContactPhoneNumber::DefinitionName, "qtg_small_mobile");
        mEditorIconMap.insert(QContactPhoneNumber::SubTypeAssistant, "qtg_small_assistant");
        mEditorIconMap.insert(QContactPhoneNumber::SubTypeCar, "qtg_small_car");
        mEditorIconMap.insert(QContactPhoneNumber::SubTypeMobile, "qtg_small_mobile");
        mEditorIconMap.insert(QContactPhoneNumber::SubTypeLandline, "qtg_small_landline");
        mEditorIconMap.insert(QContactPhoneNumber::SubTypeDtmfMenu, "qgn_prop_empty");
        mEditorIconMap.insert(QContactPhoneNumber::SubTypeFacsimile, "qtg_small_fax");
        mEditorIconMap.insert(QContactPhoneNumber::SubTypePager, "qtg_small_pagerg");

        mEditorIconMap.insert(QContactEmailAddress::DefinitionName, "qtg_small_email");

        mEditorIconMap.insert(QContactAddress::DefinitionName, "qtg_small_location");

        mEditorIconMap.insert(QContactOnlineAccount::DefinitionName, "qtg_small_internet");
        mEditorIconMap.insert(QContactOnlineAccount::SubTypeSipVoip, "qtg_small_internet");
        mEditorIconMap.insert(QContactOnlineAccount::SubTypeSip, "qtg_small_internet");
        mEditorIconMap.insert(QContactOnlineAccount::SubTypeVideoShare, "qtg_small_internet");

        mEditorIconMap.insert(QContactUrl::DefinitionName, "qtg_small_url_address.svg");

        // init mapping of contact card icons
        mLauncherIconMap.insert(QContactPhoneNumber::DefinitionName, "qtg_large_call_mobile");
        mLauncherIconMap.insert(QContactPhoneNumber::SubTypeMobile, "qtg_large_call_mobile");
        mLauncherIconMap.insert(QContactPhoneNumber::SubTypeLandline, "qtg_large_call_landline");
        mLauncherIconMap.insert(QContactPhoneNumber::SubTypeFacsimile, "qtg_large_call_fax");
        mLauncherIconMap.insert(QContactPhoneNumber::SubTypePager, "qtg_large_call_pager");
        mLauncherIconMap.insert(QContactPhoneNumber::SubTypeCar, "qtg_large_call_car");
        mLauncherIconMap.insert(QContactPhoneNumber::SubTypeAssistant, "qtg_large_call_assistant");
        mLauncherIconMap.insert(QContactPhoneNumber::SubTypeDtmfMenu, "qtg_large_call_group");

        mLauncherIconMap.insert(QContactEmailAddress::DefinitionName, "qtg_large_email");
    }

    QString getMappedDetail(const QString &string) { return mStringMap.value(string); }
    QString getMappedIcon(const QString &string) { return mEditorIconMap.value(string); }
    QString getMappedLauncherIcon(const QString &string) { return mLauncherIconMap.value(string); }

private:
    QMap<QString, QString> mStringMap;
    QMap<QString, QString> mEditorIconMap;
    QMap<QString, QString> mLauncherIconMap;

};

#endif // CNTSTRINGMAPPER_H
