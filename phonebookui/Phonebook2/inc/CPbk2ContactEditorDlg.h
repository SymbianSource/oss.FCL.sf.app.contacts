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
* Description:  Phonebook 2 contact editor dialog.
*
*/


#ifndef CPBK2CONTACTEDITORDLG_H
#define CPBK2CONTACTEDITORDLG_H

// INCLUDES
#include <e32base.h>
#include <babitflags.h>
#include <MPbk2ContactRelocatorObserver.h>
#include <MPbk2EditedContactObserver.h>
#include "TPbk2ContactEditorParams.h"
#include "MPbk2ContactEditorContactRelocator.h"
#include "MPbk2DialogEliminator.h"

// FORWARD DECLARATIONS
class MPbk2ContactEditorStrategy;
class CPbk2FieldPropertyArray;
class CPbk2StoreSpecificFieldPropertyArray;
class CPbk2PresentationContact;
class CPbk2ContactEditorDlgImpl;
class CPbk2ContactRelocator;
class MPbk2ApplicationServices;


// CLASS DECLARATION

/**
 * Phonebook 2 contact editor dialog.
 */
class CPbk2ContactEditorDlg : public CBase,
                              public MPbk2DialogEliminator,
                              public MPbk2ContactRelocatorObserver,
                              public MPbk2ContactEditorContactRelocator,
                              private MPbk2EditedContactObserver
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aParams   Contact editor parameters.
         * @param aContact  The contact to edit. Ownership is taken.
         * @param aObserver Observer for the contact being edited.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2ContactEditorDlg* NewL(
                TPbk2ContactEditorParams& aParams,
                MVPbkStoreContact* aContact,
                MPbk2EditedContactObserver& aContactObserver );
        
        /**
		* Creates a new instance of this class.
		*
		* @param aParams   Contact editor parameters.
		* @param aContact  The contact to edit. Ownership is taken.
		* @param aObserver Observer for the contact being edited.
		* @param aAppServices Application services.
		* @param aTitleText   ownership is transferred, if function doesn't leave
		* @param aEditorFieldsId Supported editor fields resource id (@see PbkPresentation.rss : R_PHONEBOOK_FIELD_PROPERTIES)
		* @return  A new instance of this class.
		*/
	   IMPORT_C static CPbk2ContactEditorDlg* NewL(
			   TPbk2ContactEditorParams& aParams,
			   MVPbkStoreContact* aContact,
			   MPbk2EditedContactObserver& aContactObserver,
			   MPbk2ApplicationServices* aAppServices,
			   HBufC* aTitleText = NULL,
			   TInt aEditorFieldsId = KErrNotFound );

        /**
         * Destructor.
         */
        virtual ~CPbk2ContactEditorDlg();

    public: // Interface

        /**
         * Executes the dialog.
         */
        IMPORT_C void ExecuteLD();

    public: // From MPbk2DialogEliminator
        void RequestExitL(
                TInt aCommandId );
        void ForceExit();
        void ResetWhenDestroyed(
                MPbk2DialogEliminator** aSelfPtr );
        
    private: // From MPbk2ContactRelocatorObserver
        void ContactRelocatedL(
                MVPbkStoreContact* aRelocatedContact );
        void ContactRelocationFailed(
                TInt aReason,
                MVPbkStoreContact* aContact );
        void ContactsRelocationFailed(
                TInt aReason,
                CVPbkContactLinkArray* aContacts );
        void RelocationProcessComplete();

    private: // From MPbk2ContactEditorContactRelocator
        TBool RelocateContactL(
                TPbk2RelocationQyeryPolicy aQueryPolicy );
        TBool RelocateContactL(
                TInt aAddItemFieldTypeResourceId,
                const TDesC& aAddItemXspName,
                TPbk2RelocationQyeryPolicy aQueryPolicy );
        TBool IsPhoneMemoryInConfigurationL();
        void EditorReadyL();

    private: // From MPbk2EditedContactObserver
        void ContactEditingComplete(
            MVPbkStoreContact* aEditedContact );
        void ContactEditingDeletedContact(
            MVPbkStoreContact* aEditedContact );
        void ContactEditingAborted();

    private: // Implementation
        CPbk2ContactEditorDlg(
                TPbk2ContactEditorParams& aParams,
                MPbk2EditedContactObserver& aContactObserver,
                MPbk2ApplicationServices* aAppServices = NULL,
                TInt aEditorFieldsId = KErrNotFound );
        void ConstructL( MVPbkStoreContact* aStoreContact );
        void HandleRelocationProcessCompleteL();
        void CreatePresentationContactL();
        void CreatePresentationContactL(MVPbkStoreContact& aStoreContact);
        TBool ContinueAfterRelocation();
        TBool IsTemplateFieldL( TInt aResId, const TDesC& aXSpName );

    private: // Types
        /// Relocation states
        enum TRelocationState
            {
            ERelocationDueToItemAddition,
            ERelocationCancelled,
            ERelocationFailed
            };

    private: // Data
        /// Own: Editor dialog implementation
        CPbk2ContactEditorDlgImpl* iImplementation;
        /// Ref: Dialog eliminator
        MPbk2DialogEliminator* iEliminator;
        /// Ref: Editor parameters
        TPbk2ContactEditorParams& iParams;
        /// Own: Store contact
        MVPbkStoreContact* iStoreContact;
        /// Own: Phonebook 2 field properties
        CPbk2FieldPropertyArray* iFieldProperties;
        /// Own: Store specific properties
        CPbk2StoreSpecificFieldPropertyArray* iSpecificFieldProperties;
        /// Own: Presentation level contact
        CPbk2PresentationContact* iContact;
        /// Ref: Observer for the contact being edited
        MPbk2EditedContactObserver& iContactObserver;
        /// Own: A strategy for the editor
        MPbk2ContactEditorStrategy* iEditorStrategy;
        /// Own: Add item field type resource id
        /// used in contact relocation process
        TInt iAddItemFieldTypeResourceId;
        /// Own: Add item field type xSP name
        /// used in contact relocation process
        HBufC* iAddItemFieldTypeXspName;
        /// Own: Contact relocator
        CPbk2ContactRelocator* iContactRelocator;
        /// Ref: Indicates relocation state
        TBitFlags8 iRelocationState;
        /// Ref: A pointer to stack variable in ExecuteLD for knowing
        /// if the destructor has already been called
        TBool* iDestroyedPtr;
        /// Ref: Pointer to dialog eliminator
        MPbk2DialogEliminator** iSelfPtr;
        /// Ref: Applications services
        MPbk2ApplicationServices* iAppServices;
		/// Own Custom title text
		HBufC* iTitleText;
		// REF: Custom editor fields 
		TInt iEditorFieldsId;
		
    };

#endif // CPBK2CONTACTEDITORDLG_H

// End of File
