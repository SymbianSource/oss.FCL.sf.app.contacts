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
*     Contact selection interface.
*
*/


#ifndef __MPbkContactSelector_H__
#define __MPbkContactSelector_H__

// INCLUDES
#include <cntdef.h>  // for TContactItemId

// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * Contact selection interface.
 */
class MPbkContactSelector 
	{
    public:
        /**
         * Returns ETrue if contact is included in the set defined by
         * this object.
         *
         * @param aId   contact id.
         * @return ETrue if the contact is included in the set, EFalse if not.
         */
        virtual TBool IsContactIncluded(TContactItemId aId) = 0;
	};


#endif // __MPbkContactSelector_H__

// End of File
