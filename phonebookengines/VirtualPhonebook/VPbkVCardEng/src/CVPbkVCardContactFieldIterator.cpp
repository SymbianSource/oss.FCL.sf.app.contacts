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
* Description:  Iterates contact field's fieldtype and data parsed from 
*                CParserProperty.
*
*/

#include "CVPbkVCardContactFieldIterator.h"
#include "CVPbkVCardData.h"
#include "TVPbkVCardDataConverter.h"

#include <TVPbkFieldVersitProperty.h>
#include <MVPbkFieldType.h>

CVPbkVCardContactFieldIterator* CVPbkVCardContactFieldIterator::NewLC( 
    CParserProperty& aProperty, CVPbkVCardData& aData )
    {
    CVPbkVCardContactFieldIterator* self = 
        new ( ELeave ) CVPbkVCardContactFieldIterator( aProperty, aData );
    CleanupStack::PushL( self );
    return self;
    }
 
CVPbkVCardContactFieldIterator::CVPbkVCardContactFieldIterator( 
    CParserProperty& aProperty, CVPbkVCardData& aData )
: iProperty ( aProperty ), iData( aData ), iCursor( -1 )
    {    
    }
    
CVPbkVCardContactFieldIterator::~CVPbkVCardContactFieldIterator()
    {
    iFieldTypes.Close();
    iVCardFieldTypes.Close();
    iVCardSubFieldTypes.Close();
    }
    
void CVPbkVCardContactFieldIterator::AppendVersitProperty( 
        TVPbkFieldVersitProperty aVersitProperty )
    {
    const MVPbkFieldType* fieldType = MatchFieldType( aVersitProperty );
    if ( fieldType )
        {
        iFieldTypes.Append( fieldType );
        iVCardFieldTypes.Append( aVersitProperty.Name() );
        iVCardSubFieldTypes.Append( aVersitProperty.SubField() );
        }
    }
    
void CVPbkVCardContactFieldIterator::AppendAttribute( MVPbkContactAttribute* aAttribute )
    {
    iAttribute = aAttribute;
    }    
    
const MVPbkFieldType* CVPbkVCardContactFieldIterator::MatchFieldType
    ( TVPbkFieldVersitProperty& aProperty )
    {
    TInt maxMatchPriority = iData.SupportedFieldTypes().MaxMatchPriority();
    const MVPbkFieldType* fieldType = NULL;
    for ( TInt i = 0; i <= maxMatchPriority; ++i )
        {
        fieldType = iData.SupportedFieldTypes().FindMatch( aProperty, i );
        if ( fieldType )
            {
            break;
            }
        }
    return fieldType;    
    }
    
TBool CVPbkVCardContactFieldIterator::HasNext()
    {
    TBool hasNext( EFalse );
    if ( ( iFieldTypes.Count() - 1 ) > iCursor )
        {
        hasNext = ETrue;
        }
    return hasNext;
    }
    
CVPbkVCardContactFieldData* CVPbkVCardContactFieldIterator::NextLC()
    {    
    ++iCursor;    
    return CurrentLC();
    }
 
CVPbkVCardContactFieldData* CVPbkVCardContactFieldIterator::CurrentLC()
    {
    TVPbkVCardDataConverter converter;
    CVPbkVCardContactFieldData* data = 
        converter.CreateDataLC( iProperty, iFieldTypes, iAttribute, iCursor );    
    return data;
    }

TInt CVPbkVCardContactFieldIterator::FindVCardFieldL( 
        TVPbkFieldTypeName aVCardFieldType, 
        TVPbkSubFieldType aVCardSubFieldType )
    {
    TInt id = KErrNotFound;
    for ( TInt i=0; i<iVCardFieldTypes.Count(); i++ )
        {
        if ( iVCardFieldTypes[i] == aVCardFieldType &&
             iVCardSubFieldTypes[i] == aVCardSubFieldType )
            {
            id = iFieldTypes[i]->FieldTypeResId();
            break;
            }
        }
    return id;
    }
    
// End of file
