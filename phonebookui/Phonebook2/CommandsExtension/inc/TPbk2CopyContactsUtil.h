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
* Description:  Phonebook 2 copy contacts result diplay utility.
*
*/


#ifndef TPBK2COPYCONTACTSUTIL_H
#define TPBK2COPYCONTACTSUTIL_H

//  INCLUDES
#include <e32std.h>

// FORWARD DECLARATION
class MPbk2ContactUiControl;
class CVPbkContactStoreUriArray;

// CLASS DECLARATION

/**
 * Phonebook 2 copy contacts result diplay utility.
 */
class TPbk2CopyContactsUtil
    {
    public: // Interface

        /**
         * Analyzes UI control selection and return the possible
         * target store URIs.
         *
         * @param aControl          UI control.
         * @param aDiscardedUris    URIs to discard, for example the URIs
         *                          of stores that failed to open. Can be
         *                          NULL.
         * @return  Array of target stores.
         */
        static CVPbkContactStoreUriArray* CopyContactTargetStoresLC(
                MPbk2ContactUiControl& aControl,
                CVPbkContactStoreUriArray* aDiscardedUris );
    };

#endif // TPBK2COPYCONTACTSUTIL_H

// End of File
