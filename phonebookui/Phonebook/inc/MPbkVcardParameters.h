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
*    Phonebook vCard parameters interface.
*
*/


#ifndef __MPbkVcardParameters_H__
#define __MPbkVcardParameters_H__

// CLASS DECLARATION

/**
 * Phonebook vCard parameters interface.
 */
class MPbkVcardParameters
    {
     public:  // Interface
        /**
         * Returns the number of vCard parameters
         */
        virtual TInt ParameterCount() const = 0;

        /**
         * Returns the aIndex:th vCard parameter.
         */
        virtual TUid ParameterAt(TInt aIndex) const = 0;
        
        /**
         * Destructor.
         */
        virtual ~MPbkVcardParameters() { }
    };

#endif // __MPbkVcardParameters_H__

// End of File
