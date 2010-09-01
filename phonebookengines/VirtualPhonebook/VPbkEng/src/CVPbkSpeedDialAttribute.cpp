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
* Description:  An attribute class for contact speed dialing.
*
*/


#include "CVPbkSpeedDialAttribute.h"

CVPbkSpeedDialAttribute::CVPbkSpeedDialAttribute() :
    iIndex(KSpeedDialIndexNotDefined)
    {
    }
    
EXPORT_C CVPbkSpeedDialAttribute* CVPbkSpeedDialAttribute::NewL(TInt aIndex)
    {
    CVPbkSpeedDialAttribute* self = new(ELeave) CVPbkSpeedDialAttribute();
    self->SetIndex(aIndex);
    return self;
    }

EXPORT_C CVPbkSpeedDialAttribute* CVPbkSpeedDialAttribute::NewL()
    {
    CVPbkSpeedDialAttribute* self = new(ELeave) CVPbkSpeedDialAttribute();
    return self;
    }
    
CVPbkSpeedDialAttribute::~CVPbkSpeedDialAttribute()
    {
    }
    
TUid CVPbkSpeedDialAttribute::AttributeType() const
    {
    return CVPbkSpeedDialAttribute::Uid();
    }

MVPbkContactAttribute* CVPbkSpeedDialAttribute::CloneLC() const
    {
    CVPbkSpeedDialAttribute* clone = new(ELeave) CVPbkSpeedDialAttribute;
    CleanupStack::PushL(clone);

    clone->SetIndex(Index());

    return clone;
    }
    
EXPORT_C void CVPbkSpeedDialAttribute::SetIndex(TInt aIndex)
    {
    iIndex = aIndex;
    }
    
EXPORT_C TInt CVPbkSpeedDialAttribute::Index() const
    {
    return iIndex;
    }

// End of File
