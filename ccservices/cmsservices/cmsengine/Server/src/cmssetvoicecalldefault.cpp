/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of auto setting default for voice call
*
*/

#include <mvpbkcontactstoreproperties.h>
#include <MVPbkContactStore.h>
#include <MVPbkStoreContact.h>

#include <CVPbkContactManager.h>
#include <CVPbkDefaultAttribute.h>
#include <CVPbkFieldTypeSelector.h>
#include <VPbkContactStoreUris.h>
#include <MVPbkFieldType.h>
#include <CVPbkContactFieldIterator.h>
#include <VPbkFieldTypeSelectorFactory.h>
#include <VPbkEng.rsg>

#include "cmssetvoicecalldefault.h"

// ---------------------------------------------------------------------------
// CmsSetVoiceCallDefault::CmsSetVoiceCallDefault
// ---------------------------------------------------------------------------
//
CmsSetVoiceCallDefault::CmsSetVoiceCallDefault()
    {
    
    }
    
// ---------------------------------------------------------------------------
// CmsSetVoiceCallDefault::~CmsSetVoiceCallDefault
// ---------------------------------------------------------------------------
//
CmsSetVoiceCallDefault::~CmsSetVoiceCallDefault()
	{
	delete iSetAttributeOperation;	
	iFieldIndexArray.Close();
	}

// ---------------------------------------------------------------------------
// CmsSetVoiceCallDefault::NewL
// ---------------------------------------------------------------------------
//
CmsSetVoiceCallDefault* CmsSetVoiceCallDefault::NewL()
    {
    CmsSetVoiceCallDefault* self =
        new( ELeave ) CmsSetVoiceCallDefault();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
	}

// ---------------------------------------------------------------------------
// CmsSetVoiceCallDefault::ConstructL
// ---------------------------------------------------------------------------
//
void CmsSetVoiceCallDefault::ConstructL()
    {  

    }

// --------------------------------------------------------------------------
// CmsSetVoiceCallDefault::AttributeProcessCompleted
// --------------------------------------------------------------------------
//
void CmsSetVoiceCallDefault::AttributeOperationComplete( MVPbkContactOperationBase& aOperation )
    {
    TInt err = KErrNone;

    if ( &aOperation == iSetAttributeOperation )
        {
        TRAP( err, DefaultSettingCompleteL() );
        }
            
    // Handle error
    if ( err != KErrNone )
         {
         ProcessFinished( err );
         }
    }

// --------------------------------------------------------------------------
// CmsSetVoiceCallDefault::AttributeProcessFailed
// --------------------------------------------------------------------------
//
void CmsSetVoiceCallDefault::AttributeOperationFailed(
        MVPbkContactOperationBase& aOperation,
        TInt aError )
    {
    ProcessFinished( aError );
    } 

// --------------------------------------------------------------------------
// CmsSetVoiceCallDefault::ContactOperationCompleted
// --------------------------------------------------------------------------
//
void CmsSetVoiceCallDefault::ContactOperationCompleted
        (TContactOpResult aResult)
    {
    if (aResult.iOpCode == EContactLock)
        {
        TRAPD( err, ToSetVoiceCallDefaultL() );
        if ( err != KErrNone )
            {
            ProcessFinished( err );
            }
        }
    else if (aResult.iOpCode == EContactCommit)
        {      
        ProcessFinished( KErrNone );
        }
    }

// --------------------------------------------------------------------------
// CmsSetVoiceCallDefault::ContactOperationFailed
// --------------------------------------------------------------------------
//
void CmsSetVoiceCallDefault::ContactOperationFailed
        ( TContactOp /*aOpCode*/, TInt aErrorCode,
          TBool /*aErrorNotified*/ )
    {
    ProcessFinished( aErrorCode );
    }

// ----------------------------------------------------------
// CmsSetVoiceCallDefault::LockContactL
// ----------------------------------------------------------
//
void CmsSetVoiceCallDefault::LockContactL()
    {
    // Lock the contact before changing it
    iStoreContact->LockL(*this);
    }

