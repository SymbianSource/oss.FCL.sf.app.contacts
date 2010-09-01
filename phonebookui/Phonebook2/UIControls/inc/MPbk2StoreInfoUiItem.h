/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  An interface for store information UI item
*
*/



#ifndef MPBK2STOREINFOUIITEM_H
#define MPBK2STOREINFOUIITEM_H

//  INCLUDES
#include <e32def.h>


// CLASS DECLARATION

/**
*  An interface for store information UI item
*
*/
class MPbk2StoreInfoUiItem
    {
    public:  // Constructors and destructor
        
        /**
        * Destructor.
        */
        virtual ~MPbk2StoreInfoUiItem() {}

    public: // New functions

		/**
		* Returns the heading text of the information item
		*
		* @return the heading text of the item
		*/
		virtual const TDesC& HeadingText() const = 0;

		/**
		* Returns the item related to heading
		*
		* @return the item related to heading
		*/
		virtual const TDesC& ItemText() const = 0;
    };

#endif      // MPBK2STOREINFOUIITEM_H
            
// End of File
