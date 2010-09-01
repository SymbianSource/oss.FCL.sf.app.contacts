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
* Description:  VOIP Call operation.
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
        _LIT(KPanicText, "CCMSVOIPCallOperation");
        User::Panic (KPanicText, aReason);
        }
#endif // _DEBUG


    } /// namespace


// ================= MEMBER FUNCTIONS =======================
//

// ----------------------------------------------------------
// CCMSVOIPCallOperation::NewL
// ----------------------------------------------------------
//
CCMSVOIPCallOperation* CCMSVOIPCallOperation::NewL(
        const RArray<CCmsContactFieldItem::TCmsContactField>& aContactFieldTypesFilter,
        const RPointerArray<CCmsContactField>& aItemArray,
        const TCmsContactor::TCMSCommLaunchFlag aCommLaunchFlag )
    {
    CCMSVOIPCallOperation* self = new (ELeave) CCMSVOIPCallOperation(
        aContactFieldTypesFilter, 
        aItemArray,
        aCommLaunchFlag );
    CleanupStack::PushL ( self);
    self->ConstructL ();
    CleanupStack::Pop ( self);
    return self;
    }

// ----------------------------------------------------------
// CCMSVOIPCallOperation::CCMSVOIPCallOperation
// ----------------------------------------------------------
//
inline CCMSVOIPCallOperation::CCMSVOIPCallOperation(
        const RArray<CCmsContactFieldItem::TCmsContactField>& aContactFieldTypesFilter,
        const RPointerArray<CCmsContactField>& aItemArray,
        const TCmsContactor::TCMSCommLaunchFlag aCommLaunchFlag ) :
    CCMSOperationHandlerBase(aContactFieldTypesFilter, aItemArray, aCommLaunchFlag)
    {
    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSVOIPCallOperation::CCMSVOIPCallOperation()"));

    return;
    }

// --------------------------------------------------------------------------
// CCMSVOIPCallOperation::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CCMSVOIPCallOperation::ConstructL()
    {
    BaseConstructL ();
    return;
    }

// ----------------------------------------------------------
// CCMSVOIPCallOperation::ExecuteLD
// ----------------------------------------------------------
//
void CCMSVOIPCallOperation::ExecuteLD()
    {
    CMS_DP (KCMSContactorLoggerFile, CMS_L ( "CCMSVOIPCallOperation::ExecuteLD()"));

    // "D" function semantics
    CleanupStack::PushL ( this);

    PrepareDataL (R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT,
            R_QTN_CCA_POPUP_VOIP_CALL, CCmsContactFieldItem::ECmsDefaultTypeVoIP);

    //Execute the operation
    TRAPD(err, StartOperationL(LaunchDialogL()))
    if ( KErrNone != err )
        {
        CMS_DP(KCMSContactorLoggerFile, CMS_L( "CCMSVOIPCallOperation::ExecuteLD(): Err: %d"),err);
        }

    CleanupStack::PopAndDestroy (this);
    CMS_DP(KCMSContactorLoggerFile, CMS_L( "CCMSVOIPCallOperation::ExecuteLD(): Done."));
    return;
    }

// ----------------------------------------------------------
// CCMSVOIPCallOperation::StartOperationL
// ----------------------------------------------------------
//
void CCMSVOIPCallOperation::StartOperationL(
        const CCmsContactFieldItem& aContactFieldItem)
    {       
    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSVOIPCallOperation::StartOperationL()"));

    __ASSERT_DEBUG (NULL != &aContactFieldItem,
                        Panic (EPanicPreCond_StartOperationL));

    CCmsPhoneCall::ExecuteL (aContactFieldItem.Data (), CCmsPhoneCall::ECmsCallTypeVoIP);

    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSVOIPCallOperation::StartOperationL(): Done."));
    return;
    }

// End of file