// --------------------------------------------------------------------------
// CmsSetVoiceCallDefault::DoSetDefaultL
// --------------------------------------------------------------------------
//
void CmsSetVoiceCallDefault::DoSetDefaultL( TVPbkDefaultType aDefaultType, TInt aIndex )
    {
    CVPbkDefaultAttribute* attr =
        CVPbkDefaultAttribute::NewL( aDefaultType );
    CleanupStack::PushL( attr );

    iSetAttributeOperation =
    iContactManager->ContactAttributeManagerL().SetFieldAttributeL
            ( iStoreContact->Fields().FieldAt( aIndex ), *attr, *this );

    CleanupStack::PopAndDestroy( attr );
    }

// --------------------------------------------------------------------------
// CmsSetVoiceCallDefault::SetVoiceCallDefaultL
// --------------------------------------------------------------------------
//
void CmsSetVoiceCallDefault::SetVoiceCallDefaultL( MVPbkStoreContact* aContact, CVPbkContactManager* aContactManager )
	{
	iFieldIndexArray.Reset();
    iStoreContact = aContact;
    iContactManager = aContactManager;
    
    if ( iStoreContact && iContactManager && IsSettingDefaultNeededL() )
    	{
    	LockContactL();
    	}
	}

// --------------------------------------------------------------------------
// CmsSetVoiceCallDefault::SetVoiceCallDefaultL
// --------------------------------------------------------------------------
//
void CmsSetVoiceCallDefault::ToSetVoiceCallDefaultL()
    {
    
    TInt count = iFieldIndexArray.Count();
    
    if ( count > 0 )
    	{
    	TInt index = KErrNotFound;
    	
    	for ( TInt i=0; i<ECCACommLauncherCallLast; i++ )
    		{
    		TInt aResourceId = GetCallFieldResourceId(i);
    		if ( FieldIndex( aResourceId, index ) )
    		    {
    		    break;
    		    }
    		}
    	
    	if ( index != KErrNotFound )
    		{
    		DoSetDefaultL( EVPbkDefaultTypePhoneNumber, index );
    		}
    	else
    		{
    		DefaultSettingCompleteL();
    		}
    	}
    else
    	{
    	// setting complete
    	DefaultSettingCompleteL();
    	}   
    }

// --------------------------------------------------------------------------
// CmsSetVoiceCallDefault::IsSettingDefaultNeeded
// --------------------------------------------------------------------------
//
TBool CmsSetVoiceCallDefault::IsSettingDefaultNeededL()
	{
	// If the contact is not in phone memory, don't set default
	if ( !IsPhoneMemoryContact( *iStoreContact ) )
		{
		return EFalse;
		}
	
    CVPbkFieldTypeSelector* selector = VPbkFieldTypeSelectorFactory
        ::BuildContactActionTypeSelectorL(
        		VPbkFieldTypeSelectorFactory::EVoiceCallSelector, iContactManager->FieldTypes());    
    
    const MVPbkStoreContactFieldCollection& fields = iStoreContact->Fields();
    TBool result = EFalse;
    TInt count = fields.FieldCount();
    
    // Voice call default attribute
    CVPbkDefaultAttribute* attr =
        CVPbkDefaultAttribute::NewL( EVPbkDefaultTypePhoneNumber );
    CleanupStack::PushL( attr );
    
		for (TInt i = 0; i < count; ++i)
            {
            const MVPbkStoreContactField& field = fields.FieldAt(i);       
            
            // If this field is voice call field
            if ( selector->IsFieldIncluded( field ) )
                {              
                // Checking if the field has been set as voice call default, 
                // if hasn't, keep field index in an array
                if ( !iContactManager->ContactAttributeManagerL().HasFieldAttributeL
                        ( *attr, field ) )
                	{
                	result = ETrue;
                	iFieldIndexArray.AppendL(i);
                	}
                // it has been set as voice call default, no need to set again
                else
                	{
                	result = EFalse;
                	iFieldIndexArray.Reset();
                	break;
                	}
                }
            }
		
	CleanupStack::PopAndDestroy( attr );
    delete selector;
    return result;
	}

