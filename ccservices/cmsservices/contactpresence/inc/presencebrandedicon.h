/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Presence in pbonebook
*
*/


#ifndef PRESENCEBRANDEDICON_H
#define PRESENCEBRANDEDICON_H

#include <e32std.h>
#include <badesca.h>

#include <mcontactpresence.h>

class MContactPresenceObs;
class CBSFactory;

// Constants
const TInt KECEDefaultBrandVersion = 1;


/**
 * CPresenceBrandedIcon
 *
 * Get the branded icon with given element and branded id
 *
 * @lib contactpresence
 * @since s60 v5.0
 */
NONSHARABLE_CLASS( CPresenceBrandedIcon ) : public CActive
    {


    public:

        /**
         * Constructor.
         * @param aBrandingFactory Branding factory object
         * @param aObs callback observer
         */
        static CPresenceBrandedIcon* NewL( CBSFactory* const aBrandingFactory ,
                                           MContactPresenceObs& aObs );
        /**
         * Destructor
         */
        virtual ~CPresenceBrandedIcon();

        /**
         * Destructor plus remove from queue
         */
        void Destroy();

        /**
         * start to get icon info
         * @aOpId aBrandingFactory Branding factory object
         * @param aBrandingFactory Branding factory object
         * @param aObs callback observer
         */
        void StartGetIconInfoL( TInt aOpId, const TDesC8& aBrandId,
                                const TDesC8& aElementId,
                                const TSize aBitmapSize=TSize(10,10));

        /**
        * Get Operation ID of this object
        */
        TInt GetOpId() const;

        inline static TInt LinkOffset();

    protected:

    // from base class CActive

        /**
         * Defined in a base class
         */
        void RunL();

        /**
         * Defined in a base class
         */
        void DoCancel();

    private:

        /**
         * Defined in a base class
         */
        CPresenceBrandedIcon( CBSFactory* aBrandingFactory ,
                              MContactPresenceObs& aObs );

        /**
         * Actual request processing for common traffic lights icons
         */
        void ProcessCommonL();
        
        /**
         * Actual request processing for service specific icons
         */
        void ProcessServiceL();        

        /**
        * Loops thro the ServiceTable/SpSettings and finds the
        * BrandLanguage associated to the BrandId (aBrandId)
        * @param aBrandId - BrandId mentioned in the ServiceTable
        * @return TLanguage - Returns the matched Language Id, else 47(ELangInternationalEnglish)
        */
        TLanguage FindBrandLanguageIdL( const TDesC8& aBrandId );
        
        /**
        * Gets the bitmap and mask for the element 
        * and fills the same in aBrandedBitmap, aBrandedMask
        * @param aBrandedBitmap - Ptr to the Bitmap Ownership is transferred to the caller
        * @param aBrandedMask - Ptr to the BitmapMask Ownership is transferred to the caller        
        */
        void GetBitmapL( CFbsBitmap*& aBrandedBitmap, 
                            CFbsBitmap*& aBrandedMask );
        
        /**
        * Gets the bitmap and mask for the element from the Branding Server
        * @param aLanguageId - Language for which Brand is requested        
        * @param aBrandedBitmap - Ptr to the Bitmap Ownership is transferred to the caller
        * @param aBrandedMask - Ptr to the BitmapMask Ownership is transferred to the caller        
        */
        void GetBitmapFromBrandingServerL(
                            TLanguage aLanguageId, 
                            CFbsBitmap*& aBrandedBitmap, 
                            CFbsBitmap*& aBrandedMask ) ;
        
    private: // Data

        /**
         * Pointer to stack variable to detect the deletion of the heap
         * instance. This case takes place if a client
         * calls destructor in callback method.
         * Own.
         */
        TBool* iDestroyedPtr;

        /**
        The link queue of which this class part of
        */
        TDblQueLink iQueLink;

        /**
         * callback observer
         */
        MContactPresenceObs& iObs;


        /**
         * latest operation id
         */
        TInt iOpId;

        /**
         * Brand id to process
         */
        HBufC8* iBrandId;

        /**
         * Element id to process
         */
        HBufC8* iElementId;

        /**
         * Branding factory not owned
         */
        CBSFactory* iBrandingFactory;

        /**
        * Size of bitmaps to return
        */
        TSize iBitmapSize;
        
        /**
         * Brand Language mentioned in the Service Table
         */
        TLanguage iBrandLanguage;
    };

TInt CPresenceBrandedIcon::LinkOffset()
    {
    return _FOFF(CPresenceBrandedIcon, iQueLink);
    }

#endif // PRESENCEBRANDEDICON_H
