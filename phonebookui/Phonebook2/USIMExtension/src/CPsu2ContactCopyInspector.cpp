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
* Description:  A contact copy inspector.
*                Handles errors related to SIM contact fields
*
*/


#include "CPsu2ContactCopyInspector.h"

#include <MPbk2ContactNameFormatter.h>

// Virtualphonebook
#include <CVPbkFieldTypeRefsList.h>
#include <MVPbkStoreContact.h>
#include <CVPbkContactFieldIterator.h>
#include <vpbkeng.rsg>  // field type res ids

CPsu2ContactCopyInspector::CPsu2ContactCopyInspector
    ( MPbk2ContactNameFormatter& aNameFormatter )
:   iNameFormatter( aNameFormatter )
    {
    // Do nothing    
    }
    
void CPsu2ContactCopyInspector::ConstructL()
    {
    // Do nothing
    }

CPsu2ContactCopyInspector* CPsu2ContactCopyInspector::NewL
    ( MPbk2ContactNameFormatter& aNameFormatter )
    {
    CPsu2ContactCopyInspector* self = 
        new ( ELeave ) CPsu2ContactCopyInspector( aNameFormatter );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); 
    return self;
    }
    
CPsu2ContactCopyInspector::~CPsu2ContactCopyInspector()
    {
    // Do nothing
    }
    
TBool CPsu2ContactCopyInspector::IsCopiedCompletelyL( 
    MVPbkStoreContact& aSourceContact, 
    RPointerArray<MVPbkStoreContact>& aSimContacts )
    {
    TBool result( EFalse );
    TInt expectedSimCntCount( 0 );
    expectedSimCntCount += ExpectedSimContactCountL( aSourceContact );
    // There should be atleast expectedSimCntCount amount of sim contacts
    if ( aSimContacts.Count() >= expectedSimCntCount )
        {
        result = ETrue;
        }
    
    if ( result )
        {
        result = CheckDataFieldsL( aSourceContact, aSimContacts );    
        }
    return result;
    }    
    
TInt CPsu2ContactCopyInspector::ExpectedSimContactCountL( 
    MVPbkStoreContact& aSourceContact )
    {
    // If in source contact is other fields than titlefields
    // there should be atleast 1 sim contact
    CVPbkFieldTypeRefsList* list = CVPbkFieldTypeRefsList::NewL();
    CleanupStack::PushL( list );
    CVPbkBaseContactFieldTypeListIterator*  iterator = 
        iNameFormatter.ActualTitleFieldsLC( *list, aSourceContact.Fields() );
    TInt activeTitleFields( list->FieldTypeCount() );
    TInt simCount( 0 );
    if ( activeTitleFields > 0 )
        {
        ++simCount;
        }    
        
    CleanupStack::PopAndDestroy( 2, list );    
    return simCount;        
    }    
    
TBool CPsu2ContactCopyInspector::CheckDataFieldsL(
    MVPbkStoreContact& aSourceContact, 
    RPointerArray<MVPbkStoreContact>& aSimContacts )
    {
    // Calculate data fields from source contact.
    // Total count of fields - active title fields - synch field - top contact field
    TInt totalSourceFieldCount( aSourceContact.Fields().FieldCount() );
    // Active title field count
    CVPbkFieldTypeRefsList* list = CVPbkFieldTypeRefsList::NewL();
    CleanupStack::PushL( list );
    CVPbkBaseContactFieldTypeListIterator*  iterator = 
        iNameFormatter.ActualTitleFieldsLC( *list, aSourceContact.Fields() );
    TInt activeSourceTitleFieldCount( list->FieldTypeCount() );
    TBool hasReadingFields ( HasReadingFields( *iterator, aSourceContact ) );
    CleanupStack::PopAndDestroy( 2, list );
        
    TInt sourceFieldCount = totalSourceFieldCount - activeSourceTitleFieldCount;
    for ( TInt i(0); i < totalSourceFieldCount; ++i )
        {
        MVPbkStoreContactField& sourceField = 
            aSourceContact.Fields().FieldAt( i );
        const MVPbkFieldType* bestMatch = sourceField.BestMatchingFieldType();
        if ( bestMatch &&
             ( bestMatch->FieldTypeResId() == R_VPBK_FIELD_TYPE_SYNCCLASS ||
               bestMatch->FieldTypeResId() == R_VPBK_FIELD_TYPE_TOPCONTACT  ))
            {
            --sourceFieldCount;
            }
        }
        
    // Now count copied fields
    // sim contact count * ( all fields - title )    
    TInt simCntCount( aSimContacts.Count() );
    TInt totalSimFieldCount( 0 );
    // If still has reading fields, those reading fields should be copied to
    // one field. And so it increases sim fields by one.
    if ( hasReadingFields )
        {
        ++totalSimFieldCount;
        }
    for ( TInt n = 0; n < simCntCount; ++n )
        {
        MVPbkStoreContact* simCnt = aSimContacts[ n ];
        TInt simCntFieldCount( simCnt->Fields().FieldCount() );
        totalSimFieldCount += simCntFieldCount;
        for ( TInt k = 0; k < simCntFieldCount; ++k )
            {
            MVPbkStoreContactField& simCntField = simCnt->Fields().FieldAt( k );
            if ( iNameFormatter.IsTitleField( simCntField ) )
                {
                --totalSimFieldCount;
                }
            }
        }
        
    return ( sourceFieldCount == totalSimFieldCount );
    }
    
TBool CPsu2ContactCopyInspector::HasReadingFields
    ( CVPbkBaseContactFieldTypeListIterator& aTitleFields, 
      MVPbkStoreContact& aSourceContact )
    {
    // 2. check if there is reading fields in rest of the fields    
    MVPbkStoreContactFieldCollection& fields = aSourceContact.Fields();
    TInt count( aSourceContact.Fields().FieldCount() );
    TBool found( EFalse );
    for( TInt i = 0; i < count && !found; ++i )
        {
        MVPbkStoreContactField& field = fields.FieldAt( i );
        if ( iNameFormatter.IsTitleField( field ) )
            {
            while( aTitleFields.HasNext() )
                {
                const MVPbkBaseContactField* titleField = aTitleFields.Next();
                if ( titleField && titleField->IsSame( field ) )
                    {
                    // field is part of active title fields
                    // check others
                    break;
                    }
                    
                const MVPbkFieldType* bestMatch = field.BestMatchingFieldType();
                TInt typeId = KErrNotFound;
                if ( bestMatch )
                    {
                    typeId = bestMatch->FieldTypeResId();                    
                    }
                    
                if( typeId == R_VPBK_FIELD_TYPE_LASTNAMEREADING ||
                    typeId == R_VPBK_FIELD_TYPE_FIRSTNAMEREADING )
                    {
                    // this is the count of title fields that are part of title
                    found = ETrue;
                    break;
                    }
                }            
            }
        }
      
    return found;        
    } 
    
// End of file
