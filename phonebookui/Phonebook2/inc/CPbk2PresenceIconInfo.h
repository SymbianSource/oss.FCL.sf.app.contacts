/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 presence icon information.
*
*/


#ifndef CPBK2PRESENCEICONINFO_H
#define CPBK2PRESENCEICONINFO_H

// INCLUDES
#include <e32base.h>

// CLASS DECLARATION
class CFbsBitmap;

// Presence icon info for specific xSP service
class CPbk2PresenceIconInfo : public CBase
    {
    public: // constructors and destructors
        IMPORT_C static CPbk2PresenceIconInfo* NewL( 
            const TDesC8& aBrandId,
            const TDesC8& aElementId,
            const TDesC16& aServiceName );
        ~CPbk2PresenceIconInfo();
            
    private: // constructors and destructors
        CPbk2PresenceIconInfo();
        void ConstructL(
            const TDesC8& aBrandId,
            const TDesC8& aElementId,
            const TDesC16& aServiceName );
            
    public: // interface
                
        /**
         * Sets icon bitmap and mask.
         *
         * @param aBitmap icon bitmap, ownership is transferred
         * @param aBitmapMask icon bitmap mask, ownership is transferred
         */
        IMPORT_C void SetBitmap( CFbsBitmap* aBitmap, CFbsBitmap* aBitmapMask );
                
        /**
         * @return branding id to identify icon branding package
         */
        IMPORT_C const TDesC8& BrandId();
                
        /**
         * @return element id in branding package
         */
        IMPORT_C  const TDesC8& ElementId();
                
        /**
         * @return element id in branding package
         */
        IMPORT_C const TDesC16& ServiceName();

        /**
         * @return icon bitmap
         */
        IMPORT_C const CFbsBitmap* IconBitmap();

        /**
         * @return icon bitmap mask
         */
        IMPORT_C const CFbsBitmap* IconBitmapMask();
                        
    private: // data
        
        /// Owned: branding id to identify icon branding package
        HBufC8* iBrandId;
        /// Owned: element id in branding package
        HBufC8* iElementId;
        /// Owned: service name
        HBufC16* iServiceName;
        /// Owned: icon
        CFbsBitmap* iBitmap;
        /// Owned: icon mask
        CFbsBitmap* iBitmapMask;
    };

#endif // CPBK2PRESENCEICONINFO_H

// End of File
