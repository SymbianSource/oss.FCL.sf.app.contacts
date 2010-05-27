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
#include "cntaction.h"

CntAction::CntAction( QString aAction ) : QObject(),
mAction( aAction ),
mContactAction( NULL )
    {
    }

CntAction::~CntAction()
    {
    delete mContactAction;
    }
    
void CntAction::execute( QContact aContact, QContactDetail aDetail )
    {
    QList<QContactActionDescriptor> all = QContactAction::actionDescriptors(mAction, "symbian");
    mContactAction = QContactAction::action( all.first() );
    if ( mContactAction )
        {
        connect(mContactAction, SIGNAL(progress(QContactAction::State, const QVariantMap&)),
                this, SLOT(progress(QContactAction::State, const QVariantMap&)));
        mContactAction->invokeAction( aContact, aDetail );
        }
    }
    
void CntAction::progress( QContactAction::State status, const QVariantMap& result )
    {
    Q_UNUSED(result);
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