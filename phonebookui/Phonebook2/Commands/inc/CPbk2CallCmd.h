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
* Description:  Phonebook 2 call command object.
*
*/


#ifndef CPBK2CALLCMD_H
#define CPBK2CALLCMD_H

// INCLUDES
#include <e32base.h>
#include <MPbk2Command.h>
#include <VPbkFieldTypeSelectorFactory.h>
#include <spdefinitions.h>

// FORWARD DECLARATIONS
class CAiwServiceHandler;
class MPbk2ContactUiControl;
class MVPbkStoreContactField;
class MVPbkStoreContact;
class CPbk2CallTypeSelector;
class CAiwDialDataExt;
class MVPbkContactFieldData;

// CLASS DECLARATION

/**
 * Phonebook 2 call command object.
 * Responsible for setting up dial data and launching the call by using AIW.
 */
NONSHARABLE_CLASS(CPbk2CallCmd) : public CBase,
                                  public MPbk2Command
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aContact          Reference to the contact.
         * @param aSelectField      Selected contact field,
         *                          ownership is taken.
         * @param aControl          Contact UI control.
         * @param aCommandId        Command id.
         * @param aServiceHandler   Reference to service handler.
         * @param aSelector         Reference to call type selector.
         * @return  A new instance of this class.
         */
        static CPbk2CallCmd* NewL(
                MVPbkStoreContact*& aContact,
                MVPbkStoreContactField* aSelectedField,
                MPbk2ContactUiControl& aControl,
                const TInt aCommandId,
                CAiwServiceHandler& aServiceHandler,
                CPbk2CallTypeSelector& aSelector );
        
        /**
         * Creates a new instance of this class.
         *
         * @param aContact          Reference to the contact.
         * @param aSelectField      Selected contact field,
         *                          ownership is taken.
         * @param aControl          Contact UI control.
         * @param aCommandId        Command id.
         * @param aServiceHandler   Reference to service handler.
         * @param aSelector         Reference to call type selector.
         * @param aActionSelector   Action type selector.
         * @return  A new instance of this class.
         */
        static CPbk2CallCmd* NewL(
                MVPbkStoreContact*& aContact,
                MVPbkStoreContactField* aSelectedField,
                MPbk2ContactUiControl& aControl,
                const TInt aCommandId,
                CAiwServiceHandler& aServiceHandler,
                CPbk2CallTypeSelector& aSelector,
                VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aActionSelector );


        /**
         * Destructor.
         */
        ~CPbk2CallCmd();

    public: // From MPbkCommand
        void ExecuteLD();
        void ResetUiControl(
                MPbk2ContactUiControl& aUiControl );
        void AddObserver(
                MPbk2CommandObserver& aObserver );

    private: // Implementation
        CPbk2CallCmd(
                MVPbkStoreContact*& aContact,
                MVPbkStoreContactField* aSelectedField,
                MPbk2ContactUiControl& aControl,
                const TInt aCommandId,
                CAiwServiceHandler& aServiceHandler,
                CPbk2CallTypeSelector& aSelector,
                VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aActionSelector );
        
        void SetCallTypeL( CAiwDialDataExt& dialData );
        TBool ExtractXspId(
            const MVPbkStoreContactField* aSelectedField, 
            TPtrC& aXSPId) const;
        TServiceId GetMatchedServiceIdL( const TDesC& aXSPId );
        TPtrC GetFieldData( const MVPbkContactFieldData& aFieldData ) const;

    private: // Data
        /// Ref: Contact to call to
        MVPbkStoreContact*& iContact;
        /// Own: Phone number field to call
        MVPbkStoreContactField* iSelectedField;
        /// Ref: Contact UI control
        MPbk2ContactUiControl* iControl;
        /// Own: Command id
        const TInt iCommandId;
        /// Ref: AIW service handler
        CAiwServiceHandler& iServiceHandler;
        /// Ref: Call type selector
        CPbk2CallTypeSelector& iSelector;
        /// Own: Action type selector
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector iActionSelector;
    };

#endif // CPBK2CALLCMD_H

// End of File
