/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the utility class for asynchronously decoding the header thumbnail image
*
*/

#include "ccappimagedecoding.h"
#include <bitmaptransforms.h>
#include <Pbk2PresentationUtils.h>
#include <imageconversion.h>

namespace{

const TInt KDelay = 500000; // 0.5s

inline TSize ScaledLoadSize( const TSize& aOriginalSize, TInt aScaleFactor )
    {
    // Divide original size with scalefactor
    // Round always up if result is not integer
    return TSize( aOriginalSize.iWidth / aScaleFactor + ( aOriginalSize.iWidth % aScaleFactor ? 1 : 0 ),
               aOriginalSize.iHeight / aScaleFactor + ( aOriginalSize.iHeight % aScaleFactor ? 1 : 0 ) );
    }
} // namespace

// ---------------------------------------------------------------------------
// Two-phase construction
// ---------------------------------------------------------------------------
//
EXPORT_C CCCAppImageDecoding* CCCAppImageDecoding::NewL(
        MCCAppImageDecodingObserver& aObserver,
        RFs& aFs,
        HBufC8* aBitmapData, 
        HBufC* aImageFile)
    {
    CCCAppImageDecoding* self = new (ELeave) CCCAppImageDecoding(
            aObserver, aFs, aBitmapData, aImageFile);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);    
    
    self->iBitmapData = aBitmapData;   // take ownship
    self->iImageFullName = aImageFile;  // take ownship

    return self;
    }

// ---------------------------------------------------------------------------
// First phase (C++) constructor
// ---------------------------------------------------------------------------
//
inline CCCAppImageDecoding::CCCAppImageDecoding(
        MCCAppImageDecodingObserver& aObserver,
        RFs& aFs,
        HBufC8* aBitmapData, 
        HBufC* aImageFile) : 
CActive( CActive::EPriorityStandard ), 
iObserver(aObserver), 
iFs(aFs)
    { 
    CActiveScheduler::Add(this); 
    }

// ---------------------------------------------------------------------------
// Second phase (C++) constructor
// ---------------------------------------------------------------------------
//
inline void CCCAppImageDecoding::ConstructL()
    {
    User::LeaveIfError( iTimer.CreateLocal() );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCCAppImageDecoding::~CCCAppImageDecoding()
    {
    Cancel();
    if (iImgDecoder)
        {
        iImgDecoder->Cancel();
        delete iImgDecoder;
        }
    if (iBitmapScaler)
        {
        iBitmapScaler->Cancel();
        delete iBitmapScaler;
        }
    delete iBitmap;
    delete iBitmapData;
    delete iImageFullName;
    iTimer.Close();
    }

// ---------------------------------------------------------------------------
// Starts the decoding process
// ---------------------------------------------------------------------------
//
EXPORT_C void CCCAppImageDecoding::StartL( const TSize& aImageSize )
    {
    Cancel();
    iDecoderState = ECcaConvertThumbnailImage;
    iBitmapSize = aImageSize;
    CreateBitmapL();
    }

// ---------------------------------------------------------------------------
// Called by the active object framework when the decoding (request) is
// completed.
// ---------------------------------------------------------------------------
//
void CCCAppImageDecoding::RunL() 
    {
    User::LeaveIfError( iStatus.Int() );
    switch ( iDecoderState )
	    {
	    case ECcaConvertThumbnailImage:
	    	{
	        iDecoderState = ECcaScaleThumbnail;
	        CropBitmapL();
	        ScaleBitmapL();
	        break;
	    	}
	
	    case ECcaScaleThumbnail:
	    	{
	    	if ( iImageFullName )
	    		{
	            iDecoderState = ECcaReadImageFromFile;
	            SetPriority( EPriorityIdle );
	            iTimer.After( iStatus, KDelay ); 
	            SetActive();
	    		}
	    	// don't break here
	    	}
	    	
	    case ECcaScaleImage:
	    	{
	        // Ownership of the bitmap is transferred
	        iObserver.BitmapReadyL( iBitmap );
	        iBitmap = NULL;
	        delete iBitmapScaler;
	        iBitmapScaler = NULL;
	        delete iImgDecoder;
	        iImgDecoder = NULL;
	        break;
	    	}
	        
	    case ECcaReadImageFromFile:
	    	{
	    	if ( iImageFullName )
	    		{
	            iDecoderState = ECcaConvertImageFromFile;
	            CreateBitmapL();
	    		}
	    	break;
	    	}
	         
	    case ECcaConvertImageFromFile:
	    	{
	        iDecoderState = ECcaScaleImage;
	        SetPriority( EPriorityStandard );
            CropBitmapL();
	        ScaleBitmapL();
	        break;
	    	}
	        
	    default:
	        break;
	    }
    }
// ---------------------------------------------------------------------------
// Called when the decoding (request) is cancelled for some reason.
// ---------------------------------------------------------------------------
//
void CCCAppImageDecoding::DoCancel()
    { 
    if (iImgDecoder)
        {
        iImgDecoder->Cancel();
        delete iImgDecoder;
        iImgDecoder = NULL;
        }
    if ( iBitmapScaler )
        {
        iBitmapScaler->Cancel();
        delete iBitmapScaler;
        iBitmapScaler = NULL;
        }
    iTimer.Cancel();
    }

void CCCAppImageDecoding::CropBitmapL()
    {
    TSize dummy;
    Pbk2PresentationImageUtils::CropImageL(
            *iBitmap, 
            Pbk2PresentationImageUtils::ECropping, 
            dummy );
    }

void CCCAppImageDecoding::ScaleBitmapL()
    {   
    iBitmapScaler = CBitmapScaler::NewL();
    iBitmapScaler->Scale( &iStatus, *iBitmap, iBitmapSize );
    SetActive();
    }

void CCCAppImageDecoding::CreateBitmapL()
    {
    TInt initDecoder( KErrGeneral );
    if ( iDecoderState == ECcaConvertThumbnailImage && iBitmapData )
        {
        iImgDecoder = CImageDecoder::DataNewL( iFs, *iBitmapData, CImageDecoder::EOptionAlwaysThread );
        initDecoder = KErrNone;
        }
    else if ( iDecoderState == ECcaConvertImageFromFile && iImageFullName )
        {
        // leaaves if file doesn't exist
        TRAP ( initDecoder, iImgDecoder = CImageDecoder::FileNewL( iFs, *iImageFullName, CImageDecoder::EOptionAlwaysThread ) );
        }
    if ( initDecoder )
        {
        delete iBitmapScaler;
        iBitmapScaler = NULL;
        delete iImgDecoder;
        iImgDecoder = NULL;
        return;
        }
    
   if ( !iBitmap )
       {
       const TFrameInfo info = iImgDecoder->FrameInfo();
	   // calculate size for the bitmap
       const TSize size( iBitmapSize );
       TSize loadSize( info.iOverallSizeInPixels );
       
       for( TInt i = 1 ; i < 9 ; i = i * 2 )
           {
		   // we can use scalers 1:1 1:2 1:4 1:8
           TSize calcSize( ScaledLoadSize( info.iOverallSizeInPixels, i ) );
           if( calcSize.iHeight < size.iHeight ||
               calcSize.iWidth < size.iWidth )
               {
               break;
               }
           loadSize = calcSize;
           }
       
       iBitmap = new ( ELeave ) CFbsBitmap;
       User::LeaveIfError( iBitmap->Create( loadSize, info.iFrameDisplayMode ));
       }
 
    iStatus = KRequestPending;
    iImgDecoder->Convert( &iStatus, *iBitmap );
    SetActive();
    }
