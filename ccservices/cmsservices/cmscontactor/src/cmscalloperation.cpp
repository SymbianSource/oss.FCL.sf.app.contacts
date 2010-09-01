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
* Description:  Call operation.
 *
*/


// INCLUDE FILES
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
        _LIT(KPanicText, "CCMSCallOperation");
        User::Panic (KPanicText, aReason);
        }
#endif // _DEBUG


    } /// namespace


// ================= MEMBER FUNCTIONS =======================
//

// ----------------------------------------------------------
// CCMSCallOperation::NewL
// ----------------------------------------------------------
//
CCMSCallOperation* CCMSCallOperation::NewL(
        const RArray<CCmsContactFieldItem::TCmsContactField>& aContactFieldTypesFilter,
        const RPointerArray<CCmsContactField>& aItemArray,
        const TCmsContactor::TCMSCommLaunchFlag aCommLaunchFlag )
    {
    CCMSCallOperation* self = new (ELeave) CCMSCallOperation(
        aContactFieldTypesFilter, 
        aItemArray,
        aCommLaunchFlag );
    CleanupStack::PushL ( self);
    self->ConstructL ();
    CleanupStack::Pop ( self);
    return self;
    }

// ----------------------------------------------------------
// CCMSCallOperation::CCMSCallOperation
// ----------------------------------------------------------
//
inline CCMSCallOperation::CCMSCallOperation(
        const RArray<CCmsContactFieldItem::TCmsContactField>& aContactFieldTypesFilter,
        const RPointerArray<CCmsContactField>& aItemArray,
        const TCmsContactor::TCMSCommLaunchFlag aCommLaunchFlag ) :
    CCMSOperationHandlerBase( aContactFieldTypesFilter, aItemArray, aCommLaunchFlag )
    {
    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSCallOperation::CCMSCallOperation()"));

    return;
    }

// --------------------------------------------------------------------------
// CCMSCallOperation::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CCMSCallOperation::ConstructL()
    {
    BaseConstructL ();
    return;
    }

// ----------------------------------------------------------
// CCMSCallOperation::ExecuteLD
// ----------------------------------------------------------
//
void CCMSCallOperation::ExecuteLD()
    {
    CMS_DP (KCMSContactorLoggerFile, CMS_L ( "CCMSCallOperation::ExecuteLD()"));

    // "D" function semantics
    CleanupStack::PushL ( this);

    PrepareDataL (R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT,
            R_QTN_CCA_POPUP_CALL, CCmsContactFieldItem::ECmsDefaultTypePhoneNumber);

    //Execute the operation
    TRAPD(err, StartOperationL(LaunchDialogL()))
    if ( KErrNone != err )
        {
        CMS_DP(KCMSContactorLoggerFile, CMS_L( "CCMSCallOperation::ExecuteLD(): Err: %d"),err);
        }

    CleanupStack::PopAndDestroy (this);
    CMS_DP(KCMSContactorLoggerFile, CMS_L( "CCMSCallOperation::ExecuteLD(): Done."));
    return;
    }

// ----------------------------------------------------------
// CCMSCallOperation::StartOperationL
// ----------------------------------------------------------
//
void CCMSCallOperation::StartOperationL(
        const CCmsContactFieldItem& aContactFieldItem)
    {       
    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSCallOperation::StartOperationL()"));

    __ASSERT_DEBUG (NULL != &aContactFieldItem,
                        Panic (EPanicPreCond_StartOperationL));

    CCmsPhoneCall::ExecuteL (aContactFieldItem.Data (), CCmsPhoneCall::ECmsCallTypeVoice);

    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSCallOperation::StartOperationL(): Done."));
    return;
    }

// End of file
