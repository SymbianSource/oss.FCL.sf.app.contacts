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

#include "cntbrowseraction.h"

#include <qcontacturl.h>
#include <qcontactfilters.h>

//Action class
CntBrowserAction::CntBrowserAction() : 
    CntAction("url")
{
}

CntBrowserAction::~CntBrowserAction()
{
}

QContactFilter CntBrowserAction::contactFilter(const QVariant& value) const
{
     Q_UNUSED(value);
     
    QContactDetailFilter urlFilter;
    urlFilter.setDetailDefinitionName(QContactUrl::DefinitionName); 
    
    return urlFilter;
}

bool CntBrowserAction::isDetailSupported(const QContactDetail &detail, const QContact &/*contact*/) const
{
    return (detail.definitionName() == QContactUrl::DefinitionName);
}

QList<QContactDetail> CntBrowserAction::supportedDetails(const QContact& contact) const
{
    return contact.details(QContactUrl::DefinitionName);
}

CntBrowserAction* CntBrowserAction::clone() const
{
     return new CntBrowserAction();
}

void CntBrowserAction::performAction()
{
    QVariant retValue;
    emitResult(GeneralError, retValue);
    
    /*
    QString service("com.nokia.services.telephony");
    QString type("dial(QString,int)");
    
    performNumberAction(service, type);
    */
}
