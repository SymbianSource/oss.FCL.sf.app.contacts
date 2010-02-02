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

#include <e32base.h>
#include <MPbk2ImageOperationObservers.h>

class CVPbkContactManager;
class CPbk2ImageManager;
class MVPbkStoreContact;
class MPbk2ImageOperation;

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
    public MPbk2ImageGetObserver
{

public:

    /**
     * Two-phased constructor.
     */
    static CCCAppMyCardImageLoader* NewL(
        CVPbkContactManager& aContactManager,
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
    void LoadContactImageL( MVPbkStoreContact& aContact );

    
private: // From MPbk2ImageGetObserver
    virtual void Pbk2ImageGetComplete(
            MPbk2ImageOperation& aOperation,
            CFbsBitmap* aBitmap );
    virtual void Pbk2ImageGetFailed(
            MPbk2ImageOperation& aOperation,
            TInt aError );

    
private: // constructors
    
    /**
     * Constructor
     */
    CCCAppMyCardImageLoader(
        CVPbkContactManager& aContactManager,
        MMyCardImageLoaderObserver& aObserver );

    /**
     * Constructor
     */
    void ConstructL();

    
private: // data
	
	/// Not own. Virtual phonebook contact manager
	CVPbkContactManager& iContactManager;
	
	/// Not own. Observer
	MMyCardImageLoaderObserver& iObserver;
	
	/// Own. Phonebook image manager
	CPbk2ImageManager* iImageManager;
	
	/// Own. Image operation
	MPbk2ImageOperation* iOperation;
};

#endif // CCAPPMYCARDIMAGELOADER_H

// End of File
