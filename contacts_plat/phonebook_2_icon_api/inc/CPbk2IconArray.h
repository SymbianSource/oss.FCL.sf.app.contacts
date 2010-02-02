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
* Description:  An icon array for Phonebook 2 icons.
*
*/


#ifndef CPBK2ICONARRAY_H
#define CPBK2ICONARRAY_H

//  INCLUDES
#include <e32base.h>
#include <pbk2iconarrayid.hrh>

//  FORWARD DECLARATIONS
class CGulIcon;
class TResourceReader;
class CPbk2IconInfoContainer;
class MPbk2FieldPropertyArray;
class CPbk2IconFactory;
class TPbk2IconId;
class CPbk2IconInfo;

// CLASS DECLARATION

/**
 *  An icon array for Phonebook 2 icons.
 * Responsible for:
 * - specializing icon array for Phonebook 
 */
class CPbk2IconArray : public CArrayPtrFlat<CGulIcon>
    {
    public: // Constructors and destructor

        /**
         * Constructs an array of core Phonebook 2 icons
         * specified in the resource.
         *
         * @param aReader   A resource reader pointed
         *                  to PBK2_ICON_ARRAY resource.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2IconArray* NewL(
                TResourceReader& aReader);

        /**
         * Constructs a core Phonebook 2 field type icon array.
         * Adds qgn_indi_marked_add into first position as the
         * Avkon needs that. Adds also en empty icon qgn_prop_nrtyp_empty
         * to make it always available.
         * The id of this array is EPbk2FieldTypeIconArrayId
         * after the construction.
         *
         * @param aFieldProperties  Phonebook 2 field properties. Reference
         *                          is not saved but used in construction
         *                          only.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2IconArray* NewL(
                const MPbk2FieldPropertyArray& aFieldProperties );

        /**
         * Constructs an icon array using a given icon info container.
         *
         * @param aReader               A resource reader pointed
         *                              to PBK2_ICON_ARRAY resource.
         * @param aIconInfoContainer    Icon infos that are needed in
         *                              icon creation.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2IconArray* NewL(
                TResourceReader& aReader,
                const CPbk2IconInfoContainer& aIconInfoContainer );

        /**
         * Destructor. Calls ResetAndDestroy.
         */
        ~CPbk2IconArray();

    public: // Interface

        /**
         * Returns the Phonebook 2 icon array id.
         *
         * @return  ID of the icon array.
         */
        IMPORT_C TPbk2IconArrayId Id() const;

        /**
         * Returns the index of the icon with aIconId in this array.
         *
         * @param aIconId   Icon id to search for.
         * @return  Index of the icon or KErrNotFound if not found.
         */
        IMPORT_C TInt FindIcon(
                const TPbk2IconId& aIconId ) const;

        /**
         * This function first calls FindIcon, and if it doesn't find
         * the icon it creates it by using core Phonebook 2 icons.
         * The created icon is then appended to this array.
         * If the icon id is defined in Phonebook UI extension then
         * it is not possible to create it by using this function.
         *
         * @param aIconId   Icon id to search for.
         * @return  Index of the icon or KErrNotFound if not found.
         */
        IMPORT_C TInt FindAndCreateIconL(
                const TPbk2IconId& aIconId );

        /**
         * Appends new icons from given resource to this array.
         * Doesn't change the if of this array.
         *
         * @param aReader               Resource reader pointed to
         *                              PBK2_ICON_ARRAY resource.
         * @param aIconInfoContainer    Icon infos that are needed in
         *                              icon creation.
         */
        IMPORT_C void AppendIconsFromResourceL(
                TResourceReader& aReader,
                const CPbk2IconInfoContainer& aIconInfoContainer );

        /**
         * Refreshes core Phonebook 2 icons.
         */
        IMPORT_C void RefreshL();

        /**
         * Refreshes the icons from the given container.
         *
         * @param aIconInfoContainer    The container of icons to refresh.
         */
        IMPORT_C void RefreshL(
                const CPbk2IconInfoContainer& aIconInfoContainer );
                
        /**
         * Appends a new icon to this array. Takes ownership of the icon.
         * Doesn't change the if of this array.
         *
         * @param aIcon                 Icon to appended. Ownership of the icon
         *                              is taken.
         */
        IMPORT_C void AppendIconL( 
            CGulIcon* aIcon, 
            const TPbk2IconId& aIconId );
        
        /**
         * Appends a new icon to this array. 
         *
         * @param aIconInfo             Icon to be appended. Ownership of the 
         *                              iconInfo is taken.
         */
        IMPORT_C void AppendIconL(
                CPbk2IconInfo* aIconInfo );

        /**
		 * Removes icon from the CArrayPtrFlat - array and the TPbk2IconId from the iIdMap - array
		 * 
		 * @param aIconId				Icon id to be removed
		 * @return KErrNone if success, KErrNotfound if icon is not found 
		 */
		IMPORT_C TInt RemoveIcon(
				const TPbk2IconId& aIconId );
		
    private: // Implementation
        CPbk2IconArray();
        void BaseConstructL();
        void ConstructL();
        void ConstructL(
                TResourceReader& aReader );
        void ConstructL(
                const MPbk2FieldPropertyArray& aFieldProperties );
        void ConstructL( 
        		TResourceReader& aReader,
        		const CPbk2IconInfoContainer& aIconInfoContainer );
        void AppendDefaultIconsL(
                const CPbk2IconFactory& aIconFactory );
        void DoAppendIconsFromResourceL(
                TResourceReader& aReader,
                const CPbk2IconFactory& aIconFactory,
                TBool aUpdateId );
        void AppendIconWithMappingL(
                const CPbk2IconFactory& aIconFactory,
                const TPbk2IconId& aIconId );
        void ReserveMemoryL(
                TInt aAmount );
        void DoRefreshL(
                const CPbk2IconFactory& aFactory );

    private: // Data
        /// Own: The ID of this icon array
        TPbk2IconArrayId iArrayId;
        /// Own: An array which maps icon Ids to indexes in this array
        class CIdArray;
        CIdArray* iIdMap;
        /// Own: Factory for creating icons
        CPbk2IconFactory* iIconFactory;
    };

#endif // CPBK2ICONARRAY_H

// End of File
