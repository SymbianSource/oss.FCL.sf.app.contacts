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
* Description:  Implementation of comm launcher view plugin
*
*/
#include "ccappcommlauncherplugin.h"
#include "ccappcommlaunchersetdefault.h"

#include <mvpbkcontactlinkarray.h>
#include <mvpbkcontactstoreproperties.h>
#include <mvpbkcontactstore.h>
#include <mvpbkstorecontact.h>
#include <mvpbkcontactoperationbase.h>

#include <cvpbkcontactmanager.h>
#include <cpbk2applicationservices.h>
#include <cvpbkdefaultattribute.h>
#include <CVPbkFieldTypeSelector.h>
#include <vpbkcontactstoreuris.h>

#include <pbk2datacaging.hrh>
#include <featmgr.h>

const TInt KGranularity = 4;
_LIT(KPbk2CommandsDllResFileName,   "Pbk2Commands.rsc");
_LIT(KPbk2UiControlsDllResFileName, "Pbk2UiControls.rsc");
_LIT(KPbk2CommonUiDllResFileName,   "Pbk2CommonUi.rsc"  );

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLSetDefault::CCCAppCommLauncherLSetDefault
// ---------------------------------------------------------------------------
//
CCCAppCommLauncherLSetDefault::CCCAppCommLauncherLSetDefault(
    CCCAppCommLauncherPlugin& aPlugin )
    :
    iPlugin(aPlugin),
    iCommandsResourceFile( *CCoeEnv::Static() ),
    iUiControlsResourceFile( *CCoeEnv::Static() ),
    iCommonUiResourceFile( *CCoeEnv::Static() )
    {
    
    }
    
