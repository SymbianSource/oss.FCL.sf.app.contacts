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
 * Description: This class offers contact viewing UI services.
 *
 */

#include "cntserviceproviderviewing.h"
#include "cntservices.h"
#include "cntservicestrings.h"
#include "cntdebug.h"

#include <qapplication.h>
#include <QPixmap>
#include <QFile>
#include <QUrl>

CntServiceProviderViewing::CntServiceProviderViewing(CntServices& aServices, QObject *parent):
    XQServiceProvider(QLatin1String(KCntServiceInterfaceView), parent),
    mServices(aServices),
    mCurrentRequestIndex(0)
{
    CNT_ENTRY
    publishAll();
    connect(this, SIGNAL(clientDisconnected()), &mServices, SLOT(quitApp()));
    CNT_EXIT
}

CntServiceProviderViewing::~CntServiceProviderViewing()
{
    CNT_ENTRY
    CNT_EXIT
}

void CntServiceProviderViewing::openContactCard( int aContactId )
    {
    CNT_ENTRY
    mCurrentRequestIndex = setCurrentRequestAsync();
    mServices.setQuittable(requestInfo().isEmbedded());
    mServices.launchContactCard( aContactId, *this );
    CNT_EXIT
    }

void CntServiceProviderViewing::openTemporaryContactCard( QString aFieldName, QString aFieldValue )
    {
    CNT_ENTRY
    mCurrentRequestIndex = setCurrentRequestAsync();
    mServices.setQuittable(requestInfo().isEmbedded());
    mServices.launchTemporaryContactCard( aFieldName, aFieldValue, *this );
    CNT_EXIT
    }

void CntServiceProviderViewing::openGroup( int aContactId )
    {
    CNT_ENTRY
    mCurrentRequestIndex = setCurrentRequestAsync();
    mServices.setQuittable(requestInfo().isEmbedded());
    mServices.launchGroupMemberView( aContactId, *this );
    CNT_EXIT
    }

void CntServiceProviderViewing::CompleteServiceAndCloseApp(const QVariant& retValue)
    {
    CNT_ENTRY
    connect(this, SIGNAL(returnValueDelivered()), qApp, SLOT(quit()));
    if ( mCurrentRequestIndex != 0 )
    {
        const bool success = completeRequest(mCurrentRequestIndex, retValue);
        if ( !success )
        {
            CNT_LOG_ARGS("Failed to complete highway request.");
        }
        mCurrentRequestIndex = 0;
    }
    CNT_EXIT
    }
