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
* Description:  Phonebook 2 icon factory.
*
*/


#include <CPbk2IconFactory.h>

// Phonebook2
#include "CPbk2IconInfo.h"
#include <RPbk2LocalizedResourceFile.h>
#include <Pbk2DataCaging.hrh>
#include <Pbk2CommonUi.rsg>
#include <CPbk2IconInfoContainer.h>

// System includes
#include <AknUtils.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <AknsUtils.h>
#include <aknenv.h>
#include <barsread.h>
#include <gulicon.h>

#include <CPbk2ServiceManager.h> 
#include <MPbk2ApplicationServices.h>
#include <CPbk2ApplicationServices.h>
// Debugging headers
#include <Pbk2Debug.h>

/// Unnamed namespace for local definitions
namespace {

_LIT( KPbk2CommonUiResFile, "Pbk2CommonUi.rsc" );

/**
 * Creates a bitmap from given icon info using Avkon services.
 *
 * @param aSkin         Skin instance.
 * @param aBitmap       Created bitmap.
 * @param aMask         Created mask.
 * @param aIconInfo     Icon information.
 */
void CreateBitmapsLC(
        MAknsSkinInstance* aSkin,
        CFbsBitmap*& aBitmap,
        CFbsBitmap*& aMask,
        const CPbk2IconInfo& aIconInfo )
    {
    //Instantiate external bitmaps. We use rather graceful approach because we're
    //handling external bitmaps. So if problems we don't leave and paralyse system
    //but just try to continue if possible.
    if (aIconInfo.IsIconExternal() )
        {
        //Check external bitmaps really exist
        if(!aIconInfo.ExternalBitmap() || !aIconInfo.ExternalMask())
            {
            User::Leave(KErrBadHandle);
            }

        //Calculate size for external icon
        TSize size = aIconInfo.ExternalIconSize();

        //Try to instantiate ext bitmaps if not yet done, otherwise duplicating fails
        if(!aIconInfo.ExternalBitmap()->Handle())
            {
            CFbsBitmap* tmp = const_cast<CFbsBitmap*>(aIconInfo.ExternalBitmap());
            AknIconUtils::SetSize( tmp, size );
            tmp = const_cast<CFbsBitmap*>(aIconInfo.ExternalMask());
            AknIconUtils::SetSize( tmp, size );
            }

        //Create bitmaps and try to duplicate their handles to point to external bitmaps
        aBitmap = new( ELeave ) CFbsBitmap();
        CleanupStack::PushL( aBitmap );
        aMask = new( ELeave ) CFbsBitmap();
        CleanupStack::PushL( aMask );
        aBitmap->Duplicate( aIconInfo.ExternalBitmap()->Handle() );
        aMask->Duplicate( aIconInfo.ExternalMask()->Handle() );

        //Try always to set size as ext icon may have been used by some other app too.
        AknIconUtils::SetSize( aBitmap, size );
        AknIconUtils::SetSize( aMask, size );
        return;
        }

    if ( aIconInfo.ColorId() == KAknsIIDNone )
        {
        AknsUtils::CreateIconLC(
             aSkin, aIconInfo.SkinId(), aBitmap, aMask,
             aIconInfo.MbmFileName(),
             aIconInfo.IconId(), aIconInfo.MaskId() );
        }
    else
        {
        AknsUtils::CreateColorIconLC(
             aSkin, aIconInfo.SkinId(),
             aIconInfo.ColorId(), aIconInfo.ColorIndex(),
             aBitmap, aMask,
             aIconInfo.MbmFileName(),
             aIconInfo.IconId(), aIconInfo.MaskId(),
             AKN_LAF_COLOR_STATIC( aIconInfo.DefaultColorIndex() ) );
        }
    }

/**
 * Creates a bitmap from given icon info using Avkon services.
 *
 * @param aSkin         Skin instance.
 * @param aBitmap       Created bitmap.
 * @param aMask         Created mask.
 * @param aIconInfo     Icon information.
 */
void CreateBitmapsL(
        MAknsSkinInstance* aSkin,
        CFbsBitmap*& aBitmap,
        CFbsBitmap*& aMask,
        const CPbk2IconInfo& aIconInfo )
    {
    CreateBitmapsLC( aSkin, aBitmap, aMask, aIconInfo );
    CleanupStack::Pop( 2 ); // bitmap, mask
    }

} /// namespace

