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
*       Creates phonebook commands.
*
*/


#ifndef __CPbkCommandFactory_H__
#define __CPbkCommandFactory_H__

//  INCLUDES
#include <e32base.h>
#include <MPbkCommandFactory.h>

//  FORWARD DECLARATIONS
class CPbkAppUi;
class TPbkContactItemField;
class CAiwServiceHandler;

//  CLASS DECLARATION 

/**
 * Creates phonebook commands.
 */
class CPbkCommandFactory : public CBase, 
                           public MPbkCommandFactory
    {
    public: // Constructor and destructor
        /**
         * Static constructor.
         * @return a new CPbkCommandFactory object
         */
        static CPbkCommandFactory* NewL();

        /**
         * Destructor.
         */
        ~CPbkCommandFactory();

    public: // From MPbkCommandFactory
        MPbkCommand* CreateSendContactCmdL(
            TPbkSendingParams aParams,
            CPbkContactEngine& aEngine,
			CBCardEngine& aBCardEng,
			TContactItemId aContactId,
            TPbkContactItemField* aField = NULL);

        MPbkCommand* CreateSendContactsCmdL(
            TPbkSendingParams aParams,
            CPbkContactEngine& aEngine,
			CBCardEngine& aBCardEng,
			const CContactIdArray& aContacts);

        MPbkCommand* CreateCallCmdL(
		    const CPbkContactItem& aContactItem,
    		const TPbkContactItemField* aSelectedField,
    		const TPbkContactItemField* aFocusedField,
            const TInt aCommandId,
            CAiwServiceHandler& aServiceHandler,
            const CPbkCallTypeSelector& aCallTypeSelector);

		MPbkCommand* CreateSendMultipleMessageCmdL(
            CPbkContactEngine& aEngine,
            TPbkSendingParams aParams,
			const CContactIdArray& aContacts,
			const TPbkContactItemField* aFocusedField,
			TBool aUseDefaultDirectly);

		MPbkCommand* CreateGoToURLCmdL(
            const CPbkContactItem& aContact,
			const TPbkContactItemField* aFocusedField);

		MPbkCommand* CreateSettingsCmdL(
            CPbkContactEngine& aEngine,
            MObjectProvider& aParent);

		TBool IsEmailEnabledL();

        MPbkCommand* CreatePocCmdL(
            TInt aCommandId,
            const CContactIdArray& aContacts,
            TUint aControlFlags,
            CAiwServiceHandler& aServiceHandler,
            const CPbkCallTypeSelector& aCallTypeSelector);
            
        MPbkCommand* CreateSyncMlCmdL(            
            TInt aCommandId,
            CAiwServiceHandler& aServiceHandler);

    private: // Implementation
        CPbkCommandFactory();
    };

#endif // __CPbkCommandFactory_H__

// End of File
