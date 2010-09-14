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
* Description:  Phonebook 2 title pane picture factory.
*
*/


#include "Pbk2TitlePanePictureFactory.h"

// Phonebook 2
#include "CPbk2UIExtensionManager.h"
#include "MPbk2UIExtensionIconSupport.h"
#include <CPbk2IconArray.h>
#include <CPbk2StoreProperty.h>
#include <CPbk2StorePropertyArray.h>
#include <CPbk2StoreViewDefinition.h>
#include <pbk2uicontrols.rsg>

// Virtual Phonebook
#include <CVPbkContactViewDefinition.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkStoreContact.h>
#include <VPbkContactView.hrh>

// System includes
#include <AknIconUtils.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <akntitle.h>
#include <AknUtils.h>
#include <barsread.h>
#include <gulicon.h>


/// Unnamed namespace for local definitions
namespace {

/**
 * Creates icon array.
 *
 * @return  Icon array.
 */
CPbk2IconArray* CreateIconArrayLC()
    {
    CPbk2IconArray* iconArray = NULL;
    TResourceReader reader;
    CCoeEnv::Static()->
        CreateResourceReaderLC( reader, R_PBK2_CONTACT_LIST_ICONS );

    // Create icon array
    iconArray = CPbk2IconArray::NewL( reader );
    CleanupStack::PopAndDestroy(); // reader
    CleanupStack::PushL( iconArray );

    // Add icons from UI extensions
    CPbk2UIExtensionManager* extManager =
        CPbk2UIExtensionManager::InstanceL();
    extManager->PushL();
    extManager->IconSupportL().AppendExtensionIconsL( *iconArray );

    CleanupStack::PopAndDestroy( extManager );

    return iconArray;
    }

} /// namespace


// --------------------------------------------------------------------------
// Pbk2TitlePanePictureFactory::CreateTitlePanePictureL
// --------------------------------------------------------------------------
//
EXPORT_C CEikImage* Pbk2TitlePanePictureFactory::CreateTitlePanePictureLC
        ( const MVPbkStoreContact* aContact,
          CPbk2StorePropertyArray& aStoreProperties )
    {
    CEikImage* image = new ( ELeave ) CEikImage;
    CleanupStack::PushL( image );

    CPbk2IconArray* iconArray = CreateIconArrayLC();

    // Get icon index
    TInt index = KErrNotFound;
    TVPbkContactStoreUriPtr uri =
        aContact->ParentStore().StoreProperties().Uri();
    const CPbk2StoreProperty*
        storeProperty = aStoreProperties.FindProperty( uri );

    if( storeProperty )
        {
        TArray<const CPbk2StoreViewDefinition*>
            views = storeProperty->Views();
        TInt count = views.Count();
        TInt i = 0;
        for( i = 0; i < count && index == KErrNotFound; i++ )
            {
            if( views[i]->ViewDefinition().Type() == EVPbkContactsView )
                {
                const TPbk2IconId& id = views[i]->ViewItemIconId();
                index = iconArray->FindIcon( id );
                }
            }
        }

    if( index >= 0 )
        {
        // Get pointers to bitmaps in array
        CFbsBitmap* bitmap = iconArray->At( index )->Bitmap();
        CFbsBitmap* mask = iconArray->At( index )->Mask();

        // Calculate icon size
        TRect mainPane;
        AknLayoutUtils::LayoutMetricsRect(
            AknLayoutUtils::ETitlePane, mainPane );
        TAknLayoutRect titleIconPaneLayoutRect;
        titleIconPaneLayoutRect.LayoutRect(
            mainPane,
            AknLayoutScalable_Avkon::title_pane_g2( 0 ).LayoutLine() );
        TSize iconSize = titleIconPaneLayoutRect.Rect().Size();
        User::LeaveIfError( AknIconUtils::SetSize( bitmap, iconSize ) );

        // Create duplicates of the icon to be used
        CFbsBitmap* pBitmap = new( ELeave ) CFbsBitmap();
        CleanupStack::PushL( pBitmap );
        User::LeaveIfError( pBitmap->Duplicate( bitmap->Handle() ) );
        CFbsBitmap* pMask = new( ELeave ) CFbsBitmap();
        CleanupStack::PushL( pMask );
        User::LeaveIfError( pMask->Duplicate( mask->Handle() ) );
        CleanupStack::Pop( 2 ); // pBitmap, pMask

        // Transfer ownership to the image
        image->SetBitmap( pBitmap );
        image->SetMask( pMask );
        }

    CleanupStack::PopAndDestroy( iconArray );
    return image;
    }

// End of File
