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
* Description:  Phonebook 2 contact navigator.
*
*/


#include "CPbk2ContactNavigation.h"

// Phonebook 2
#include "CPbk2IconInfo.h"
#include <MPbk2NavigationObserver.h>
#include <CPbk2IconFactory.h>
#include <CPbk2AppUiBase.h>
#include <CPbk2TabGroupContainer.h>
#include <CPbk2IconInfoContainer.h>
#include <Pbk2UID.h>

// Virtual Phonebook
#include <MVPbkContactViewBase.h>
#include <MVPbkViewContact.h>

// System includes
#include <aknnavide.h>
#include <eikspane.h>
#include <StringLoader.h>
#include <akntabgrp.h>
#include <AknsUtils.h>

// Debugging headers
#include <Pbk2Debug.h>

/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS

const TInt KOneContact = 1;
const TInt KZeroContacts = 0;
const TInt KNaviDirectionLeft = -1;
const TInt KNaviDirectionRight = 1;
const TInt KNaviDirectionNone = 0;
const TInt KGranularity( 2 );

#ifdef _DEBUG

enum TPanicCode
    {
    EPanicPreCond_NavigateContactL = 1,
    };

void Panic(TPanicCode aReason)
    {
    _LIT( KPanicText, "CPbk2ContactNavigation" );
    User::Panic( KPanicText, aReason );
    }

#endif // _DEBUG

}  /// namespace


// --------------------------------------------------------------------------
// CPbk2ContactNavigation::CPbk2ContactNavigation
// --------------------------------------------------------------------------
//
CPbk2ContactNavigation::CPbk2ContactNavigation
        ( TUid aViewId, MPbk2NavigationObserver& aObserver,
          MPbk2ContactNavigationLoader& aNavigationLoader,
          TArray<MVPbkContactStore*> aStoreList,
          const TInt aNavigatorFormatResourceId ) :
            CPbk2NavigationBase( aObserver, aNavigationLoader, aStoreList ),
            iViewId( aViewId ),
            iNavigatorFormatResourceId( aNavigatorFormatResourceId )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactNavigation::~CPbk2ContactNavigation
