/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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


// INCLUDE FILES
#include    "CVPbkVCardParserParamArray.h"
#include    <vcard.h>


// ==================== MEMBER FUNCTIONS ====================

CVPbkVCardParserParamArray::~CVPbkVCardParserParamArray()
    {
    // Free contents of this array for leave-safety
    ResetAndDestroy();
    }

CVPbkVCardParserParamArray::CVPbkVCardParserParamArray(TInt aGranularity)
    : CArrayPtrFlat<CParserParam>(aGranularity)
    {
    }

//  End of File 
