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
* Description:  Virtual Phonebook Util
*
*/


#ifndef VPBKUTIL_H
#define VPBKUTIL_H

// INCLUDES
#include <e32base.h>
#include <badesca.h>

/**
 * Virtual Phonebook internal utility classes and functions.
 */
namespace VPbkEngUtils
    {

    /**
     * Helper class for shared Versit extension string storage.
     * Reference counted to let different CVPbkFieldTypeList instances
     * share the same store. Reference counting is needed when the field
     * type lists are cloned.
     */
    class CTextStore : public CBase
        {
        public:
            IMPORT_C CTextStore();
            IMPORT_C TPtrC8 AddL(const TDesC8& aText);

        private:
            CDesC8ArrayFlat iTextArray;
        };

    }

#endif  // VPBKUTIL_H
//End of file

