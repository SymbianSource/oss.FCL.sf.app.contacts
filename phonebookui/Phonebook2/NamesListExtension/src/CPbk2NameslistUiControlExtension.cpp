/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 Nameslist UI control composite UI extension.
*
*/


#include "CPbk2NameslistUiControlExtension.h"

#include <CPbk2IconArray.h>
#include <TPbk2IconId.h>
#include <Pbk2IconId.hrh>
#include <Pbk2Commands.hrh>
#include <Pbk2UID.h>
#include <MPbk2ContactUiControlUpdate.h>
#include <MVPbkViewContact.h>
#include <MVPbkContactLink.h>
#include <CVPbkContactManager.h>
#include <MVPbkBaseContactFieldCollection.h>
#include <MVPbkBaseContactField.h>
#include <MVPbkFieldType.h>
#include <MVPbkContactFieldData.h>
#include <VPbkEng.rsg>
#include <CVPbkTopContactManager.h>
#include <Pbk2InternalUID.h>
#include <mpbk2commanditemupdater.h>

//ECE
#include <gulicon.h>
#include <fbs.h>
#include <AknUtils.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <aknlayoutscalable_apps.cdl.h>

#include "CPbk2UIExtensionInformation.h"
#include <CPbk2UIExtensionPlugin.h>
#include <Pbk2UIExtension.hrh>
#include <TPbk2IconId.h>
#include "Pbk2NamesListExUID.h"
#include "CPbk2EcePresenceEngine.h"

enum TNlxPanicCodes
    {
    ENlxPanicEmptyPackage,
    ENlxPanicIconNull,
    ENlxPanicNoLinks,
    ENlxPanicStrayIcon,
    ENlxPanicStrayIconInfo
    };

/// Unnamed namespace for local definitions
namespace {

TSize CalculateListIconSize()
    {
    TRect mainPane;
    AknLayoutUtils::LayoutMetricsRect(
        AknLayoutUtils::EMainPane, mainPane );
    TAknLayoutRect listLayoutRect;
    listLayoutRect.LayoutRect(
        mainPane,
        AknLayoutScalable_Apps::list_double_large_graphic_phob2_pane_g2( 0 ).LayoutLine() );
    return listLayoutRect.Rect().Size();
    }
}


// -----------------------------------------------------------------------------
// CPbk2NameslistUiControlExtension::CPbk2NameslistUiControlExtension
// -----------------------------------------------------------------------------
//
CPbk2NameslistUiControlExtension::CPbk2NameslistUiControlExtension
        ( CVPbkContactManager& aContactManager,
          CSpbContentProvider& aContentProvider,
          CPbk2MyCard* aMyCard )  :
            iContactManager( aContactManager ),
            iContentProvider( aContentProvider ),
            iMyCard( aMyCard ),
            iIconId( TPbk2IconId() )
    {
    }

// -----------------------------------------------------------------------------
// CPbk2NameslistUiControlExtension::ConstructL
// -----------------------------------------------------------------------------
//
void CPbk2NameslistUiControlExtension::ConstructL()
    {
	// if my card is provided
	if( iMyCard )
		{
		// set this to be observer of the my card
		iMyCard->SetObserverL( *this );
		// check if the mycard has already a thumbnail
		iBitmap = iMyCard->MyCardBitmap();
		}
	
	iPresenceEngine = CPbk2EcePresenceEngine::NewL(
            iContactManager,
            *this,
            CalculateListIconSize() );
    iContentProvider.AddObserverL( *this );
    }

