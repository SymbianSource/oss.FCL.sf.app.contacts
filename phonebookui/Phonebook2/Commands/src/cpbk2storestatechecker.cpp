/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Checks store state
*
*/


// INCLUDE FILES
#include "cpbk2storestatechecker.h"

// Phonebook 2
#include <pbk2uicontrols.rsg>
#include <MPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>
#include <CPbk2StoreProperty.h>
#include <CPbk2StorePropertyArray.h>
#include "MPbk2StoreStateCheckerObserver.h"

// Virtual Phonebook
#include <MVPbkStoreContact.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>

// System includes
#include <aknnotewrappers.h> 
#include <coemain.h>
#include <StringLoader.h>

// --------------------------------------------------------------------------
// CPbk2StoreStateChecker::CPbk2StoreStateChecker
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CPbk2StoreStateChecker::CPbk2StoreStateChecker(
        MVPbkContactStore& aContactStore,
        MPbk2StoreStateCheckerObserver& aObserver ) :  
    iContactStore( aContactStore ),
    iObserver( aObserver )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2StoreStateChecker::CPbk2StoreStateChecker
// --------------------------------------------------------------------------
//
CPbk2StoreStateChecker::~CPbk2StoreStateChecker()
    {
    iContactStore.Close( *this );
    }

// --------------------------------------------------------------------------
// CPbk2StoreStateChecker::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CPbk2StoreStateChecker* CPbk2StoreStateChecker::NewL( 
        MVPbkContactStore& aContactStore,
        MPbk2StoreStateCheckerObserver& aObserver )
    {
    CPbk2StoreStateChecker* self = new( ELeave ) CPbk2StoreStateChecker
        ( aContactStore, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2StoreStateChecker::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2StoreStateChecker::ConstructL()
    {
    iContactStore.OpenL( *this );
    }

// --------------------------------------------------------------------------
// CPbk2StoreStateChecker::StoreReady
// --------------------------------------------------------------------------
//
void CPbk2StoreStateChecker::StoreReady( MVPbkContactStore& aContactStore )
    {
    aContactStore.Close( *this );
    iObserver.StoreState( MPbk2StoreStateCheckerObserver::EStoreAvailable );
    }

// --------------------------------------------------------------------------
// CPbk2StoreStateChecker::StoreUnavailable
// --------------------------------------------------------------------------
//
void CPbk2StoreStateChecker::StoreUnavailable( 
        MVPbkContactStore& /*aContactStore*/, 
        TInt /*aReason*/ )
    {
    iObserver.StoreState( MPbk2StoreStateCheckerObserver::EStoreUnavailable );
    }
// --------------------------------------------------------------------------
// CPbk2StoreStateChecker::HandleStoreEventL
// --------------------------------------------------------------------------
//
void CPbk2StoreStateChecker::HandleStoreEventL(
                        MVPbkContactStore& /*aContactStore*/, 
                        TVPbkContactStoreEvent /*aStoreEvent*/)
    {
    // No need to handle store event
    }

// --------------------------------------------------------------------------
// CPbk2StoreStateChecker::ShowUnavailableNoteL
// --------------------------------------------------------------------------
//
void CPbk2StoreStateChecker::ShowUnavailableNoteL()
    {
    const CPbk2StoreProperty* prop =
        Phonebook2::Pbk2AppUi()->ApplicationServices().StoreProperties().
            FindProperty( iContactStore.StoreProperties().Uri() );
            
    HBufC* text = NULL;
    if ( prop )
        {
        if ( prop->StoreName().Length() > 0 )
            {
            text = StringLoader::LoadLC
                        ( R_QTN_PHOB_STORE_NOT_AVAILABLE,
                          prop->StoreName() );            
            }
        }
    if ( !text )
        {
        text = StringLoader::LoadLC
                    ( R_QTN_PHOB_STORE_NOT_AVAILABLE,
                      iContactStore.StoreProperties().Uri().UriDes() );            
        }
    CAknInformationNote* note =
        new(ELeave) CAknInformationNote( ETrue );
    // Show "not available" note
    note->ExecuteLD(*text);
    CleanupStack::PopAndDestroy(text);
    }

//  End of File  