// ---------------------------------------------------------------------------
// CmsSetVoiceCallDefault::FieldIndex
// ---------------------------------------------------------------------------
//
TBool CmsSetVoiceCallDefault::FieldIndex( TInt aResourceId, TInt& aIndex )
    {
	TBool result = EFalse;
	
	if( iStoreContact )
		{
    	const MVPbkBaseContactFieldCollection& fields = iStoreContact->Fields();	
    	TInt count = iFieldIndexArray.Count();
        
        for (TInt i = 0; i < count; ++i)
            {
            const MVPbkFieldType* fieldType = 
                fields.FieldAt(iFieldIndexArray[i]).BestMatchingFieldType();          
            
            if ( fieldType && fieldType->FieldTypeResId() == aResourceId )
                {
                result = ETrue;
                aIndex = iFieldIndexArray[i];
                break;
                }
            }		
		}
	return result;
    }

// ---------------------------------------------------------------------------
// CmsSetVoiceCallDefault::NextAttribute
// ---------------------------------------------------------------------------
//
TInt CmsSetVoiceCallDefault::GetCallFieldResourceId( TInt aPriority )
    {
    
	TInt resourceId;
	switch ( aPriority )
        {
		case ECCACommLauncherCallMobile:
			resourceId = R_VPBK_FIELD_TYPE_MOBILEPHONEGEN;
			break;
		case ECCACommLauncherCallMobileWork:
			resourceId = R_VPBK_FIELD_TYPE_MOBILEPHONEWORK;
			break;
		case ECCACommLauncherCallMobileHome:
			resourceId = R_VPBK_FIELD_TYPE_MOBILEPHONEHOME;
			break;
		case ECCACommLauncherCallTelephone:
			resourceId = R_VPBK_FIELD_TYPE_LANDPHONEGEN;
			break;
		case ECCACommLauncherCallTelephoneWork:
			resourceId = R_VPBK_FIELD_TYPE_LANDPHONEWORK;
			break;
		case ECCACommLauncherCallTelephoneHome:
			resourceId = R_VPBK_FIELD_TYPE_LANDPHONEHOME;
			break;
		case ECCACommLauncherCallCarPhone:
			resourceId = R_VPBK_FIELD_TYPE_CARPHONE;
			break;
		case ECCACommLauncherCallAssistant:
			resourceId = R_VPBK_FIELD_TYPE_ASSTPHONE;
			break;
		default:
			break;
        }
	return resourceId;
    }

// ---------------------------------------------------------------------------
// CmsSetVoiceCallDefault::DefaultSettingCompleteL
// ---------------------------------------------------------------------------
//
void CmsSetVoiceCallDefault::DefaultSettingCompleteL()
	{
	// commit the contact
	iStoreContact->CommitL( *this );
	}

// --------------------------------------------------------------------------
// CmsSetVoiceCallDefault::ProcessFinished
// --------------------------------------------------------------------------
//
void CmsSetVoiceCallDefault::ProcessFinished( TInt aError )
    {       
    }

// ---------------------------------------------------------------------------
// CmsSetVoiceCallDefault::HandleError
// ---------------------------------------------------------------------------
//
void CmsSetVoiceCallDefault::HandleError( TInt aError )
    {
    }

// --------------------------------------------------------------------------
// CmsSetVoiceCallDefault::IsPhoneMemoryContact
// --------------------------------------------------------------------------
//
TBool CmsSetVoiceCallDefault::IsPhoneMemoryContact(
        const MVPbkStoreContact& aContact) const
    {
    TBool ret = EFalse;

    TVPbkContactStoreUriPtr uri =
        aContact.ParentStore().StoreProperties().Uri();

    TVPbkContactStoreUriPtr phoneMemoryUri
        ( VPbkContactStoreUris::DefaultCntDbUri() );

    if (uri.Compare( phoneMemoryUri,
        TVPbkContactStoreUriPtr::EContactStoreUriStoreType ) == 0)
        {
        ret = ETrue;
        }

    return ret;
    }
