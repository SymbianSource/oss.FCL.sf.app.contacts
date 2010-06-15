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
#include "cntactionlauncher.h"

CntActionLauncher::CntActionLauncher( QString aAction ) : QObject(),
mAction( aAction ),
mContactAction( NULL )
    {
    }

CntActionLauncher::~CntActionLauncher()
    {
    delete mContactAction;
    }
    
void CntActionLauncher::execute( QContact aContact, QContactDetail aDetail )
    {
    QList<QContactActionDescriptor> all = QContactAction::actionDescriptors(mAction, "symbian");
    mContactAction = QContactAction::action( all.first() );
    if ( mContactAction )
        {
        connect(mContactAction, SIGNAL(stateChanged(QContactAction::State)),
                this, SLOT(progress(QContactAction::State)));
        mContactAction->invokeAction( aContact, aDetail );
        }
    }
 
/*!
Launch dynamic action
*/
void CntActionLauncher::execute( QContact aContact, QContactDetail aDetail, QContactActionDescriptor aActionDescriptor )
{
    // detail might be empty -> in that case engine uses the preferred detail for the selected action
    mContactAction = QContactAction::action( aActionDescriptor );
    connect(mContactAction, SIGNAL(stateChanged(QContactAction::State)),
                this, SLOT(progress(QContactAction::State)));
    mContactAction->invokeAction( aContact, aDetail );
}

void CntActionLauncher::progress( QContactAction::State status )
    {
    switch(status)
       {
       case QContactAction::FinishedState:
       case QContactAction::FinishedWithErrorState:
           emit actionExecuted( this );
           break;
       default:
           break;
       }
    }

// End of File
