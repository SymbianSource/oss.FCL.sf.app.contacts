/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 extra drive checker
*
*/


#ifndef TPBK2MULTIDRIVEAVAILABLE_H
#define TPBK2MULTIDRIVEAVAILABLE_H

//  INCLUDES
#include <f32file.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * Phonebook 2 contact editor parameters.
 */
class TPbk2MultiDriveAvailable
    {

    public: 

        /**
         * Are there any extra drive available
         *
         * @param aFs Open file server session   
         */
        static TBool MultiDriveAvailable( RFs& aFs );

    };

#endif // TPBK2MULTIDRIVEAVAILABLE_H

// End of File
