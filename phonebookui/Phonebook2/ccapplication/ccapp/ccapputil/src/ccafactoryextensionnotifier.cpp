/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: CCCaFactoryExtensionNotifier
*
*/


#include "ccafactoryextensionnotifier.h"
#include <ecom.h>
#include <ccaextensionfactory.hrh>
#include <ccaextensionfactory.h>

// --------------------------------------------------------------------------
// CleanupResetAndDestroy
// --------------------------------------------------------------------------
void CleanupResetAndDestroy( TAny* aObj )
    {
    static_cast<RImplInfoPtrArray*>( aObj )->ResetAndDestroy();
    }

// =========================== MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CCCaFactoryExtensionNotifier::CCCaFactoryExtensionNotifier()
// ----------------------------------------------------------------------------
//
inline CCCaFactoryExtensionNotifier::CCCaFactoryExtensionNotifier()
: CActive( EPriorityStandard )
    {
    CActiveScheduler::Add(this);
    }

// ----------------------------------------------------------------------------
// CCCaFactoryExtensionNotifier::~CCCaFactoryExtensionNotifier()
// ----------------------------------------------------------------------------
//
CCCaFactoryExtensionNotifier::~CCCaFactoryExtensionNotifier()
    {
    Cancel();
    if( iEComSession )
        {
        iEComSession->Close();
        }
      
    delete iExtensionFactory;
    }

// ----------------------------------------------------------------------------
// CCCaFactoryExtensionNotifier* CCCaFactoryExtensionNotifier::NewL()
// ----------------------------------------------------------------------------
//
EXPORT_C CCCaFactoryExtensionNotifier* CCCaFactoryExtensionNotifier::NewL()
    {
    CCCaFactoryExtensionNotifier* self =
            new (ELeave) CCCaFactoryExtensionNotifier();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self;
    return self;
    }

// ----------------------------------------------------------------------------
// CCCaFactoryExtensionNotifier::ConstructL()
// ----------------------------------------------------------------------------
//
inline void CCCaFactoryExtensionNotifier::ConstructL()
    {
    iEComSession = &(REComSession::OpenL());
    SetActive();
    iEComSession->NotifyOnChange( iStatus );
    }

// ----------------------------------------------------------------------------
// CCCAExtensionFactory* CCCaFactoryExtensionNotifier::CreateExtensionFactoryL()
// ----------------------------------------------------------------------------
//
void CCCaFactoryExtensionNotifier::CreateExtensionFactoryL()
    {
    RImplInfoPtrArray implementations;
    CleanupStack::PushL( TCleanupItem( CleanupResetAndDestroy,
                                       &implementations) );

    // get the list of available plugins
    REComSession::ListImplementationsL(
                        TUid::Uid( CCCAEXTENSIONFACTORY_ECOM_INTERFACE ),
                        implementations );

    if( implementations.Count() > 0)
        {
        if(!iExtensionFactory)
            {
            CImplementationInformation* implInfo = implementations[0];
            iExtensionFactory =
                    CCCAExtensionFactory::NewL( implInfo->ImplementationUid() );
            // extension factory is created -> notify clients
            iCallBack.CallBack();
            }
        }
    else if( iExtensionFactory )
        {
        delete iExtensionFactory;
        iExtensionFactory = NULL;
        iCallBack.CallBack();  // extension factory is removed -> notify clients
        }
    CleanupStack::PopAndDestroy(); //implementations
    }

// ----------------------------------------------------------------------------
// CCCAExtensionFactory* CCCaFactoryExtensionNotifier::ExtensionFactory()
// ----------------------------------------------------------------------------
//
EXPORT_C CCCAExtensionFactory* CCCaFactoryExtensionNotifier::ExtensionFactory()
    {
    return iExtensionFactory;
    }

// ----------------------------------------------------------------------------
// CCCaFactoryExtensionNotifier::ObserveExtensionFactory( TCallBack& aCallBack )
// ----------------------------------------------------------------------------
//
EXPORT_C void CCCaFactoryExtensionNotifier::ObserveExtensionFactoryL( 
        TCallBack& aCallBack )
    {
    iCallBack = aCallBack;
    CreateExtensionFactoryL();
    }

// ----------------------------------------------------------------------------
// CCCaFactoryExtensionNotifier::DoCancel()
// ----------------------------------------------------------------------------
//
void CCCaFactoryExtensionNotifier::DoCancel()
    {
    iEComSession->CancelNotifyOnChange( iStatus );
    }

// ----------------------------------------------------------------------------
// CCCaFactoryExtensionNotifier::RunL()
// ----------------------------------------------------------------------------
//
void CCCaFactoryExtensionNotifier::RunL()
    {
    if( iStatus.Int() == KErrNone )
        {
        CreateExtensionFactoryL();
        }
    SetActive();
    iEComSession->NotifyOnChange( iStatus );
    }


