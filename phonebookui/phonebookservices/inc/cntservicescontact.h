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

#ifndef CNTSERVICESCONTACT_H_
#define CNTSERVICESCONTACT_H_

#include <QList>
#include <QString>
#include <xqserviceipcmarshal.h>

/*Supported actions*/
const QString KCntActionAll("all");
const QString KCntActionCall("call");
const QString KCntActionSms("sms");
const QString KCntActionEmail("email");

/*Supported filters*/
const QString KCntFilterDisplayAll("all");
const QString KCntFilterDisplayFavorites("favorites");


class CntServicesContact
{
public:
    CntServicesContact() {};
    virtual ~CntServicesContact() {};

    QString mDisplayName;
    QString mPhoneNumber;
    QString mEmailAddress;
    int mContactId; //should be changed to QContactUniqueId

    template <typename Stream> void serialize(Stream &stream) const;
    template <typename Stream> void deserialize(Stream &stream);
};

template <typename Stream> inline void CntServicesContact::serialize(Stream &s) const
{
    s << mDisplayName;
    s << mPhoneNumber;
    s << mEmailAddress;
    s << mContactId;
}

template <typename Stream> inline void CntServicesContact::deserialize(Stream &s)
{
    s >> mDisplayName;
    s >> mPhoneNumber;
    s >> mEmailAddress;
    s >> mContactId;
}

typedef QList<CntServicesContact> CntServicesContactList;

Q_DECLARE_USER_METATYPE(CntServicesContact)
Q_DECLARE_USER_METATYPE_NO_OPERATORS(CntServicesContactList)

#endif /* CNTSERVICESCONTACT_H_ */
