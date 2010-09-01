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
*     Implements AIW interest array for views.
*
*/


#ifndef __CPbkAiwInterestArray_H__
#define __CPbkAiwInterestArray_H__

//  INCLUDES
#include <e32base.h>
#include <MPbkAiwInterestItem.h>

// FORWARD DECLARATIONS
class CAiwServiceHandler;
class CPbkContactEngine;


/**
 * Array of AIW items.
 */
class CPbkAiwInterestArray : 
        public CBase, private MPbkAiwInterestItem
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
         * @param aEngine Phonebook contact engine
         */
        IMPORT_C static CPbkAiwInterestArray* NewL(CPbkContactEngine& aEngine);

        /**
         * Destructor.
         */
        ~CPbkAiwInterestArray();

    public:  // interface
        /**
         * Factory function for creating an interest item.
         * @param aInterestId id of the AIW interest
         * @param aMenuResourceId resource id of the menu to attach the id
         * @param aInterestResourceId resource id of the AIW interest
         * @param aAttachBaseServiceInterest if ETrue, attaches the AIW
         *        resource id to AIW base services also (enables command
         *        handling via AIW without menu commands)
         */
        void CreateInterestItemAndAttachL(
            TInt aInterestId,
            TInt aMenuResourceId,
            TInt aInterestResourceId,
            TBool aAttachBaseServiceInterest);

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

    private:  // implementation
        CPbkAiwInterestArray(CPbkContactEngine& aEngine);
        void ConstructL();
        void AttachL(
            TInt aMenuResourceId,
            TInt aInterestResourceId,
            TBool aAttachBaseServiceInterest);

    private: // data
        /// Own: AIW service handler
        CAiwServiceHandler* iServiceHandler;
        /// Own: array of interest items
        RPointerArray<MPbkAiwInterestItem> iInterestItems;
        /// Ref: phonebook contact engine
        CPbkContactEngine& iEngine;
    };

#endif // __CPbkAiwInterestArray_H__
            
// End of File
