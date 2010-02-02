/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 select property phase factory.
*
*/


// INCLUDE FILES
#include "Pbk2SelectPropertyPhaseFactory.h"

// Phonebook 2
#include "CPbk2SelectCreateNewPropertyPhase.h"
#include "CPbk2SelectSinglePropertyPhase.h"
#include "CPbk2SelectMultiplePropertyPhase.h"
#include "CPbk2SelectUpdateEmptyPropertyPhase.h"
#include "CPbk2SelectCreateEmptyPropertyPhase.h"

// --------------------------------------------------------------------------
// Pbk2SelectPropertyPhaseFactory::CreateSelectCreateNewPropertyPhase
// --------------------------------------------------------------------------
//
MPbk2ServicePhase*
    Pbk2SelectPropertyPhaseFactory::CreateSelectCreateNewPropertyPhaseL
        ( MPbk2ServicePhaseObserver& aObserver, HBufC8* aFilterBuffer,
          HBufC* aDataBuffer, TVPbkContactStoreUriPtr aSavingStoreUri,
          MPbk2SelectFieldProperty*& aSelectFieldProperty,
          TInt& aResult, TUint& aNoteFlags )
    {
    MPbk2ServicePhase* selectPropertyPhase = NULL;

    if ( aFilterBuffer && aDataBuffer && aDataBuffer->Length() > 0 )
        {
        selectPropertyPhase = CPbk2SelectCreateNewPropertyPhase::NewL
//            ( aSavingStoreUri, aObserver, *aFilterBuffer, 
            ( aSavingStoreUri, aObserver, aFilterBuffer,   //FIX FOR ECE BUILD BREAK CAUSED BY NON-ECE TASK
                                                           //Phonebook2:E0103149:TSW Errors:EHSN-7EM3G6: Fix "VoIP: Crash occurs when pressing "Ok" to a contact after selecting Add to contacts->Update existing to the VoIP address in Notepad."            
              aSelectFieldProperty, aResult, aNoteFlags );
        }
    else
        {
        selectPropertyPhase = CPbk2SelectCreateEmptyPropertyPhase::NewL
            ( aObserver, aSavingStoreUri, aSelectFieldProperty,
              aResult, aNoteFlags );
        }

    return selectPropertyPhase;
    }

// --------------------------------------------------------------------------
// Pbk2SelectPropertyPhaseFactory::CreateSelectCreateNewPropertyPhase
// --------------------------------------------------------------------------
//
MPbk2ServicePhase*
    Pbk2SelectPropertyPhaseFactory::CreateSelectSinglePropertyPhaseL
        ( MPbk2ServicePhaseObserver& aObserver, HBufC8* aFilterBuffer,
          HBufC* aDataBuffer, MVPbkContactLinkArray* aContactLinks,
          MPbk2SelectFieldProperty*& aSelectFieldProperty,
          TInt& aResult, TUint& aNoteFlags )
    {
    MPbk2ServicePhase* selectPropertyPhase = NULL;

    if ( aFilterBuffer && aDataBuffer && aDataBuffer->Length() > 0 )
        {
        selectPropertyPhase = CPbk2SelectSinglePropertyPhase::NewL
//            ( aObserver, aContactLinks, *aFilterBuffer,
            ( aObserver, aContactLinks, aFilterBuffer,   //FIX FOR ECE BUILD BREAK CAUSED BY NON-ECE TASK            
                                                         //Phonebook2:E0103149:TSW Errors:EHSN-7EM3G6: Fix "VoIP: Crash occurs when pressing "Ok" to a contact after selecting Add to contacts->Update existing to the VoIP address in Notepad."                        
              aSelectFieldProperty, aResult );
        }
    else
        {
        selectPropertyPhase = CPbk2SelectUpdateEmptyPropertyPhase::NewL
            ( aObserver, *aContactLinks, aSelectFieldProperty, aResult,
             aNoteFlags );
        }

    return selectPropertyPhase;
    }

// --------------------------------------------------------------------------
// Pbk2SelectPropertyPhaseFactory::CreateSelectMultiPropertyPhaseL
// --------------------------------------------------------------------------
//
MPbk2ServicePhase*
    Pbk2SelectPropertyPhaseFactory::CreateSelectMultiPropertyPhaseL
        ( MPbk2ServicePhaseObserver& aObserver, HBufC8* aFilterBuffer,
          HBufC* aDataBuffer, MVPbkContactLinkArray* aContactLinks,
          MPbk2SelectFieldProperty*& aSelectFieldProperty,
          TInt& /* aResult */, TUint& /* aNoteFlags */ )
    {
    MPbk2ServicePhase* selectPropertyPhase = NULL;

    if ( aFilterBuffer && aDataBuffer && aDataBuffer->Length() > 0 )
        {
        selectPropertyPhase = CPbk2SelectMultiplePropertyPhase::NewL
            ( aObserver, aContactLinks, *aFilterBuffer,
              aSelectFieldProperty );
        }
    else
        {
        // Assigning nothing to multiple contacts is not supported
        User::Leave( KErrArgument );
        }

    return selectPropertyPhase;
    }

// End of File
