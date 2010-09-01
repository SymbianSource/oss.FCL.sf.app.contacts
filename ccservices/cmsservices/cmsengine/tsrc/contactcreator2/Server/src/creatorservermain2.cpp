/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

#include "creatorserver2.h"
// #include "creatorservermain2.h"

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
		TRAP( error, CCreatorServer2::StartThread() );
		delete cleanup;
		}
	__UHEAP_MARKEND;
    #ifdef _DEBUG
	    CCreatorServer2::WriteToLog( _L8( "CreatorServer2 exits - Error: %d" ), error );
	#endif 
	return error;
	}

//  End of File  

