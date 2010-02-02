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
* Description:  Phonebook 2 call type selector.
*
*/

#ifndef CPBK2CALLTYPESELECTOR_H
#define CPBK2CALLTYPESELECTOR_H

// INCLUDES
#include <e32base.h>
#include <aiwdialdata.h>
#include <VPbkFieldType.hrh>
#include <VPbkFieldTypeSelectorFactory.h>

// FORWARD DECLARATIONS
class MVPbkBaseContactField;
class MVPbkStoreContact;
class MPbk2ContactUiControl;
class CVPbkContactManager;
class RVPbkContactFieldDefaultPriorities;

// CLASS DECLARATION

/**
 * Phonebook 2 call type selector.
 */
NONSHARABLE_CLASS(CPbk2CallTypeSelector) : public CBase
    {
    public: // Constructor and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aContactManager   Virtual Phonebook contact manager.
         * @return  A new instance of this class.
         */
        static CPbk2CallTypeSelector* NewL(
                CVPbkContactManager& aContactManager );

        /**
         * Destructor.
         */
        ~CPbk2CallTypeSelector();

    public: // Interface

        /**
         * Selects the call type based on command id,
         * focused field and selected field.
         *
         * @param aCommandId        Command id.
         * @param aContact          The contact.
         * @param aFocusedField     The field which was focused.
         * @param aSelectedField    The field which was selected.
         * @return  Selected call type.
         */
        CAiwDialData::TCallType SelectCallTypeL(
                const TInt aCommandId,
                MVPbkStoreContact& aContact,
                const MVPbkBaseContactField* aFocusedField,
                const MVPbkBaseContactField& aSelectedField ) const;

        /**
         * Selects the call type between a normal phone call
         * (CS/VoIP) or a POC call. This functions is required
         * because with the send key one can launch two totally
         * different operations.
         *
         * @param aSelectedField    The field which was selected,
         *                          can be NULL.
         * @return  ETrue if a telephony call is to be launched,
         *          EFalse indicates a POC call.
         */
        TBool OkToLaunchTelephonyCallL(
                const MVPbkBaseContactField* aSelectedField ) const;

        /**
         * Aids the POC consumer to select a PoC call type.
         *
         * @param aControl  Contact UI control.
         * @return  AIW parameters to be passed to PoC provider.
         */
        TInt SelectPocCallType(
                const MPbk2ContactUiControl& aControl ) const;

        /**
         * Aids the PoC consumer to select a PoC call type
         * in case of a PoC key press.
         *
         * @param aControl  Contact UI control.
         * @return  AIW parameters to be passed to PoC provider.
         */
        TInt SelectPocCallTypeForPocKeyPress(
                const MPbk2ContactUiControl& aControl ) const;

        /**
         * Aids the address select to select which one
         * of the contact's defaults to use.
         * On return the priorities parameter contains defaults to use and
         * is left in to the CleanupStack.
         *
         * @param aDefaults     Prioritized array of contact defaults.
         * @param aContact      The contact.
         */
        void SetDefaultPrioritiesLC(
                RVPbkContactFieldDefaultPriorities& aDefaults,
                MVPbkStoreContact& aContact ) const;
        
        /**
         * Aids the address select to select which one
         * of the contact's defaults to use.
         * On return the priorities parameter contains defaults to use and
         * is left in to the CleanupStack.
         *
         * @param aDefaults     Prioritized array of contact defaults.
         * @param aContact      The contact.
         * @param aSelector     Action type selector.
         */
        void SetDefaultPrioritiesLC(
                RVPbkContactFieldDefaultPriorities& aDefaults,
                MVPbkStoreContact& aContact,
                VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aSelector ) const;

        /**
         * Selects the voip call type based on selected field.
         *
         * @param aSelectedField    The field which was selected.
         * @return  Selected call type for voip.
         */
        CAiwDialData::TCallType SelectVoipCallTypeL(
                const MVPbkBaseContactField* aSelectedField ) const;
        
    private: // Implementation
        CPbk2CallTypeSelector(
                CVPbkContactManager& aContactManager );
        void ConstructL();
        CAiwDialData::TCallType SelectCallTypeL(
                MVPbkStoreContact& aContact,
                const MVPbkBaseContactField& aSelectedField ) const;
        CAiwDialData::TCallType SelectCallTypeL(
                const MVPbkBaseContactField& aSelectedField ) const;
        CAiwDialData::TCallType SelectCallTypeBasedOnFieldTypeL(
                const MVPbkBaseContactField& aSelectedField ) const;
        TBool HasContactAttributeL(
                TVPbkDefaultType aType,
                MVPbkStoreContact& aContact ) const;

    private: // Data
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iContactManager;
    };

#endif // CPBK2CALLTYPESELECTOR_H

// End of File
