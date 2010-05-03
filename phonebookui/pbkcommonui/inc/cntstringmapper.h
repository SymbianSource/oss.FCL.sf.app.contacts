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

class Loc
{
public:
    inline Loc( QString aId, QString aContext, QString aLoc );
    inline ~Loc();
    
public:
    QString mId;
    QString mContext;
    QString mLoc;
};
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
        
        mLocList.append(Loc(QContactPhoneNumber::SubTypeMobile, "", hbTrId("txt_phob_dblist_mobile")));
        mLocList.append(Loc(QContactPhoneNumber::SubTypeMobile, QContactDetail::ContextHome,hbTrId("txt_phob_dblist_mobile_home")));
        mLocList.append(Loc(QContactPhoneNumber::SubTypeMobile, QContactDetail::ContextWork,hbTrId("txt_phob_dblist_mobile_work")));
        mLocList.append(Loc(QContactPhoneNumber::SubTypeLandline, "",hbTrId("txt_phob_dblist_phone")));
        mLocList.append(Loc(QContactPhoneNumber::SubTypeLandline, QContactDetail::ContextHome, hbTrId("txt_phob_dblist_phone_home")));
        mLocList.append(Loc(QContactPhoneNumber::SubTypeLandline, QContactDetail::ContextWork, hbTrId("txt_phob_dblist_phone_work")));
        mLocList.append(Loc(QContactPhoneNumber::SubTypePager, "", hbTrId("txt_phob_dblist_pager")));
        mLocList.append(Loc(QContactPhoneNumber::SubTypeAssistant, "", hbTrId("txt_phob_dblist_assistant")));
        mLocList.append(Loc(QContactPhoneNumber::SubTypeCar, "", hbTrId("txt_phob_dblist_car")));
        mLocList.append(Loc(QContactOnlineAccount::SubTypeSipVoip, "", hbTrId("txt_phob_dblist_internet_telephone")));
        mLocList.append(Loc(QContactOnlineAccount::SubTypeSipVoip, QContactDetail::ContextHome, hbTrId("txt_phob_dblist_internet_telephone_home")));
        mLocList.append(Loc(QContactOnlineAccount::SubTypeSipVoip, QContactDetail::ContextWork, hbTrId("txt_phob_dblist_internet_telephone_work")));
        mLocList.append(Loc(QContactPhoneNumber::SubTypeFacsimile, "", hbTrId("txt_phob_dblist_fax")));
        mLocList.append(Loc(QContactPhoneNumber::SubTypeFacsimile, QContactDetail::ContextHome, hbTrId("txt_phob_dblist_fax_home")));
        mLocList.append(Loc(QContactPhoneNumber::SubTypeFacsimile, QContactDetail::ContextWork, hbTrId("txt_phob_dblist_fax_work")));
        mLocList.append(Loc(QContactOnlineAccount::SubTypeSip, "", hbTrId("txt_phob_dblist_ptt")));

        mContactCardMenuLocList.append(Loc(QContactPhoneNumber::DefinitionName, "", hbTrId("txt_phob_menu_call_mobile")));
        mContactCardMenuLocList.append(Loc(QContactPhoneNumber::DefinitionName, QContactDetail::ContextHome, hbTrId("txt_phob_menu_call_mobile_home")));
        mContactCardMenuLocList.append(Loc(QContactPhoneNumber::DefinitionName, QContactDetail::ContextWork, hbTrId("txt_phob_menu_call_mobile_work")));
        mContactCardMenuLocList.append(Loc(QContactPhoneNumber::SubTypeAssistant, "", hbTrId("txt_phob_menu_call_assistant")));
        mContactCardMenuLocList.append(Loc(QContactPhoneNumber::SubTypeAssistant, QContactDetail::ContextHome, hbTrId("txt_phob_menu_call_assistant")));
        mContactCardMenuLocList.append(Loc(QContactPhoneNumber::SubTypeAssistant, QContactDetail::ContextWork, hbTrId("txt_phob_menu_call_assistant")));   
        mContactCardMenuLocList.append(Loc(QContactPhoneNumber::SubTypeCar, "", hbTrId("txt_phob_menu_call_car_phone")));
        mContactCardMenuLocList.append(Loc(QContactPhoneNumber::SubTypeCar, QContactDetail::ContextHome, hbTrId("txt_phob_menu_call_car_phone")));
        mContactCardMenuLocList.append(Loc(QContactPhoneNumber::SubTypeCar, QContactDetail::ContextWork, hbTrId("txt_phob_menu_call_car_phone")));
        mContactCardMenuLocList.append(Loc(QContactPhoneNumber::SubTypeMobile, QContactDetail::ContextHome,hbTrId("txt_phob_menu_call_mobile_home")));
        mContactCardMenuLocList.append(Loc(QContactPhoneNumber::SubTypeMobile, QContactDetail::ContextWork,hbTrId("txt_phob_menu_call_mobile_work")));
        mContactCardMenuLocList.append(Loc(QContactPhoneNumber::SubTypeMobile, "", hbTrId("txt_phob_menu_call_mobile")));
        mContactCardMenuLocList.append(Loc(QContactPhoneNumber::SubTypeLandline, "",hbTrId("txt_phob_menu_call_phone")));
        mContactCardMenuLocList.append(Loc(QContactPhoneNumber::SubTypeLandline, QContactDetail::ContextHome, hbTrId("txt_phob_menu_call_phone_home")));
        mContactCardMenuLocList.append(Loc(QContactPhoneNumber::SubTypeLandline, QContactDetail::ContextWork, hbTrId("txt_phob_menu_call_phone_work")));
        mContactCardMenuLocList.append(Loc(QContactPhoneNumber::SubTypeFacsimile, "", hbTrId("txt_phob_menu_fax")));
        mContactCardMenuLocList.append(Loc(QContactPhoneNumber::SubTypeFacsimile, QContactDetail::ContextHome, hbTrId("txt_phob_menu_fax_home")));
        mContactCardMenuLocList.append(Loc(QContactPhoneNumber::SubTypeFacsimile, QContactDetail::ContextWork, hbTrId("txt_phob_menu_fax_work")));
        mContactCardMenuLocList.append(Loc(QContactPhoneNumber::SubTypePager, "", hbTrId("txt_phob_menu_call_pager")));
        mContactCardMenuLocList.append(Loc(QContactOnlineAccount::SubTypeSipVoip, "", hbTrId("txt_phob_menu_call_internet_call")));
        mContactCardMenuLocList.append(Loc(QContactOnlineAccount::SubTypeSipVoip, QContactDetail::ContextHome, hbTrId("txt_phob_menu_call_internet_call_home")));
        mContactCardMenuLocList.append(Loc(QContactOnlineAccount::SubTypeSipVoip, QContactDetail::ContextWork, hbTrId("txt_phob_menu_call_internet_call_work")));
        mContactCardMenuLocList.append(Loc(QContactOnlineAccount::SubTypeSip, "", hbTrId("txt_phob_menu_sip")));
        mContactCardMenuLocList.append(Loc(QContactEmailAddress::DefinitionName, "", hbTrId("txt_phob_menu_email")));
        mContactCardMenuLocList.append(Loc(QContactEmailAddress::DefinitionName, QContactDetail::ContextHome, hbTrId("txt_phob_menu_email_home")));
        mContactCardMenuLocList.append(Loc(QContactEmailAddress::DefinitionName, QContactDetail::ContextWork, hbTrId("txt_phob_menu_email_work")));
        
        mContactCardLocList.append(Loc(QContactPhoneNumber::DefinitionName, "", hbTrId("txt_phob_dblist_call_mobile")));
        mContactCardLocList.append(Loc(QContactPhoneNumber::DefinitionName, QContactDetail::ContextHome, hbTrId("txt_phob_dblist_call_mobile_home")));
        mContactCardLocList.append(Loc(QContactPhoneNumber::DefinitionName, QContactDetail::ContextWork, hbTrId("txt_phob_dblist_call_mobile_work")));
        mContactCardLocList.append(Loc(QContactPhoneNumber::SubTypeAssistant, "", hbTrId("txt_phob_dblist_call_assistant")));
        mContactCardLocList.append(Loc(QContactPhoneNumber::SubTypeAssistant, QContactDetail::ContextHome, hbTrId("txt_phob_dblist_call_assistant")));
        mContactCardLocList.append(Loc(QContactPhoneNumber::SubTypeAssistant, QContactDetail::ContextWork, hbTrId("txt_phob_dblist_call_assistant")));   
        mContactCardLocList.append(Loc(QContactPhoneNumber::SubTypeCar, "", hbTrId("txt_phob_dblist_call_car_phone")));
        mContactCardLocList.append(Loc(QContactPhoneNumber::SubTypeCar, QContactDetail::ContextHome, hbTrId("txt_phob_dblist_call_car_phone")));
        mContactCardLocList.append(Loc(QContactPhoneNumber::SubTypeCar, QContactDetail::ContextWork, hbTrId("txt_phob_dblist_call_car_phone")));
        mContactCardLocList.append(Loc(QContactPhoneNumber::SubTypeMobile, QContactDetail::ContextHome,hbTrId("txt_phob_dblist_call_mobile_home")));
        mContactCardLocList.append(Loc(QContactPhoneNumber::SubTypeMobile, QContactDetail::ContextWork,hbTrId("txt_phob_dblist_call_mobile_work")));
        mContactCardLocList.append(Loc(QContactPhoneNumber::SubTypeMobile, "", hbTrId("txt_phob_dblist_call_mobile")));
        mContactCardLocList.append(Loc(QContactPhoneNumber::SubTypeLandline, "",hbTrId("txt_phob_dblist_call_phone")));
        mContactCardLocList.append(Loc(QContactPhoneNumber::SubTypeLandline, QContactDetail::ContextHome, hbTrId("txt_phob_dblist_call_phone_home")));
        mContactCardLocList.append(Loc(QContactPhoneNumber::SubTypeLandline, QContactDetail::ContextWork, hbTrId("txt_phob_dblist_call_phone_work")));
        mContactCardLocList.append(Loc(QContactPhoneNumber::SubTypeFacsimile, "", hbTrId("txt_phob_dblist_fax")));
        mContactCardLocList.append(Loc(QContactPhoneNumber::SubTypeFacsimile, QContactDetail::ContextHome, hbTrId("txt_phob_dblist_fax_home")));
        mContactCardLocList.append(Loc(QContactPhoneNumber::SubTypeFacsimile, QContactDetail::ContextWork, hbTrId("txt_phob_dblist_fax_work")));
        mContactCardLocList.append(Loc(QContactPhoneNumber::SubTypePager, "", hbTrId("txt_phob_dblist_call_pager")));
        mContactCardLocList.append(Loc(QContactOnlineAccount::SubTypeSipVoip, "", hbTrId("txt_phob_dblist_call_internet_call")));
        mContactCardLocList.append(Loc(QContactOnlineAccount::SubTypeSipVoip, QContactDetail::ContextHome, hbTrId("txt_phob_dblist_call_internet_call_home")));
        mContactCardLocList.append(Loc(QContactOnlineAccount::SubTypeSipVoip, QContactDetail::ContextWork, hbTrId("txt_phob_dblist_call_internet_call_work")));
        mContactCardLocList.append(Loc(QContactOnlineAccount::SubTypeSip, "", hbTrId("txt_phob_dblist_sip")));
        mContactCardLocList.append(Loc(QContactEmailAddress::DefinitionName, "", hbTrId("txt_phob_dblist_email")));
        mContactCardLocList.append(Loc(QContactEmailAddress::DefinitionName, QContactDetail::ContextHome, hbTrId("txt_phob_dblist_email_home")));
        mContactCardLocList.append(Loc(QContactEmailAddress::DefinitionName, QContactDetail::ContextWork, hbTrId("txt_phob_dblist_email_work")));
        
        // init mapping of contact card icons
        mContactCardIconList.append(Loc(QContactPhoneNumber::DefinitionName, "", "qtg_large_call_mobile"));
        mContactCardIconList.append(Loc(QContactPhoneNumber::DefinitionName, QContactDetail::ContextHome, "qtg_large_call_mobile_home"));
        mContactCardIconList.append(Loc(QContactPhoneNumber::DefinitionName, QContactDetail::ContextWork, "qtg_large_call_mobile_work"));
        mContactCardIconList.append(Loc(QContactPhoneNumber::SubTypeMobile, "", "qtg_large_call_mobile"));
        mContactCardIconList.append(Loc(QContactPhoneNumber::SubTypeMobile, QContactDetail::ContextHome, "qtg_large_call_mobile_home"));
        mContactCardIconList.append(Loc(QContactPhoneNumber::SubTypeMobile, QContactDetail::ContextWork, "qtg_large_call_mobile_work"));
        mContactCardIconList.append(Loc(QContactPhoneNumber::SubTypeLandline, "", "qtg_large_call_landline"));
        mContactCardIconList.append(Loc(QContactPhoneNumber::SubTypeLandline, QContactDetail::ContextHome, "qtg_large_call_landline_home"));
        mContactCardIconList.append(Loc(QContactPhoneNumber::SubTypeLandline, QContactDetail::ContextWork, "qtg_large_call_landline_work"));
        mContactCardIconList.append(Loc(QContactPhoneNumber::SubTypeFacsimile, "", "qtg_large_call_fax"));
        mContactCardIconList.append(Loc(QContactPhoneNumber::SubTypeFacsimile, QContactDetail::ContextHome, "qtg_large_call_fax_home"));
        mContactCardIconList.append(Loc(QContactPhoneNumber::SubTypeFacsimile, QContactDetail::ContextWork, "qtg_large_call_fax_work"));
        mContactCardIconList.append(Loc(QContactPhoneNumber::SubTypePager, "", "qtg_large_call_pager"));
        mContactCardIconList.append(Loc(QContactPhoneNumber::SubTypeCar, "", "qtg_large_call_car"));
        mContactCardIconList.append(Loc(QContactPhoneNumber::SubTypeAssistant, "", "qtg_large_call_assistant"));
        mContactCardIconList.append(Loc(QContactPhoneNumber::SubTypeDtmfMenu, "", "qtg_large_call_group"));
        mContactCardIconList.append(Loc(QContactOnlineAccount::SubTypeSipVoip, "", "qtg_large_call_voip"));
        mContactCardIconList.append(Loc(QContactOnlineAccount::SubTypeSipVoip, QContactDetail::ContextHome, "qtg_large_call_voip_home"));
        mContactCardIconList.append(Loc(QContactOnlineAccount::SubTypeSipVoip, QContactDetail::ContextWork, "qtg_large_call_voip_work"));
        mContactCardIconList.append(Loc(QContactOnlineAccount::SubTypeSip, "", ""));
        mContactCardIconList.append(Loc(QContactEmailAddress::DefinitionName, "", "qtg_large_email"));
        mContactCardIconList.append(Loc(QContactEmailAddress::DefinitionName, QContactDetail::ContextHome, "qtg_large_email_home"));
        mContactCardIconList.append(Loc(QContactEmailAddress::DefinitionName, QContactDetail::ContextWork, "qtg_large_email_work"));
    }

    QString getMappedDetail(const QString &string) { return mStringMap.value(string); }
    QString getMappedIcon(const QString &string) { return mEditorIconMap.value(string); }
    
	QString getLocString( QString aDefId, QString aContext )
    {
        QString str = aDefId;
        foreach ( Loc loc, mLocList )
        {
            if ( loc.mId == aDefId && loc.mContext == aContext )
            {
                str = loc.mLoc;
                break;
            }
        }
        return str;
    }

    QString getContactCardMenuLocString( QString aDefId, QString aContext )
    {
        QString str;
        foreach ( Loc loc, mContactCardMenuLocList )
        {
            if ( loc.mId == aDefId && loc.mContext == aContext )
            {
                str = loc.mLoc;
                break;
            }
        }
        return str;
    }
    QString getContactCardListLocString( QString aDefId, QString aContext )
    {
        QString str;
        foreach ( Loc loc, mContactCardLocList )
        {
            if ( loc.mId == aDefId && loc.mContext == aContext )
            {
                str = loc.mLoc;
                break;
            }
        }
        return str;
    }
    QString getContactCardIconString( QString aDefId, QString aContext )
    {
        QString str;
        foreach ( Loc loc, mContactCardIconList )
        {
            if ( loc.mId == aDefId && loc.mContext == aContext )
            {
                str = loc.mLoc;
                break;
            }
        }
        return str;
    }

private:
    QMap<QString, QString> mStringMap;
    QMap<QString, QString> mEditorIconMap;
    QMap<QString, QString> mLauncherIconMap;

	QList<Loc> mLocList;    
	QList<Loc> mContactCardMenuLocList;
    QList<Loc> mContactCardLocList;
    QList<Loc> mContactCardIconList;
};

inline Loc::Loc(QString aId, QString aContext, QString aLoc ) :
mId( aId ),
mContext( aContext ),
mLoc( aLoc )
{    
}
inline Loc::~Loc()
{
}
#endif // CNTSTRINGMAPPER_H
