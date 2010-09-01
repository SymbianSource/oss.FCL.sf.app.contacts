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
* Description:  Phonebook 2 contact editor contact relocator interface.
*
*/


#ifndef MPBK2CONTACTEDITORCONTACTRELOCATOR_H
#define MPBK2CONTACTEDITORCONTACTRELOCATOR_H

#include <CPbk2ContactRelocator.h>

#define TPbk2RelocationQyeryPolicy \
    Pbk2ContactRelocator::TPbk2ContactRelocationQueryPolicy

// CLASS DECLARATION

/**
 * Phonebook 2 contact editor contact relocator interface.
 */
class MPbk2ContactEditorContactRelocator
    {
    public: // Interface

        /**
         * Relocates the contact.
         * @param aQueryPolicy              Query policy.
         * @return  ETrue if user has accepted relocation, otherwise EFalse.
         *          NOTE! Do not delete relocator instance. Relocator will
         *                be completed asynchronusly through aObserver.
         */
        virtual TBool RelocateContactL(
            TPbk2RelocationQyeryPolicy aQueryPolicy ) = 0;

        /**
         * Relocates the contact. Use this method when the user tries to add
         * an unsupported field type to the contact.
         *
         * @param aAddItemSelectionIndex    Indicates the index of the
         *                                  field type in the add item dialog.
         * @param aAddItemXspName           Field type xSP name
         * @param aQueryPolicy              Query policy.
         * @return  ETrue if user has accepted relocation, otherwise EFalse.
         *          NOTE! Do not delete relocator instance. Relocator will
         *                be completed asynchronusly through aObserver.
         */
        virtual TBool RelocateContactL(
                TInt aAddItemSelectionIndex,
                const TDesC& aAddItemXspName,
                TPbk2RelocationQyeryPolicy aQueryPolicy ) = 0;

        /**
         * Returns ETrue if phone memory is in configuration.
         *
         * @return ETrue if phone memory is in configuration,
         *         EFalse otherwise.
         */
        virtual TBool IsPhoneMemoryInConfigurationL() = 0;

        /**
         * Notifies the relocator, that contact editor got
         * ready and initialized.
         */
        virtual void EditorReadyL() = 0;
    };

#endif // MPBK2CONTACTEDITORCONTACTRELOCATOR_H

// End of File
