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
* Description:  Phonebook 2 thumbnail loader.
*
*/


#include "CPbk2ThumbnailLoader.h"

// Phonebook 2

/// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactOperationBase.h>

// Debugging headers
#include <Pbk2Debug.h>


/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG
enum TPanic
    {
    EPanicLogic_VPbkSingleContactOperationComplete,
    EPanicLogic_VPbkSingleContactOperationFailed
    };

void Panic(TPanic aPanic)
    {
    _LIT(KPanicCat, "CPbk2ThumbnailLoader");
    User::Panic(KPanicCat, aPanic);
    }

#endif // _DEBUG

} /// namespace


// --------------------------------------------------------------------------
// CPbk2ThumbnailLoader::CPbk2ThumbnailLoader
// --------------------------------------------------------------------------
//
CPbk2ThumbnailLoader::CPbk2ThumbnailLoader
        ( CVPbkContactManager& aContactManager ) :
            iContactManager( aContactManager )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailLoader::~CPbk2ThumbnailLoader
// --------------------------------------------------------------------------
//
CPbk2ThumbnailLoader::~CPbk2ThumbnailLoader()
    {
    delete iThumbnailPopup;
    delete iStoreContact;
    delete iRetrieveOperation;
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailLoader::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2ThumbnailLoader::ConstructL()
    {
    iThumbnailPopup = CPbk2ThumbnailPopup::NewL( iContactManager );
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailLoader::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2ThumbnailLoader* CPbk2ThumbnailLoader::NewL
        ( CVPbkContactManager& aContactManager )
    {
    CPbk2ThumbnailLoader* self =
        new ( ELeave ) CPbk2ThumbnailLoader( aContactManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailLoader::LoadThumbnailL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ThumbnailLoader::LoadThumbnailL
        ( MVPbkContactLink* aContactLink, const CEikListBox* aListBox,
          TPbk2ThumbnailLocation aThumbnailLocation )
    {
    iListBox = aListBox;
    
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;
    
    delete iThumbnailPopup;
    iThumbnailPopup = NULL;
    iThumbnailPopup = CPbk2ThumbnailPopup::NewL( iContactManager );
    
    if ( aContactLink )
        {
        iThumbnailPopup->SetThumbnailLocation( aThumbnailLocation );
        iRetrieveOperation = iContactManager.RetrieveContactL
            ( *aContactLink, *this );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailLoader::LoadThumbnail
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ThumbnailLoader::LoadThumbnail
        ( MVPbkStoreContact* aStoreContact, const CEikListBox* aListBox,
          TPbk2ThumbnailLocation aThumbnailLocation )
    {
    iListBox = aListBox;
    
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;
    
    delete iThumbnailPopup;
    iThumbnailPopup = NULL;
    TRAPD(err, iThumbnailPopup = CPbk2ThumbnailPopup::NewL( iContactManager ));
    
    if ( !err && aStoreContact )
        {
        iThumbnailPopup->SetThumbnailLocation( aThumbnailLocation );
        iThumbnailPopup->Load( *aStoreContact, *this, iListBox );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailLoader::HideThumbnail
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ThumbnailLoader::HideThumbnail()
    {
    iThumbnailPopup->HideAndCancel();
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailLoader::Refresh
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ThumbnailLoader::Refresh()
    {
    iThumbnailPopup->Refresh();
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailLoader::Reset
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ThumbnailLoader::Reset()
    {
    // Cancel loading...
    HideThumbnail();
    // ...and destroy the contact
    delete iStoreContact;
    iStoreContact = NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailLoader::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailLoader::VPbkSingleContactOperationComplete
        ( MVPbkContactOperationBase& PBK2_DEBUG_ONLY ( aOperation ),
          MVPbkStoreContact* aContact )
    {
    __ASSERT_DEBUG( &aOperation == iRetrieveOperation,
        Panic( EPanicLogic_VPbkSingleContactOperationComplete ) );

    delete iRetrieveOperation;
    iRetrieveOperation = NULL;

    delete iStoreContact;
    iStoreContact = aContact;

    iThumbnailPopup->Load( *iStoreContact, *this, iListBox );
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailLoader::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailLoader::VPbkSingleContactOperationFailed
        ( MVPbkContactOperationBase& PBK2_DEBUG_ONLY( aOperation ),
          TInt /*aError*/ )
    {
    __ASSERT_DEBUG( &aOperation == iRetrieveOperation,
        Panic( EPanicLogic_VPbkSingleContactOperationFailed)  );
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailLoader::PopupLoadComplete
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailLoader::PopupLoadComplete()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ThumbnailLoader::PopupLoadFailed
// --------------------------------------------------------------------------
//
void CPbk2ThumbnailLoader::PopupLoadFailed( TInt /*aError*/ )
    {
    // Do nothing
    }

// End of File
