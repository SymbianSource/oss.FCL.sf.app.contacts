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
* Description:  Virtual Phonebook VCard attribute handler
*
*/


#include "CVPbkVCardAttributeHandler.h"
#include <CVPbkDefaultAttribute.h>


CVPbkVCardAttributeHandler::CVPbkVCardAttributeHandler()
    {
    // do nothing
    }

CVPbkVCardAttributeHandler* CVPbkVCardAttributeHandler::NewL()
    {
    CVPbkVCardAttributeHandler* self = new (ELeave) CVPbkVCardAttributeHandler();
    return self;
    }
    
CVPbkVCardAttributeHandler::~CVPbkVCardAttributeHandler()
    {
    delete iAttribute;
    }    
    
TBool CVPbkVCardAttributeHandler::CreateAttributeL(TVPbkFieldTypeParameter aParameter)
    {
    TBool retval(EFalse);
    delete iAttribute;
    iAttribute = NULL;
    if (aParameter == EVPbkVersitParamPREF)
        {
        iAttribute = CVPbkDefaultAttribute::NewL(EVPbkDefaultTypePhoneNumber);
        retval = ETrue;
        }
    return retval;
    }
    
MVPbkContactAttribute* CVPbkVCardAttributeHandler::Attribute() const
    {
    return iAttribute;
    }
    
// End of file
