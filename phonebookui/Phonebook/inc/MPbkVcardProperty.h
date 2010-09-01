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
*    Phonebook vCard property interface.
*
*/


#ifndef __MPbkVcardProperty_H__
#define __MPbkVcardProperty_H__

//  INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class MPbkVcardParameters;

// CLASS DECLARATION

/**
 * Phonebook vCard property interface. 
 */
class MPbkVcardProperty
    {
     public:  // Interface
         /**
          * Returns this vCard property's name.
          */
         virtual TUid PropertyName() const = 0;

         /**
          * Returns the parameters of this vCard property.
          */
         virtual const MPbkVcardParameters& PropertyParameters() const = 0;
        
        /**
         * Destructor.
         */
        virtual ~MPbkVcardProperty() { }
    };


#endif // __MPbkVcardProperty_H__

// End of File
