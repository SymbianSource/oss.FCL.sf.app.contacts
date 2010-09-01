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
*     Storage class
*
*/


#ifndef __CPbkEngineLocalStorage_H__
#define __CPbkEngineLocalStorage_H__

// INCLUDE FILES
#include <e32base.h>

// FORWARD DECLARATIONS
class CPbkContactEngine;
class CPbkSINDHandlerInterface;


/**
* This class is used as a store for engine and sindhandler pointers for use
* of Thread's local storage.
*/
NONSHARABLE_CLASS(CPbkEngineLocalStorage): public CBase
    {
    public: // Constructors and destructor
        
        /**
        * Creates phonebook engine local storage
        * @param aPbkContactEngine Reference to phonebook contact engine.
        * @return a new instance of this class.
        */ 
    
        static CPbkEngineLocalStorage* NewL(
            CPbkContactEngine& aPbkContactEngine);
    
        /**
        * Destructor
        */
        ~CPbkEngineLocalStorage();

    private: // Implementation
            
        CPbkEngineLocalStorage(CPbkContactEngine& aEngine);
    

    public:
        /**
        * Engine
        * No ownership is vested in this structure.
        *
        * @since 3.0
        */
        CPbkContactEngine& iEngine;
        
        /**
        * SIND Handler ECOM Plugin implementation.
        * Owns.
        *
        * @since 3.0
        */
        CPbkSINDHandlerInterface* iSINDHandler;
        
        /**        
        *CPbkSINDHandler instance identifier key.
        *
        * @since 3.0
        */
        TUid iDtorIDKey;
    };

#endif // __CPbkEngineLocalStorage_H__

// end of file
