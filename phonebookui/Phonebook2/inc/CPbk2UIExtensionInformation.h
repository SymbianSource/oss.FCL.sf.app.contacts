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
* Description:  Phonebook 2 UI Extension information.
*
*/


#ifndef CPBK2UIEXTENSIONINFORMATION_H
#define CPBK2UIEXTENSIONINFORMATION_H

// INCLUDE FILES
#include <e32base.h>
#include <TPbk2UIExtensionRange.h>
#include <RPbk2LocalizedResourceFile.h>
#include <Pbk2UIExtension.hrh>
#include <Pbk2IconId.hrh>

// FORWARD DECLARATIONS
class TResourceReader;
class CPbk2UIExtensionMenu;
class CPbk2UIExtensionIconInformation;

/**
 * Phonebook 2 UI Extension view id structure.
 * An instance of this class is created from
 * PHONEBOOK2_EXTENSION_VIEW_ID resource definition.
 */
class TPbk2UIExtensionViewId
    {
    public: // Construction
        /**
         * Constructor.
         *
         * @param aReader   Resource reader.
         */
        TPbk2UIExtensionViewId(
                TResourceReader& aReader );

        /**
         * Constructor.
         *
         * @param aViewId   View identifier.
         * @param aViewResourceId   Id to a AVKON_VIEW resource structure.
         */
        IMPORT_C TPbk2UIExtensionViewId(TUid aViewId, 
                TInt aViewResourceId);

    public: // Interface
        /**
         * Returns the view id.
         *
         * @return  View id.
         */
        TUid ViewId() const;

        /**
         * Returns the view resource id.
         *
         * @return  View resource id.
         */
        TInt ViewResourceId() const;

    private: // Data
        /// Own: View id
        TUid iViewId;
        /// Own: View resource id.
        TInt iViewResourceId;
    };

/**
 * Phonebook 2 UI Extension information.
 * An instance of this class is created from
 * PHONEBOOK2_EXTENSION_INFORMATION resource definition.
 */
NONSHARABLE_CLASS(CPbk2UIExtensionInformation) : public CBase
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class by reading the
         * data from the resources.
         *
         * @param aFileName     File that contains the
         *                      resources for this object.
         * @return  A new instance of this class.
         */
        static CPbk2UIExtensionInformation* NewLC(
                const TDesC& aFileName );

        /**
         * Destructor.
         */
        ~CPbk2UIExtensionInformation();

    public: // Interface

        /**
         * Returns the number of menus that the plug-in
         * brings to application.
         *
         * @return  Number of menus the plug-in brings to application.
         */
        TInt MenuCount() const;

        /**
         * Returns the menu at the given index.
         *
         * @param aIndex    Index of the menu to return.
         * @return  The menu at given index.
         */
        CPbk2UIExtensionMenu& MenuAt(
                TInt aIndex );

        /**
         * Adds aMenu to the extension information.
         *
         * @param aExtensionMenu    Menu to add to this information.
         */
        IMPORT_C void AddMenuL( CPbk2UIExtensionMenu* aExtensionMenu );

        /**
         * Returns the implementation uid of the plug-in.
         *
         * @return  Implementation uid of the plug-in.
         */
        TUid ImplementationUid() const;

        /**
         * Returns the loading policy of the extension.
         *
         * @return  Loading policy of the extension.
         */
        TUint LoadingPolicy() const;

        /**
         * Checks if the plug-in represented by this resource implements
         * given menu command. Note that the this may return ETrue even
         * though the plug-in does not actually implement the command but
         * only has the command in its command id range.
         *
         * @param aCommandId    Command id to check.
         * @return  ETrue if the plug-in represented by this resource
         *          implements given menu command, EFalse otherwise.
         */
        TBool ImplementsMenuCommand(
                TInt aCommandId );

        /**
         * Checks if the plug-in represented by this resource implements
         * view with the given view id. Note that this may return ETrue even
         * though the plug-in does not actually implement the view but only
         * has the view in its view id range.
         *
         * @param aViewId   View id to check.
         * @return  ETrue if the plug-in represented by this resource
         *          implements given view, EFalse otherwise.
         */
        TBool ImplementsView(
                TUid aViewId );

        /**
         * Returns ETrue if this extension has modifications to
         * the view graph, EFalse otherwise.
         *
         * @return  ETrue if this extension has modifications to
         *          the view graph, EFalse otherwise.
         */
        TBool HasViewGraphModifications() const;

        /**
         * Creates a resource reader for the view graph modifications.
         *
         * @param aReader   The reader to create.
         */
        void CreateViewGraphModificationReaderLC(
                TResourceReader& aReader ) const;

        /**
         * Returns an array of extension view ids.
         *
         * @return  An array of extension view ids.
         */
        TArray<TPbk2UIExtensionViewId> ViewIds() const;

        /**
         * Adds a View id object to the extension view ids.
         *
         * @param aViewId   Extension view id.
         */
        IMPORT_C void AddViewIdL(TPbk2UIExtensionViewId& aViewId);

        /**
         * Returns ETrue if the extension has own icons.
         *
         * @return  ETrue if the extension has own icons.
         */
        TBool HasIconExtension() const;

        /**
         * Returns the icon information of the extension.
         *
         * @return  The icon information of the extension.
         */
        CPbk2UIExtensionIconInformation& IconInformation() const;

        /**
         * Returns ETrue if the extension has defined store
         * property array in the resource.
         *
         * @return  ETrue if the extension has defined store property array.
         */
        TBool HasStorePropertyArray() const;

        /**
         * Creates a resource reader for the store property array.
         * Not to be called if HasStorePropertyArray returns EFalse.
         *
         * @param aReader   The reader to create.
         */
        void CreateStorePropertyArrayReaderLC(
                TResourceReader& aReader );

    private: // Implementation
        CPbk2UIExtensionInformation();
        void ConstructL(
                const TDesC& aFileName );

    private: // Data
        /// Own: Resource file
        RPbk2LocalizedResourceFile iResourceFile;
        /// Own: Implementation UID
        TUid iImplementationUid;
        /// Own: Loading policy
        TUint iLoadingPolicy;
        /// Own: Extension's menu command range
        TPbk2UIExtensionRange iMenuCommandRange;
        /// Own: Array of extension menus
        RPointerArray<CPbk2UIExtensionMenu> iMenus;
        /// Own: Array of extension's view ids
        RArray<TPbk2UIExtensionViewId> iViewIds;
        /// Own: View graph modification resource id
        TInt iViewGraphModificationResId;
        /// Own: The icon information of the extension
        CPbk2UIExtensionIconInformation* iIconInformation;
        /// Own: An array for commands ids that are
        ///      overwritten by the extension
        RArray<TInt> iOverWrittenCmds;
        /// Own: The id of the store property array
        TInt iStorePropertyArrayResId;
    };

#endif // CPBK2UIEXTENSIONINFORMATION_H

// End of File
