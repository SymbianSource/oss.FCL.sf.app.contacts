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
* Description:  Holds icon ID information and an actual icon and its mask.
*
*/


#ifndef NLXICONINFO_H
#define NLXICONINFO_H

//  INCLUDES
#include <e32base.h>
#include <TPbk2IconId.h>


// FORWARD DECLARATIONS
class CFbsBitmap;

// CLASS DECLARATION

/**
 * Holds icon ID information and an actual icon and its mask.
 */
NONSHARABLE_CLASS(CPbk2NlxPresenceIconInfo) :
        public CBase
    {
    public:  // Constructors and destructor
        
        /**
         * Creates a new instance of this class.
         *
         * @param aContactLink Contact link. Takes ownership.
         * @return  A new instance of this class.
         */
        static CPbk2NlxPresenceIconInfo* NewLC( const TDesC8& aBrandId, const TDesC8& aElementId, TInt aIconId );
        
        ~CPbk2NlxPresenceIconInfo();

    private: // Construction & destruction
        CPbk2NlxPresenceIconInfo( TInt aIconId );
        void ConstructL( const TDesC8& aBrandId, const TDesC8& aElementId );

    public: // New methods

        TPtrC8 BrandId() const;

        TPtrC8 ElementId() const;

        /**
         * Takes ownership of icons.
         */
        void SetIcons( CFbsBitmap* aBitmap, CFbsBitmap* aBitmapMask );

        const CFbsBitmap* Bitmap() const;

        const CFbsBitmap* BitmapMask() const;

        TInt IconId() const;

    private: // Data
        CFbsBitmap* iBitmap;
        CFbsBitmap* iBitmapMask;
        HBufC8* iBrandId;
        HBufC8* iElementId;
        TInt iIconId;
    };

#endif // NLXICONINFO_H
           
// End of File
