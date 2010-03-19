/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <qglobal.h>
#include <xqserviceprovider.h>
#include <xqappmgr.h>
#include <xqaiwrequest.h>
#include <xqservicerequest.h>
#include "qthighway_stub_helper.h"

QString qtHighwayStubService;
QString qtHighwayStubMessage;
QString qtHighwayStubOperation;


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//

void QtHighwayStubHelper::reset()
{
    qtHighwayStubService.clear();
    qtHighwayStubMessage.clear();
    qtHighwayStubOperation.clear();
}

QString QtHighwayStubHelper::service()
{
    return qtHighwayStubService;
}

QString QtHighwayStubHelper::message()
{
    return qtHighwayStubMessage;
}

QString QtHighwayStubHelper::operation()
{
    return qtHighwayStubOperation;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
XQServiceProvider::XQServiceProvider( const QString& service, QObject *parent )
{
    Q_UNUSED(service);
    Q_UNUSED(parent);
}

XQServiceProvider::~XQServiceProvider()
{
    
}

void XQServiceProvider::SetPlugin(QObject* impl_plugin)
{
    Q_UNUSED(impl_plugin);
}

void XQServiceProvider::publishAll()
{
    
}

int XQServiceProvider::setCurrentRequestAsync()
{
    return 0;
}
    
bool XQServiceProvider::completeRequest(int index, const QVariant& retValue)
{
    Q_UNUSED(index);
    Q_UNUSED(retValue);
    return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
XQApplicationManager::XQApplicationManager()
{

}

XQApplicationManager::~XQApplicationManager()
{
    
}

XQAiwRequest *XQApplicationManager::create(const QString& interface, const QString& operation, bool embedded)
{
    qtHighwayStubService = interface;
    qtHighwayStubMessage = operation;
    return 0;
}

XQAiwRequest *XQApplicationManager::create(const QString& service, const QString& interface, const QString& operation, bool embedded)
{
    qtHighwayStubService = service;
    qtHighwayStubMessage = interface;
    qtHighwayStubOperation = operation;
    return 0;
}


bool XQAiwRequest::send()
{
    return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
XQServiceRequest::XQServiceRequest(
    const QString& service, const QString& message, const bool& synchronous)
{
    qtHighwayStubService = service;
    qtHighwayStubMessage = message;
}

XQServiceRequest::~XQServiceRequest()
{

}


void XQServiceRequest::setArguments(const QList<QVariant> &arguments)
{
    Q_UNUSED(arguments)
}

QString XQServiceRequest::service() const
{
    return qtHighwayStubService;
}

QString XQServiceRequest::message() const
{
    return qtHighwayStubMessage;
}

bool XQServiceRequest::send(QVariant& retValue) 
{
    return true;
}

bool XQServiceRequest::send() 
{
    return true;
}

void XQServiceRequest::addArg(const QVariant& v)
{

}
