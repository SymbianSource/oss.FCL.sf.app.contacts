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
*       Classes for storing Phonebook contact data in a contact database 
*       independent way.
*
*/


#ifndef __CPbkVcardParameters_H__
#define __CPbkVcardParameters_H__

//  INCLUDES
#include <e32base.h>    // CBase
#include "MPbkVcardParameters.h"

// FORWARD DECLARATIONS
class TResourceReader;


// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * Phonebook vCard parameters.
 */
NONSHARABLE_CLASS(CPbkVcardParameters) : 
        public CBase, 
        public MPbkVcardParameters
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class and initializes it from resources.
		 * @param aReader resource reference
         */
        static CPbkVcardParameters* NewL(TResourceReader& aReader);

        /**
         * Default constructor.
         */
        CPbkVcardParameters();

        /**
         * Second phase constructor.
		 * @param resource reference
         */
        void ConstructL(TResourceReader& aReader);

        /**
         * Destructor.
         */
        ~CPbkVcardParameters();

     public:  // from MPbkVcardParameters
        TInt ParameterCount() const;
        TUid ParameterAt(TInt aIndex) const;
        
    private:  // Data
		/// Own: parameter array
        RArray<TUid> iParameters;
    };


#endif // __CPbkVcardParameters_H__

// End of File
