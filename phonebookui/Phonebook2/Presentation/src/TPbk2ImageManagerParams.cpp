/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*       Provides methods for generic image loading parameters.
*
*/


// INCLUDE FILES
#include "TPbk2ImageManagerParams.h"

// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// TPbk2ImageManagerParams::TPbk2ImageManagerParams
// --------------------------------------------------------------------------
//
EXPORT_C TPbk2ImageManagerParams::TPbk2ImageManagerParams() 
: 	iSize(0, 0),
 	iFlags(EFlagsNull), 
	iDisplayMode(EColor16M),
	iFrameNumber(0) 
	{
	}

//  End of File  
