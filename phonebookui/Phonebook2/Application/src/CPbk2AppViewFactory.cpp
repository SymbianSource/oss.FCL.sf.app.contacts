/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 application view factory.
*
*/


// INCLUDE FILES
#include "CPbk2AppViewFactory.h"

// Phonebook 2
#include "CPbk2AppUi.h"
#include "CPbk2ContactInfoAppView.h"
#include "CPbk2SettingsView.h"
#include <Pbk2ViewId.hrh>
#include <CPbk2UIExtensionManager.h>
#include <MPbk2UIExtensionFactory.h>
#include <MPbk2ApplicationServices.h>

// Debugging headers
#include <Pbk2Profile.h>

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG

enum TPanicCode
    {
    EPanicUnknownViewId = 1,
    EPanicViewIdMismatch
    };

void Panic(TPanicCode aReason)
    {
    _LIT( KPanicText, "CPbk2AppViewFactory" );
    User::Panic(KPanicText,aReason);
    }

#endif // _DEBUG

} /// namespace


// --------------------------------------------------------------------------
// CPbk2AppViewFactory::CPbk2AppViewFactory
// --------------------------------------------------------------------------
//
inline CPbk2AppViewFactory::CPbk2AppViewFactory( CPbk2AppUi& aAppUi ):
        iAppUi( aAppUi )
    {
    }

// --------------------------------------------------------------------------
// CPbk2AppViewFactory::~CPbk2AppViewFactory
// --------------------------------------------------------------------------
//
CPbk2AppViewFactory::~CPbk2AppViewFactory()
    {
    Release( iExtensionManager );
    }

// --------------------------------------------------------------------------
// CPbk2AppViewFactory::NewL
// --------------------------------------------------------------------------
//
CPbk2AppViewFactory* CPbk2AppViewFactory::NewL( CPbk2AppUi& aAppUi )
    {
    CPbk2AppViewFactory* self = NewLC( aAppUi );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2AppViewFactory::NewLC
// --------------------------------------------------------------------------
//
CPbk2AppViewFactory* CPbk2AppViewFactory::NewLC( CPbk2AppUi& aAppUi )
    {
    CPbk2AppViewFactory* self = new ( ELeave ) CPbk2AppViewFactory( aAppUi );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2AppViewFactory::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2AppViewFactory::ConstructL()
    {
    iExtensionManager = CPbk2UIExtensionManager::InstanceL();
    }

// --------------------------------------------------------------------------
// CPbk2AppViewFactory::CreateAppViewL
// --------------------------------------------------------------------------
//
CAknView* CPbk2AppViewFactory::CreateAppViewL( TUid aId )
    {
    PBK2_PROFILE_START(Pbk2Profile::EAppViewFactoryCreateAppViewL);
    CAknView* view = NULL;

    // Create the view object
    switch ( aId.iUid )
        {
        case EPbk2ContactInfoViewId:
            {
            PBK2_PROFILE_START
                ( Pbk2Profile::EAppViewFactoryContactInfoAppViewNewL );
            view = CPbk2ContactInfoAppView::NewL
                ( iAppUi.ApplicationServices().ContactManager() );
            PBK2_PROFILE_END
                ( Pbk2Profile::EAppViewFactoryContactInfoAppViewNewL );
            break;
            }
        case EPbk2SettingsViewId:
            {
            PBK2_PROFILE_START
                ( Pbk2Profile::EAppViewFactorySettingsAppViewNewL );
            view = CPbk2SettingsView::NewL();
            PBK2_PROFILE_END
                ( Pbk2Profile::EAppViewFactorySettingsAppViewNewL );
            break;
            }
        default:
            {
            // If the view is not created by any of the cases above,
            // we know that the view is brought by some extension because
            // the views are created based on the view graph definition
            view = iExtensionManager->FactoryL()->
                CreatePhonebook2ViewL( aId );
            break;
            }
        }

    __ASSERT_DEBUG( view, Panic( EPanicUnknownViewId ) );
    __ASSERT_DEBUG( view->Id() == aId, Panic( EPanicViewIdMismatch ) );

    // Return the created view object
    PBK2_PROFILE_END(Pbk2Profile::EAppViewFactoryCreateAppViewL);
    return view;
    }

// --------------------------------------------------------------------------
// CPbk2AppViewFactory::CreateAppViewLC
// --------------------------------------------------------------------------
//
CAknView* CPbk2AppViewFactory::CreateAppViewLC( TUid aId )
    {
    CAknView* view = CreateAppViewL( aId );
    CleanupStack::PushL( view );
    return view;
    }

// End of File
