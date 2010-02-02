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
*       Class deriving from CParserProperty to access its 
*       protected iArrayOfParams member.
*
*/


// INCLUDE FILES
#include    "CBCardParserProperty.h"


// ==================== MEMBER FUNCTIONS ====================

const CBCardParserProperty& CBCardParserProperty::Cast
        (const CParserProperty& aProperty)
    {
    // The cast below is safe because this class does not have any member data
    // or base classes in addition to CParserProperty.
    return static_cast<const CBCardParserProperty&>(aProperty);
    }

const CArrayPtr<CParserParam>* CBCardParserProperty::ArrayOfParams() const
    {
    return iArrayOfParams;
    }


//  End of File 
