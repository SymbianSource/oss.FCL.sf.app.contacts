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
        _LIT(KPanicText, "CCMSVideoCallOperation");
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
CCMSVideoCallOperation* CCMSVideoCallOperation::NewL(
        const RArray<CCmsContactFieldItem::TCmsContactField>& aContactFieldTypesFilter,
        const RPointerArray<CCmsContactField>& aItemArray,
        const TCmsContactor::TCMSCommLaunchFlag aCommLaunchFlag )
    {
    CCMSVideoCallOperation* self = new (ELeave) CCMSVideoCallOperation(
        aContactFieldTypesFilter, 
        aItemArray,
        aCommLaunchFlag );
    CleanupStack::PushL ( self);
    self->ConstructL ();
    CleanupStack::Pop ( self);
    return self;
    }

// ----------------------------------------------------------
// CCMSVideoCallOperation::CCMSVideoCallOperation
// ----------------------------------------------------------
//
inline CCMSVideoCallOperation::CCMSVideoCallOperation(
        const RArray<CCmsContactFieldItem::TCmsContactField>& aContactFieldTypesFilter,
        const RPointerArray<CCmsContactField>& aItemArray,
        const TCmsContactor::TCMSCommLaunchFlag aCommLaunchFlag ) :
    CCMSOperationHandlerBase( aContactFieldTypesFilter, aItemArray, aCommLaunchFlag )
    {
    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSVideoCallOperation::CCMSVideoCallOperation()"));

    return;
    }

// --------------------------------------------------------------------------
// CCMSVideoCallOperation::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CCMSVideoCallOperation::ConstructL()
    {
    BaseConstructL ();
    return;
    }

// ----------------------------------------------------------
// CCMSVideoCallOperation::ExecuteLD
// ----------------------------------------------------------
//
void CCMSVideoCallOperation::ExecuteLD()
    {
    CMS_DP (KCMSContactorLoggerFile, CMS_L ( "CCMSVideoCallOperation::ExecuteLD()"));

    // "D" function semantics
    CleanupStack::PushL ( this);

    PrepareDataL (R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT,
            R_QTN_CCA_POPUP_VIDEO_CALL, CCmsContactFieldItem::ECmsDefaultTypeVideoNumber);

    //Execute the operation
    TRAPD(err, StartOperationL(LaunchDialogL()))
    if ( KErrNone != err )
        {
        CMS_DP(KCMSContactorLoggerFile, CMS_L( "CCMSVideoCallOperation::ExecuteLD(): Err: %d"),err);
        }

    CleanupStack::PopAndDestroy (this);
    CMS_DP(KCMSContactorLoggerFile, CMS_L( "CCMSVideoCallOperation::ExecuteLD(): Done."));
    return;
    }

// ----------------------------------------------------------
// CCMSVideoCallOperation::StartOperationL
// ----------------------------------------------------------
//
void CCMSVideoCallOperation::StartOperationL(
        const CCmsContactFieldItem& aContactFieldItem)
    {       
    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSVideoCallOperation::StartOperationL()"));

    __ASSERT_DEBUG (NULL != &aContactFieldItem,
                        Panic (EPanicPreCond_StartOperationL));

    CCmsPhoneCall::ExecuteL (aContactFieldItem.Data (), CCmsPhoneCall::ECmsCallTypeVideo);

    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSVideoCallOperation::StartOperationL(): Done."));
    return;
    }

// End of file
