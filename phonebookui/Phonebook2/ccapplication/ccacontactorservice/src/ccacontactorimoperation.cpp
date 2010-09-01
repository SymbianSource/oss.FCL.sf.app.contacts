/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the im operation
*
*/


// INCLUDE FILES

#include "ccacontactorheaders.h"

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CCmsContactorImPluginParameter::CCmsContactorImPluginParameter()
// -----------------------------------------------------------------------------
//
CCmsContactorImPluginParameter::CCmsContactorImPluginParameter()
    {
	
    }

// -----------------------------------------------------------------------------
// CCmsContactorImPluginParameter::~CCmsContactorImPluginParameter()
// -----------------------------------------------------------------------------
//
CCmsContactorImPluginParameter::~CCmsContactorImPluginParameter()
    {
    delete iDisplayName;
    delete iContactLinkArray;
    }

// -----------------------------------------------------------------------------
// CCmsContactorImPluginParameter::NewLC()
// -----------------------------------------------------------------------------
//
CCmsContactorImPluginParameter* CCmsContactorImPluginParameter::NewLC(
		const TDesC& aDisplayName, const TDesC8& aContactLinkArray)
    {
    CCmsContactorImPluginParameter* self=new (ELeave)CCmsContactorImPluginParameter();
    CleanupStack::PushL(self);
    self->ConstructL( aDisplayName, aContactLinkArray );
    return self;
    }
	
// -----------------------------------------------------------------------------
// CCmsContactorImPluginParameter::NewL()
// -----------------------------------------------------------------------------
//
CCmsContactorImPluginParameter* CCmsContactorImPluginParameter::NewL(
		const TDesC& aDisplayName, const TDesC8& aContactLinkArray)
    {
    CCmsContactorImPluginParameter* self=CCmsContactorImPluginParameter::NewLC(
    		aDisplayName, aContactLinkArray );
    CleanupStack::Pop(); // self
    return self;
    }

// -----------------------------------------------------------------------------
// CCmsContactorImPluginParameter::ConstructL()
// -----------------------------------------------------------------------------
//
void CCmsContactorImPluginParameter::ConstructL( const TDesC& aDisplayName, 
        const TDesC8& aContactLinkArray )
    {
    iDisplayName = aDisplayName.AllocL();
    iContactLinkArray = aContactLinkArray.AllocL();
    }

// -----------------------------------------------------------------------------
// CCmsContactorImPluginParameter::DisplayName()
// -----------------------------------------------------------------------------
//
const TDesC& CCmsContactorImPluginParameter::DisplayName() const 
    {
    return *iDisplayName;
    }

// -----------------------------------------------------------------------------
// CCmsContactorImPluginParameter::ContactLinkArray()
// -----------------------------------------------------------------------------
//
const TDesC8& CCmsContactorImPluginParameter::ContactLinkArray() const
    {
    return *iContactLinkArray;
    }

// -----------------------------------------------------------------------------
// CCmsContactorImPluginParameter::ParameterExtension()
// -----------------------------------------------------------------------------
//
TAny* CCmsContactorImPluginParameter::ParameterExtension( TUid aExtensionUid )
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// CCCAContactorIMOperation::CCCAContactorIMOperation()
// -----------------------------------------------------------------------------
//
CCCAContactorIMOperation::CCCAContactorIMOperation(const TDesC& aParam,
		MCmsContactorImPluginParameter* aCmsParameter) : 
    CCCAContactorOperation(aParam),
    iCmsParameter(aCmsParameter)
    {
    iPlugin = NULL;
    }

// -----------------------------------------------------------------------------
// CCCAContactorIMOperation::~CCCAContactorIMOperation()
// -----------------------------------------------------------------------------
//
CCCAContactorIMOperation::~CCCAContactorIMOperation()
    {
    delete iPlugin;
    iPlugin = NULL;

    delete iSPSettings;
    delete iCmsParameter;
    }

