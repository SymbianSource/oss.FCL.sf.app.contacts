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
* Description:  Phonebook 2 address selection parameters.
*
*/


#ifndef TPBK2ADDRESSSELECTPARAMS_H
#define TPBK2ADDRESSSELECTPARAMS_H

// INCLUDES
#include <e32base.h>
#include <barsread.h>
#include <VPbkFieldType.hrh>
#include <RVPbkContactFieldDefaultPriorities.h>
#include <VPbkFieldTypeSelectorFactory.h>

// FORWARD DECLARATIONS
class MVPbkStoreContact;
class MVPbkStoreContactField;
class MPbk2ContactNameFormatter;
class CVPbkContactManager;
class CPbk2FieldPropertyArray;

// CLASS DECLARATION

/**
 * Phonebook 2 address selection parameters.
 * Responsible for carrying the parameters to the address select class.
 */
class TPbk2AddressSelectParams
    {
    public: // Construction

        /**
         * Constructor.
         *
         * @param aContact            Contact from where to select
         *                            an address.
         * @param aContactManager     Virtual Phonebook contact manager.
         * @param aNameFormatter      Name formatter.
         * @param aFieldPropertyArray Phonebook 2 field property array.
         * @param aReader             Resource reader pointed to
         *                            PBK2_ADDRESS_SELECT structure.
         * @param aTitleResId         Resource id for title text.
         *                            Default = 0. If default, the memory
         *                            entry name is used as a title.
         */
         IMPORT_C TPbk2AddressSelectParams(
                MVPbkStoreContact& aContact,
                const CVPbkContactManager& aContactManager,
                MPbk2ContactNameFormatter& aNameFormatter,
                const CPbk2FieldPropertyArray& aFieldPropertyArray,
                TResourceReader& aReader,
                TInt aTitleResId = 0 );

    public: // Interface

        /**
         * Sets the focused field of the contact. If focused field
         * is a non-empty address field it is returned directly by
         * ExecuteLD. Defaults to NULL.
         *
         * @param aFocusedField       The focused field to set.
         */
        IMPORT_C void SetFocusedField(
                const MVPbkStoreContactField* aFocusedField );

        /**
         * Sets the default to be used in address select. The
         * defaults in the array should be in priority order.
         * If the priorities array is not set, no defaults are used.
         * Focused field overrides default address if both are
         * available.
         *
         * @param aDefaultPriorities  An array of default properties in
         *                            priority order.
         */
        IMPORT_C void SetDefaultPriorities(
                const RVPbkContactFieldDefaultPriorities& aDefaultPriorities );

        /**
         * Set to ETrue to use default address of the contact directly.
         * Focused field overrides default address if both are
         * available. Default value for this parameter is EFalse.
         *
         * @param aUseDefaultDirectly   Defines should the default address
         *                              be used directly.
         */
        IMPORT_C void SetUseDefaultDirectly(
                TBool aUseDefaultDirectly );

        /**
         * Set to ETrue if the query should be shown always, even if
         * it possible to select only one field. This overrides defaults
         * also. Default value for this parameter is EFalse.
         *
         * @param aQueryAlways  Set to ETrue if the query should
         *                      be always shown.
         */
        IMPORT_C void SetQueryAlways(
                TBool aQueryAlways );

        /**
         * Sets custom title type. Indicates whether the resource string
         * must have one "%U" in it, which the contact name will replace.
         *
         * @param aIncludeContactName   If ETrue the prompt will include
         *                              contact name in it. If EFalse,
         *                              the prompt include contact name.
         *                              Default is ETrue.
         */
        IMPORT_C void SetTitleToIncludeContactName(
                TBool aIncludeContactName );

        /**
         * Set to ETrue if the warnings should be suppressed.
         * Default value for this parameter is EFalse.
         *
         * @param aSuppressWarnings     Set to ETrue if the warnings
         *                              should be suppressed.
         */
        IMPORT_C void SetSuppressWarnings(
                TBool aSuppressWarnings );

    public: // Data
        /// Ref: Contact where to select an address from
        MVPbkStoreContact& iContact;
        /// Ref: Virtual Phonebook contact manager
        const CVPbkContactManager& iContactManager;
        /// Ref: Contact name formatter.
        MPbk2ContactNameFormatter& iNameFormatter;
        /// Ref: Phonebook 2 field property array.
        const CPbk2FieldPropertyArray& iFieldPropertyArray;
        /// Ref: Resource reader.
        TResourceReader& iResReader;
        /// Own: Title resource id
        TInt iTitleResId;
        /// Ref: Focused field of the contact.
        const MVPbkStoreContactField* iFocusedField;
        /// Ref: Prioritized array of defaults to use.
        const RVPbkContactFieldDefaultPriorities* iDefaultPriorities;
        /// Own: Indicates should the default address be used directly
        TBool iUseDefaultDirectly;
        /// Own: Indicates should the query be shown always
        TBool iQueryAlways;
        /// Own: Indicates whether to include contact name in custom prompt.
        ///      Custom prompt refers to iTitleResId.
        TBool iIncludeContactNameInPrompt;
        /// Own: Indicates are the warnings suppressed
        TBool iSuppressWarnings;
        /// Own: Communication method
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector iCommMethod;

    private: // Friends
        friend class CPbk2AddressSelect;
    };

#endif // TPBK2ADDRESSSELECTPARAMS_H

// End of File
