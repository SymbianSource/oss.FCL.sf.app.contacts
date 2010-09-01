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
*     Implements Call AIW interest item functionality.
*
*/


#ifndef __CPbkAiwInterestItemCall_H__
#define __CPbkAiwInterestItemCall_H__

//  INCLUDES
#include <e32base.h>
#include "CPbkAiwInterestItemBase.h"
#include <MPbkAiwInterestItem.h>

// FORWARD DECLARATIONS
class CPbkContactEngine;
class CAiwServiceHandler;
class CPbkContactItem;
class CPbkCallTypeSelector;
class MPbkCommand;

// CLASS DECLARATION

/**
 * Base AIW interest item. Implements common AIW interest logic.
 *
 */
class CPbkAiwInterestItemCall : 
        public CPbkAiwInterestItemBase, public MPbkAiwInterestItem
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
         */
        static CPbkAiwInterestItemCall* NewL(
            CAiwServiceHandler& aServiceHandler,
            CPbkContactEngine& aEngine);

        /**
         * Destructor.
         */
        ~CPbkAiwInterestItemCall();

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
        CPbkAiwInterestItemCall(
            CAiwServiceHandler& aServiceHandler,
            CPbkContactEngine& aEngine);
        void ConstructL();
        MPbkCommand* CreateCallCmdObjectL(
            TInt aMenuCommandId,
            const CPbkContactItem& aContact,
            const CContactIdArray& aContacts,
            const TPbkContactItemField* aFocusedField);
        MPbkCommand* CreatePocCmdObjectL(
            TInt aMenuCommandId,
            const CContactIdArray& aContacts,
            const TPbkContactItemField* aFocusedField);

    private: // data
        /// Ref: Phonebook contact engine
        CPbkContactEngine& iEngine;
        /// Own: Call type selector
        CPbkCallTypeSelector* iSelector;
    };

#endif // __CPbkAiwInterestItemCall_H__
            
// End of File
