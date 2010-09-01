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
*    Phonebook vCard property.
*
*/


#ifndef __CPbkVcardProperty_H__
#define __CPbkVcardProperty_H__

//  INCLUDES
#include "MPbkVcardProperty.h"
#include "CPbkVcardParameters.h"

// CLASS DECLARATION

/**
 * Phonebook vCard property.
 */
NONSHARABLE_CLASS(CPbkVcardProperty) : 
        public CBase, public MPbkVcardProperty
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class and intializes it from 
         * resources.
		 * @param aReader resource reference
         */
        static CPbkVcardProperty* NewLC(TResourceReader& aReader);

        /**
         * Destructor.
         */
        ~CPbkVcardProperty();

     public:  // from MPbkVcardProperty
         TUid PropertyName() const;
         const MPbkVcardParameters& PropertyParameters() const;
        
    private:  // Implementation
        CPbkVcardProperty();
        void ConstructL(TResourceReader& aReader);

    private:  // Data
		/// Own: property name
        TUid iName;
		/// Own: property parameters
        CPbkVcardParameters iParameters;
    };


#endif // __CPbkVcardProperty_H__

// End of File