// --------------------------------------------------------------------------
//
CPbk2ContactNavigation::~CPbk2ContactNavigation()
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactNavigation::NewL
// --------------------------------------------------------------------------
//
CPbk2ContactNavigation* CPbk2ContactNavigation::NewL
        ( TUid aViewId, MPbk2NavigationObserver& aObserver,
          MPbk2ContactNavigationLoader& aNavigationLoader,
          TArray<MVPbkContactStore*> aStoreList,
          const TInt aNavigatorFormatResourceId )
    {
    CPbk2ContactNavigation* self = new ( ELeave ) CPbk2ContactNavigation
        ( aViewId, aObserver, aNavigationLoader, aStoreList,
          aNavigatorFormatResourceId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactNavigation::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2ContactNavigation::ConstructL()
    {
    BaseConstructL();
    }

// --------------------------------------------------------------------------
// CPbk2ContactNavigation::UpdateNaviIndicatorsL
// --------------------------------------------------------------------------
//
void CPbk2ContactNavigation::UpdateNaviIndicatorsL
        ( const MVPbkContactLink& aContactLink ) const
    {
    // Get navigation decorator
    CAknNavigationControlContainer* naviPane =
        static_cast<CAknNavigationControlContainer*>
            ( CEikonEnv::Static()->AppUiFactory()->StatusPane()->ControlL
                ( TUid::Uid( EEikStatusPaneUidNavi ) ) );

    CAknNavigationDecorator* decorator = naviPane->Top();

    if ( decorator )
        {
        decorator->SetNaviDecoratorObserver
            (  const_cast<CPbk2ContactNavigation*>( this ) );
        CAknNavigationDecorator::TControlType controlType = 
            decorator->ControlType();
        // Set ScrollButtonVisible to ETrue except
        // the controlType is CAknNavigationDecorator::ENaviLabel
        if( CAknNavigationDecorator::ENaviLabel != controlType )
            {
            decorator->MakeScrollButtonVisible( ETrue );
            }
        }

    TInt index = 0;
    TInt contactCount = 1;
    if ( ScrollView() )
        {
        index = ScrollView()->IndexOfLinkL( aContactLink );
        contactCount = ScrollView()->ContactCountL();
        }

    // Create the navigation text
    CArrayFixFlat<TInt>* values =
        new ( ELeave ) CArrayFixFlat<TInt>( KGranularity );
    CleanupStack::PushL( values );
    values->AppendL( index + 1 );    // current contact
    values->AppendL( contactCount ); // max scrollable contacts
    HBufC* buf = StringLoader::LoadLC
        ( iNavigatorFormatResourceId, *values );

    // Update the tabgroup text
    SetTabTextL( iViewId, *buf );
    CleanupStack::PopAndDestroy( 2 ); // values, buf

    // Set navigation pointers when there's something to scroll
    if (decorator)
        {
        if ( contactCount > KOneContact )
            {
            decorator->SetScrollButtonDimmed(
                CAknNavigationDecorator::ELeftButton, EFalse );
            decorator->SetScrollButtonDimmed(
                CAknNavigationDecorator::ERightButton, EFalse );
            }
        else
            {
            decorator->SetScrollButtonDimmed(
                CAknNavigationDecorator::ELeftButton, ETrue );
            decorator->SetScrollButtonDimmed(
                CAknNavigationDecorator::ERightButton, ETrue );
            }
        }
    naviPane->DrawNow();
    }

// --------------------------------------------------------------------------
// CPbk2ContactNavigation::HandleCommandKeyL
// --------------------------------------------------------------------------
//
TBool CPbk2ContactNavigation::HandleCommandKeyL
        ( const TKeyEvent& aKeyEvent, TEventCode /*aType*/ )
    {
    TBool ret = EFalse;

    // Initialize directionality to none
    TInt dir = KNaviDirectionNone;

    switch ( aKeyEvent.iCode )
        {
        case EKeyLeftArrow:
            {
            dir = KNaviDirectionLeft;
            ret = ETrue;
            break;
            }
        case EKeyRightArrow:
            {
            dir = KNaviDirectionRight;
            ret = ETrue;
            break;
            }
        }

    if ( dir != KNaviDirectionNone )
        {
        // Handle mirrored layout by negating the directionality
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            dir = -1 * dir;
            }
        NavigateContactL( dir );
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2ContactNavigation::ContactAddedToView
// --------------------------------------------------------------------------
//
void CPbk2ContactNavigation::ContactAddedToView
        ( MVPbkContactViewBase& aView, TInt aIndex,
          const MVPbkContactLink& aContactLink )
    {
    CPbk2NavigationBase::ContactAddedToView( aView, aIndex, aContactLink );
    
    TRAPD( err, UpdateAfterViewEventL() );
    if ( err != KErrNone )
        {
        CCoeEnv::Static()->HandleError( err );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactNavigation::ContactRemovedFromView
// --------------------------------------------------------------------------
//
void CPbk2ContactNavigation::ContactRemovedFromView
        ( MVPbkContactViewBase& aView, TInt aIndex,
          const MVPbkContactLink& aContactLink )
    {
    CPbk2NavigationBase::ContactRemovedFromView( aView, aIndex, 
        aContactLink );
    
    TRAPD( err, UpdateAfterViewEventL() );
    if ( err != KErrNone )
        {
        CCoeEnv::Static()->HandleError( err );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactNavigation::HandleNaviDecoratorEventL
// --------------------------------------------------------------------------
//
void CPbk2ContactNavigation::HandleNaviDecoratorEventL( TInt aEventID )
    {
    if ( AknLayoutUtils::PenEnabled() )
        {
        TInt dir = KNaviDirectionNone;

        switch ( aEventID )
            {
            case EAknNaviDecoratorEventRightTabArrow:
                {
                dir = KNaviDirectionRight;
                break;
                }
            case EAknNaviDecoratorEventLeftTabArrow:
                {
                dir = KNaviDirectionLeft;
                break;
                }
            }

        if ( dir != KNaviDirectionNone )
            {
            // Handle mirrored layout by negating the directionality
            if ( AknLayoutUtils::LayoutMirrored() )
                {
                dir = -1 * dir;
                }
            NavigateContactL( dir );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactNavigation::NavigateContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactNavigation::NavigateContactL( TInt aDirection )
    {
    __ASSERT_DEBUG(ScrollView(),
        Panic(EPanicPreCond_NavigateContactL));

    if (ScrollView())
        {
        // Get the contact link
        MVPbkContactLink* currentContact = NavigationLoader().ContactLinkLC();
        TInt contactIndex = KErrNotSupported; // KErrNotFound can't be used

        if ( currentContact )
            {
            contactIndex = ScrollView()->IndexOfLinkL
                ( *currentContact ) + aDirection;
            }
        CleanupStack::PopAndDestroy(); // currentContact

        if ( contactIndex != KErrNotSupported )
            {
            const TInt contactCount = ScrollView()->ContactCountL();
            if ( contactCount > KOneContact )
                {
                if ( contactIndex < KZeroContacts )
                    {
                    contactIndex = contactCount + contactIndex;
                    }
                else if ( contactIndex >= contactCount )
                    {
                    contactIndex = contactIndex - contactCount;
                    }

                MVPbkContactLink* contactLink=
                    ScrollView()->CreateLinkLC( contactIndex );
                if ( contactLink )
                    {
                    NavigationLoader().ChangeContactL( *contactLink );
                    }
                CleanupStack::PopAndDestroy(); // contactLink
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactNavigation::UpdateAfterViewEventL
// --------------------------------------------------------------------------
//
void CPbk2ContactNavigation::UpdateAfterViewEventL() const
    {
    MVPbkContactLink* currentContact = NavigationLoader().ContactLinkLC();
    if ( currentContact )
        {
        UpdateNaviIndicatorsL( *currentContact );
        }
    CleanupStack::PopAndDestroy(); // currentContact
    }

// --------------------------------------------------------------------------
// CPbk2ContactNavigation::SetTabTextL
// Sets the tab text for a view.
// --------------------------------------------------------------------------
//
void CPbk2ContactNavigation::SetTabTextL
        ( const TUid aViewId, const TDesC& aText ) const
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

        aknTabGroup->ReplaceTabL( aViewId.iUid, aText, bitmap, mask );
        CleanupStack::Pop( 2 ); // mask, bitmap
        CleanupStack::PopAndDestroy( factory );
        }
    }

// End of File
