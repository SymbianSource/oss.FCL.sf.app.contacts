/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Utility class for asynchronously decoding the header thumbnail image
*
*/


#ifndef CCAPPIMAGEDECODING_H_
#define CCAPPIMAGEDECODING_H_

#include <e32base.h>

class CBitmapScaler;
class CImageDecoder;
class CFbsBitmap;
class RFs;

class MCCAppImageDecodingObserver
	{
public:
	/**
	 * Bitmap is decoded.
	 * @param aBitmap Decoded bitmap, ownership is transferred.
	 */ 
	virtual void BitmapReadyL( CFbsBitmap* aBitmap ) = 0;

protected:
	MCCAppImageDecodingObserver() {};
	virtual ~MCCAppImageDecodingObserver() {};
	};


/**
 * Utility class for asynchronously decoding the header thumbnail image
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ccapputil.dll
 *  @since S60 v9.2
 */
NONSHARABLE_CLASS( CCCAppImageDecoding ) : public CActive
    {
public:
    /**
     * Two-phased constructor
     * 
     * @param aObserver 
     * @param aBitmapData Descriptor containing the bitmap data stream, takes ownership
     * @param aImageFile Descriptor containing image file name full path, takes ownership
     */
    IMPORT_C static CCCAppImageDecoding* NewL(
            MCCAppImageDecodingObserver& aObserver,
            RFs& aFs,
            HBufC8* aBitmapData, 
            HBufC* aImageFile );
    
    /**
     * Destructor
     */
    ~CCCAppImageDecoding();
    
    /**
     * Starts the decoding process
     * 
     * @param aImageSize defines image size
     */
    IMPORT_C void StartL( const TSize& aImageSize );

private:
    /**
     * From CActive
     * (see details from baseclass )
     */
    void RunL();
    
    /**
     * From CActive
     * (see details from baseclass )
     */
    void DoCancel();
private:
    /**
     * First-phase (C++) constructor
     */
    inline CCCAppImageDecoding(
            MCCAppImageDecodingObserver& aObserver,
            RFs& aFs,
            HBufC8* aBitmapData, 
            HBufC* aImageFile);
    
    /**
     * Second phase constructor
     */
    inline void ConstructL();
    
    /**
     * Scales bitmap
     *
     */
    void ScaleBitmapL();

    void CropBitmapL();
    
    /**
     * Create bitmap
     *
     */
    void CreateBitmapL();
    
    enum TCcaImageDecoderState
        {
        ECcaConvertThumbnailImage = 1,
        ECcaScaleThumbnail,
        ECcaReadImageFromFile,
        ECcaConvertImageFromFile,
        ECcaScaleImage
        };
    
    /**
     * The image scaler whose service is called to scale the image
     * Own.
     */
    CBitmapScaler* iBitmapScaler;
    
    /**
     * The image decoder whose service is called to decode the image
     * Own.
     */
    CImageDecoder* iImgDecoder;
    
    MCCAppImageDecodingObserver& iObserver;
    
    /**
     * Handle to the file server session for loading the bitmap
     */ 
    RFs& iFs;
    
    /**
     * The bitmap.
     * Owned until decoding is complete, then ownersip is transferred to header control.
     */
    CFbsBitmap* iBitmap; // owned until completion
    
    /**
     * Own.
     */
    HBufC8* iBitmapData;

    /**
     * Own.
     */
    HBufC* iImageFullName;
    
    /** 
      * Contact bitmap size 
      */ 
    TSize iBitmapSize;
    
    /** 
      * Decoder internal state
      */ 
    TInt iDecoderState;
    
    /** 
      * Timer used to delay image conversion
      */ 
    RTimer iTimer;
    };

#endif /*CCAPPIMAGEDECODING_H_*/
