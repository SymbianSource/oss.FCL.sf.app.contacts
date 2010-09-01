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
*       Wrapper for selectable fields.
*
*/


// INCLUDE FILES

#include "TPbk2AddItemWrapper.h"

// Phonebook 2
#include <MPbk2FieldProperty.h>
#include <CPbk2FieldPropertyGroup.h>

namespace
    {
    #ifdef _DEBUG
        enum TPanicCode
            {
            EPanic_PropertyAt_OOB = 1
            };
            
        void Panic( TPanicCode aPanic )
            {
            _LIT( KPanicCat, "TPbk2AddItemWrapper" );
            User::Panic( KPanicCat(), aPanic );
            }
    #endif // _DEBUG
    }

// ========================== MEBER FUNCTIONS ===============================

EXPORT_C TPbk2AddItemWrapper::TPbk2AddItemWrapper
        ( const MPbk2FieldProperty& aFieldProperty ) :
        iFieldProperty(&aFieldProperty), iPropertyGroup(NULL)
    {
    }

EXPORT_C TPbk2AddItemWrapper::TPbk2AddItemWrapper
        ( const CPbk2FieldPropertyGroup& aPropertyGroup ) :
	    iFieldProperty(NULL), iPropertyGroup(&aPropertyGroup)
    {
    }

EXPORT_C TInt TPbk2AddItemWrapper::PropertyCount() const
    {
    if (iFieldProperty)
        {
        const TInt oneProperty = 1;
        return oneProperty;
        }
    else
        {
        return iPropertyGroup->Count();
        }
    }

EXPORT_C const MPbk2FieldProperty& TPbk2AddItemWrapper::PropertyAt(
    TInt aIndex) const
    {
    if (iFieldProperty)
        {
        return *iFieldProperty;
        }
    else
        {
        __ASSERT_DEBUG( iPropertyGroup->Count() > aIndex,
            Panic( EPanic_PropertyAt_OOB ) );
        return iPropertyGroup->At(aIndex);
        }
    }

const TDesC& TPbk2AddItemWrapper::Label() const
    {
    if (iFieldProperty)
        {
        return iFieldProperty->AddItemText();
        }
    else
        {
        return iPropertyGroup->Label();
        }
    }

const TPbk2IconId& TPbk2AddItemWrapper::IconId() const
    {
    if (iFieldProperty)
        {
        return iFieldProperty->IconId();
        }
    else
        {
        return iPropertyGroup->IconId();        
        }    
    }

TInt TPbk2AddItemWrapper::AddItemOrdering() const
    {
    if (iFieldProperty)
        {
        return iFieldProperty->AddItemOrdering();
        }
    else
        {
        return iPropertyGroup->AddItemOrdering();        
        }            
    }

const CPbk2FieldPropertyGroup* TPbk2AddItemWrapper::Group() const
    {
    return iPropertyGroup;
    }

// End of File
