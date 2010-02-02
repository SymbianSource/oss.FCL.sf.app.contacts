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
*    Plug-in interface for SIND Handler
*
*/


#ifndef __CPBKSINDHANDLERINTERFACE_H__
#define __CPBKSINDHANDLERINTERFACE_H__

// INCLUDES
#include <e32base.h>
#include <ecom/ecom.h>
#include <cntdef.h> // TContactItemId

// CONSTANTS
const TUid KPbkSINDHandlerInterfaceUid = { 0x101f84fd };

// FORWARD DECLARATIONS
class TResourceReader;
class CPbkFieldsInfo;
class TPbkContactItemField;


// CLASS DECLARATION

/**
 * @internal This interface is internal to Phonebook.
 *
 * Plug-in interface for SIND handler extension.
 */
class CPbkSINDHandlerInterface : public CBase
    {
    public: // Constructor and destructor
        /**
         * Constructor. 
         * @param[in] aResolutionData eCom resolution data.         
         */
        static CPbkSINDHandlerInterface* NewLC(const TDesC8& aResolutionData);
        
        /**
         * Destructor.
         */
        ~CPbkSINDHandlerInterface();

    public: // Interface
    
        /**
         * Returns the voice tag field index for given contact.
         *
         * @return  voice tag field index for given contact, 
         *          KErrGeneral on failure.
         */
		virtual TInt VoiceTagField(TContactItemId aContactId) = 0;
		
    protected: // Implementation
        CPbkSINDHandlerInterface();

    private:  // Data
        /// Own: destructor id key
        TUid iDtor_ID_Key;
    };

// INLINE FUNCTIONS

inline CPbkSINDHandlerInterface* CPbkSINDHandlerInterface::NewLC
		(const TDesC8& aResolutionData)
	{
    TEComResolverParams resolverParams;
    resolverParams.SetDataType(aResolutionData);
    TAny* ptr = REComSession::CreateImplementationL
        (KPbkSINDHandlerInterfaceUid,
        _FOFF(CPbkSINDHandlerInterface,iDtor_ID_Key),
        resolverParams);
    CPbkSINDHandlerInterface* impl = 
        static_cast<CPbkSINDHandlerInterface*>(ptr);
    CleanupStack::PushL(impl);
    return impl;
	}

inline CPbkSINDHandlerInterface::~CPbkSINDHandlerInterface()
    {
    REComSession::DestroyedImplementation(iDtor_ID_Key);
    }

inline CPbkSINDHandlerInterface::CPbkSINDHandlerInterface()
    {
    }

#endif // __CPBKSINDHANDLERINTERFACE_H__

// End of File