// --------------------------------------------------------------------------
// CPbk2IconFactory::CPbk2IconFactory
// --------------------------------------------------------------------------
//
CPbk2IconFactory::CPbk2IconFactory(
        const CPbk2IconInfoContainer* aIconContainer ) :
            iIconContainer( aIconContainer )
    {
    }

// --------------------------------------------------------------------------
// CPbk2IconFactory::~CPbk2IconFactory
// --------------------------------------------------------------------------
//
CPbk2IconFactory::~CPbk2IconFactory()
    {
    delete iAllIconsContainer;
    Release( iAppServices );
    }

// --------------------------------------------------------------------------
// CPbk2IconFactory::NewLC
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2IconFactory* CPbk2IconFactory::NewL()
    {
    CPbk2IconFactory* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2IconFactory::NewLC
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2IconFactory* CPbk2IconFactory::NewLC()
    {
    CPbk2IconFactory* self = new( ELeave ) CPbk2IconFactory( NULL );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2IconFactory::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2IconFactory* CPbk2IconFactory::NewL(
        const CPbk2IconInfoContainer& aIconContainer )
    {
    CPbk2IconFactory* self = NewLC( aIconContainer );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2IconFactory::NewLC
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2IconFactory* CPbk2IconFactory::NewLC(
        const CPbk2IconInfoContainer& aIconContainer )
    {
    CPbk2IconFactory* self = new( ELeave )
        CPbk2IconFactory( &aIconContainer );
    CleanupStack::PushL( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2IconFactory::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2IconFactory::ConstructL()
    {
    CCoeEnv& coeEnv = *CCoeEnv::Static();

    RPbk2LocalizedResourceFile resFile( coeEnv );
    resFile.OpenLC( KPbk2RomFileDrive,
                   KDC_RESOURCE_FILES_DIR, KPbk2CommonUiResFile );

    TResourceReader reader;
    coeEnv.CreateResourceReaderLC( reader, R_PBK2_ICON_INFO_ARRAY );

    iAllIconsContainer = CPbk2IconInfoContainer::NewL( reader );
    
    //Calculate preferred size for xsp service icons 
    TRect mainPane;
    AknLayoutUtils::LayoutMetricsRect(
        AknLayoutUtils::EMainPane, mainPane );
    TAknLayoutRect listLayoutRect;
    listLayoutRect.LayoutRect(
        mainPane,
        AknLayoutScalable_Avkon::list_single_graphic_pane_g1(0).LayoutLine() );
    TSize size(listLayoutRect.Rect().Size());

    // Add xsp service icons2
    if ( !iAppServices )
    	{
    	iAppServices = CPbk2ApplicationServices::InstanceL();
    	}
    CPbk2ServiceManager& servMan( iAppServices->ServiceManager()); 
    
    const CPbk2ServiceManager::RServicesArray& services = servMan.Services();
    TUid uid;
    uid.iUid = KPbk2ServManId;
    for ( TInt i = 0; i < services.Count(); i++ )
        {
        const CPbk2ServiceManager::TService& service = services[i];
        if ( service.iBitmapId && service.iBitmap )
            {
            TPbk2IconId id = TPbk2IconId( uid, services[i].iBitmapId );
            CPbk2IconInfo* info = CPbk2IconInfo::NewLC(
                id, services[i].iBitmap, services[i].iMask, size );
            iAllIconsContainer->AppendIconL(info);
            CleanupStack::Pop(info);
            }
        }    
    
    CleanupStack::PopAndDestroy( 2 ); // resFile, reader
    iIconContainer = iAllIconsContainer;
    }

// --------------------------------------------------------------------------
// CPbk2IconFactory::CreateIconL
// --------------------------------------------------------------------------
//
EXPORT_C CGulIcon* CPbk2IconFactory::CreateIconL(
        const TPbk2IconId& aIconId ) const
    {
    CGulIcon* icon = CreateIconLC( aIconId );
    if ( icon )
        {
        CleanupStack::Pop( icon );
        }
    return icon;
    }

// --------------------------------------------------------------------------
// CPbk2IconFactory::CreateIconLC
// --------------------------------------------------------------------------
//
EXPORT_C CGulIcon* CPbk2IconFactory::CreateIconLC(
        const TPbk2IconId& aIconId ) const
    {
    CGulIcon* icon = CGulIcon::NewLC();

    const CPbk2IconInfo* iconInfo = iIconContainer->Find( aIconId );

    if ( !iconInfo )
        {
        return icon;
        }

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2IconInfoContainer::LoadBitmapL icon found"));

    icon->SetBitmapsOwnedExternally(EFalse);

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2IconInfoContainer::LoadBitmapL about to create icon"));

    CreateBitmapsL( AknsUtils::SkinInstance(),
        bitmap, mask, *iconInfo );

    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ( "CPbk2IconInfoContainer::LoadBitmapL icon create done" ) );

    icon->SetBitmap(bitmap);
    icon->SetMask(mask);

    return icon;
    }

// --------------------------------------------------------------------------
// CPbk2IconFactory::CreateIconL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2IconFactory::CreateIconL( const TPbk2IconId& aIconId,
        MAknsSkinInstance& aSkin, CFbsBitmap*& aBitmap, CFbsBitmap*& aMask )
    {
    CreateIconLC( aIconId, aSkin, aBitmap, aMask );
    CleanupStack::Pop( 2 ); // aBitmap, aMask
    }

// --------------------------------------------------------------------------
// CPbk2IconFactory::CreateIconLC
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2IconFactory::CreateIconLC( const TPbk2IconId& aIconId,
        MAknsSkinInstance& aSkin, CFbsBitmap*& aBitmap, CFbsBitmap*& aMask )
    {
    const CPbk2IconInfo* iconInfo = iIconContainer->Find( aIconId );
    if ( iconInfo )
        {
        CreateBitmapsLC( &aSkin, aBitmap, aMask, *iconInfo );
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    }

// --------------------------------------------------------------------------
// CPbk2IconFactory::CreateImageL
// --------------------------------------------------------------------------
//
EXPORT_C CEikImage* CPbk2IconFactory::CreateImageL(
        const TPbk2IconId& aIconId ) const
    {
    CEikImage* image = CreateImageLC( aIconId );
    if ( image )
        {
        CleanupStack::Pop( image );
        }
    return image;
    }

// --------------------------------------------------------------------------
// CPbk2IconFactory::CreateImageLC
// --------------------------------------------------------------------------
//
EXPORT_C CEikImage* CPbk2IconFactory::CreateImageLC(
        const TPbk2IconId& aIconId ) const
    {
    CEikImage* image = NULL;
    const CPbk2IconInfo* iconInfo = iIconContainer->Find( aIconId );
    if ( iconInfo )
        {
        image = new(ELeave) CEikImage;
        CleanupStack::PushL( image );

        CFbsBitmap* bitmap = NULL;
        CFbsBitmap* mask = NULL;

        CreateBitmapsL( AknsUtils::SkinInstance(),
            bitmap, mask, *iconInfo );

        image->SetBitmap( bitmap );
        image->SetMask( mask );
        }

    return image;
    }

// --------------------------------------------------------------------------
// CPbk2IconFactory::AppendIconL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2IconFactory::AppendIconL(
        CPbk2IconInfo* aIconInfo )
    {
    if( !iAllIconsContainer )
        {
        ConstructL();
        }

    iAllIconsContainer->AppendIconL(aIconInfo);
    iIconContainer = iAllIconsContainer;
    }

// End of File
