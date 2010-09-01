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
* Description:  
*
*/


// INCLUDE FILES
#include "CVPbkDefaultAttribute.h"

CVPbkDefaultAttribute::CVPbkDefaultAttribute() :   
        iDefaultType(EVPbkDefaultTypeUndefined)
    {    
    }
    
EXPORT_C CVPbkDefaultAttribute* CVPbkDefaultAttribute::NewL(
        TVPbkDefaultType aType)
    {
    CVPbkDefaultAttribute* self = new(ELeave) CVPbkDefaultAttribute();
    self->SetDefaultType(aType);
    return self;
    }
    
EXPORT_C CVPbkDefaultAttribute* CVPbkDefaultAttribute::NewL()
    {
    CVPbkDefaultAttribute* self = new(ELeave) CVPbkDefaultAttribute();
    return self;
    }    

CVPbkDefaultAttribute::~CVPbkDefaultAttribute()
    {    
    }
    
EXPORT_C void CVPbkDefaultAttribute::SetDefaultType(TVPbkDefaultType aType)
    {
    iDefaultType = aType;
    }
    
EXPORT_C TVPbkDefaultType CVPbkDefaultAttribute::DefaultType() const
    {
    return iDefaultType;
    }
        
TUid CVPbkDefaultAttribute::AttributeType() const
    {
    return CVPbkDefaultAttribute::Uid();
    }
    
MVPbkContactAttribute* CVPbkDefaultAttribute::CloneLC() const
    {
    CVPbkDefaultAttribute* clone = 
        CVPbkDefaultAttribute::NewL(DefaultType());
    CleanupStack::PushL(clone);
    return clone;    
    }

// End of file

