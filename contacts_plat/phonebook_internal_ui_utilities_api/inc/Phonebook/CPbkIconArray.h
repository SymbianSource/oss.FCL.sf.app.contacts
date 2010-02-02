/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*     Icon array helper class
*
*/


#ifndef __PBKICONARRAY_H__
#define __PBKICONARRAY_H__

//  INCLUDES
#include <e32base.h>        // CBase
#include <PbkIconId.hrh>    // TPbkIconId

//  FORWARD DECLARATIONS
class CGulIcon;
class TResourceReader;
class CPbkIconInfoContainer;


// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * Icon array helper class.
 */
class CPbkIconArray : public CArrayPtrFlat<CGulIcon>
    {
    public:  // Constructors and destructor
        /**
         * C++ Constructor.
		 * @param aGranurality array granurality
         */
        IMPORT_C CPbkIconArray(TInt aGranurality);

        /**
         * Destructor.
         */
        IMPORT_C ~CPbkIconArray();

        /**
         * Second phase constructor: loads contents of array from resources.
         *
         * @param aResId    id of a AKN_ICON_ARRAY or a 
         *                  AKN_ICON_ARRAY_CONTAINER resource.
         */
        IMPORT_C void ConstructFromResourceL(TInt aResId);

        /**
         * Second phase constructor: loads contents of array from resources.
         *
         * @param aReader   resource reader pointed to a 
         *                  AKN_ICON_ARRAY or a AKN_ICON_ARRAY_CONTAINER resource.
         */
        IMPORT_C void ConstructFromResourceL(TResourceReader& aReader);

    public:  // interface
        /**
         * Returns index of icon with id aIconId in this array, KErrNotFound 
         * if not found.
         */
        IMPORT_C TInt FindIcon(TPbkIconId aIconId) const;

        /**
         * Appends new icons from resource to this array.
         *
         * @param aArrayIconInfoId new icons info
         * @param aArrayIconId actual icon array to be added
         */
        IMPORT_C void AppendIconsFromResourceL
                (TInt aArrayIconInfoId, TInt aArrayIconId);

        /**
         * Refreshes the icons that are in this list and whose info can
         * be found from given icon info resource. Other icons are left
         * as they are.
         * @param aArrayIconInfoId Icon info to look for icons to be refreshed.
         */
        IMPORT_C void RefreshL(TInt aArrayIconInfoId);

    private:  // Implementation
        void ReadFromResourceL(TResourceReader& aReader, CPbkIconInfoContainer& aIconInfoContainer);

    private:  // Data
        /// Own: array which maps icon Ids to indexes in this array
        CArrayFixFlat<TPbkIconId> iIdMap;
    };

#endif // __PBKICONARRAY_H__

// End of File
