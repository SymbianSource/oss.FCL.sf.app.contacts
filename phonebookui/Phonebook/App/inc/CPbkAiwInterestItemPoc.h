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
*     Implements Poc AIW interest item functionality.
*
*/


#ifndef __CPbkAiwInterestItemPoc_H__
#define __CPbkAiwInterestItemPoc_H__

//  INCLUDES
#include <e32base.h>
#include "CPbkAiwInterestItemBase.h"
#include <MPbkAiwInterestItem.h>

// FORWARD DECLARATIONS
class CPbkContactEngine;
class CAiwServiceHandler;
class CAiwGenericParamList;
class CPbkCallTypeSelector;

// CLASS DECLARATION

/**
 * Base AIW interest item. Implements common AIW interest logic.
 *
 */
class CPbkAiwInterestItemPoc : 
        public CPbkAiwInterestItemBase, public MPbkAiwInterestItem
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
         */
        static CPbkAiwInterestItemPoc* NewL(
            CAiwServiceHandler& aServiceHandler,
            CPbkContactEngine& aEngine);

        /**
         * Destructor.
         */
        ~CPbkAiwInterestItemPoc();

    public:  // from MPbkAiwInterestItem
        TBool InitMenuPaneL(
            TInt aResourceId,
            CEikMenuPane& aMenuPane,
            TUint aFlags);
        TBool HandleCommandL(
            TInt aMenuCommandId,
            const CContactIdArray& aContacts,
            const TPbkContactItemField* aFocusedField = NULL,
            TInt aServiceCommandId = KNullHandle,
            MPbkAiwCommandObserver* aObserver = NULL);
        void AttachL(
            TInt aMenuResourceId,
            TInt aInterestResourceId,
            TBool aAttachBaseServiceInterest);
        
    private:  // implementation
        CPbkAiwInterestItemPoc(
            CAiwServiceHandler& aServiceHandler,
            CPbkContactEngine& aEngine);
        void ConstructL();

    private: // data
        /// Own: menu filtering flags
        TUint iFlags;
        /// Ref: Phonebook contact engine
        CPbkContactEngine& iEngine;
        /// Own: call type selector
        CPbkCallTypeSelector* iSelector;
    };

#endif // __CPbkAiwInterestItemPoc_H__
            
// End of File
