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
*    
*
*/


#ifndef __PbkFieldInfoWrite_H__
#define __PbkFieldInfoWrite_H__

//  INCLUDES
#include <e32std.h>
#include "CPbkFieldInfo.h"
#include "CPbkFieldInfoGroup.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 */
class PbkFieldInfoWrite
    {
    public: // Interface
        static void SetOrderingItem
            (CPbkFieldInfo& aFieldInfo, TPbkOrderingItem aOrderingItem);

        static void SetAddItemOrdering
            (CPbkFieldInfo& aFieldInfo, TPbkAddItemOrdering aAddItemOrdering);

    private: // Implementation
        /**
         * Hidden constructor to prevent intantiation of this class.
         */
        PbkFieldInfoWrite();
    };

/**
 * @internal Only Phonebook internal use supported!
 *
 */
class PbkFieldInfoGroupWrite
    {
    public: // Interface
        static void SetAddItemOrdering
            (CPbkFieldInfoGroup& aGroup, TPbkAddItemOrdering aAddItemOrdering);

    private: // Implementation
        /**
         * Hidden constructor to prevent intantiation of this class.
         */
        PbkFieldInfoGroupWrite();
    };

inline void PbkFieldInfoWrite::SetOrderingItem
        (CPbkFieldInfo& aFieldInfo, TPbkOrderingItem aOrderingItem)
    {
    aFieldInfo.iOrderingItem = aOrderingItem;
    }

inline void PbkFieldInfoWrite::SetAddItemOrdering
        (CPbkFieldInfo& aFieldInfo, TPbkAddItemOrdering aAddItemOrdering)
    {
    aFieldInfo.iAddItemOrdering = aAddItemOrdering;
    }

inline void PbkFieldInfoGroupWrite::SetAddItemOrdering
        (CPbkFieldInfoGroup& aGroup, TPbkAddItemOrdering aAddItemOrdering)
    {
    aGroup.iAddItemOrdering = aAddItemOrdering;
    }

#endif // __PbkFieldInfoWrite_H__

// End of File
