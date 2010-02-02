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

// System
#include <fbs.h>


// ---------------------------------------------------------------------------
// CCCAppMyCardImageLoader::NewL
// ---------------------------------------------------------------------------
//
CCCAppMyCardImageLoader* CCCAppMyCardImageLoader::NewL(
    CVPbkContactManager& aContactManager,
    MMyCardImageLoaderObserver& aObserver )
    {
    CCA_DP(KMyCardLogFile, CCA_L("->CCCAppMyCardImageLoader::NewL()"));
    
    CCCAppMyCardImageLoader* self = 
        new ( ELeave ) CCCAppMyCardImageLoader( aContactManager, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
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
    
    delete iImageManager;
    delete iOperation;

    CCA_DP(KMyCardLogFile, 
        CCA_L("<-CCCAppMyCardImageLoader::~CCCAppMyCardImageLoader()"));
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardImageLoader::CCCAppMyCardImageLoader
// ---------------------------------------------------------------------------
//
inline CCCAppMyCardImageLoader::CCCAppMyCardImageLoader(
    CVPbkContactManager& aContactManager,
    MMyCardImageLoaderObserver& aObserver ) :
        iContactManager( aContactManager ),
        iObserver( aObserver )
    {
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardImageLoader::ConstructL
// ---------------------------------------------------------------------------
//
inline void CCCAppMyCardImageLoader::ConstructL()
    {
    iImageManager = CPbk2ImageManager::NewL( iContactManager );
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardImageLoader::LoadContactImageL
// ---------------------------------------------------------------------------
//
void CCCAppMyCardImageLoader::LoadContactImageL( 
    MVPbkStoreContact& aContact )
    {
    CCA_DP(KMyCardLogFile, 
        CCA_L("->CCCAppMyCardImageLoader::LoadContactImageL()"));
    
    // cancel previous operations
    delete iOperation;
    iOperation = NULL;
    
    const MVPbkFieldType* thumbType = iContactManager.FieldTypes().Find(
        R_VPBK_FIELD_TYPE_THUMBNAILPIC );
  
    if( thumbType )
        {
        if( iImageManager->HasImage( aContact, *thumbType ) )
            {
            // contact has image. load it.
            iOperation = iImageManager->GetImageAsyncL( 
                NULL, aContact, *thumbType, *this );
            }
        }
    
    if( !iOperation )
        {
        // no operation means the contact does not have image
        iObserver.ThumbnailLoadError( KErrNotFound );
        }
    
    CCA_DP(KMyCardLogFile, 
        CCA_L("<-CCCAppMyCardImageLoader::LoadContactImageL()"));
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardImageLoader::Pbk2ImageGetComplete
// ---------------------------------------------------------------------------
//
void CCCAppMyCardImageLoader::Pbk2ImageGetComplete(
    MPbk2ImageOperation& /*aOperation*/,
    CFbsBitmap* aBitmap )
    {
    CCA_DP(KMyCardLogFile, 
        CCA_L("->CCCAppMyCardImageLoader::Pbk2ImageGetComplete()"));
    
    delete iOperation;
    iOperation = NULL;
    
    // keep this last, so that observer can delete the image loader when it has
    // received the notification.
    iObserver.ThumbnailReady( aBitmap );
    
    CCA_DP(KMyCardLogFile, 
        CCA_L("<-CCCAppMyCardImageLoader::Pbk2ImageGetComplete()"));    
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardImageLoader::Pbk2ImageGetFailed
// ---------------------------------------------------------------------------
//
void CCCAppMyCardImageLoader::Pbk2ImageGetFailed( 
    MPbk2ImageOperation& /*aOperation*/,
    TInt aError )
    {
    CCA_DP(KMyCardLogFile, 
        CCA_L("->CCCAppMyCardImageLoader::Pbk2ImageGetFailed() %d"), aError );    
    
    delete iOperation;
    iOperation = NULL;

    // keep this last, so that observer can delete the image loader when it has
    // received the notification.
    iObserver.ThumbnailLoadError( aError );

    CCA_DP(KMyCardLogFile, 
        CCA_L("<-CCCAppMyCardImageLoader::Pbk2ImageGetFailed()"));    
    }

// End of File
