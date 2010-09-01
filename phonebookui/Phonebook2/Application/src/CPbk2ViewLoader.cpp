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
* Description:  Phonebook2 view loader
*
*/


// INCLUDE FILES
#include "CPbk2ViewLoader.h"

// Phonebook 2
#include "MPbk2ViewLoaderObserver.h"

// Virtual Phonebook
#include <MVPbkContactViewBase.h>

// --------------------------------------------------------------------------
// CPbk2ViewLoader::CPbk2ViewLoader
// --------------------------------------------------------------------------
//
CPbk2ViewLoader::CPbk2ViewLoader(
        MVPbkContactViewBase& aView,
        MPbk2ViewLoaderObserver& aObserver ) : 
    iView( aView ), iObserver( aObserver )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ViewLoader::~CPbk2ViewLoader
// --------------------------------------------------------------------------
//
CPbk2ViewLoader::~CPbk2ViewLoader()
    {
    iView.RemoveObserver( *this );
    }

// --------------------------------------------------------------------------
// CPbk2ViewLoader::NewL
// --------------------------------------------------------------------------
//
CPbk2ViewLoader* CPbk2ViewLoader::NewL(
        MVPbkContactViewBase& aView,
        MPbk2ViewLoaderObserver& aObserver )
    {
    CPbk2ViewLoader* self =
        new( ELeave ) CPbk2ViewLoader( aView, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ViewLoader::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2ViewLoader::ConstructL()
    {   
    iView.AddObserverL( *this );
    }

// --------------------------------------------------------------------------
// CPbk2ViewLoader::ContactViewReady
// --------------------------------------------------------------------------
//
void CPbk2ViewLoader::ContactViewReady(
        MVPbkContactViewBase& aView )
    {
    iObserver.ViewLoaded( aView );
    }
    
// --------------------------------------------------------------------------
// CPbk2ViewLoader::ContactViewUnavailable
// --------------------------------------------------------------------------
//
void CPbk2ViewLoader::ContactViewUnavailable(
        MVPbkContactViewBase& /*aView*/ )
    {
    // Do nothing
    }
            
// --------------------------------------------------------------------------
// CPbk2ViewLoader::ContactAddedToView
// --------------------------------------------------------------------------
//
void CPbk2ViewLoader::ContactAddedToView(
        MVPbkContactViewBase& /*aView*/,
        TInt /*aIndex*/,
        const MVPbkContactLink& /*aContactLink*/ )
    {
    // Do nothing
    }
            
// --------------------------------------------------------------------------
// CPbk2ViewLoader::ContactRemovedFromView
// --------------------------------------------------------------------------
//
void CPbk2ViewLoader::ContactRemovedFromView(
        MVPbkContactViewBase& /*aView*/,
        TInt /*aIndex*/,
        const MVPbkContactLink& /*aContactLink*/ )
    {
    // Do nothing
    }
            
// --------------------------------------------------------------------------
// CPbk2ViewLoader::ContactViewError
// --------------------------------------------------------------------------
//
void CPbk2ViewLoader::ContactViewError(
        MVPbkContactViewBase& aView,
        TInt aError,
        TBool /*aErrorNotified*/ )
    {
    iObserver.ViewError( aView, aError );
    }

//  End of File
