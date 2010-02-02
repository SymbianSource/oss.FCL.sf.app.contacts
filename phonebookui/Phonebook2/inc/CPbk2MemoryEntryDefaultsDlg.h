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
* Description:  Phonebook 2 memory entry defaults dialog.
*
*/


#ifndef CPBK2MEMORYENTRYDEFAULTSDLG_H
#define CPBK2MEMORYENTRYDEFAULTSDLG_H

// INCLUDES
#include <aknPopup.h>                       // CAknPopupList
#include <MPbk2DefaultAttributeProcessObserver.h>
#include <VPbkFieldTypeSelectorFactory.h>

// FORWARD DECLARATIONS
class CEikColumnListBox;
class CPbk2PresentationContact;
class CVPbkContactManager;
class CPbk2DefaultAttributeProcess;
class MVPbkStoreContactField;

// CLASS DECLARATION

/**
 * Phonebook 2 memory entry defaults dialog.
 * This is a custom popup for Phonebook 2 application's
 * contacts' default settings.
 *
 * Responsible for:
 * - populating the list box with correct content, this
 *   is difficult since supported defaults are heavily variated
 * - setting or removing the default attributes to/from the contact
 *   by driving default attribute processor
 */
class CPbk2MemoryEntryDefaultsDlg :
            public CAknPopupList,
            public MPbk2DefaultAttributeProcessObserver
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aContact  Contact whose defaults are chosen.
         * @param aManager  Virtual Phonebook contact manager.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2MemoryEntryDefaultsDlg* NewL(
                CPbk2PresentationContact& aContact,
                CVPbkContactManager& aManager );

        /**
         * Destructor.
         * Also cancels and dismisses this popup list, in other words makes
         * ExecuteLD return as if cancel was pressed.
         */
        ~CPbk2MemoryEntryDefaultsDlg();

    public: // Interface

        /**
         * Runs the dialog.
         */
        IMPORT_C void ExecuteLD();

    private: // From CAknPopupList
        void ProcessCommandL(
                TInt aCommandId );
        void HandleListBoxEventL(
                CEikListBox* aListBox,
                TListBoxEvent aEventType );

    private: // From MPbk2DefaultAttributeProcessObserver
        void AttributeProcessCompleted();
        void AttributeProcessFailed(
                TInt aErrorCode );

    private: // Implementation
        CPbk2MemoryEntryDefaultsDlg(
                CPbk2PresentationContact& aContact,
                CVPbkContactManager& aManager );
        void ConstructL();
        void CreateLinesL() const;
        TBool DefaultsAssignPopupL(
                VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aSelectorID );
        void FixIndex(
                TInt& aIndex ) const;
        TBool IsSupported(
                const VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aSelectorID ) const;
        TBool IsAvailableL(
                const VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aSelectorID ) const;
        void SetDefaultL(
                const VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aSelectorID,
                MVPbkStoreContactField& aField );
        void RemoveDefaultL(
                const VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aSelectorID );
        void CreateTableOfDefaultsL();
        void ReadDefaultsFromRepositoryL();
        
    private: // Data
        /// Ref: The contact
        CPbk2PresentationContact& iContact;
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iManager;
        /// Own: Default selection listbox
        CEikFormattedCellListBox* iListBox;
        /// Ref: Referred TBool is set ETrue in destructor
        TBool* iDestroyedPtr;
        /// Own: Contact field the default is assigned to
        MVPbkStoreContactField* iField;
        /// Own: Attribute set/remove process
        CPbk2DefaultAttributeProcess* iAttributeProcess;
        /// Own: Local variation flags
        TInt iLVFlags;
        /// Own: Table of used Defaults item
        CArrayFixFlat<VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector>* iDefaultsTable;
        /// Own: Indicates if focus has been dragged around the list, helps with single tap launching
        TBool iHasBeenDragged;
    };

#endif // CPBK2MEMORYENTRYDEFAULTSDLG_H

// End of File
