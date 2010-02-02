/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook engine local storage
*
*/


// System includes
#include <ecom/ecom.h>
#include <CPbkSINDHAndlerInterface.h>

// User includes
#include "cpbkenginelocalstorage.h"

// ======== MEMBER FUNCTIONS ========

CPbkEngineLocalStorage::CPbkEngineLocalStorage(
    CPbkContactEngine& aPbkContactEngine): iEngine(aPbkContactEngine)
    {
    }

CPbkEngineLocalStorage* CPbkEngineLocalStorage::NewL(
    CPbkContactEngine&  aPbkContactEngine)
    {
    return new( ELeave ) CPbkEngineLocalStorage(aPbkContactEngine);    
    }

CPbkEngineLocalStorage::~CPbkEngineLocalStorage()
    {    
    if (iSINDHandler)
        {
        REComSession::DestroyedImplementation( 
            iDtorIDKey );
        delete iSINDHandler;     
        }            
    }