// -----------------------------------------------------------------------------
// CCCAContactorIMOperation::NewLC()
// -----------------------------------------------------------------------------
//
CCCAContactorIMOperation* CCCAContactorIMOperation::NewLC(const TDesC& aParam,
		MCmsContactorImPluginParameter* aCmsParameter)
    {
    CCCAContactorIMOperation* self = new (ELeave)CCCAContactorIMOperation(aParam,
    		aCmsParameter);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CCCAContactorIMOperation::NewL()
// -----------------------------------------------------------------------------
//
CCCAContactorIMOperation* CCCAContactorIMOperation::NewL(const TDesC& aParam,
		MCmsContactorImPluginParameter* aCmsParameter)
    {
    CCCAContactorIMOperation* self=CCCAContactorIMOperation::NewLC(aParam,
    		aCmsParameter);
    CleanupStack::Pop(); // self;
    return self;
    }

// -----------------------------------------------------------------------------
// CCCAContactorIMOperation::ConstructL()
// -----------------------------------------------------------------------------
//
void CCCAContactorIMOperation::ConstructL()
    {
    iSPSettings = CSPSettings::NewL();
    }

// -----------------------------------------------------------------------------
// CCCAContactorIMOperation::ExecuteLD()
// -----------------------------------------------------------------------------
//
void CCCAContactorIMOperation::ExecuteLD()
    {
    CleanupStack::PushL(this);
    TPtrC aXspId = iParam;

    TUid dllUid = ResolveEcomUidL( ExtractService(aXspId)  );    
    LoadEcomL( dllUid, aXspId );
    CleanupStack::PopAndDestroy(this);
    }

// --------------------------------------------------------------------------
// CCCAContactorIMOperation::ResolveEcomUidL
// --------------------------------------------------------------------------
//
TUid CCCAContactorIMOperation::ResolveEcomUidL( const TDesC& aServiceId )
    {    
    // Resolve ECom UID from SP settings
    TInt launchId = KErrNotFound;  
               
    launchId = DoResolveEcomUidL( aServiceId ); 
    TUid launchUid = {launchId };    
    return launchUid;
    }

// --------------------------------------------------------------------------
// CCCAContactorIMOperation::DoResolveEcomUidL
// --------------------------------------------------------------------------
//
TInt CCCAContactorIMOperation::DoResolveEcomUidL( const TDesC& aServiceId )
    {    
    // Resolve ECom UID from SP settings
    TInt err = KErrNone;
    TInt launchId = KErrNotFound;  
    CDesCArrayFlat* nameArray = NULL;      
    
    RIdArray ids;
    CleanupClosePushL( ids );
    
    nameArray = new (ELeave) CDesCArrayFlat(2);
    CleanupStack::PushL( nameArray );    
    
    err = iSPSettings->FindServiceIdsL( ids );  
    User::LeaveIfError( err );     
    err = iSPSettings->FindServiceNamesL( ids, *nameArray );  
    User::LeaveIfError( err );     
    
    TInt count = nameArray->MdcaCount();
    for ( TInt i(0); i < count; i++)
        {
        // search the mathching service name
        TPtrC p = nameArray->MdcaPoint( i );
        if (!p.CompareF( aServiceId ))
            {
            // We have found the service, now get the plugin id
            CSPProperty* IMLaunchIdProperty = CSPProperty::NewLC();
            err = iSPSettings->FindPropertyL( ids[i], ESubPropertyIMLaunchUid, *IMLaunchIdProperty );
            User::LeaveIfError( err );               
            err = IMLaunchIdProperty->GetValue( launchId );
            User::LeaveIfError( err ); 
            CleanupStack::PopAndDestroy( IMLaunchIdProperty );            
            break;
            }
        }    
    CleanupStack::PopAndDestroy( nameArray );
    CleanupStack::PopAndDestroy( ); // >>> ids
    
    return launchId; 
    }
    
// --------------------------------------------------------------------------
// CCCAContactorIMOperation::LoadEcomL
// --------------------------------------------------------------------------
//
void CCCAContactorIMOperation::LoadEcomL( TUid aUidImp, const TDesC& aXspId  )
    {         
    TUid destructorId;

    TAny* volatile implementation =
        REComSession::CreateImplementationL( aUidImp,
                                             destructorId );
                                             
    iPlugin = reinterpret_cast< CCmsContactorImPluginBase* >( implementation );    
    iPlugin->SetDestructorId( destructorId ); 
    iPlugin->SetCmsParameter( iCmsParameter );
    iPlugin->ExecuteL( aXspId ); 
    }

// --------------------------------------------------------------------------
// CCCAContactorIMOperation::ExtractService
// --------------------------------------------------------------------------
//
TPtrC CCCAContactorIMOperation::ExtractService( const TDesC& aXspId )
    {         
    TInt pos = aXspId.Find(KColon);
    if ( pos >= 0)
        {
        // ok input
        return aXspId.Left(pos);
        }
    else
        {
        // return something in illegal input case
        return TPtrC(KNullDesC);
        }
    }
// End of File
