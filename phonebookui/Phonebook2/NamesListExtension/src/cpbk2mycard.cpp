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
* Description:  .
*
*/

// class header
#include "cpbk2mycard.h"

// Pbk2
#include <MPbk2AppUi.h>
#include <CPbk2ApplicationServices.h>
#include "mpbk2mycardobserver.h"
#include <MVPbkFieldType.h>
#include <Pbk2InternalUID.h>
#include "pbk2nameslistexiconid.hrh"
#include "Pbk2NamesListExUID.h"

// Virtual Phonebook
#include <VPbkContactStoreUris.h>
#include <TVPbkContactStoreUriPtr.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStore2.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkContactLink.h>
#include <MVPbkStoreContact.h>

// thumbnail loading
#include <CPbk2ImageManager.h>
#include <TPbk2ImageManagerParams.h>
#include <VPbkEng.rsg>
#include <fbs.h>
#include <AknUtils.h>
#include <aknlayoutscalable_apps.cdl.h>

TSize CalculateThumbnailSize()
    {
    TAknWindowComponentLayout layout( 
            TAknWindowComponentLayout::Compose(
        AknLayoutScalable_Apps::list_double_large_graphic_phob2_pane( 0 ),
        AknLayoutScalable_Apps::list_double_large_graphic_phob2_pane_g1( 0 ) ) );    
    
    TRect mainPane;
    AknLayoutUtils::LayoutMetricsRect(
        AknLayoutUtils::EMainPane, mainPane );
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect(
        mainPane,
        layout.LayoutLine() );
    return layoutRect.Rect().Size();
    }

// --------------------------------------------------------------------------
// CPbk2MyCard::NewL
// --------------------------------------------------------------------------
//
CPbk2MyCard* CPbk2MyCard::NewL( MVPbkContactStore& aPhoneStore )
    {
    CPbk2MyCard* self = new (ELeave) CPbk2MyCard( aPhoneStore );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2MyCard::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2MyCard::ConstructL()
    {
	// first get application services and contact manager
	iAppServices = CPbk2ApplicationServices::InstanceL();
	iContactManager = &iAppServices->ContactManager();
	
	// create image manager for fetching the thumbnail
	iImageManager = CPbk2ImageManager::NewL( *iContactManager );
    iPhoneStore->OpenL(*this);
    }

// --------------------------------------------------------------------------
// CPbk2MyCard::CPbk2MyCard
// --------------------------------------------------------------------------
//
CPbk2MyCard::CPbk2MyCard( MVPbkContactStore& aPhoneStore )
:iMyCardState( EUnkown ), 
 iPhoneStore( &aPhoneStore ),
 iIconId( TUid::Uid( KPbk2ControlCmdItemExtensionUID ), 1 ),
 iEmptyId( TUid::Uid( KPbk2NamesListExtPluginUID2 ), 
             EPbk2EceNoMyCardThumbnailIconId )
    {
    }

// --------------------------------------------------------------------------
// CPbk2MyCard::~CPbk2MyCard
// --------------------------------------------------------------------------
//
CPbk2MyCard::~CPbk2MyCard()
    {
	
	
	delete iThumbBitmap;
	delete iMyCardStoreContact;
	delete iImageManager;
    delete iMyCardLink;
    delete iOperation;
    Release( iAppServices );
    iObserverArray.Reset();
    if(iPhoneStore)
        {
        iPhoneStore->Close(*this);
        }
    
    }

// --------------------------------------------------------------------------
// CPbk2MyCard::SetObserverL
// --------------------------------------------------------------------------
//
void CPbk2MyCard::SetObserverL(MPbk2MyCardObserver& aObserver)
	{
	iObserverArray.AppendL(&aObserver);
	}

// --------------------------------------------------------------------------
// CPbk2MyCard::RemoveObserver
// --------------------------------------------------------------------------
//
void CPbk2MyCard::RemoveObserver(MPbk2MyCardObserver& aObserver)
	{
	const TInt index(iObserverArray.Find(&aObserver));
	if(index >= 0)
		{
		iObserverArray.Remove(index);
		}
	}

// --------------------------------------------------------------------------
// CPbk2MyCard::NotifyObservers
// --------------------------------------------------------------------------
//
void CPbk2MyCard::NotifyObservers( MPbk2MyCardObserver::TMyCardStatusEvent aEvent )
	{
	const TInt count(iObserverArray.Count());
	for(TInt i = 0 ; i < count ; ++i)
		{
		iObserverArray[i]->MyCardEvent( aEvent );	
		}
	}

// --------------------------------------------------------------------------
// CPbk2MyCard::MyCardLink
// --------------------------------------------------------------------------
//
MVPbkContactLink* CPbk2MyCard::MyCardLink() const
	{
	return iMyCardLink;
	}

// --------------------------------------------------------------------------
// CPbk2MyCard::MyCardBitmap
// --------------------------------------------------------------------------
//
CFbsBitmap* CPbk2MyCard::MyCardBitmap() const
	{
	// if fetched
	if( iThumbBitmap )
		{
		// create duplicate bitmap
		CFbsBitmap* dstbitmap = new (ELeave) CFbsBitmap;
		CleanupStack::PushL( dstbitmap );
	
		//Get the handle to source bitmap
		TInt srchandle = iThumbBitmap->Handle();
		
		//Duplicate the bitmap handle. Increases the RefCount of bitmap 
		//managed but Fbs Server
		User::LeaveIfError( dstbitmap->Duplicate( srchandle ) );
		CleanupStack::Pop( dstbitmap );
		return dstbitmap;
		}
	return NULL;
	}

// --------------------------------------------------------------------------
// CPbk2MyCard::MyCardIconId
// --------------------------------------------------------------------------
//
const TPbk2IconId& CPbk2MyCard::MyCardIconId() const
	{
	// if my card thumbnail exists
	if( iThumbBitmap )
		{
		return iIconId;
		}
	return iEmptyId;
	}

// --------------------------------------------------------------------------
// CPbk2MyCard::MyCardStoreContact
// --------------------------------------------------------------------------
//
MVPbkStoreContact* CPbk2MyCard::MyCardStoreContact() const
    {
    return iMyCardStoreContact;
    }

// --------------------------------------------------------------------------
// CPbk2MyCard::VPbkSingleContactLinkOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2MyCard::VPbkSingleContactLinkOperationComplete(
        MVPbkContactOperationBase& /*aOperation*/,
        MVPbkContactLink* aLink )
    {
    // iOperation still owns the contact link until this method returns.
    // we can't delete iOperation here
    delete iMyCardLink;
    iMyCardLink = aLink;
    iMyCardState = EExisting;
    
    NotifyObservers( MPbk2MyCardObserver::EStateStatusResolved );
    
    // fetch store contact
    iRetrieveOperation = iContactManager->RetrieveContactL( *iMyCardLink, *this );
    }

