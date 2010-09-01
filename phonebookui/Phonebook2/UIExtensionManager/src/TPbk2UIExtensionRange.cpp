/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Range that is allocated for a plug-in.
*
*/


#include "TPbk2UIExtensionRange.h"
#include <barsread.h>

#include <Pbk2Debug.h>

TPbk2UIExtensionRange::TPbk2UIExtensionRange() :
    iFirstId(0),
    iLastId(0)
    {
    }

void TPbk2UIExtensionRange::ReadFromResource(TResourceReader& aReader)
    {
    iFirstId = aReader.ReadInt16();
    iLastId = aReader.ReadInt16();
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "TPbk2UIExtensionRange::ReadFromResource, firstId %d,lastId %d"),
            iFirstId, iLastId );
    }

TBool TPbk2UIExtensionRange::IsIdIncluded(TInt aId) const
    {
    return (iFirstId <= aId && aId <= iLastId);
    }

// End of file
