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

#include "cmsserver.h"
#include "cmsdebug.h"

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
    
    //Write exe name to log file        
    TBuf8<KMaxFileName> exe;
    TFileName name( RProcess().FileName() );
    exe.Copy( name );
    PRINT2( _L( "*** E32Main(): Exe loaded from %S - Log deletion: %d"), &exe, KErrNone );
    
	if( cleanup )
		{
		TRAP( error, CCmsServer::StartThreadL() );
		delete cleanup;
		}
		
    PRINT1( _L( "*** E32Main() completes: %d" ), error );
    
	__UHEAP_MARKEND; 
	return error;
	}

//  End of File  

