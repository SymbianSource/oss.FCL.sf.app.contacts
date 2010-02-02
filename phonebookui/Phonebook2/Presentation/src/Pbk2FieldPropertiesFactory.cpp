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
* Description:  A class for creating phonebook2 field properties
*
*/


#include <Pbk2FieldPropertiesFactory.h>
#include <CPbk2FieldPropertyArray.h>
#include <f32file.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CreateL
// ---------------------------------------------------------------------------
//
EXPORT_C MPbk2FieldPropertyArray* Pbk2FieldPropertiesFactory::CreateL(
        const MVPbkFieldTypeList& aSupportedFieldTypeList, RFs* aRFs )
    {
    RFs fs;
    RFs ownFs;
    if ( !aRFs )
        {
        User::LeaveIfError( ownFs.Connect() );
        CleanupClosePushL( ownFs );
        fs = ownFs;
        }
    else
        {
        fs = *aRFs;
        }
    
    MPbk2FieldPropertyArray* properties = 
        CPbk2FieldPropertyArray::NewL( aSupportedFieldTypeList, fs );
    
    if ( ownFs.Handle() )
        {
        CleanupStack::PopAndDestroy(); // ownFs   
        }
    
    return properties;
    }


// ---------------------------------------------------------------------------
// CreateLC
// ---------------------------------------------------------------------------
//
EXPORT_C MPbk2FieldPropertyArray* Pbk2FieldPropertiesFactory::CreateLC(
        const MVPbkFieldTypeList& aSupportedFieldTypeList, RFs* aRFs )
    {
    MPbk2FieldPropertyArray* result = 
        CreateL( aSupportedFieldTypeList, aRFs );
    CleanupDeletePushL( result );
    return result;
    }
