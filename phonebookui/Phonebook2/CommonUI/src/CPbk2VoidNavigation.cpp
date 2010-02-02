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
* Description:  Phonebook 2 no navigation navigator.
*
*/


#include "CPbk2VoidNavigation.h"

// Phonebook 2
#include "CPbk2IconInfo.h"
#include <CPbk2AppUiBase.h>
#include <CPbk2IconFactory.h>
#include <CPbk2TabGroupContainer.h>
#include <CPbk2IconInfoContainer.h>
#include <Pbk2UID.h>

// Virtual Phonebook
#include <MVPbkContactLink.h>
#include <MVPbkContactViewBase.h>

// System includes
#include <aknnavide.h>
#include <eikspane.h>
#include <akntabgrp.h>
#include <AknsUtils.h>

// Debugging headers
#include <Pbk2Debug.h>

// --------------------------------------------------------------------------
// CPbk2VoidNavigation::CPbk2VoidNavigation
// --------------------------------------------------------------------------
//
CPbk2VoidNavigation::CPbk2VoidNavigation
        ( TUid aViewId, MPbk2NavigationObserver& aObserver,
          MPbk2ContactNavigationLoader& aNavigationLoader,
          TArray<MVPbkContactStore*> aStoreList ) :
            CPbk2NavigationBase( aObserver, aNavigationLoader, aStoreList ),
            iViewId( aViewId )
    {
    }

// --------------------------------------------------------------------------
// CPbk2VoidNavigation::~CPbk2VoidNavigation
// --------------------------------------------------------------------------
//
CPbk2VoidNavigation::~CPbk2VoidNavigation()
    {
    }

// --------------------------------------------------------------------------
// CPbk2VoidNavigation::NewL
// --------------------------------------------------------------------------
//
CPbk2VoidNavigation* CPbk2VoidNavigation::NewL
        ( TUid aViewId, MPbk2NavigationObserver& aObserver,
          MPbk2ContactNavigationLoader& aNavigationLoader,
          TArray<MVPbkContactStore*> aStoreList )
    {
    CPbk2VoidNavigation* self = new ( ELeave ) CPbk2VoidNavigation
        ( aViewId, aObserver, aNavigationLoader, aStoreList );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2VoidNavigation::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2VoidNavigation::ConstructL()
    {
    BaseConstructL();
    }

// --------------------------------------------------------------------------
// CPbk2VoidNavigation::UpdateNaviIndicatorsL
// --------------------------------------------------------------------------
//
void CPbk2VoidNavigation::UpdateNaviIndicatorsL
        ( const MVPbkContactLink& /*aContactLink*/ ) const
    {
    // Get navigation decorator
    CAknNavigationControlContainer* naviPane =
        static_cast<CAknNavigationControlContainer*>
            ( CEikonEnv::Static()->AppUiFactory()->StatusPane()->ControlL
                ( TUid::Uid( EEikStatusPaneUidNavi ) ) );

    CAknNavigationDecorator* decorator = naviPane->Top();

    if ( decorator )
        {
        decorator->MakeScrollButtonVisible( ETrue );
        }

    // Update the tabgroup text
    SetTabTextToEmptyL( iViewId );

    naviPane->DrawNow();
    }

// --------------------------------------------------------------------------
// CPbk2VoidNavigation::HandleCommandKeyL
// --------------------------------------------------------------------------
//
TBool CPbk2VoidNavigation::HandleCommandKeyL
        ( const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/ )
    {
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2VoidNavigation::SetTabTextToEmptyL
// Sets the tab text for a view.
// --------------------------------------------------------------------------
//
void CPbk2VoidNavigation::SetTabTextToEmptyL
        ( const TUid aViewId ) const
    {
    MPbk2AppUi* appUi = Phonebook2::Pbk2AppUi();

    CPbk2TabGroupContainer* tabGroups = appUi->TabGroups();

    TPbk2IconId iconId( TUid::Uid( KPbk2UID3 ), aViewId.iUid );
    const CPbk2IconInfo* iconInfo = NULL;

    if ( tabGroups )
        {
        iconInfo = tabGroups->TabIcons().Find( iconId );
        }

    if ( iconInfo )
        {
        CAknNavigationDecorator* decorator =
            tabGroups->TabGroupFromViewId( aViewId.iUid );
        CAknTabGroup* aknTabGroup = static_cast<CAknTabGroup*>
            ( decorator->DecoratedControl() );

        CFbsBitmap* bitmap = NULL;
        CFbsBitmap* mask = NULL;

        CPbk2IconFactory* factory =
            CPbk2IconFactory::NewLC( tabGroups->TabIcons() );
        factory->CreateIconLC(
            iconId, *AknsUtils::SkinInstance(), bitmap, mask );

        aknTabGroup->ReplaceTabL( aViewId.iUid, bitmap, mask );
        CleanupStack::Pop( 2 ); // mask, bitmap
        CleanupStack::PopAndDestroy( factory );
        }
    }


// End of File
