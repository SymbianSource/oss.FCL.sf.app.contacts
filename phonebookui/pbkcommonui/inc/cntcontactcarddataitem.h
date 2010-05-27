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

#ifndef CNTCONTACTCARDDATAITEM_H
#define CNTCONTACTCARDDATAITEM_H

#include <QObject>
#include <qmobilityglobal.h>
#include <qtcontacts.h>
#include <hbicon.h>

QTM_USE_NAMESPACE

class CntContactCardDataItem: public QObject
{
    Q_OBJECT    

public:
    
enum CntContactCardPosition
{ 
        EInternalServices = 0,
        ECallMobile = 100,
        ECallMobileSendMessage,
        ECallMobileHome,
        ECallMobileHomeSendMessage,
        ECallMobileWork,
        ECallMobileWorkSendMessage,
        ECallPhone,
        ECallPhoneHome,
        ECallPhoneWork,
        ECallFax,
        ECallFaxHome,
        ECallFaxWork,
        ECallPager,
        ECallAssistant,
        ECallCar,
        EEmail,
        EEmailHome,
        EEmailWork,
        EUrl,
        EUrlHome,
        EUrlWork,
        ECallDynamic,
        ECallDynamicHome,
        ECallDynamicWork,
        EEmailDynamic,
        EEmailDynamicHome,
        EEmailDynamicWork,
        EUrlDynamic,
        EUrlDynamicHome,
        EUrlDynamicWork,
        EInternetDynamic,
        EInternetDynamicHome,
        EInternetDynamicWork,
        EAddressDynamic,
        EAddressDynamicHome,
        EAddressDynamicWork,
        EDynamic,
        EGenericDynamic,
        ELastAction = 200,
        ELastActionHome,
        ELastActionWork,
        EExternalServices = 250,
        ESeparator = 300,
        EInternetTelephone,
        EInternetTelephoneHome,
        EInternetTelephoneWork,
        ESip,
        EAddress,
        EAddressHome,
        EAddressWork,
        ECompanyDetails,
        EBirthday,
        EAnniversary,
        ERingtone,
        ENote,
        ESpouse,
        EChildren,
        EOther = 400
};    
    
public:
    CntContactCardDataItem(const QString& aTitle, int aPosition, bool aIsFocusable);
    virtual ~CntContactCardDataItem();

public:
    void setTitleText(const QString& aTitle);
    void setValueText(const QString& aValueText, Qt::TextElideMode aValueTextElideMode = Qt::ElideRight);
    void setIcon(HbIcon aIcon);
    void setSecondaryIcon(HbIcon aSecondaryIcon);
    void setAction(const QString& aAction);
    void setContactDetail(QContactDetail aDetail);
    void setActionDescriptor(const QContactActionDescriptor& aActionDescriptor);
    void setLongPressText(const QString& aLongPressText);

public:
    QString titleText() { return mTitle; }
    QString valueText() { return mValueText; }
    HbIcon  icon() { return mIcon; }
    HbIcon  secondaryIcon() { return mSecondaryIcon; }
    QString action() { return mAction; }
    QContactDetail detail() { return mDetail; }
    Qt::TextElideMode elideMode() { return mValueTextElideMode; }
    bool isFocusable() { return mIsFocusable; }
    QContactActionDescriptor actionDescriptor() { return mActionDescriptor; }
    QString longPressText() { return mLongPressText; }
    int position() { return mPosition; }
 
private:
    HbIcon                      mIcon;
    HbIcon                      mSecondaryIcon;
    QString                     mTitle;
    QString                     mValueText;
    QString                     mAction;
    QContactDetail              mDetail;
    Qt::TextElideMode           mValueTextElideMode;
    bool                        mIsFocusable;
    QContactActionDescriptor    mActionDescriptor;
    QString                     mLongPressText;
    
public:
    int                         mPosition;
};

#endif /* CNTCONTACTCARDDATAITEM_H */
