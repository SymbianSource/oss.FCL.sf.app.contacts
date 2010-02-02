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
* Description:  Manages dynamic properties of the contact
*
*/


#include "CPbk2ContactFieldDynamicProperties.h"

// From VirtualPhonebook
#include <MVPbkBaseContactField.h>
#include <MVPbkFieldType.h>


CPbk2ContactFieldDynamicProperties* CPbk2ContactFieldDynamicProperties::NewLC()
    {
    CPbk2ContactFieldDynamicProperties* self = 
        new (ELeave) CPbk2ContactFieldDynamicProperties();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
CPbk2ContactFieldDynamicProperties::CPbk2ContactFieldDynamicProperties()
    {
    // Do nothing
    }
    
CPbk2ContactFieldDynamicProperties::~CPbk2ContactFieldDynamicProperties()
    {
    iHiddenFieldResourceIds.Close();
    }
    
void CPbk2ContactFieldDynamicProperties::ConstructL()
    {
    // Do nothing
    }
    
TInt CPbk2ContactFieldDynamicProperties::AddHiddenFieldResourceId( TInt aFieldResId )
    {
    TInt result( KErrNone );
    TInt found( iHiddenFieldResourceIds.Find( aFieldResId ) );
    if ( found == KErrNotFound )
        {
        result = iHiddenFieldResourceIds.Append( aFieldResId );        
        }
        
    return result;
    }
    
TBool CPbk2ContactFieldDynamicProperties::IsHiddenField
    ( const MVPbkBaseContactField& aField ) const  
    {
    if ( iHiddenFieldResourceIds.Find( 
            aField.BestMatchingFieldType()->FieldTypeResId() ) != KErrNotFound )
        {
        return ETrue;
        }
    return EFalse;
    }
    
// End of file
