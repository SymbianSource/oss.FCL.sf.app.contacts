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
* Description:  Utility class for asynchronously decoding the header thumbnail image
*
*/


#ifndef CCAPPCOMMLAUNCHERIMAGEDECODING_H_
#define CCAPPCOMMLAUNCHERIMAGEDECODING_H_

#include "ccappcommlauncherheaders.h"

class CCCAppCommLauncherHeaderControl;
class CBitmapScaler;
/**
 * Utility class for asynchronously decoding the header thumbnail image
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ccappcommlauncherplugin.dll
 *  @since S60 v5.0
 */
class CCCAppCommLauncherImageDecoding : public CActive
    {
public:
    /**
     * Two-phased constructor
     * 
     * @param aHeader The header control to which the bitmap is to be inserted after loading
     * @param aBitmapData Descriptor containing the bitmap data stream
     * @param aImageFile Descriptor containing image file name full path
     */
    static CCCAppCommLauncherImageDecoding* NewL(CCCAppCommLauncherHeaderControl* aHeader, const TDesC8* aBitmapData, const TDesC* aImageFile);
    
    /**
     * Destructor
     */
    ~CCCAppCommLauncherImageDecoding();
    
    /**
     * Starts the decoding process
     * 
     * @param aImageSize defines image size
     */
    void StartL( TSize aImageSize );
    
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
     * 
     * @param aHeader The header control to which the bitmap is to be inserted after loading 
     */
    CCCAppCommLauncherImageDecoding(CCCAppCommLauncherHeaderControl* aHeader);
    
    /**
     * Second phase constructor
     * 
     * @param aBitmapData Descriptor containing the bitmap data stream
     * @param aImageFile Descriptor containing image file name full path
     */
    void ConstructL(const TDesC8* aBitmapData, const TDesC* aImageFile);
    
    /**
     * Scales bitmap
     *
     */
    void ScaleBitmapL();

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
    
    /**
     * The header control to which the bitmap is to be inserted after loading
     * Not own.
     */
    CCCAppCommLauncherHeaderControl* iHeader;
    
    /**
     * Handle to the file server session for loading the bitmap
     */ 
    RFs iFs;
    
    /**
     * The bitmap.
     * Owned until decoding is complete, then ownersip is transferred to header control.
     */
    CFbsBitmap* iBitmap; // owned until completion
    
    /**
     * Copy of the bitmap data stream
     * Own.
     */
    HBufC8* iBitmapData;

    /**
     * Copy of image name with path
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

#endif /*CCAPPCOMMLAUNCHERIMAGEDECODING_H_*/
