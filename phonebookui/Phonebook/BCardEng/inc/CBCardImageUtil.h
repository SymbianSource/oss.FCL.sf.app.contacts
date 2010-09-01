/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*       Image manipulation utilities needed in BCardEngine.    
*
*/


#ifndef __CBCARDIMAGEUTIL_H__
#define __CBCARDIMAGEUTIL_H__

//  INCLUDES
#include    <e32base.h>

// FORWARD DECLARATIONS
class CFbsBitmap;

// CLASS DECLARATION

NONSHARABLE_CLASS(CBCardImageUtil) : 
        public CBase
	{
    public: // constructor and destructor
        /**
         * Constructor
         */
        static CBCardImageUtil* NewL();

        /**
         * Destructor
         */
        ~CBCardImageUtil();

    public:  // implementation
        /**
        * Saves the given bitmap to a 8-bit descriptor according in
        * jpeg format
        *
        * @param    aBitmap The bitmap to store.
        * @return   The resulting jpeg descriptor. Note: caller must
        *           take the ownership.
        */
        HBufC8* BitmapToDescL(CFbsBitmap& aBitmap);

	private: // Callbacks for image conversion
		static void ConversionDone(TAny* aThis, TInt aError);
        void ConversionDone(TInt aError);
        
    private: // member data
		/// Own: ETrue while running
        TBool iRunning;
		/// Own: error code
        TInt iError;
		/// Own: active scheduler
        CActiveSchedulerWait iWait;
    };

#endif // __CBCARDIMAGEUTIL_H__
            
// End of File
