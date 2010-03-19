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

#include "mobcntmessageaction.h"

#include <xqservicerequest.h>
#include <qcontactphonenumber.h>
#include <qcontactfilters.h>

//Action class
MobCntMessageAction::MobCntMessageAction() : 
    MobCntAction("message")
{
}

MobCntMessageAction::~MobCntMessageAction()
{
}

MobCntMessageAction* MobCntMessageAction::clone() const
{
	 return new MobCntMessageAction();
}

void MobCntMessageAction::performAction()
{
    QString service("com.nokia.services.hbserviceprovider.conversationview");
    QString type("send(QString,qint32,QString)");
    
    performNumberAction(service, type);
}



