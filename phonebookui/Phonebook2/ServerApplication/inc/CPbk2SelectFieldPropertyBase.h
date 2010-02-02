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
* Description:  Phonebook 2 contact field property selector base.
*
*/


#ifndef CPBK2SELECTFIELDPROPERTYBASE_H
#define CPBK2SELECTFIELDPROPERTYBASE_H

// INCLUDES
#include <e32base.h>
#include "MPbk2SelectFieldProperty.h"

// FORWARD DECLARATIONS
class TPbk2AddItemWrapper;
class TVPbkContactStoreUriPtr;
class MVPbkContactStore;
class CPbk2FieldPropertyArray;
class MVPbkFieldType;
class MPbk2DialogEliminator;

// CLASS DECLARATION

/**
 * Phonebook 2 contact field property selector base.
 * Responsible for:
 * - creating add item wrappers
 * - launching the select field query dialog
 * - returning the selected field type
 */
class CPbk2SelectFieldPropertyBase : public CBase,
                                     public MPbk2SelectFieldProperty
    {
    public: // Construction and destruction

        /**
         * Destructor.
         */
        ~CPbk2SelectFieldPropertyBase();

    public: // From MPbk2SelectFieldProperty
        const MVPbkFieldType* SelectedFieldType() const;
        void Cancel(
                TInt aCommandId );

    protected: // Implementation
        CPbk2SelectFieldPropertyBase(
                HBufC8& aSelector,
                TInt aResourceId );
        void BaseConstructL();
        void CreateFieldPropertiesArrayL(
                MVPbkContactStore& aStore );
        void CreateWrappersL(
                TVPbkContactStoreUriPtr aStoreUri );
        TInt ShowSelectFieldQueryL();

    protected: // Data
        /// Own: Add item field type wrappers
        RArray<TPbk2AddItemWrapper> iWrappers;
        /// Own: Phonebook field properties
        CPbk2FieldPropertyArray* iFieldProperties;
        /// Ref: Add item dialog eliminator
        MPbk2DialogEliminator* iAddItemDlgEliminator;
        /// Ref: Selected add item field type
        const MVPbkFieldType* iSelectedFieldType;
        /// Ref: Externalized field type selector buffer
        HBufC8& iSelector;
        /// Ref: Resource id of the dialog title
        TInt iDialogTitleResourceId;
    };

#endif // CPBK2SELECTFIELDPROPERTYBASE_H

// End of File
