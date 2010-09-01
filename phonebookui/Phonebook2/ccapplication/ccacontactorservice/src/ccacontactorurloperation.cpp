/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of url launching operation
*
*/


// INCLUDE FILES
#include "ccacontactorheaders.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCCAContactorURLOperation::CCCAContactorURLOperation()
// -----------------------------------------------------------------------------
//
CCCAContactorURLOperation::CCCAContactorURLOperation(const TDesC& aParam) : CCCAContactorOperation(aParam)
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CCCAContactorURLOperation::~CCCAContactorURLOperation()
// -----------------------------------------------------------------------------
//
CCCAContactorURLOperation::~CCCAContactorURLOperation()
    {
    }

// -----------------------------------------------------------------------------
// CCCAContactorURLOperation::NewLC()
// -----------------------------------------------------------------------------
//
CCCAContactorURLOperation* CCCAContactorURLOperation::NewLC(const TDesC& aParam)
    {
    CCCAContactorURLOperation* self = new (ELeave)CCCAContactorURLOperation(aParam);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CCCAContactorURLOperation::NewL()
// -----------------------------------------------------------------------------
//
CCCAContactorURLOperation* CCCAContactorURLOperation::NewL(const TDesC& aParam)
    {
    CCCAContactorURLOperation* self=CCCAContactorURLOperation::NewLC(aParam);
    CleanupStack::Pop(); // self;
    return self;
    }

// -----------------------------------------------------------------------------
// CCCAContactorURLOperation::ConstructL()
// -----------------------------------------------------------------------------
//
void CCCAContactorURLOperation::ConstructL()
    {

    }

// -----------------------------------------------------------------------------
// CCCAContactorURLOperation::ExecuteLD()
// -----------------------------------------------------------------------------
//
void CCCAContactorURLOperation::ExecuteLD()
    {
    CleanupStack::PushL(this);
    _LIT(KFourAndSpace, "4 ");
    const TInt KBrowserUid = 0x10008D39;
    HBufC* param = HBufC::NewLC( iParam.Size() + 2 );
    param->Des().Copy( KFourAndSpace );
    param->Des().Append( iParam );
    TUid id( TUid::Uid( KBrowserUid ) );
    TApaTaskList taskList( CEikonEnv::Static()->WsSession() );
    TApaTask task = taskList.FindApp( id );
    if ( task.Exists() )
        {
        HBufC8* param8 = HBufC8::NewLC( param->Length() );
        param8->Des().Append( *param );
        task.SendMessage( TUid::Uid( 0 ), *param8 ); // Uid is not used
        CleanupStack::PopAndDestroy(param8);
        }
    else 
        {
        RApaLsSession appArcSession;
        User::LeaveIfError( appArcSession.Connect() ); // connect to AppArc server
        CleanupClosePushL( appArcSession );
        TThreadId id;
        User::LeaveIfError
            ( appArcSession.StartDocument( *param, TUid::Uid( KBrowserUid ), id ) );
        CleanupStack::PopAndDestroy( &appArcSession );
        }
    CleanupStack::PopAndDestroy(); // param
    CleanupStack::PopAndDestroy(this);
    }
// Enf of File