// --------------------------------------------------------------------------
// CPbk2MyCard::VPbkSingleContactLinkOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2MyCard::VPbkSingleContactLinkOperationFailed(
        MVPbkContactOperationBase& /*aOperation*/,
        TInt /*aError*/ )
    {
    delete iOperation;
    iOperation = NULL;
    delete iMyCardLink;
    iMyCardLink = NULL;
    iMyCardState = ENonExisting;
    
    NotifyObservers( MPbk2MyCardObserver::EStateStatusResolved );
    }


// ---------------------------------------------------------------------------
// CPbk2MyCard::VPbkSingleContactOperationComplete
// ---------------------------------------------------------------------------
//
void CPbk2MyCard::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& /*aOperation*/,
        MVPbkStoreContact* aContact )
    {
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;
    
    delete iMyCardStoreContact;
    iMyCardStoreContact = aContact;
    
    // load thumbnail
    LoadThumbnailL( *iMyCardStoreContact );
    }

// ---------------------------------------------------------------------------
// CPbk2MyCard::VPbkSingleContactOperationFailed
// ---------------------------------------------------------------------------
//
void CPbk2MyCard::VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& /*aOperation*/, 
        TInt /*aError*/ )
    {
	// cannot fetch store contact
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;
    }

// ---------------------------------------------------------------------------
// CPbk2MyCard::Pbk2ImageGetComplete
// ---------------------------------------------------------------------------
//
void CPbk2MyCard::Pbk2ImageGetComplete(
    MPbk2ImageOperation& /*aOperation*/,
    CFbsBitmap* aBitmap )
    {
	delete iImageOperation;
	iImageOperation = NULL;
	//delete existing
	delete iThumbBitmap;
	iThumbBitmap = aBitmap;
	
	// inform observers
	NotifyObservers( MPbk2MyCardObserver::EStateThumbnailLoaded );
    }

// ---------------------------------------------------------------------------
// CPbk2MyCard::Pbk2ImageGetFailed
// ---------------------------------------------------------------------------
//
void CPbk2MyCard::Pbk2ImageGetFailed( 
    MPbk2ImageOperation& /*aOperation*/,
    TInt /*aError*/ )
    {
	// image fetching failed
    delete iImageOperation;
    iImageOperation = NULL;
    // inform observers
    NotifyObservers( MPbk2MyCardObserver::EStateThumbnailNotFound );
    }

