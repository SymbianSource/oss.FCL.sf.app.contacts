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
* Description:  Phonebook 2 key event dealer.
*
*/


#include "CPbk2KeyEventDealer.h"

// Phonebook 2
#include <MPbk2KeyEventHandler.h>

// System includes
#include <coemain.h>
#include <coeaui.h>

// --------------------------------------------------------------------------
// CPbk2KeyEventDealer::CPbk2KeyEventDealer
// --------------------------------------------------------------------------
//
CPbk2KeyEventDealer::CPbk2KeyEventDealer
        ( MPbk2KeyEventHandler& aKeyEventHandler ) :
            iKeyEventHandler( aKeyEventHandler )
    {
    }

// --------------------------------------------------------------------------
// CPbk2KeyEventDealer::~CPbk2KeyEventDealer
// --------------------------------------------------------------------------
//
CPbk2KeyEventDealer::~CPbk2KeyEventDealer()
    {
    CCoeAppUi* appUi = CCoeEnv::Static()->AppUi();
    if ( appUi )
        {
        appUi->RemoveFromStack( this );
        }
    }

// --------------------------------------------------------------------------
// CPbk2KeyEventDealer::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2KeyEventDealer::ConstructL()
    {
    // The priority must be greater than ECoeStackPriorityDialog
    // in order to be called by the stack before dialogs
    CCoeEnv::Static()->AppUi()->AddToStackL( this, ECoeStackPriorityCba );
    }

// --------------------------------------------------------------------------
// CPbk2KeyEventDealer::NewL
// --------------------------------------------------------------------------
//
CPbk2KeyEventDealer* CPbk2KeyEventDealer::NewL
        ( MPbk2KeyEventHandler& aKeyEventHandler )
    {
    CPbk2KeyEventDealer* self =
        new ( ELeave ) CPbk2KeyEventDealer( aKeyEventHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2KeyEventDealer::OfferKeyEventL
// --------------------------------------------------------------------------
//
TKeyResponse CPbk2KeyEventDealer::OfferKeyEventL
        ( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TKeyResponse result = EKeyWasNotConsumed;

    if  ( iKeyEventHandler.Pbk2ProcessKeyEventL( aKeyEvent, aType ) )
        {
        result = EKeyWasConsumed;
        }

    return result;
    }

// End of File
