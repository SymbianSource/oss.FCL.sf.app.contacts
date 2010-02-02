/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Application creation globals
*
*/


#include "ccaappapplication.h"
#include <eikstart.h>

// ---------------------------------------------------------------------------
// Constructs and returns an application object.
// ---------------------------------------------------------------------------
//
CApaApplication* NewApplication()
	{
	return new CCCAAppApplication;
	}

// ---------------------------------------------------------------------------
// Main function of the application executable.
// ---------------------------------------------------------------------------
//
TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}

