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
* Description:  Phonebook 2 PoC command object.
*
*/



#ifndef CPBK2POCCMD_H
#define CPBK2POCCMD_H

// INCLUDES
#include <e32base.h>
#include <MPbk2Command.h>

// FORWARD DECLARATIONS
class CAiwServiceHandler;
class CPbk2CallTypeSelector;
class MPbk2ContactUiControl;
class MVPbkStoreContact;
class MVPbkStoreContactField;

// CLASS DECLARATION

/**
 * Phonebook 2 PoC command object.
 */
NONSHARABLE_CLASS(CPbk2PocCmd) : public CBase,
                                 public MPbk2Command
    {
    public: // Construction and destruction
        /**
         * Creates a new instance of this class.
         *
         * @param aCommandId        Command id.
         * @param aServiceHandler   Reference to service handler.
         * @param aSelector         Reference to call type selector.
         * @param aControl          Contact UI control.
         * @param aContact          Reference to store contact.
         * @param aSelectField      Selected contact field,
         *                          ownership is taken.
         * @return  A new instance of this class.
         */
        static CPbk2PocCmd* NewL(
                const TInt aCommandId,
                CAiwServiceHandler& aServiceHandler,
                CPbk2CallTypeSelector& aSelector,
                MPbk2ContactUiControl& aControl,
                const MVPbkStoreContact& aContact,
                MVPbkStoreContactField* aSelectedField );

        /**
         * Destructor.
         */
        ~CPbk2PocCmd();

    public: // From MPbkCommand
        void ExecuteLD();
        void ResetUiControl(
                MPbk2ContactUiControl& aUiControl );
        void AddObserver(
                MPbk2CommandObserver& aObserver );

    private: // Implementation
        CPbk2PocCmd(
                const TInt aCommandId,
                CAiwServiceHandler& aServiceHandler,
                CPbk2CallTypeSelector& aSelector,
                MPbk2ContactUiControl& aControl,
                const MVPbkStoreContact& aContact,
                MVPbkStoreContactField* aSelectedField );

    private: // Data
        /// Own: Command id
        const TInt iCommandId;
        /// Ref: AIW service handler
        CAiwServiceHandler& iServiceHandler;
        /// Ref: Call type selector
        CPbk2CallTypeSelector& iSelector;
        /// Ref: Contact UI control
        MPbk2ContactUiControl* iControl;
        /// Ref: Contact to call to
        const MVPbkStoreContact& iContact;
        /// Own: Field to call to
        MVPbkStoreContactField* iSelectedField;
    };

#endif // CPBK2POCCMD_H

// End of File
