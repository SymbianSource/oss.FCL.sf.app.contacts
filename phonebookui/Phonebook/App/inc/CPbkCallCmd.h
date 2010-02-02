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
*      Call command object.
*
*/


#ifndef __CPbkCallCmd_H__
#define __CPbkCallCmd_H__

// INCLUDES
#include <e32base.h>
#include <cntdef.h>
#include <MPbkCommand.h>
#include <MPbkBackgroundProcess.h>
#include <CPbkAddressSelect.h>
#include <AiwDialDataTypes.h>


// FORWARD DECLARATIONS
class CPbkContactItem;
class TPbkContactItemField;
class CAiwServiceHandler;
class CPbkCallTypeSelector;


// CLASS DECLARATION

/**
 * Call command object.
 */
class CPbkCallCmd :
        public CBase,
        public MPbkCommand,
        private MPbkProcessObserver
    {
    public: // Interface
		/**
         * Static constructor.
         * @param aContactItem reference to contact item
         * @param aSelectField select contact field, can be NULL
         * @param aFocusedField focused contact field, can be NULL
         * @param aCommandId command id
         * @param aServiceHandler service handler
         * @param aSelector call type selector
         * @return a new call command object.
         */
		static CPbkCallCmd* NewL (
		    const CPbkContactItem& aContactItem,
    		const TPbkContactItemField* aSelectedField,
    		const TPbkContactItemField* aFocusedField,
            const TInt aCommandId,
            CAiwServiceHandler& aServiceHandler,
            const CPbkCallTypeSelector& aSelector);

        /**
         * Destructor.
         */
        ~CPbkCallCmd();

    public:  // from MPbkCommand
        void ExecuteLD();
        
    public:  // from MPbkProcessObserver
        void ProcessFinished(MPbkBackgroundProcess& aProcess);

    private:  // Implementation
        CPbkCallCmd (
		    const CPbkContactItem& aContactItem,
    		const TPbkContactItemField* aSelectedField,
    		const TPbkContactItemField* aFocusedField,
            const TInt aCommandId,
            CAiwServiceHandler& aServiceHandler,
            const CPbkCallTypeSelector& aSelector);
        void SetupDialDataL();

    private:  // Data
		/// Ref: the contact to call
		const CPbkContactItem& iContact;
		/// Ref: the phonenumber field to call
		const TPbkContactItemField* iSelectedField;
		/// Ref: the focused field of the contact
		const TPbkContactItemField* iFocusedField;
        /// Own: command id
        const TInt iCommandId;
        /// Ref: AIW service handler
        CAiwServiceHandler& iServiceHandler;
        /// Ref: Call type selector
        const CPbkCallTypeSelector& iSelector;
        /// Own: dial data
        TAiwDialDataV1 iDialData;
    };

#endif // __CPbkCallCmd_H__

// End of File
