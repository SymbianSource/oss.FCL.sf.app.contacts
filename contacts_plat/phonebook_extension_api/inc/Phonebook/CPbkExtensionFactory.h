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
*    Phonebook extension factory ECom interface
*
*/


#ifndef __CPbkExtensionFactory_H__
#define __CPbkExtensionFactory_H__

#include <ecom/ecom.h>
#include <mpbkextensionfactory.h>

class CPbkExtensionFactory : public CBase,
                             public MPbkExtensionFactory
    {
    public:
        static CPbkExtensionFactory* NewL(TUid aUid);
        ~CPbkExtensionFactory();
    private:
        TUid iDtorIDKey;
    };

inline CPbkExtensionFactory* CPbkExtensionFactory::NewL(TUid aUid)
    {
    TAny* ptr = NULL;
    ptr = REComSession::CreateImplementationL
        (aUid, _FOFF(CPbkExtensionFactory, iDtorIDKey));

    return reinterpret_cast<CPbkExtensionFactory*>(ptr);
    }    

inline CPbkExtensionFactory::~CPbkExtensionFactory()
    {
    REComSession::DestroyedImplementation(iDtorIDKey);
    }

#endif // __CPbkExtensionFactory_H__

// End of File