// --------------------------------------------------------------------------
// CPbk2MyCard::LoadThumbnailL
// --------------------------------------------------------------------------
//
void CPbk2MyCard::LoadThumbnailL( MVPbkBaseContact& aContact )
	{
	// cancel previous operations
	delete iImageOperation;
	iImageOperation = NULL;

	const MVPbkFieldType* thumbType = iContactManager->FieldTypes().Find(
			R_VPBK_FIELD_TYPE_THUMBNAILPIC );
    // if there's picture field
	if( thumbType )
		{
		if( iImageManager->HasImage( aContact, *thumbType ) )
			{
			TSize size( CalculateThumbnailSize() );
		
			// Define parameters for thumbnail
			TPbk2ImageManagerParams params;
			TInt useCropping = 0x0008;
			// set params
			params.iSize = size;
			params.iFlags = TPbk2ImageManagerParams::EScaleImage |
							TPbk2ImageManagerParams::EKeepAspectRatio |
							useCropping;	//CROP IMAGE	
			// contact has image. load it.
			iImageOperation = iImageManager->GetImageAsyncL( 
				&params, aContact, *thumbType, *this );
			}
		// no image
		else
			{
			// reset thumbnail
			delete iThumbBitmap;
			iThumbBitmap = NULL;
			// inform observers
			NotifyObservers( MPbk2MyCardObserver::EStateThumbnailNotFound );
			}
		}
	}

// --------------------------------------------------------------------------
// CPbk2MyCard::StoreReady
// --------------------------------------------------------------------------
//
void CPbk2MyCard::StoreReady( MVPbkContactStore& aContactStore )
    {
    delete iOperation;
    iOperation = NULL;
    
    MVPbkContactStore2* phoneStoreExtension =
        static_cast<MVPbkContactStore2*>(aContactStore.ContactStoreExtension(
                KMVPbkContactStoreExtension2Uid));
    if ( phoneStoreExtension )
		{
		TRAPD( err, iOperation = phoneStoreExtension->OwnContactLinkL(*this) );
		if( err )
			{
			// TODO: How is this handled?
			}
		}
    }

// --------------------------------------------------------------------------
// CPbk2MyCard::StoreUnavailable
// --------------------------------------------------------------------------
//
void CPbk2MyCard::StoreUnavailable(
        MVPbkContactStore& /*aContactStore*/,
        TInt /*aReason*/ )
    {
    }

// --------------------------------------------------------------------------
// CPbk2MyCard::HandleStoreEventL
// --------------------------------------------------------------------------
//
void CPbk2MyCard::HandleStoreEventL(
        MVPbkContactStore& /*aContactStore*/,
        TVPbkContactStoreEvent aStoreEvent )
    {
	// my card is not set yet
    if( !iMyCardLink &&
        aStoreEvent.iContactLink &&
        ( aStoreEvent.iEventType == TVPbkContactStoreEvent::EContactAdded || 
          aStoreEvent.iEventType == TVPbkContactStoreEvent::EContactChanged ) ) 
        {
		// check is new or changed contact own contact
        delete iOperation;
        iOperation = NULL;
        
        MVPbkContactStore2* phoneStoreExtension =
            static_cast<MVPbkContactStore2*>(iPhoneStore->ContactStoreExtension(
                    KMVPbkContactStoreExtension2Uid));
        if ( phoneStoreExtension )
        	{
			iOperation = phoneStoreExtension->OwnContactLinkL( *this );
        	}
        }
    else if( iMyCardLink && aStoreEvent.iContactLink )
    	{
   		if( aStoreEvent.iEventType == TVPbkContactStoreEvent::EContactDeleted )
			{
			// Check is MyCard deleted
			if( iMyCardLink->IsSame( *aStoreEvent.iContactLink ) )
				{
				delete iMyCardLink;
				iMyCardLink = NULL;
				iMyCardState = ENonExisting;
				NotifyObservers( MPbk2MyCardObserver::EStateStatusResolved );
				
				// remove thumbnail and inform
				delete iThumbBitmap;
				iThumbBitmap = NULL;
				// inform also that thumbnail is removed
				NotifyObservers( MPbk2MyCardObserver::EStateThumbnailNotFound );
				}
			}
   		// contact has changed, check the thumbnail
   		else if( aStoreEvent.iEventType == TVPbkContactStoreEvent::EContactChanged  )
   			{
			// Check is MyCard changed
			if( iMyCardLink->IsSame( *aStoreEvent.iContactLink ) )
				{
				// fetch store contact and check thumbnail
				iRetrieveOperation = 
						iContactManager->RetrieveContactL( *iMyCardLink, *this );
				}
   			}
    	}// else if
    }

// end of file
