/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Manages adding new items to the contact
*
*/


#ifndef CPBK2ADDITEMMANAGER_H
#define CPBK2ADDITEMMANAGER_H

//  INCLUDES
#include <e32base.h>
#include <MPbk2FieldProperty.h>
#include "TPbk2ContactEditorParams.h"

// FORWARD DECLARATIONS
class CPbk2PresentationContact;
class MPbk2ContactEditorFieldArray;
class MPbk2FieldPropertyArray;
class TPbk2AddItemWrapper;
class MVPbkStoreContact;
class CPbk2FieldGroupPropertyArray;
class MPbk2ApplicationServices;

// CLASS DECLARATION


/**
 *  Manages adding new items to the contact.
 *
 */
NONSHARABLE_CLASS(CPbk2AddItemManager) : public CBase
    {
    public:  // Constructors and destructor
    	class TReturnValue
    		{
    		public:
    			TReturnValue()
    			:iGruopId(EPbk2FieldGroupIdNone), 
    			iControlId(KErrNone)
				{}
	
    		public:
    			TInt iControlId;
    			TPbk2FieldGroupId iGruopId;
	};

        /**
         * Creates a new instance of this class.
         *
         * @param aContact          The contact to add fields.
         * @param aFieldArray       The field array for adding new fields.
         * @return  A new instance of this class.
         */
        static CPbk2AddItemManager* NewL(
                CPbk2PresentationContact& aContact,
                MPbk2ContactEditorFieldArray& aFieldArray, 
                TPbk2ContactEditorParams& aParams,
                MPbk2ApplicationServices* aAppServices );

        /**
         * Destructor.
         */
        ~CPbk2AddItemManager();

    public: // Interface
        /**
         * Executes the add item dialog and returns the control id
         * of the field that was added upmost.
         *
         * @param aFieldTypeResId   Add item field type resource id.
         *                          If positive or zero, the list box
         *                          will not be run, but a corresponding
         *                          control id will be returned.
         *                          If negative, the list box will be run
         *                          and selected index will be returned.
         * @param aFieldTypeXspName Add item field type xSP name.
         * @return  The control id.
         */
    	TReturnValue AddFieldsL(
                TInt& aFieldTypeResId, TPtrC& aFieldTypeXspName);
            
    private:  // Implementation
        CPbk2AddItemManager(
                CPbk2PresentationContact& aContact,
                MPbk2ContactEditorFieldArray& aFieldArray, 
                TPbk2ContactEditorParams& aParams,
                MPbk2ApplicationServices* aAppServices );
        TReturnValue AddFieldsToUiAndContactL(
                const TPbk2AddItemWrapper& aWrapper );
        TReturnValue RunDialogAndAddFieldsL(
                RArray<TPbk2AddItemWrapper>& aWrappers,
                TInt& aFieldTypeResId,
                TPtrC& aFieldTypeXspName );
        void AddPhoneMemoryFieldTypesL(
                CArrayPtr<const MPbk2FieldProperty>& aProperties );
        TReturnValue DoAddFieldsL(
                CArrayPtr<const MPbk2FieldProperty>& aProperties,
                TInt& aFieldTypeResId,
                TPtrC& aFieldTypeXspName );
        TBool IsFieldAdditionPossibleL(
                const CPbk2PresentationContact& aContact,
                const MPbk2FieldProperty& aProperty );
        void MakePhoneContactL(
                const CPbk2PresentationContact& aContact );

    private: // Data
        /// Ref: The contact to add fields
        CPbk2PresentationContact* iContact;
        /// Ref: The UI field array for adding new fields
        MPbk2ContactEditorFieldArray* iFieldArray;
        /// Ref: The field properties
        const MPbk2FieldPropertyArray* iProperties;
        /// Own: Temporary phone memory presentation contact
        CPbk2PresentationContact* iTemporaryPresentationContact;
        /// Own: Temporary phone memory store contact
        MVPbkStoreContact* iTemporaryStoreContact;
        /// Own: field group properties
        CPbk2FieldGroupPropertyArray* iGroupProperties;
        /// Own: field type xSP name
        HBufC* iXSpName;
        /// Ref: Editor parameters
        TPbk2ContactEditorParams& iParams;
        // Ref: Pbk2 application services
        MPbk2ApplicationServices* iAppServices;
    };

#endif // CPBK2ADDITEMMANAGER_H
            
// End of File
