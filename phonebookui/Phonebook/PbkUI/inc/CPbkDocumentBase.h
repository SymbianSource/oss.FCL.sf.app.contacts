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
*     Phonebook document class definition
*
*/


#ifndef __CPbkDocumentBase_H__
#define __CPbkDocumentBase_H__

//  INCLUDES
#include <AknDoc.h>


// FORWARD DECLARATIONS
class CPbkContactEngine;


// CLASS DECLARATION

/**
 * Phonebook application document base class. 
 */
class CPbkDocumentBase :
		public CAknDocument
    {
    public:  // Interface
        /**
         * Returns the engine object.
         */
        virtual CPbkContactEngine* Engine() const =0;

    protected: // Constructors and destructor
        /**
         * Constructor.
		 * @param aApp reference to eik application object
         */
	    IMPORT_C CPbkDocumentBase(CEikApplication& aApp);

        /**
         * Destructor.
         */
	    IMPORT_C ~CPbkDocumentBase();
    };


#endif // __CPbkDocumentBase_H__
            
// End of File
