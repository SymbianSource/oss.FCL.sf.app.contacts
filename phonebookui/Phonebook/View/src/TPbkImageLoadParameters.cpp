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
*       Provides methods for generic image loading parameters.
*
*/


// INCLUDE FILES
#include "TPbkImageLoadParameters.h"

// ================= MEMBER FUNCTIONS =======================

EXPORT_C TPbkImageLoadParameters::TPbkImageLoadParameters() :
    iFlags(EFlagsNull),
    iSize(0,0), 
    iDisplayMode(EColor16M),
    iFrameNumber(0)
    {
    }


//  End of File  
