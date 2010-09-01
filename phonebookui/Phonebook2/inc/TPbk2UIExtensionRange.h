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
* Description:  Range that is allocated for a plug-in.
*
*/


#ifndef TPBK2UIEXTENSIONRANGE_H
#define TPBK2UIEXTENSIONRANGE_H

// INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class TResourceReader;

/**
 * Range that is allocated for a plug-in.
 */
struct TPbk2UIExtensionRange
    {
    public: // Construction
        TPbk2UIExtensionRange();

        /**
         * Reads the range from resources.
         * @param aReader Resource reader for reading the range data.
         * @see PHONEBOOK2_EXTENSION_RANGE
         */
        void ReadFromResource(TResourceReader& aReader);

        /**
         * Returns ETrue if the id is included in this range.
         * @param aId Id to check.d
         * @return ETrue if the id is included in this range.
         */
        TBool IsIdIncluded(TInt aId) const;
        
    private: // Data
        /// Own: First Id in this range
        TInt iFirstId;
        /// Own: Last Id in this range
        TInt iLastId;
    };
    
#endif // TPBK2UIEXTENSIONRANGE_H
//End of file
