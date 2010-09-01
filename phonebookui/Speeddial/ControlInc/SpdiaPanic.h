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
* Description:     Panic functions.
*
*/






#ifndef SPDIAPANIC_H
#define SPDIAPANIC_H

// INCLUDES
#include <e32base.h>

// DATA TYPE
enum TSpdiaPanic
    {
    ESdmPanicNotSupported,
    ESdmPanicPbkError,
    ESdmPanicPbkThumbnailGetComplete
    };

// FUNCTION PROTOTYPES
GLREF_C void Panic(TSpdiaPanic aPanic);

#endif  // SPDIAPANIC_H
// End of file
