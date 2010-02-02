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
*      Helper class utilizing Versit's protected functions.
*
*/


// INCLUDE FILES
#include "CBCardParser.h"

CBCardParser* CBCardParser::NewL()
	{
	CBCardParser* self=new(ELeave) CBCardParser();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}

CBCardParser::CBCardParser() : CVersitParser(ESupportsVersion)
    {
    }

CDesCArray* CBCardParser::MakePropertyValueCDesCArrayL(TPtr16 aStringValue)
    {
    return CVersitParser::MakePropertyValueCDesCArrayL(aStringValue);
    }

// End of File
