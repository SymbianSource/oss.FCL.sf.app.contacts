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
*       Class for storing pointers to CParserParam objects. The reason for
*       this class is the usage of cleanupstack; destructor deletes the
*       instances of the pointers.     
*
*/


// INCLUDE FILES
#include    "CBCardParserParamArray.h"
#include    <vcard.h>


// ==================== MEMBER FUNCTIONS ====================

CBCardParserParamArray::~CBCardParserParamArray()
    {
    // Free contents of this array for leave-safety
    ResetAndDestroy();
    }

CBCardParserParamArray::CBCardParserParamArray(TInt aGranurality)
    : CArrayPtrFlat<CParserParam>(aGranurality)
    {
    }

//  End of File 