// ---------------------------------------------------------------------------
// CCCAppCommLauncherLSetDefault::~CCCAppCommLauncherLSetDefault
// ---------------------------------------------------------------------------
//
CCCAppCommLauncherLSetDefault::~CCCAppCommLauncherLSetDefault()
	{
	delete iDefaultActions;
	delete iWaitFinish;
	
	delete iRetrieveOperation;
	delete iSetAttributeOperation;
	delete iLinks;
	delete iStoreContact;
	
	// Close contact store
	if( iContactStore )
	   {
	   iContactStore->Close( *this );
	   }
	
	if(iCmsWrapper)
	   {
	   iCmsWrapper->Release();
	   }
	delete iContactLinkArrayDes;
	
	Release( iAppServices );
	iCommandsResourceFile.Close();
	iUiControlsResourceFile.Close();
	iCommonUiResourceFile.Close();
	}

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLSetDefault::NewL
// ---------------------------------------------------------------------------
//
CCCAppCommLauncherLSetDefault* CCCAppCommLauncherLSetDefault::NewL(
    CCCAppCommLauncherPlugin& aPlugin )
    {
    CCCAppCommLauncherLSetDefault* self =
        new( ELeave ) CCCAppCommLauncherLSetDefault( aPlugin );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
	}

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLSetDefault::ConstructL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherLSetDefault::ConstructL()
    {
    //iPbkCmd = CCCAppCommLauncherPbkCmd::NewL( iPlugin );
    iWaitFinish = new (ELeave) CActiveSchedulerWait();
    
    iCommandsResourceFile.OpenL(
    KPbk2RomFileDrive, KDC_RESOURCE_FILES_DIR, KPbk2CommandsDllResFileName );
    iUiControlsResourceFile.OpenL(
    KPbk2RomFileDrive, KDC_RESOURCE_FILES_DIR, KPbk2UiControlsDllResFileName );
    iCommonUiResourceFile.OpenL(
    KPbk2RomFileDrive, KDC_RESOURCE_FILES_DIR, KPbk2CommonUiDllResFileName );
    iAppServices = CPbk2ApplicationServices::InstanceL();
    
    iCmsWrapper = CCCAppCmsContactFetcherWrapper::InstanceL();
    
    iContactLinkArrayDes = iCmsWrapper->ContactIdentifierLC();       
    CleanupStack::Pop(iContactLinkArrayDes);

    iDefaultActions =
    	        new( ELeave ) CArrayFixFlat<VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector>( KGranularity );

    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLSetDefault::VPbkSingleContactOperationComplete
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherLSetDefault::VPbkSingleContactOperationComplete(
    MVPbkContactOperationBase& /*aOperation*/,
    MVPbkStoreContact* aContact)
	{
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;

    delete iStoreContact;
    iStoreContact = aContact;
    
    TInt index;
    VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector callSelector = VPbkFieldTypeSelectorFactory::EVoiceCallSelector;
    VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector messageSelector = VPbkFieldTypeSelectorFactory::EUniEditorSelector;
    
    if ( !IsPhoneMemoryContact( *iStoreContact ) ||
    		(!IsContactActionHasField( callSelector, index ) &&
    	    		!IsContactActionHasField( messageSelector, index )) ||
    	    		(FindContactFieldWithAttributeL( MapSelectorIdToDefaultType(callSelector ) ) &&
    	    	    		FindContactFieldWithAttributeL( MapSelectorIdToDefaultType( messageSelector ) )) ||
    	    	    		(!iStoreContact) )
    	
    	{
    	ProcessFinished(KErrNone);
    	}

    
    else // ( iStoreContact )
    	{
    	TInt err = KErrNone;
            TRAP( err, LockContactL() );
            if ( err != KErrNone )
                {
                ProcessFinished( err );
                }
        }
	}

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLSetDefault::VPbkSingleContactOperationFailed
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherLSetDefault::VPbkSingleContactOperationFailed(
    MVPbkContactOperationBase& /*aOperation*/,
    TInt /*aError*/)
	{
	ProcessFinished(KErrNone);
	}

// --------------------------------------------------------------------------
// CCCAppCommLauncherLSetDefault::AttributeProcessCompleted
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherLSetDefault::AttributeOperationComplete( MVPbkContactOperationBase& aOperation )
    {
    TInt err = KErrNone;

    if ( &aOperation == iSetAttributeOperation )
        {
        // Move to next attribute
        TRAP( err, SetNextL() );
        }
            
    // Handle error
    if ( err != KErrNone )
         {
         ProcessFinished( err ); // Complete the setting
         }
    }

// --------------------------------------------------------------------------
// CCCAppCommLauncherLSetDefault::AttributeProcessFailed
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherLSetDefault::AttributeOperationFailed(
        MVPbkContactOperationBase& aOperation,
        TInt aError )
    {
    ProcessFinished( aError );
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLSetDefault::StoreReady
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherLSetDefault::StoreReady(
    MVPbkContactStore& aContactStore )
    {
    iContactStore = &aContactStore;
    
    // operation completes by VPbkSingleContactOperationComplete
    // or VPbkSingleContactOperationFailed
    if( iRetrieveOperation  )
        {
        delete iRetrieveOperation;
        iRetrieveOperation = NULL;
        }
    
    TRAPD( err,iRetrieveOperation = iAppServices->ContactManager().RetrieveContactL( iLinks->At( 0 ), *this ); )
    
    if( err != KErrNone )
        {
        HandleError( err );
        }
    } 

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLSetDefault::StoreUnavailable
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherLSetDefault::StoreUnavailable(
    MVPbkContactStore& /*aContactStore*/,
    TInt /*aReason*/)
    {
    ProcessFinished(KErrNone);
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLSetDefault::HandleStoreEventL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherLSetDefault::HandleStoreEventL(
    MVPbkContactStore& /*aContactStore*/,
    TVPbkContactStoreEvent aStoreEvent)
    {
    switch ( aStoreEvent.iEventType )
    	{
    	case TVPbkContactStoreEvent::EContactChanged:
    		{
    		// do nothing for now
    		}
    		break;
    	default:
    		break;
    	}
    }

// --------------------------------------------------------------------------
// CCCAppCommLauncherLSetDefault::ContactOperationCompleted
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherLSetDefault::ContactOperationCompleted
        (TContactOpResult aResult)
    {
    if (aResult.iOpCode == EContactLock)
        {
        TRAPD( err, SetNextL() );
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
// CCCAppCommLauncherLSetDefault::ContactOperationFailed
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherLSetDefault::ContactOperationFailed
        ( TContactOp /*aOpCode*/, TInt aErrorCode,
          TBool /*aErrorNotified*/ )
    {
    ProcessFinished( aErrorCode );
    }

// ----------------------------------------------------------
// CCCAppCommLauncherLSetDefault::WaitFinish
// ----------------------------------------------------------
//
void CCCAppCommLauncherLSetDefault::WaitFinish()
    {
    if ( iWaitFinish && !iWaitFinish->IsStarted() )
        {     
        iWaitFinish->Start();
        }
    }

// ----------------------------------------------------------
// CCCAppCommLauncherLSetDefault::StopWait
// ----------------------------------------------------------
//
void CCCAppCommLauncherLSetDefault::StopWait()
	{
	if ( iWaitFinish && iWaitFinish->IsStarted() )
		{
		iWaitFinish->AsyncStop();
		}
	}

// ----------------------------------------------------------
// CCCAppCommLauncherLSetDefault::ExecuteAssignDefaultL
// ----------------------------------------------------------
//
void CCCAppCommLauncherLSetDefault::ExecuteAssignDefaultL()
	{
	iDefaultActions->Reset();    	        
	iDefaultActions->AppendL( VPbkFieldTypeSelectorFactory::EVoiceCallSelector );
	iDefaultActions->AppendL( VPbkFieldTypeSelectorFactory::EUniEditorSelector );
	    
    if( iLinks )
        {
        delete iLinks;
        iLinks = NULL;
        }
    
    iLinks = iAppServices->ContactManager().CreateLinksLC( *iContactLinkArrayDes );

    if ( iLinks && iLinks->Count() > 0 )
        {
        // operation completes by StoreReady,
        //  StoreUnavailable or HandleStoreEventL
        (iLinks->At( 0 )).ContactStore().OpenL( *this );
        }

    CleanupStack::Pop( 1 ); // iLinks
	}

// ----------------------------------------------------------
// CCCAppCommLauncherLSetDefault::LockContactL
// ----------------------------------------------------------
//
void CCCAppCommLauncherLSetDefault::LockContactL()
    {
    // We must lock the contact for changes
    iStoreContact->LockL(*this);
    }

// --------------------------------------------------------------------------
// CCCAppCommLauncherLSetDefault::FindContactFieldWithAttributeL
// --------------------------------------------------------------------------
//
TBool
CCCAppCommLauncherLSetDefault::FindContactFieldWithAttributeL
            ( TVPbkDefaultType aDefaultType )
    {
    TBool result = EFalse;

    // Loop through contact's fields and find the specified field
    const TInt fieldCount = iStoreContact->Fields().FieldCount();
    CVPbkDefaultAttribute* attr =
        CVPbkDefaultAttribute::NewL( aDefaultType );
    CleanupStack::PushL( attr );

    for ( TInt i=0; i < fieldCount; ++i )
        {
        MVPbkStoreContactField& candidate =
        iStoreContact->Fields().FieldAt( i );

        // Check if field has default attribute defaultType
        if ( iAppServices->ContactManager().ContactAttributeManagerL().HasFieldAttributeL
                ( *attr, candidate ) )
            {
            result = ETrue;
            break;
            }
        }
    CleanupStack::PopAndDestroy( attr );
    return result;
    }

// --------------------------------------------------------------------------
// CCCAppCommLauncherLSetDefault::DoSetDefaultL
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherLSetDefault::DoSetDefaultL( TVPbkDefaultType aDefaultType, TInt aIndex )
    {
    // Set given attribute to given field
    CVPbkDefaultAttribute* attr =
        CVPbkDefaultAttribute::NewL( aDefaultType );
    CleanupStack::PushL( attr );

    delete iSetAttributeOperation;
    iSetAttributeOperation = NULL;
    iSetAttributeOperation =
    iAppServices->ContactManager().ContactAttributeManagerL().SetFieldAttributeL
            ( iStoreContact->Fields().FieldAt( aIndex ), *attr, *this );

    CleanupStack::PopAndDestroy( attr );
    }

// --------------------------------------------------------------------------
// CCCAppCommLauncherLSetDefault::SetNextL
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherLSetDefault::SetNextL()
    {
    VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector selector = NextAttribute();

    if ( selector != VPbkFieldTypeSelectorFactory::EEmptySelector)
    	{
    	TInt index;
    	TVPbkDefaultType defaultType = MapSelectorIdToDefaultType(selector);
    	if ( IsContactActionHasField( selector, index ) &&
    			!FindContactFieldWithAttributeL( defaultType ) )
    		{
			DoSetDefaultL( defaultType, index );
    		}
    	else
    		{
    		SetNextL();
    		}
    	}
    else
    	{
    	// setting complete
    	DefaultSettingCompleteL();
    	}
    }

// --------------------------------------------------------------------------
// CCCAppCommLauncherLSetDefault::IsContactActionHasField
// --------------------------------------------------------------------------
//
TBool CCCAppCommLauncherLSetDefault::IsContactActionHasField(
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction, TInt& aIndex)
    {   
    CVPbkFieldTypeSelector* selector = VPbkFieldTypeSelectorFactory
        ::BuildContactActionTypeSelectorL(
            aContactAction, iAppServices->ContactManager().FieldTypes());    
    
    const MVPbkStoreContactFieldCollection& fields = iStoreContact->Fields();
    TBool result = EFalse;
    TInt count = fields.FieldCount();
			
		for (TInt i = 0; i < count; ++i)
            {
            const MVPbkStoreContactField& field = fields.FieldAt(i);
            if (selector->IsFieldIncluded(field))
                {
                result = ETrue;
                aIndex = i;
                break;
                }
            }    
    delete selector;
    return result;
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLSetDefault::MapSelectorIdToDefaultType
// ---------------------------------------------------------------------------
//
TVPbkDefaultType CCCAppCommLauncherLSetDefault::MapSelectorIdToDefaultType( 
		const VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aSelectorID )
    {
    TVPbkDefaultType defaultType = EVPbkDefaultTypeUndefined;
    switch ( aSelectorID )
        {
        case VPbkFieldTypeSelectorFactory::EVoiceCallSelector:
            {
            defaultType = EVPbkDefaultTypePhoneNumber;
            break;
            }
        case VPbkFieldTypeSelectorFactory::EUniEditorSelector:
            {
            if (FeatureManager::FeatureSupported(KFeatureIdMMS))
                {
                defaultType = EVPbkDefaultTypeMms;
                }
            else if (FeatureManager::FeatureSupported(KFeatureIdEmailOverSms))
                {
                defaultType = EVPbkDefaultTypeEmailOverSms;
                }
            else
                {
                defaultType = EVPbkDefaultTypeSms;
                }
            break;
            }
        default:
        	break;
        }
    return defaultType;
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLSetDefault::NextAttribute
// ---------------------------------------------------------------------------
//
VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector 
CCCAppCommLauncherLSetDefault::NextAttribute()
    {
    VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector selector = VPbkFieldTypeSelectorFactory::EEmptySelector;
    
    TInt count = 0;
    if ( iDefaultActions )
        {
        count = iDefaultActions->Count();
        }

    if ( count > 0 )
        {
        selector = iDefaultActions->At( count - 1 ); // zero-based
        iDefaultActions->Delete( count -1 ); // zero-based
        }
    return selector;
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLSetDefault::DefaultSettingCompleteL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherLSetDefault::DefaultSettingCompleteL()
	{
	// commit the contact
	iStoreContact->CommitL( *this );
	}

// --------------------------------------------------------------------------
// CCCAppCommLauncherLSetDefault::ProcessFinished
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherLSetDefault::ProcessFinished( TInt aError )
    {       
    if ( aError == KErrNone)
        {

        }

    /*if ( aError != KErrNone )
        {
        CCoeEnv::Static()->HandleError( aError );
        }*/
   
    iPlugin.DefaultSettingComplete();
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherLSetDefault::HandleError
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherLSetDefault::HandleError( TInt aError )
    {
    CCoeEnv::Static()->HandleError( aError );
    }

// --------------------------------------------------------------------------
// CCCAppCommLauncherLSetDefault::IsPhoneMemoryContact
// --------------------------------------------------------------------------
//
TBool CCCAppCommLauncherLSetDefault::IsPhoneMemoryContact(
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
