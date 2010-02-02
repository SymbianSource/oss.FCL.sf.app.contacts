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
*      Poc command object.
*
*/


#ifndef __CPbkPocCmd_H__
#define __CPbkPocCmd_H__

// INCLUDES
#include <e32base.h>
#include <cntdef.h>
#include <MPbkCommand.h>
#include <MPbkBackgroundProcess.h>
#include <AiwDialDataTypes.h>
#include <AiwPoCParameters.h>


// FORWARD DECLARATIONS
class CAiwServiceHandler;
class CAiwGenericParamList;
class CPbkCallTypeSelector;

// CLASS DECLARATION

/**
 * Poc command object.
 */
class CPbkPocCmd :
        public CBase,
        public MPbkCommand,
        private MPbkProcessObserver
    {
    public: // Interface
		/**
         * Static constructor.
         * @param aCommandId command id
         * @param aContacts the contacts to operate with
         * @param aControlFlags control's menu filtering flags
         * @param aServiceHandler service handler
         * @param aEngine reference to Phonebook engine
         * @param aSelector reference to call type selector
         * @return a new call command object.
         */
		static CPbkPocCmd* NewL (
            TInt aCommandId,
            const CContactIdArray& aContacts,
            TUint aControlFlags,          
            CAiwServiceHandler& aServiceHandler,
            const CPbkCallTypeSelector& aSelector);

        /**
         * Destructor.
         */
        ~CPbkPocCmd();

    public:  // from MPbkCommand
        void ExecuteLD();
        
    public:  // from MPbkProcessObserver
        void ProcessFinished(MPbkBackgroundProcess& aProcess);

    private:  // Implementation
        CPbkPocCmd (
            TInt aCommandId,
            const CContactIdArray& aContacts,
            TUint aControlFlags,
            CAiwServiceHandler& aServiceHandler,
            const CPbkCallTypeSelector& aSelector);

    private:  // Data
        /// Own: command id
        TInt iCommandId;
        /// Ref: AIW service handler
        CAiwServiceHandler& iServiceHandler;
        /// Ref: the contacts to operate with
        const CContactIdArray& iContacts;
        /// Ref: reference to call type selector
        const CPbkCallTypeSelector& iSelector;
        /// Own: menu filtering flags
        TUint iFlags;
    };

#endif // __CPbkPocCmd_H__

// End of File
