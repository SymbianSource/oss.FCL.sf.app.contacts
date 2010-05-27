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

#include "mobcntbrowseraction.h"

#include <qcontacturl.h>
#include <qcontactfilters.h>

//Action class
MobCntBrowserAction::MobCntBrowserAction() : 
    MobCntAction("url")
{
}

MobCntBrowserAction::~MobCntBrowserAction()
{
}

QContactFilter MobCntBrowserAction::contactFilter(const QVariant& value) const
{
     Q_UNUSED(value);
     
    QContactDetailFilter urlFilter;
    urlFilter.setDetailDefinitionName(QContactUrl::DefinitionName); 
    
    return urlFilter;
}

bool MobCntBrowserAction::isDetailSupported(const QContactDetail &detail, const QContact &/*contact*/) const
{
    return (detail.definitionName() == QContactUrl::DefinitionName);
}

QList<QContactDetail> MobCntBrowserAction::supportedDetails(const QContact& contact) const
{
    return contact.details(QContactUrl::DefinitionName);
}

MobCntBrowserAction* MobCntBrowserAction::clone() const
{
     return new MobCntBrowserAction();
}

void MobCntBrowserAction::performAction()
{
    QVariant retValue;
    emitResult(GeneralError, retValue);
    
    /*
    QString service("com.nokia.services.telephony");
    QString type("dial(QString,int)");
    
    performNumberAction(service, type);
    */
}
