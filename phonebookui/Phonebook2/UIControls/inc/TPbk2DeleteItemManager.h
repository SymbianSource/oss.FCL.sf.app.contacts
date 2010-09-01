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
* Description:  Phonebook 2 contact editor delete item manager.
*
*/


#ifndef TPBK2DELETEITEMMANAGER_H
#define TPBK2DELETEITEMMANAGER_H

// INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class CPbk2PresentationContact;
class MPbk2ContactEditorFieldArray;
class MVPbkStoreContactField;
class MPbk2ApplicationServices;
// CLASS DECLARATION

/**
 * Phonebook 2 contact editor delete item manager.
 * Responsible for managing item deletion in contact editor.
 */
class TPbk2DeleteItemManager
    {
    public:  // Constructors and destructor
        
        /**
         * C++ constructor.
         *
         * @param aContact      The contact to add fields to.
         * @param aFieldArray   The field array for removing the field.
         * @param aAppServices  Pbk2 application services.
        */
        TPbk2DeleteItemManager(
                CPbk2PresentationContact& aContact,
                MPbk2ContactEditorFieldArray& aFieldArray,
                MPbk2ApplicationServices* aAppServices );

    public: // Interface
        
        /**
         * Executes the confirmation query and deletes 
         * the field that has the given control id.
         *
         * @param aControlId    Id of the control to delete.
         * @return  ETrue if the field was deleted.
         */
        TBool DeleteFieldL(
                TInt aControlId );
    
    private: // Prohibitions
        // Prohibit copy constructor when not deriving from CBase
        TPbk2DeleteItemManager(
                const TPbk2DeleteItemManager& );
        // Prohibit assigment operator when not deriving from CBase
        TPbk2DeleteItemManager& operator=(
                const TPbk2DeleteItemManager& );

    private: // Implementation
        void InformDeleteSpeedDialL(
                const MVPbkStoreContactField& storeField ) const;

    private: // Data
        /// Ref: The contact to add the fields
        CPbk2PresentationContact& iContact;
        /// Ref: The UI field array for adding new fields
        MPbk2ContactEditorFieldArray& iFieldArray;
        ///Ref: Pbk2 Application services 
        MPbk2ApplicationServices* iAppServices;
    };

#endif // TPBK2DELETEITEMMANAGER_H
            
// End of File
