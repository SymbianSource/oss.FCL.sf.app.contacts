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
* Description:  Prepend command object.
*
*/


#ifndef CPBK2PREPENDCMD_H
#define CPBK2PREPENDCMD_H

// INCLUDES
#include <e32base.h>
#include <MPbk2Command.h>

// FORWARD DECLARATIONS
class MPbk2ContactUiControl;
class MVPbkContactOperationBase;
class MVPbkStoreContact;
class CAiwServiceHandler;
class CPbk2PresentationContact;
class CPbk2ContactUiControlSubstitute;
class CPbk2CallTypeSelector;
class CPbk2FieldPropertyArray;

// CLASS DECLARATIONS

/**
 * Phonebook 2 prepend command implementation.
 * Responsible for:
 *  - User can edit phone number before make call.
 *  - Prepending do not edit contact information.
 */
NONSHARABLE_CLASS(CPbk2PrependCmd) : public CBase,
                                     public MPbk2Command
    {
    public: // Constructor and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aMenuCommandId    Menu command id.
         * @param aUiControl        UI control.
         * @param aServiceHandler   Aiw service handler.
         * @param aSelector         Call type selector.
         * @param aStoreContact     Store contact.
         * @return  A new instance of this class.
         */
        static CPbk2PrependCmd* NewL(
                TInt aMenuCommandId,
                MPbk2ContactUiControl& aUiControl,
                CAiwServiceHandler& aServiceHandler,
                CPbk2CallTypeSelector& aSelector,
                MVPbkStoreContact*& aStoreContact,
                CPbk2FieldPropertyArray& aFieldProperties);

        /**
         * Destructor.
         */
        ~CPbk2PrependCmd();

    public: // From MPbk2Command
        void ExecuteLD();
        void ResetUiControl(
                MPbk2ContactUiControl& aUiControl );
        void AddObserver(
                MPbk2CommandObserver& aObserver );

    private: // Implementation
        CPbk2PrependCmd( 
                TInt aMenuCommandId,
                MPbk2ContactUiControl& aUiControl,
                CAiwServiceHandler& aServiceHandler,
                CPbk2CallTypeSelector& aSelector,
                MVPbkStoreContact*& aStoreContact,
                CPbk2FieldPropertyArray& aFieldProperties);
        void ConstructL();
        void ExecuteDlgL();
        CPbk2PresentationContact* CreatePresentationContactLC();
        TInt ToStoreFieldIndexL( TInt aPresIndex );
        TInt MaxLengthOfEditorL( TInt aFieldStoreIndex );
                        
    private: // Data
        /// Ref: UI control
        MPbk2ContactUiControl* iUiControl;                
        /// Ref: AIW service handler
        CAiwServiceHandler& iServiceHandler;
        /// Ref: Call type selector
        CPbk2CallTypeSelector& iSelector;
        /// Own: UI control substitute
        CPbk2ContactUiControlSubstitute* iUiControlSubstitute;
        /// Own: Temporary contact
        MVPbkStoreContact* iTempContact;
        /// Own: Menu command id
        TInt iMenuCommandId;
        /// Ref: Store contact
        MVPbkStoreContact* iStoreContact;
        /// Ref:
        CPbk2FieldPropertyArray& iFieldProperties;
    };

#endif // CPBK2PREPENDCMD_H

// End of File
