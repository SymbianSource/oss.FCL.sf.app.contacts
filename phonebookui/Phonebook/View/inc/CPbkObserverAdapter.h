/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handles the adaptation between phonebook's observer classes
*
*/



#ifndef CPbkObserverAdapter_H
#define CPbkObserverAdapter_H

//  INCLUDES
#include <e32base.h>
#include <MPbkThumbnailOperationObservers.h> // MPbkThumbnailGetObserver

// FORWARD DECLARATIONS
class MPbkImageGetObserver;
class MPbkImageSetObserver;
class CFbsBitmap;
class MPbkImageGetImageObserver;
// CLASS DECLARATION

/**
*  Adapter for phonebook internal use.
*  This class handles the adaptation between 
*  MPbkThumbnailGetObserver/MPbkImageGetObserver, 
*  MPbkThumbnailSetObserver/MPbkImageSetObserver and
*  MPbkThumbnailGetImageObserver/MPbkImageGetImageObserver classes.
*
*  @lib Pbkview.lib
*  @since Series 60 3.0
*/
class CPbkObserverAdapter : public CBase, public MPbkThumbnailGetObserver,
    public MPbkThumbnailSetObserver, public MPbkThumbnailGetImageObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CPbkObserverAdapter* NewL();
        
        /**
        * Destructor.
        */
        ~CPbkObserverAdapter();

    public: // API for retrieving adapted interfaces. 
        /**
        * Sets MPbkImageGetObserver reference value to adapter
        * @param aPbkImageGetObserver Reference to MPbkImageGetObserver
        */
        void SetPbkImageGetObserver(MPbkImageGetObserver& aPbkImageGetObserver);

        /**
        * Returns a reference to the adapted MPbkThumbnailGetObserver interface
        * @return Reference to MPbkThumbnailGetObserver
        */ 
        MPbkThumbnailGetObserver& PbkThumbnailGetObserver();


        /**
        * Sets MPbkImageSetObserver reference value to adapter
        * @param aPbkImageSetObserver Reference to MPbkImageSetObserver
        */
        void SetPbkImageSetObserver(MPbkImageSetObserver& aPbkImageSetObserver);

        /**
        * Returns a reference to the adapted MPbkThumbnailSetObserver interface
        * @return Reference to MPbkThumbnailSetObserver
        */         
        MPbkThumbnailSetObserver& PbkThumbnailSetObserver();
        
        /**
        * Sets MPbkImageGetImageObserver reference value to adapter
        * @param aPbkImageGetImageObserver Reference to MPbkImageGetImageObserver
        */
        void SetPbkImageGetImageObserver(MPbkImageGetImageObserver& 
            aPbkImageGetImageObserver);

        /**
        * Returns a reference to the adapted MPbkThumbnailGetImageObserver interface
        * @return Reference to MPbkThumbnailGetImageObserver
        */                 
        MPbkThumbnailGetImageObserver& PbkThumbnailGetImageObserver();

    private: // Functions for MPbkThumbnailGetObserver 
        // and MPbkImageGetObserver conversion

        /**
        * Sets MPbkImageGetObserver reference value to adapter
        * @param aPbkImageGetObserver Reference to MPbkImageGetObserver
        */
        void DoSetImageGetObserver(MPbkImageGetObserver& 
            aPbkImageGetObserver);

        /**
        * Returns the observer reference.
        * NULL is returned if observer not set earlier.
        * @see SetImageGetObserver
        * @return A reference to MPbkImageGetObserver instance.
        * NULL is returned if no observer available.        
        */
        MPbkImageGetObserver* ImageGetObserver();

    private: // MPbkThumbnailGetObserver

        // iPbkThumbnailGetObserver must be set before following 
        // members can be called
        void PbkThumbnailGetComplete
            (MPbkThumbnailOperation& aOperation, CFbsBitmap* aBitmap);
        void PbkThumbnailGetFailed
            (MPbkThumbnailOperation& aOperation, TInt aError);


    private: // Functions for MPbkThumbnailSetObserver 
        // and MPbkImageSetObserver conversion

        /**
        * Sets MPbkImageSetObserver reference value to adapter
        * @param aPbkImageSetObserver Reference to MPbkImageSetObserver
        */
        void DoSetImageSetObserver(MPbkImageSetObserver& 
            aPbkImageSetObserver);

        /**
        * Returns the observer reference.
        * NULL is returned if observer not set earlier.
        * @see SetImageSetObserver
        * @return A reference to MPbkImageSetObserver instance.
        * NULL is returned if no observer available.
        */
        MPbkImageSetObserver* ImageSetObserver();

    private: // MPbkThumbnailSetObserver
        void PbkThumbnailSetComplete(MPbkThumbnailOperation& aOperation);
        void PbkThumbnailSetFailed(MPbkThumbnailOperation& aOperation, TInt aError);

    private: // Functions for MPbkThumbnailGetImageObserver
        // and MPbkImageGetImageObserver conversion

        /**
        * Sets MPbkImageGetImageObserver reference value to adapter
        * @param aPbkImageGetImageObserver Reference to MPbkImageGetImageObserver
        */
        void DoSetImageGetImageObserver(MPbkImageGetImageObserver& 
            aPbkImageGetImageObserver);

        /**
        * Returns the observer reference.
        * NULL is returned if observer not set earlier.
        * @see SetImageGetImageObserver
        * @return A reference to MPbkImageGetImageObserver instance.
        * NULL is returned if no observer available.
        */
        MPbkImageGetImageObserver* ImageGetImageObserver();


    private: // MPbkThumbnailGetImageObserver
        void PbkThumbnailGetImageComplete
            (MPbkThumbnailOperation& aOperation, CPbkImageDataWithInfo* aImageData);
        void PbkThumbnailGetImageFailed
            (MPbkThumbnailOperation& aOperation, TInt aError);           

    private:

        /**
        * C++ default constructor.
        */
        CPbkObserverAdapter();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // FOR TESTING PURPOSES ONLY
        friend class UT_CPbkObserverAdapter;

    private:    // Data
        /// Ref: Image get observer
        MPbkImageGetObserver* iPbkImageGetObserver;

        /// Ref: Image get observer
        MPbkImageSetObserver* iPbkImageSetObserver;

        /// Ref: Image get image observer
        MPbkImageGetImageObserver* iPbkImageGetImageObserver;

    };

#endif      // CPbkObserverAdapter_H   
            
// End of File
