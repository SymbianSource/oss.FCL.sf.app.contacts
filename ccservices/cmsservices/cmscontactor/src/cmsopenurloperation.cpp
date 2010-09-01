/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Open URL operation. 
 *
*/


#include "cmscontactorheaders.h"

/// Unnamed namespace for local definitions
namespace
    {

#ifdef _DEBUG
    enum TPanicCode
        {
        EPanicPreCond_StartOperationL = 1
        };

    void Panic(TPanicCode aReason)
        {
        _LIT(KPanicText, "CCMSOpenURLOperation");
        User::Panic (KPanicText, aReason);
        }
#endif // _DEBUG


    } /// namespace

// ----------------------------------------------------------
// CCMSOpenURLOperation::NewL
// ----------------------------------------------------------
//
CCMSOpenURLOperation* CCMSOpenURLOperation::NewL(
        const RArray<CCmsContactFieldItem::TCmsContactField>& aContactFieldTypesFilter,
        const RPointerArray<CCmsContactField>& aItemArray,
        const TCmsContactor::TCMSCommLaunchFlag aCommLaunchFlag )
    {
    CCMSOpenURLOperation* self = new (ELeave) CCMSOpenURLOperation(
        aContactFieldTypesFilter, 
        aItemArray,
        aCommLaunchFlag );
    CleanupStack::PushL ( self);
    self->ConstructL ();
    CleanupStack::Pop ( self);
    return self;
    }

// ----------------------------------------------------------
// CCMSOpenURLOperation::CCMSOpenURLOperation
// ----------------------------------------------------------
//
inline CCMSOpenURLOperation::CCMSOpenURLOperation(
        const RArray<CCmsContactFieldItem::TCmsContactField>& aContactFieldTypesFilter,
        const RPointerArray<CCmsContactField>& aItemArray,
        const TCmsContactor::TCMSCommLaunchFlag aCommLaunchFlag ) :
    CCMSOperationHandlerBase(aContactFieldTypesFilter, aItemArray, aCommLaunchFlag)
    {
    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSOpenURLOperation::CCMSOpenURLOperation()"));

    return;
    }

// --------------------------------------------------------------------------
// CCMSOpenURLOperation::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CCMSOpenURLOperation::ConstructL()
    {
    BaseConstructL ();
    return;
    }

// ----------------------------------------------------------
// CCMSOpenURLOperation::ExecuteLD
// ----------------------------------------------------------
//
void CCMSOpenURLOperation::ExecuteLD()
    {
    CMS_DP (KCMSContactorLoggerFile, CMS_L ( "CCMSOpenURLOperation::ExecuteLD()"));

    // "D" function semantics
    CleanupStack::PushL ( this);

    PrepareDataL (R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT,
            R_QTN_CCA_POPUP_URL, CCmsContactFieldItem::ECmsDefaultTypeUndefined);

    //Execute the operation
    TRAPD(err, StartOperationL(LaunchDialogL()))
    if ( KErrNone != err )
        {
        CMS_DP(KCMSContactorLoggerFile, CMS_L( "CCMSOpenURLOperation::ExecuteLD(): Err: %d"),err);
        }

    CleanupStack::PopAndDestroy (this);
    CMS_DP(KCMSContactorLoggerFile, CMS_L( "CCMSOpenURLOperation::ExecuteLD(): Done."));
    return;
    }

// ----------------------------------------------------------
// CCMSOpenURLOperation::StartOperationL
// ----------------------------------------------------------
//
void CCMSOpenURLOperation::StartOperationL(
        const CCmsContactFieldItem& aContactFieldItem)
    {
    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSOpenURLOperation::StartOperationL()"));

    __ASSERT_DEBUG (NULL != &aContactFieldItem,
                        Panic (EPanicPreCond_StartOperationL));

    /*TAiwVariant var(aContactFieldItem.Data());
    TAiwGenericParam aiwparam(EGenericParamURL, var);
    
    RCriteriaArray interest;
    CleanupClosePushL ( interest);
    CAiwCriteriaItem* criteria = CAiwCriteriaItem::NewLC ( 0x101FD684,
            KAiwCmdView, KAiwContentTypeAll);
    criteria->SetServiceClass ( TUid::Uid (KAiwClassMenu));
    User::LeaveIfError ( interest.Append ( criteria));

    CAiwServiceHandler* serviceHandler = CAiwServiceHandler::NewLC ();
    serviceHandler->AttachL ( interest);

    CAiwGenericParamList& paramList = serviceHandler->InParamListL ();
    paramList.AppendL(aiwparam);

    serviceHandler->ExecuteMenuCmdL( EBrowserAiwCmdOpenUrl, paramList,
            serviceHandler->OutParamListL (), 0, NULL);
    serviceHandler->DetachL ( interest);

    CleanupStack::PopAndDestroy ( serviceHandler);
    CleanupStack::PopAndDestroy ( criteria);
    CleanupStack::PopAndDestroy (); //interest  */
    
    // Browser's AIW provider supports only the menu service class, 
    // for which we would need to provide a menu. Since we have no 
    // menu to provide we can't use AIW to lauch the browser, so instead 
    // we'll lauch it directy.    
    _LIT(KFourAndSpace, "4 ");
    const TInt KBrowserUid = 0x10008D39;
    HBufC* param = HBufC::NewLC( aContactFieldItem.Data().Size() + 2 );
    param->Des().Copy( KFourAndSpace );
    param->Des().Append( aContactFieldItem.Data() );
    TUid id( TUid::Uid( KBrowserUid ) );
    TApaTaskList taskList( CEikonEnv::Static()->WsSession() );
    TApaTask task = taskList.FindApp( id );
    if ( task.Exists() )
        {
        HBufC8* param8 = HBufC8::NewLC( param->Length() );
        param8->Des().Append( *param );
        task.SendMessage( TUid::Uid( 0 ), *param8 ); // Uid is not used
        CleanupStack::PopAndDestroy(param8);
        }
    else 
        {
        RApaLsSession appArcSession;
        User::LeaveIfError( appArcSession.Connect() ); // connect to AppArc server
        CleanupClosePushL( appArcSession );
        TThreadId id;
        User::LeaveIfError
            ( appArcSession.StartDocument( *param, TUid::Uid( KBrowserUid ), id ) );
        CleanupStack::PopAndDestroy( &appArcSession );
        }
    CleanupStack::PopAndDestroy(); // param   

    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSOpenURLOperation::StartOperationL(): Done."));
    return;
    }

// End of file
