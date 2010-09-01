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

#ifndef CCAPPMYCARDIMAGELOADER_H
#define CCAPPMYCARDIMAGELOADER_H

#include <CCAppImageDecoding.h>

class MVPbkStoreContact;
class CCCAppImageDecoding;
class CFbsBitmap;

/**
 * MyCard image loading observer class
 */
class MMyCardImageLoaderObserver
{
public:
    
    /**
     * Notification method to report loaded images to observer.
     * 
	 * @param aThumbnail loaded contact thumbnail. Ownership transferred to
	 *        observer.
     */
    virtual void ThumbnailReady( CFbsBitmap* aThumbnail ) = 0;
    
    /**
     * Notification method to report failed load requests
     * 
     * @param aError Generic symbian error code
     */
    virtual void ThumbnailLoadError( TInt aError ) = 0;

protected:
    virtual ~MMyCardImageLoaderObserver() {}
};

/**
 *  Contact image loader
 *
 *  @lib ccappmycardplugin.dll
 *  @since S60 9.2
 */
NONSHARABLE_CLASS( CCCAppMyCardImageLoader ) : 
    public CBase,
    public MCCAppImageDecodingObserver
{

public:

    /**
     * Two-phased constructor.
     */
    static CCCAppMyCardImageLoader* NewL(
        MMyCardImageLoaderObserver& aObserver );

    /**
     * Destructor.
     */
    ~CCCAppMyCardImageLoader();

    
public: // Interface
       
    /**
     * Start loading contacts image. Will cancel any previous requests.
     * Once loading is complete the new image is delivered to observer 
     * with a call to MMyCardImageLoaderObserver::ThumbnailReady.
     * 
     * @param aContact source contact for the image
     */
    void LoadContactImageL( 
            MVPbkStoreContact& aContact, 
            const TSize& aThumbnailSize );
    
    /**
     * Start loading contacts image. Will cancel any previous requests.
     * Once loading is complete the new image is delivered to observer 
     * with a call to MMyCardImageLoaderObserver::ThumbnailReady.
     * 
     * @param aImageData source data for the image
     * @param aImageFileName filename of the image
     */
    void LoadImageL( 
        const TDesC8& aImageData,
        const TDesC& aImageFileName,
        const TSize& aThumbnailSize );
    
    void ResizeImageL( const TSize& aThumbnailSize );
    
private:    // From MCCAppImageDecodingObserver
    void BitmapReadyL( CFbsBitmap* aBitmap );
    
private: // constructors
    
    /**
     * Constructor
     */
    inline CCCAppMyCardImageLoader(
        MMyCardImageLoaderObserver& aObserver );
    
private: // data
	
	/// Not own. Observer
	MMyCardImageLoaderObserver& iObserver;
	
	/// Own.
	HBufC8* iImageBuffer;

    /// Own.
    HBufC* iImageFileName;
    
    /// Own
    CCCAppImageDecoding* iImageDecoding;
};

#endif // CCAPPMYCARDIMAGELOADER_H

// End of File
