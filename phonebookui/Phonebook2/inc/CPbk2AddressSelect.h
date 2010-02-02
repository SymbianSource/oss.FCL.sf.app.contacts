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
* Description:  Phonebook 2 address selection.
*
*/


#ifndef CPBK2ADDRESSSELECT_H
#define CPBK2ADDRESSSELECT_H

//  INCLUDES
#include <e32base.h>
#include <coedef.h>   // TKeyResponse
#include <w32std.h>   // TKeyEvent, TEventCode
#include <MPbk2DialogEliminator.h>

// FORWARD DECLARATIONS
class MVPbkStoreContactField;
class CVPbkFieldTypeSelector;
class CVPbkFieldFilter;
class CPbk2SelectFieldDlg;
class TPbk2AddressSelectParams;
class MVPbkContactAttributeManager;
class MVPbkStoreContact;
class CPbk2PresenceIconInfo;

// CLASS DECLARATION

/**
 * Phonebook 2 address selection.
 * Responsible for selecting an appropriate contact field
 * from given contact. The selection is based on the
 * given parameters.
 */
class CPbk2AddressSelect : public CBase,
                           public MPbk2DialogEliminator
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aParams   Address select parameters.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2AddressSelect* NewL(
                TPbk2AddressSelectParams& aParams );
                
        /**
         * Creates a new instance of this class.
         *
         * @param aParams   Address select parameters.
         * @param aFieldTypeSelector   Field type selector.
         * @param aStoreContactsArray  Additional contacts
         * @param aPresenceIconsArray  Presence icons array
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2AddressSelect* NewL(
                TPbk2AddressSelectParams& aParams,
                CVPbkFieldTypeSelector& aFieldTypeSelector,
                const TArray<MVPbkStoreContact*>* aStoreContactsArray,
                const TArray<CPbk2PresenceIconInfo*>* aPresenceIconsArray );

        /**
         * Destructor.
         * Cancels address selection query if active.
         */
        IMPORT_C ~CPbk2AddressSelect();

    public: // Interface

        /**
         * Runs an address selection query (if necessary).
         * Note! The caller is responsible for the contact field
         * object returned.
         *
         * @return  NULL if query was cancelled, selected field if
         *          query was accepted. Caller takes the ownership
         *          of the returned field.
         */
        IMPORT_C MVPbkStoreContactField* ExecuteLD();

        /**
         * Exit any address selection query. Makes ExecuteLD return
         * which means this object is destroyed after this function returns!
         *
         * @param aAccept   ETrue means the query selection is accepted and
         *                  EFalse that query is canceled.
         */
        IMPORT_C void AttemptExitL(
                TBool aAccept );

        /**
         * Override to handle key events in the list query.
         *
         * @param aKeyEvent     Key event.
         * @param aType         Key event type.
         * @return  EKeyWasConsumed if the key event was consumed,
         *          EKeyWasNotConsumed otherwise.
         */
        IMPORT_C virtual TKeyResponse Pbk2ControlKeyEventL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );

        /**
         * Sets CBA button labels if needed.
         * Overrides buttons defined in the resource file.
         */
        IMPORT_C void SetCba(
                TInt aCbaResourceId );
        
    public: // From MPbk2DialogEliminator
        void RequestExitL(
                TInt aCommandId );
        void ForceExit();
        void ResetWhenDestroyed(
                MPbk2DialogEliminator** aSelfPtr );

    private: // Implementation
        CPbk2AddressSelect(
                TPbk2AddressSelectParams& aParams,
                const TArray<MVPbkStoreContact*>* aStoreContactsArray,
                const TArray<CPbk2PresenceIconInfo*>* aPresenceIconsArray );
        void ConstructL( 
            CVPbkFieldTypeSelector* aFieldTypeSelector );
        TBool AddressField(
                const MVPbkStoreContactField& aField ) const;
        void NoAddressesL(
                TPbk2AddressSelectParams aParams ) const;
        void SelectFieldL();
        void SelectFromApplicableFieldsL();
        TInt IndexOfDefaultFieldL();
        TBool SelectFromDefaultFieldsL();
        MVPbkStoreContactField* FindDefaultFieldLC();
        void SetSelectedFieldL(
                const MVPbkStoreContactField* aField );
        HBufC* LoadDialogTitleL();
        TBool AreGeoFieldsForAddressesL();
        HBufC* GetVOIPDialogTitleL();

    private: // Friends
        /// Field selection dialog for this class
        class CSelectFieldDlg;
        friend class CSelectFieldDlg;

    private: // Data
        /// Ref: Address select parameters
        TPbk2AddressSelectParams& iParams;
        /// Own: Field type selector
        CVPbkFieldTypeSelector* iFieldTypeSelector;
        /// Own: Field filter
        CVPbkFieldFilter* iFieldFilter;
        /// Own: Field selection dialog
        CPbk2SelectFieldDlg* iFieldDlg;
        /// Ref: Set to ETrue when the execution enters destructor
        TBool* iDestroyedPtr;
        /// Ref: The selected field
        MVPbkStoreContactField* iSelectedField;
        /// Ref: Virtual Phonebook attribute manager
        MVPbkContactAttributeManager* iAttributeManager;
        /// Own: No addresses for a name prompt text resource id
        TInt iNoAddressesForNamePromptResource;
        /// Own: No address prompt text resource id
        TInt iNoAddressesPromptResource;
        /// Own: Soft key resource id
        TInt iSoftKeyResource;
        /// Ref: Set to NULL when the execution enters destructor
        MPbk2DialogEliminator** iSelfPtr;
        /// Ref: Additional contacts
        const TArray<MVPbkStoreContact*>* iStoreContactsArray;
        /// Own: Field filter array for additional contacts
        RPointerArray<CVPbkFieldFilter> iFieldFilterArray;
        /// Ref: Presence icons array
        const TArray<CPbk2PresenceIconInfo*>* iPresenceIconsArray;
    };

#endif // CPBK2ADDRESSSELECT_H

// End of File
