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
* Description:  IM operation.
 *
*/


// INCLUDE FILES
#include "cmscontactorheaders.h"
#include "cmscontactfielditem.h"
// IM Launch plugin
#include <cmscontactorimpluginbase.h>
#include <cmscontactorimpluginifids.hrh>
// service table
#include <spsettings.h>
#include <spentry.h>
#include <spproperty.h>
#include <spdefinitions.h>

#include <ecom.h>

#ifdef _DEBUG
    enum TPanicCode
        {
        EPanicPreCond_StartOperationL = 1
        };

    void Panic(TPanicCode aReason)
        {
        _LIT(KPanicText, "CCMSIMOperation");
        User::Panic (KPanicText, aReason);
        }
#endif // _DEBUG
    
_LIT(KColon, ":");


// ================= MEMBER FUNCTIONS =======================
//

// ----------------------------------------------------------
// CCMSIMOperation::NewL
// ----------------------------------------------------------
//
CCMSIMOperation* CCMSIMOperation::NewL(
        const RArray<CCmsContactFieldItem::TCmsContactField>& aContactFieldTypesFilter,
        const RPointerArray<CCmsContactField>& aItemArray,
        const TCmsContactor::TCMSCommLaunchFlag aCommLaunchFlag )
    {
    CCMSIMOperation* self = new (ELeave) CCMSIMOperation( 
        aContactFieldTypesFilter, 
        aItemArray,
        aCommLaunchFlag );
    CleanupStack::PushL ( self);
    self->ConstructL ();
    CleanupStack::Pop ( self);
    return self;
    }

// ----------------------------------------------------------
// CCMSIMOperation::CCMSIMOperation
// ----------------------------------------------------------
//
inline CCMSIMOperation::CCMSIMOperation(
        const RArray<CCmsContactFieldItem::TCmsContactField>& aContactFieldTypesFilter,
        const RPointerArray<CCmsContactField>& aItemArray,
        const TCmsContactor::TCMSCommLaunchFlag aCommLaunchFlag ) :
    CCMSOperationHandlerBase(aContactFieldTypesFilter, aItemArray, aCommLaunchFlag)
    {
    CMS_DP (KCMSContactorLoggerFile, CMS_L ( "CCMSIMOperation::CCMSIMOperation()"));
    iPlugin = NULL;
    return;
    }
    
// ----------------------------------------------------------
// CCMSIMOperation::~CCMSIMOperation
// ----------------------------------------------------------
//
CCMSIMOperation::~CCMSIMOperation()
    {    
    delete iPlugin;
    iPlugin = NULL;
    REComSession::FinalClose();   
    delete iSPSettings;
    }
       
// --------------------------------------------------------------------------
// CCMSIMOperation::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CCMSIMOperation::ConstructL()
    {   
    BaseConstructL();
    iSPSettings = CSPSettings::NewL(); 
    return;
    }

// ----------------------------------------------------------
// CCMSIMOperation::ExecuteLD
// ----------------------------------------------------------
//
void CCMSIMOperation::ExecuteLD()
    {
    CMS_DP (KCMSContactorLoggerFile, CMS_L ( "CCMSIMOperation::ExecuteLD()"));

    // "D" function semantics
    CleanupStack::PushL ( this);
    
    PrepareDataL (R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT,
            R_QTN_CCA_POPUP_MESSAGE, CCmsContactFieldItem::ECmsDefaultTypeImpp);   
    TRAPD(err, StartOperationL( LaunchDialogL() ));    
         
    if ( KErrNone != err )
        {
        CMS_DP(KCMSContactorLoggerFile, CMS_L( "CCMSIMOperation::ExecuteLD(): Err: %d"),err);
        }

    CleanupStack::PopAndDestroy (this);
    CMS_DP(KCMSContactorLoggerFile, CMS_L( "CCMSIMOperation::ExecuteLD(): Done."));
    return;
    }

// ----------------------------------------------------------
// CCMSIMOperation::StartOperationL
// ----------------------------------------------------------
//
void CCMSIMOperation::StartOperationL(
        const CCmsContactFieldItem& aContactFieldItem)
    {
    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSVOIPCallOperation::StartOperationL()"));

    __ASSERT_DEBUG (NULL != &aContactFieldItem,
                        Panic (EPanicPreCond_StartOperationL));
        
    TPtrC aXspId = aContactFieldItem.Data();

    TUid dllUid = ResolveEcomUidL( ExtractService(aXspId)  );    
    LoadEcomL( dllUid, aXspId );

    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSVOIPCallOperation::StartOperationL(): Done."));
    return;
    }

// --------------------------------------------------------------------------
// CCMSIMOperation::ResolveEcomUidL
// --------------------------------------------------------------------------
//
TUid CCMSIMOperation::ResolveEcomUidL( const TDesC& aServiceId )
    {    
    // Resolve ECom UID from SP settings
    TInt launchId = KErrNotFound;  
               
    launchId = DoResolveEcomUidL( aServiceId );               
    TUid launchUid = {launchId };    
    return launchUid;
    }

// --------------------------------------------------------------------------
// CCMSIMOperation::DoResolveEcomUidL
// --------------------------------------------------------------------------
//
TInt CCMSIMOperation::DoResolveEcomUidL( const TDesC& aServiceId )
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
// CCMSIMOperation::LoadEcomL
// --------------------------------------------------------------------------
//
void CCMSIMOperation::LoadEcomL( TUid aUidImp, const TDesC& aXspId  )
    {         
    TUid destructorId;

    TAny* volatile implementation =
        REComSession::CreateImplementationL( aUidImp,
                                             destructorId );
                                             
    iPlugin = reinterpret_cast< CCmsContactorImPluginBase* >( implementation );    
    iPlugin->SetDestructorId( destructorId );    
    iPlugin->ExecuteL( aXspId );    
    }

// --------------------------------------------------------------------------
// CCMSIMOperation::ExtractService
// --------------------------------------------------------------------------
//
TPtrC CCMSIMOperation::ExtractService( const TDesC& aXspId )
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
        return aXspId;
        }
    }

// End of file
