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
*      Call type selector.
*
*/


#ifndef __CPbkCallTypeSelector_H__
#define __CPbkCallTypeSelector_H__

// INCLUDES
#include <AiwDialDataTypes.h>
#include <TPbkContactItemField.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class CPbkContactItem;
class CPbkContactEngine;
class MPbkGlobalSetting;

// CLASS DECLARATION

/**
 * Call type selector.
 */
class CPbkCallTypeSelector : public CBase
    {
    public: // Constructor and destructor

        /**
         * Two-phased constructor.
         * @param aEngine reference to Phonebook engine
         */
        static CPbkCallTypeSelector* NewL(
            CPbkContactEngine& aEngine);
    
        /**
         * Destructor.
         */
        ~CPbkCallTypeSelector();
    
    
    public: // Interface
		/**
         * Selects the call type based on command id,
         * focused field and selected field.
         * @param aCommandId command id
         * @param aContact the contact
         * @param aFocusedField the field which was focused
         * @param aSelectedField the field which was selected
         * @return selected call type
         */
        TInt SelectCallType(
            const TInt aCommandId,
            const CPbkContactItem& aContact,
            const TPbkContactItemField* aFocusedField,
            const TPbkContactItemField& aSelectedField) const;

		/**
         * Selects the call type between a normal phone call
         * (CS/VoIP) or a POC call. This functions is required
         * because with the send key one can launch two totally
         * different operations.
         * @param aSelectedField the field which was selected,
         * can be NULL
         * @return ETrue if a telephony call is to be launched,
         * EFalse indicates a POC call to be launched.
         */
        TBool OkToLaunchTelephonyCall(
            const TPbkContactItemField* aSelectedField) const;

		/**
         * Aids the POC provider to select POC call type.
         * @param aControlFlags menu filtering flags
         * @return AIW parameters to be passed to POC provider.
         */
        TInt SelectPocCallType(
            TUint aControlFlags) const;
        
        /**
         * Returns one command id that is used when creating PoC call
         * using PoC key.
         * @param aControlFlags menu filtering flags
         * @return one AIW command id to be passed as iCommandId
         */
        TInt SelecPocCallTypeForPocKeyPress(TUint aControlFlags) const;
        
		/**
         * Aids the address select to select which one
         * of the contacts defaults to use.
         * @param aContact the contact
         * @return the default field to use
         */
        const TPbkContactItemField* SelectDefaultToUse(
            const CPbkContactItem& aContact) const;
        

    private: // Implementation
        CPbkCallTypeSelector(
            CPbkContactEngine& aEngine);
        void ConstructL();
        TInt SelectCallType(
            const CPbkContactItem& aContact,
            const TPbkContactItemField& aSelectedField) const;
        TInt SelectCallType(
            const TPbkContactItemField& aSelectedField) const;
        TInt SelectCallTypeBasedOnFieldType(
            const TPbkContactItemField& aSelectedField,
            TBool aSelectBetweenCsAndVoip) const;
        TInt SelectBetweenCsAndVoip() const;
        TInt GetDefaults(
            const CPbkContactItem& aContact) const;
        
    private: // data
        /// Own: used to query preferred telephony information
        MPbkGlobalSetting* iPersistentSetting;
        /// Ref: reference to Phonebook engine
        CPbkContactEngine& iEngine;
    };
    

#endif // __CPbkCallTypeSelector_H__

// End of File
