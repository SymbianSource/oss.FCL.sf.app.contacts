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
        mStringMap.insert(QContactPhoneNumber::SubTypeLandline, hbTrId("Landline"));
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
        mEditorIconMap.insert(QContactPhoneNumber::DefinitionName, ":/icons/qtg_small_mobile.svg");
        mEditorIconMap.insert(QContactPhoneNumber::SubTypeAssistant, ":/icons/qtg_small_assistant.svg");
        mEditorIconMap.insert(QContactPhoneNumber::SubTypeCar, ":/icons/qtg_small_car.svg");
        mEditorIconMap.insert(QContactPhoneNumber::SubTypeMobile, ":/icons/qtg_small_mobile.svg");
        mEditorIconMap.insert(QContactPhoneNumber::SubTypeLandline, ":/icons/qtg_small_landline.svg");
        mEditorIconMap.insert(QContactPhoneNumber::SubTypeDtmfMenu, ":/icons/qgn_prop_empty.svg");
        mEditorIconMap.insert(QContactPhoneNumber::SubTypeFacsimile, ":/icons/qtg_small_fax.svg");
        mEditorIconMap.insert(QContactPhoneNumber::SubTypePager, ":/icons/qtg_small_pager.svg");

        mEditorIconMap.insert(QContactEmailAddress::DefinitionName, ":/icons/qtg_small_email.svg");

        mEditorIconMap.insert(QContactAddress::DefinitionName, ":/icons/qtg_small_location.svg");

        mEditorIconMap.insert(QContactOnlineAccount::DefinitionName, ":/icons/qtg_small_internet.svg");
        mEditorIconMap.insert(QContactOnlineAccount::SubTypeSipVoip, ":/icons/qtg_small_internet.svg");
        mEditorIconMap.insert(QContactOnlineAccount::SubTypeSip, ":/icons/qtg_small_internet.svg");
        mEditorIconMap.insert(QContactOnlineAccount::SubTypeVideoShare, ":/icons/qtg_small_internet.svg");

        mEditorIconMap.insert(QContactUrl::DefinitionName, ":/icons/qgn_prop_empty.svg");

        // init mapping of contact card icons
        mLauncherIconMap.insert(QContactPhoneNumber::DefinitionName, ":/icons/qtg_large_call_mobile.svg");
        mLauncherIconMap.insert(QContactPhoneNumber::SubTypeMobile, ":/icons/qtg_large_call_mobile.svg");
        mLauncherIconMap.insert(QContactPhoneNumber::SubTypeLandline, ":/icons/qtg_large_call_landline.svg");
        mLauncherIconMap.insert(QContactPhoneNumber::SubTypeFacsimile, ":/icons/qtg_large_call_fax.svg");
        mLauncherIconMap.insert(QContactPhoneNumber::SubTypePager, ":/icons/qtg_large_call_pager.svg");
        mLauncherIconMap.insert(QContactPhoneNumber::SubTypeCar, ":/icons/qtg_large_call_car.svg");
        mLauncherIconMap.insert(QContactPhoneNumber::SubTypeAssistant, ":/icons/qtg_large_call_assistant.svg");
        mLauncherIconMap.insert(QContactPhoneNumber::SubTypeDtmfMenu, ":/icons/qtg_large_call_group.svg");

        mLauncherIconMap.insert(QContactEmailAddress::DefinitionName, ":/icons/qtg_large_email.svg");
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
