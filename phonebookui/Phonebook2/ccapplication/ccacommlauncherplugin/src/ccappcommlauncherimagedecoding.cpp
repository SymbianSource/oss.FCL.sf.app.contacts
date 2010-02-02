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

#include <bitmaptransforms.h>
#include "ccappcommlauncherimagedecoding.h"

const TInt KDelay = 500000; // 0.5s
// ---------------------------------------------------------------------------
// Two-phase construction
// ---------------------------------------------------------------------------
//
CCCAppCommLauncherImageDecoding* CCCAppCommLauncherImageDecoding::NewL(
        CCCAppCommLauncherHeaderControl* aHeader, const TDesC8* aBitmapData, const TDesC* aImageFile)
    {
    CCCAppCommLauncherImageDecoding* self = new (ELeave) CCCAppCommLauncherImageDecoding(aHeader);
    CleanupStack::PushL(self);
    self->ConstructL(aBitmapData, aImageFile);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// First phase (C++) constructor
// ---------------------------------------------------------------------------
//
CCCAppCommLauncherImageDecoding::CCCAppCommLauncherImageDecoding(CCCAppCommLauncherHeaderControl* aHeader) : 
CActive( CActive::EPriorityStandard ), iHeader(aHeader)
    { 
    CActiveScheduler::Add(this); 
    }

// ---------------------------------------------------------------------------
// ConstructL, second phase constructor
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherImageDecoding::ConstructL(const TDesC8* aBitmapData, const TDesC* aImageFile)
    {
    if ( aImageFile )
        {
        iImageFullName = aImageFile->AllocL();
        }
    if ( aBitmapData )
        {
        iBitmapData = aBitmapData->AllocL();
        }
    User::LeaveIfError( iFs.Connect() );
    iTimer.CreateLocal();
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCCAppCommLauncherImageDecoding::~CCCAppCommLauncherImageDecoding()
    {
    DoCancel();
    if (iBitmap)
        {
        delete iBitmap;
        iBitmap = NULL;
        }
    if (iBitmapData)
        {
        delete iBitmapData;
        iBitmapData = NULL;
        }
    if (iImageFullName)
        {
        delete iImageFullName;
        }
    iFs.Close();
    iTimer.Close();
    }

// ---------------------------------------------------------------------------
// Starts the decoding process
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherImageDecoding::StartL( TSize aImageSize )
    {
    iDecoderState = ECcaConvertThumbnailImage;
    iBitmapSize = aImageSize;
    CreateBitmapL();
    }

// ---------------------------------------------------------------------------
// Called by the active object framework when the decoding (request) is
// completed.
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherImageDecoding::RunL() 
    {
    User::LeaveIfError( iStatus.Int() );
    switch ( iDecoderState )
	    {
	    case ECcaConvertThumbnailImage:
	    	{
	        iDecoderState = ECcaScaleThumbnail;
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
	        iHeader->SetBitmap( iBitmap );
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
void CCCAppCommLauncherImageDecoding::DoCancel()
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

void CCCAppCommLauncherImageDecoding::ScaleBitmapL()
    {   
    iBitmapScaler = CBitmapScaler::NewL();
    iBitmapScaler->Scale( &iStatus, *iBitmap, iBitmapSize );
    SetActive();
    }

void CCCAppCommLauncherImageDecoding::CreateBitmapL()
    {
    if ( iDecoderState == ECcaConvertThumbnailImage )
        {
        iImgDecoder = CImageDecoder::DataNewL( iFs, *iBitmapData, CImageDecoder::EOptionAlwaysThread );
        }
    else if ( iDecoderState == ECcaConvertImageFromFile )
        {
        // leaaves if file doesn't exist
        TRAPD ( initDecoder, iImgDecoder = CImageDecoder::FileNewL( iFs, *iImageFullName, CImageDecoder::EOptionAlwaysThread ) );
        if ( initDecoder )
        	{
	        delete iBitmapScaler;
	        iBitmapScaler = NULL;
	        delete iImgDecoder;
	        iImgDecoder = NULL;
        	return;
        	}
        }
   
   if ( !iBitmap )
       {
       TFrameInfo info = iImgDecoder->FrameInfo();
       iBitmap = new ( ELeave ) CFbsBitmap;
       User::LeaveIfError( iBitmap->Create( info.iOverallSizeInPixels, info.iFrameDisplayMode ));
       }
 
    iStatus = KRequestPending;
    iImgDecoder->Convert( &iStatus, *iBitmap );
    SetActive();
    }
