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
*       Dummy version of deprecated class CPbkPhotoCardControl for PbkView.dll 
*       BC maintenance on Series60 2.0. 
*       NEVER put functions in public APIs to this file.
*
*/


// INCLUDES
#include <cntdef.h>

// FORWARD DECLARATIONS
class CCoeControl;
class CPbkContactEngine;
class CPbkViewState;


/**
 * Dummy class for BC maintenance.
 * @since 2.0
 */
NONSHARABLE_CLASS(CPbkPhotoCardControl)
    {
public:
    IMPORT_C static CPbkPhotoCardControl* NewL(const CCoeControl* aParent, CPbkContactEngine* aEngine, TContactItemId aContactId);
    IMPORT_C TContactItemId ContactId() const;
    IMPORT_C CPbkViewState* GetStateLC() const;
    IMPORT_C void UpdateL(TContactItemId aContactId);
    IMPORT_C TBool IsPicture() const;
    };

EXPORT_C CPbkPhotoCardControl* CPbkPhotoCardControl::NewL(const CCoeControl* /*aParent*/, CPbkContactEngine* /*aEngine*/, TContactItemId /*aContactId*/)
    { 
    return NULL;
    }

EXPORT_C TContactItemId CPbkPhotoCardControl::ContactId() const
    { 
    return KNullContactId;
    }

EXPORT_C CPbkViewState* CPbkPhotoCardControl::GetStateLC() const
    { 
    return NULL;
    }

EXPORT_C void CPbkPhotoCardControl::UpdateL(TContactItemId /*aContactId*/)
    { 
    }

EXPORT_C TBool CPbkPhotoCardControl::IsPicture() const
    { 
    return EFalse;
    }

// End of File
