/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#include "creatorserver.h"
#include "creatorservermain.h"

// ----------------------------------------------------------
// E32Main
// 
// ----------------------------------------------------------
//     
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
    TInt error = KErrNoMemory;
    CTrapCleanup* cleanup = CTrapCleanup::New();
	if( cleanup )
		{
		TRAP( error, CCreatorServer::StartThread() );
		delete cleanup;
		}
	__UHEAP_MARKEND;
    #ifdef _DEBUG
	    CCreatorServer::WriteToLog( _L8( "CreatorServer exits - Error: %d" ), error );
	#endif 
	return error;
	}

//  End of File  