// -----------------------------------------------------------------------------
// CPbk2NameslistUiControlExtension::NewL
// -----------------------------------------------------------------------------
//
CPbk2NameslistUiControlExtension* CPbk2NameslistUiControlExtension::NewL
        ( CVPbkContactManager& aContactManager,
          CSpbContentProvider& aContentProvider,
          CPbk2MyCard* aMyCard )
    {
    CPbk2NameslistUiControlExtension* self =
        new ( ELeave ) CPbk2NameslistUiControlExtension
            ( aContactManager, aContentProvider, aMyCard );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CPbk2NameslistUiControlExtension::~CPbk2NameslistUiControlExtension
// -----------------------------------------------------------------------------
//
CPbk2NameslistUiControlExtension::~CPbk2NameslistUiControlExtension()
    {
    delete iPresenceEngine;
    iDummyArray.Close();
    iContentProvider.RemoveObserver( *this );
    // if my card is provided
    if( iMyCard )
    	{
    	iMyCard->RemoveObserver( *this );
    	}
    }

// -----------------------------------------------------------------------------
// CPbk2NameslistUiControlExtension::DoRelease
// -----------------------------------------------------------------------------
//
void CPbk2NameslistUiControlExtension::DoRelease()
    {
    delete this;
    }

// -----------------------------------------------------------------------------
// CPbk2NameslistUiControlExtension::GetDynamicIconsL
// -----------------------------------------------------------------------------
//
const TArray<TPbk2IconId> CPbk2NameslistUiControlExtension::GetDynamicIconsL
    ( const MVPbkViewContact* aContactHandle )
    {
    //Add here the values of TPbk2IconId that are preferred to be shown
    //in the list. Icons must already be present in the icon array.
    iDummyArray.Reset();

    if ( aContactHandle )
        {
        // first check presence icon, highest priority
        MVPbkContactLink* link = aContactHandle->CreateLinkLC();
        if ( link )
            {
            // Requests presence notifications, if hasn't already done so.
            iPresenceEngine->SubscribeContactPresenceL( *link );

            TPbk2IconId iconId;
            TBool found = iPresenceEngine->GetIconIdForContact( *link, iconId );
            if ( found )
                {
                // The icon has already been fetched from the presence server etc.
                if ( iIconArray->FindIcon( iconId ) == KErrNotFound )
                    {
                    // The icon is not yet in the array. Add it.
                    CGulIcon* gulIcon = iPresenceEngine->CreateIconCopyLC( iconId );
                    iIconArray->AppendIconL( gulIcon, iconId );
                    CleanupStack::Pop( gulIcon );
                    }
                iDummyArray.AppendL( iconId );
                }
            CleanupStack::PopAndDestroy(); // link
            }

        //Top contact indicator
        if ( CVPbkTopContactManager::IsTopContact( *aContactHandle ) )
            {
            iDummyArray.AppendL( TPbk2IconId(
                TUid::Uid(KPbk2UID3), EPbk2qgn_prop_pb_topc ));
            }
        }

    //Presence indicator
    return iDummyArray.Array();
    }

// -----------------------------------------------------------------------------
// CPbk2NameslistUiControlExtension::SetContactUiControlUpdate
// -----------------------------------------------------------------------------
//
void CPbk2NameslistUiControlExtension::SetContactUiControlUpdate
        ( MPbk2ContactUiControlUpdate* aContactUpdator )
    {
    //Register observer interface (MPbk2ContactUiControlUpdate) of aContactUpdator
    //In case changes in data, call  UpdateContact(const MVPbkContactLink& aContactLink )
    iContactUpdator = aContactUpdator;
    }

// -----------------------------------------------------------------------------
// CPbk2NameslistUiControlExtension::ContactUiControlExtensionExtension
// -----------------------------------------------------------------------------
//
TAny* CPbk2NameslistUiControlExtension::ContactUiControlExtensionExtension(
    TUid aExtensionUid )
    {
    if ( aExtensionUid == KMPbk2ContactUiControlExtensionExtension2Uid )
        {
        return static_cast<MPbk2ContactUiControlExtension2*>( this );
        }
    else if( aExtensionUid == TUid::Uid( KPbk2ContactUiControlExtensionExtensionUID ) ) 
        {
        return static_cast<MPbk2ContactUiControlDoubleListboxExtension*>( this );
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CPbk2NameslistUiControlExtension::SetIconArray
//
// For use via ContactUiControlExtensionExtension
// -----------------------------------------------------------------------------
//
void CPbk2NameslistUiControlExtension::SetIconArray(CPbk2IconArray& aIconArray)
    {
    iIconArray = &aIconArray;
    // if bitmap is allready set. MyCardEvent - event has come before this one or there was an image on creation
    if( iBitmap && iMyCard )
    	{
		AddThumbnailToIconArrayL();
    	}
    }

// -----------------------------------------------------------------------------
// CPbk2NameslistUiControlExtension::ContactPresenceChanged
// -----------------------------------------------------------------------------
//
void CPbk2NameslistUiControlExtension::ContactPresenceChanged(
    const MVPbkContactLink& aLink )
    {
    if ( iContactUpdator )
        {
        iContactUpdator->UpdateContact( aLink );
        }
    }

// -----------------------------------------------------------------------------
// CPbk2NameslistUiControlExtension::FormatDataL
// -----------------------------------------------------------------------------
//
void CPbk2NameslistUiControlExtension::FormatDataL(
        const MVPbkViewContact& aViewContact,
        MPbk2DoubleListboxDataElement& aDataElement )
    {
    MVPbkContactLink* link = aViewContact.CreateLinkLC();
    if( link )
        {
        HBufC* txt = NULL;
        TPbk2IconId iconId;
        CSpbContentProvider::TSpbContentType type = CSpbContentProvider::ETypeNone; 
        iContentProvider.GetContentL( *link, txt, iconId, type );

        // select proper element type based on content type
        MPbk2DoubleListboxDataElement::TTextDataType elemType = 
            MPbk2DoubleListboxDataElement::ETypeGenericText;
        if( type == CSpbContentProvider::ETypePhoneNumber )
            {
            elemType = MPbk2DoubleListboxDataElement::ETypePhoneNumber;
            }
        
        aDataElement.SetText(
            MPbk2DoubleListboxDataElement::EStatusText, txt, elemType );

        CleanupStack::PopAndDestroy();  // link
        }
    }

// -----------------------------------------------------------------------------
// CPbk2NameslistUiControlExtension::SetCommandItemUpdater
// -----------------------------------------------------------------------------
//
void CPbk2NameslistUiControlExtension::SetCommandItemUpdater(
    MPbk2CommandItemUpdater* aCommandUpdater )
    {
    // Register command item updater interface
    iCommandUpdater = aCommandUpdater;
    }

// -----------------------------------------------------------------------------
// CPbk2NameslistUiControlExtension::ContentUpdated
// -----------------------------------------------------------------------------
//
void CPbk2NameslistUiControlExtension::ContentUpdated(
    MVPbkContactLink& aLink,
    MSpbContentProviderObserver::TSpbContentEvent aEvent )
    {
    if( aEvent == MSpbContentProviderObserver::EContentChanged )
        {
        MVPbkContactLink* mylink = iMyCard ? iMyCard->MyCardLink() : NULL;
        if( mylink && mylink->IsSame( aLink ) )
            {
            // mycard updated -> update mycard command item instead of contact
            if( iCommandUpdater )
                {
                iCommandUpdater->UpdateCommandItem( EPbk2CmdOpenMyCard );
                }
            }
        else if( iContactUpdator )
            {
            // some other contactB
            iContactUpdator->UpdateContact( aLink );
            }
        }
    }

// -----------------------------------------------------------------------------
// CPbk2NameslistUiControlExtension::MyCardEvent
// -----------------------------------------------------------------------------
//
void CPbk2NameslistUiControlExtension::MyCardEvent( TMyCardStatusEvent aEvent )
	{
	// thumbnail is loaded
	if( aEvent == EStateThumbnailLoaded && iMyCard )    
		{
		// get bitmap
		iBitmap = iMyCard->MyCardBitmap();
			
		// if icon array is already set, add icon
		if( iIconArray )
			{
			AddThumbnailToIconArrayL();
			}
		}
	}

// -----------------------------------------------------------------------------
// CPbk2NameslistUiControlExtension::AddThumbnailToIconArrayL
// -----------------------------------------------------------------------------
//
void CPbk2NameslistUiControlExtension::AddThumbnailToIconArrayL( )
	{
	if( iMyCard )
		{
		iIconId = iMyCard->MyCardIconId();
		}
	
	// if thumb icon is not created, create it and append to array
	if( !iThumbIcon )
		{
		iThumbIcon = CGulIcon::NewLC();
		CleanupStack::Pop( iThumbIcon );
		iThumbIcon->SetBitmap( iBitmap );
		iBitmap = NULL;
		// takes ownership
		iIconArray->AppendIconL( iThumbIcon, iIconId );
		}
	    
	// else change the bitmap. This is to prevent 
	// thumbnail indexes on thumbnail manager to get out of sync
	else
		{
		iThumbIcon->SetBitmap( iBitmap );
		iBitmap = NULL;
		}
	}


// End of File
