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
*    Wrapper for selectable items.
*
*/


#ifndef __TPbkAddItemWrapper_H__
#define __TPbkAddItemWrapper_H__

//  INCLUDES
#include <e32base.h>        // CBase
#include "PbkFields.hrh"    // TPbkAddItemOrdering
#include "PbkIconId.hrh"    // TPbkIconId

// FORWARD DECLARATIONS
class CPbkFieldInfo;
class CPbkFieldInfoGroup;

// CLASS DECLARATION

/**
 * Wrapper for selectable items in selecting fields 
 * to add in CPbkMemoryEntryAddItemDlg.
 */
NONSHARABLE_CLASS(TPbkAddItemWrapper)
    {
    public:  // Constructors and destructor        
        /**
         * Constructor for creating a wrapper for CPbkFieldInfo aFieldInfo.
         */
        TPbkAddItemWrapper(const CPbkFieldInfo& aFieldInfo);
        /**
         * Constructor for creating a wrapper for CPbkFieldInfoGroup aInfoGroup.
         */
        TPbkAddItemWrapper(const CPbkFieldInfoGroup& aInfoGroup);
         
        /**
         * Destructor.
         */
        ~TPbkAddItemWrapper();

    public:  // implementation
        /**
         * Returns the label of the instance.
         */
        const TDesC& Label() const;        
        /**
         * Returns the icon id of the instance.
         */
        TPbkIconId IconId() const;        
        /**
         * Returns the add item ordering value of the instance.
         */
        TPbkAddItemOrdering AddItemOrdering() const;
        /**
         * Returns the count of CPbkFieldInfos that this instance
         * contains; one if this wrapps a aFieldInfo, otherwise
         * the count of CPbkFieldInfos in a wrapped CPbkFieldInfoGroup.
         */
        TInt FieldInfoCount() const;        
        /**
         * Returns the field info at gíven index. If CPbkFieldInfo is wrapped
         * returns always that regardless of the parameter.
         * @param aIndex Index for the desired field info. Meanigful only
         *        if CPbkFieldInfoGroup is wrapped
         */
        CPbkFieldInfo* FieldInfoAt(const TInt aIndex) const;
                       
    private:  // Data
		/// Ref: field info object
        const CPbkFieldInfo*        iFieldInfo;
		/// Ref: field info group object
        const CPbkFieldInfoGroup*   iInfoGroup;
    };

#endif // __TPbkAddItemWrapper_H__

// End of File
