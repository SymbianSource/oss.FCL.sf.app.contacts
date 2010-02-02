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
* Description:  Phonebook 2 names list state factory.
*
*/


#include "CPbk2NamesListStateFactory.h"

// Phonebook 2
#include "MPbk2NamesListState.h"
#include "CPbk2NamesListEmptyState.h"
#include "CPbk2NamesListNotReadyState.h"
#include "CPbk2NamesListReadyState.h"
#include "CPbk2NamesListFilteredState.h"
#include "CPbk2NamesListStartupState.h"
#include "CPbk2NamesListHiddenState.h"
#include "CPbk2ContactViewListBox.h"
#include "CPbk2NamesListControl.h"
#include "MPbk2UiControlEventSender.h"
#include <MPbk2ControlObserver.h>
#include <MPbk2UiControlCmdItem.h>
#include "CPbk2PredictiveSearchFilter.h"

// Debugging headers
#include <Pbk2Debug.h>

/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicInvalidState = 1
    };

void Panic(TPanicCode aReason)
    {
    _LIT( KPanicText, "CPbk2NamesListStateFactory" );
    User::Panic( KPanicText, aReason );
    }

#endif // _DEBUG


} /// namespace


// --------------------------------------------------------------------------
// CPbk2NamesListStateFactory::CPbk2NamesListStateFactory
// --------------------------------------------------------------------------
//
inline CPbk2NamesListStateFactory::CPbk2NamesListStateFactory(
        const CCoeControl& aContainer,
        CPbk2ContactViewListBox& aListBox,
        MPbk2FilteredViewStack& aViewStack,
        CAknSearchField* aFindBox,
        CPbk2ThumbnailLoader& aThumbnailLoader,
        MPbk2UiControlEventSender& aEventSender,
        MPbk2ContactNameFormatter& aNameFormatter,
        RPointerArray<MPbk2UiControlCmdItem>& aCommandItems,
        CPbk2PredictiveSearchFilter& aSearchFilter ):
            iContainer( aContainer ),
            iListBox( aListBox ),
            iViewStack( aViewStack ),
            iFindBox( aFindBox ),
            iThumbnailLoader( aThumbnailLoader ),
            iEventSender( aEventSender ),
            iNameFormatter( aNameFormatter ),
            iCommandItems( aCommandItems ),
            iSearchFilter(aSearchFilter)
    {
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStateFactory::~CPbk2NamesListStateFactory
// --------------------------------------------------------------------------
//
CPbk2NamesListStateFactory::~CPbk2NamesListStateFactory()
    {
    delete iSendControlEvent;
    iStateArray.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStateFactory::NewL
// --------------------------------------------------------------------------
//
CPbk2NamesListStateFactory* CPbk2NamesListStateFactory::NewL(
        const CCoeControl& aContainer,
        CPbk2ContactViewListBox& aListBox,
        MPbk2FilteredViewStack& aViewStack,
        CAknSearchField* aFindBox,
        CPbk2ThumbnailLoader& aThumbnailLoader,
        MPbk2UiControlEventSender& aEventSender,
        MPbk2ContactNameFormatter& aNameFormatter,
        RPointerArray<MPbk2UiControlCmdItem>& aCommandItems,
        CPbk2PredictiveSearchFilter& aSearchFilter )
    {
    CPbk2NamesListStateFactory* self =
        new ( ELeave ) CPbk2NamesListStateFactory
            ( aContainer, aListBox, aViewStack,
              aFindBox, aThumbnailLoader, aEventSender,
              aNameFormatter, aCommandItems, aSearchFilter );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self  );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStateFactory::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2NamesListStateFactory::ConstructL()
    {
    iSendControlEvent = CIdle::NewL( CActive::EPriorityStandard );
    }
    
// --------------------------------------------------------------------------
// CPbk2NamesListStateFactory::ActivateStartupStateL
// --------------------------------------------------------------------------
//
MPbk2NamesListState& CPbk2NamesListStateFactory::ActivateStartupStateL
        ( MPbk2NamesListState* aPreviousState )
    {
    return ActivateStateL
        ( CPbk2NamesListControl::EStateStartup, aPreviousState );
    }
    
// --------------------------------------------------------------------------
// CPbk2NamesListStateFactory::ActivateNotReadyStateL
// --------------------------------------------------------------------------
//
MPbk2NamesListState& CPbk2NamesListStateFactory::ActivateNotReadyStateL
        ( MPbk2NamesListState* aPreviousState )
    {
    return ActivateStateL
        ( CPbk2NamesListControl::EStateNotReady, aPreviousState );
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStateFactory::ActivateEmptyStateL
// --------------------------------------------------------------------------
//
MPbk2NamesListState& CPbk2NamesListStateFactory::ActivateEmptyStateL
        (MPbk2NamesListState* aPreviousState)
    {
    return ActivateStateL
        ( CPbk2NamesListControl::EStateEmpty, aPreviousState );
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStateFactory::ActivateReadyStateL
// --------------------------------------------------------------------------
//
MPbk2NamesListState& CPbk2NamesListStateFactory::ActivateReadyStateL
        ( MPbk2NamesListState* aPreviousState )
    {
    return ActivateStateL
        ( CPbk2NamesListControl::EStateReady, aPreviousState );
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStateFactory::ActivateFilteredStateL
// --------------------------------------------------------------------------
//
MPbk2NamesListState& CPbk2NamesListStateFactory::ActivateFilteredStateL
        ( MPbk2NamesListState* aPreviousState )
    {
    return ActivateStateL
        ( CPbk2NamesListControl::EStateFiltered, aPreviousState );
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStateFactory::ActivateHiddenStateL
// --------------------------------------------------------------------------
//
MPbk2NamesListState& CPbk2NamesListStateFactory::ActivateHiddenStateL(
                MPbk2NamesListState* aPreviousState )
    {
    return ActivateStateL
            ( CPbk2NamesListControl::EStateHidden, aPreviousState );
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStateFactory::ActivateStateL
// --------------------------------------------------------------------------
//
MPbk2NamesListState& CPbk2NamesListStateFactory::ActivateStateL
        ( TInt aState, MPbk2NamesListState* aPreviousState )
    {    
    TBool found = EFalse;
    MPbk2NamesListState* stateObject = NULL;

    // Check wheter the state has already been created
    for ( TInt i = 0; i < iStateArray.Count() && !found; ++i )
        {
        stateObject = iStateArray[i];
        if ( stateObject->NamesListState() == aState )
            {
            found = ETrue;
            }
        }

    // If the state was not found, create it
    if ( !found )
        {
        switch ( aState )
            {
            case CPbk2NamesListControl::EStateStartup:
                {
                stateObject = CPbk2NamesListStartupState::NewLC
                    ( &iContainer, iListBox, iFindBox );
                iStateArray.AppendL( stateObject );
                CleanupStack::Pop(); // stateObject
                break;
                }
                
            case CPbk2NamesListControl::EStateNotReady:
                {
                stateObject = CPbk2NamesListNotReadyState::NewLC
                    ( &iContainer, iListBox, iFindBox );
                iStateArray.AppendL( stateObject );
                CleanupStack::Pop(); // stateObject
                break;
                }

            case CPbk2NamesListControl::EStateEmpty:
                {
                stateObject = CPbk2NamesListEmptyState::NewLC
                    ( &iContainer, iListBox, iFindBox, iThumbnailLoader, iCommandItems );  
                iStateArray.AppendL( stateObject );
                CleanupStack::Pop(); // stateObject
                break;
                }

            case CPbk2NamesListControl::EStateReady:
                {
                stateObject = CPbk2NamesListReadyState::NewLC
                    ( &iContainer, iListBox, iViewStack, iFindBox, 
                      iThumbnailLoader, iEventSender, iNameFormatter,
                      iCommandItems, iSearchFilter );
                iStateArray.AppendL( stateObject );
                CleanupStack::Pop(); // stateObject
                break;
                }

            case CPbk2NamesListControl::EStateFiltered:
                {
                stateObject = CPbk2NamesListFilteredState::NewLC
                    ( &iContainer, iListBox, iViewStack, iFindBox, 
                      iEventSender, iNameFormatter, iSearchFilter, iCommandItems );  
                iStateArray.AppendL( stateObject );
                CleanupStack::Pop(); // stateObject
                break;
                }
            case CPbk2NamesListControl::EStateHidden:
                stateObject = CPbk2NamesListHiddenState::NewLC();
                iStateArray.AppendL( stateObject );
                CleanupStack::Pop(); // stateObject
                break;

            default:
                {
                __ASSERT_DEBUG( EFalse, Panic( EPanicInvalidState ) );
                break;
                }
            }
        }

    if ( stateObject != aPreviousState )
        {
        if ( aPreviousState )
            {
            aPreviousState->DeactivateState();
            }
        stateObject->ActivateStateL();
        iNamesListState = stateObject->NamesListState();
        // Send state change event to control observers
        if ( !iSendControlEvent->IsActive() )
            {
            iSendControlEvent->Start( TCallBack( &SendEventToObservers, this ));
            }
        }

    return *stateObject;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStateFactory::SendEventToObservers
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListStateFactory::SendEventToObservers( TAny* aSelf )
    {
    CPbk2NamesListStateFactory* self = static_cast<CPbk2NamesListStateFactory*>( aSelf );
    self->DoSendEventToObservers();
    // Don't continue
    return 0;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListStateFactory::DoSendEventToObservers
// --------------------------------------------------------------------------
//
void CPbk2NamesListStateFactory::DoSendEventToObservers()
    {
    TPbk2ControlEvent event( TPbk2ControlEvent::EControlStateChanged );
    event.iInt = iNamesListState;
    TRAPD( err, iEventSender.SendEventToObserversL( event ) );
    if ( err != KErrNone )
        {
        CCoeEnv::Static()->HandleError( err );
        }
    }
// End of File
