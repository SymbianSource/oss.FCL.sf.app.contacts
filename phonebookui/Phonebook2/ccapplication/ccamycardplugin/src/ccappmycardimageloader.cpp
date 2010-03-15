/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of MyCard image loader
*
*/

#include "ccappmycardimageloader.h"
#include "ccappmycardcommon.h"

// phonebook
#include <CVPbkContactManager.h>
#include <MVPbkStoreContact.h>
#include <MVPbkFieldType.h>

#include <CPbk2ImageManager.h>
#include <TPbk2ImageManagerParams.h>
#include <VPbkEng.rsg>
#include <MVPbkContactFieldBinaryData.h>
#include <MVPbkContactFieldTextData.h>

// System
#include <fbs.h>
#include <eikenv.h>


// ---------------------------------------------------------------------------
// CCCAppMyCardImageLoader::NewL
// ---------------------------------------------------------------------------
//
CCCAppMyCardImageLoader* CCCAppMyCardImageLoader::NewL(
    MMyCardImageLoaderObserver& aObserver )
    {
    CCA_DP(KMyCardLogFile, CCA_L("->CCCAppMyCardImageLoader::NewL()"));
    
    CCCAppMyCardImageLoader* self = 
        new ( ELeave ) CCCAppMyCardImageLoader( aObserver );
    
    CCA_DP(KMyCardLogFile, CCA_L("<-CCCAppMyCardImageLoader::NewL()"));
    return self;
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardImageLoader::~CCCAppMyCardImageLoader
// ---------------------------------------------------------------------------
//
CCCAppMyCardImageLoader::~CCCAppMyCardImageLoader()
    {
    CCA_DP(KMyCardLogFile, 
        CCA_L("->CCCAppMyCardImageLoader::~CCCAppMyCardImageLoader()"));

    delete iImageDecoding;
    delete iImageBuffer;
    delete iImageFileName;

    CCA_DP(KMyCardLogFile, 
        CCA_L("<-CCCAppMyCardImageLoader::~CCCAppMyCardImageLoader()"));
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardImageLoader::CCCAppMyCardImageLoader
// ---------------------------------------------------------------------------
//
inline CCCAppMyCardImageLoader::CCCAppMyCardImageLoader(
    MMyCardImageLoaderObserver& aObserver ) :
        iObserver( aObserver )
    {
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardImageLoader::LoadContactImageL
// ---------------------------------------------------------------------------
//
void CCCAppMyCardImageLoader::LoadContactImageL( 
    MVPbkStoreContact& aContact,
    const TSize& aThumbnailSize )
    {
    CCA_DP(KMyCardLogFile, 
        CCA_L("->CCCAppMyCardImageLoader::LoadContactImageL()"));

    MVPbkStoreContactFieldCollection& fields = aContact.Fields();
    
    delete iImageBuffer;
    iImageBuffer = NULL;
    delete iImageFileName;
    iImageFileName = NULL;
    
    const TInt fieldCount = fields.FieldCount();                           
    
    // Check all the fields and store first and last name
    for ( TInt i = 0; i < fieldCount;  ++i )
        {
        MVPbkStoreContactField& field = fields.FieldAt( i );    
        const MVPbkFieldType* fieldType = field.BestMatchingFieldType();
        
        if ( fieldType )
            {
            if ( fieldType->FieldTypeResId() == R_VPBK_FIELD_TYPE_THUMBNAILPIC )
                {
                MVPbkContactFieldData& contactField = field.FieldData(); 
                iImageBuffer = 
                    MVPbkContactFieldBinaryData::Cast(contactField).BinaryData().AllocL();
                }
            
            if ( fieldType->FieldTypeResId() == R_VPBK_FIELD_TYPE_CALLEROBJIMG )
                {
                MVPbkContactFieldData& contactField = field.FieldData();                        
                iImageFileName = MVPbkContactFieldTextData::Cast(contactField).Text().AllocL();
                }
            }
        }
        
    if( iImageBuffer )
        {
        delete iImageDecoding;
        iImageDecoding = NULL;
        
        RFs& fs = CEikonEnv::Static()->FsSession();
        iImageDecoding = CCCAppImageDecoding::NewL(
                *this, 
                fs, 
                iImageBuffer, 
                iImageFileName );
        iImageFileName = NULL;  // ownership is moved to CCCAppImageDecoding
        iImageBuffer = NULL;  // ownership is moved to CCCAppImageDecoding   
        iImageDecoding->StartL( aThumbnailSize );      
        }
    else
        {
        iObserver.ThumbnailLoadError( KErrNotFound );
        }        
    
    CCA_DP(KMyCardLogFile, 
        CCA_L("<-CCCAppMyCardImageLoader::LoadContactImageL()"));
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardImageLoader::LoadImageL
// ---------------------------------------------------------------------------
//
void CCCAppMyCardImageLoader::LoadImageL( 
    const TDesC8& aImageData,
    const TDesC& aImageFileName,
    const TSize& aThumbnailSize )
    {
    CCA_DP(KMyCardLogFile, 
        CCA_L("->CCCAppMyCardImageLoader::LoadImageL()"));

    delete iImageDecoding;
    iImageDecoding = NULL;
    
    RFs& fs = CEikonEnv::Static()->FsSession();
    iImageDecoding = CCCAppImageDecoding::NewL(
            *this, 
            fs, 
            aImageData.AllocLC(), 
            aImageFileName.AllocLC() );
    CleanupStack::Pop( 2 ); // imagedata, imagefilename
    iImageDecoding->StartL( aThumbnailSize );      

    CCA_DP(KMyCardLogFile, 
        CCA_L("<-CCCAppMyCardImageLoader::LoadImageL()"));
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardImageLoader::ResizeImageL
// ---------------------------------------------------------------------------
//
void CCCAppMyCardImageLoader::ResizeImageL( const TSize& aThumbnailSize )
    {
    if( iImageDecoding  )
        {
        iImageDecoding->StartL( aThumbnailSize );      
        }
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardImageLoader::BitmapReadyL
// ---------------------------------------------------------------------------
//
void CCCAppMyCardImageLoader::BitmapReadyL( CFbsBitmap* aBitmap )
    {
    iObserver.ThumbnailReady( aBitmap );
    }


// End of File
