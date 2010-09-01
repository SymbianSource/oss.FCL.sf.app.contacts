/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


//  CLASS HEADER
#include "T_MVPbkContactStoreMain.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>

//  INTERNAL INCLUDES

T_MVPbkContactStoreMain* T_MVPbkContactStoreMain::NewL()
    {
    T_MVPbkContactStoreMain* self = T_MVPbkContactStoreMain::NewLC();
    CleanupStack::Pop();

    return self;
    }

T_MVPbkContactStoreMain* T_MVPbkContactStoreMain::NewLC()
    {
    T_MVPbkContactStoreMain* self = new( ELeave ) T_MVPbkContactStoreMain();
    CleanupStack::PushL( self );

	self->ConstructL(); 

    return self;
    }

T_MVPbkContactStoreMain::~T_MVPbkContactStoreMain()
    {
    }

T_MVPbkContactStoreMain::T_MVPbkContactStoreMain()
    {
    }

void T_MVPbkContactStoreMain::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuite::ConstructL( _L( "ContactStore tester mainsuite" ) );

    // TODO: Read store uri's from resource file
    }

//  END OF FILE
