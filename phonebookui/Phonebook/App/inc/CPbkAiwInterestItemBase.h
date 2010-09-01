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
*     Implements basic AIW interest item functionality.
*
*/


#ifndef __CPbkAiwInterestItemBase_H__
#define __CPbkAiwInterestItemBase_H__

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CAiwServiceHandler;

// CLASS DECLARATION

/**
 * Base AIW interest item. Implements common AIW interest logic.
 *
 */
class CPbkAiwInterestItemBase : public CBase
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
         */
        CPbkAiwInterestItemBase(CAiwServiceHandler& aServiceHandler);

        /**
         * Destructor.
         */
        ~CPbkAiwInterestItemBase();

    public:  // interface
        void AttachL(
            TInt aMenuResourceId,
            TInt aInterestResourceId,
            TBool aAttachBaseServiceInterest);
  
    private: // types
        struct TAttachedResources
            {
            TAttachedResources()
                {
                iMenuResourceId = 0;
                iInterestResourceId = 0;
                iBaseServiceAttached = 0;
                }
            TInt iMenuResourceId;
            TInt iInterestResourceId;
            TBool iBaseServiceAttached;
            };

    protected:  // data
        /// Ref: AIW service handler
        CAiwServiceHandler& iServiceHandler;

    private: // data
        /// Own: Attached resources
        CArrayFixFlat<TAttachedResources> iAttachedResources;
    };

#endif // __CPbkAiwInterestItemBase_H__
            
// End of File
