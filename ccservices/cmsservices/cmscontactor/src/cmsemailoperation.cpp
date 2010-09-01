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
* Description:  Email operation. 
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
        _LIT(KPanicText, "CCMSEmailOperation");
        User::Panic (KPanicText, aReason);
        }
#endif // _DEBUG


    } /// namespace


// ================= MEMBER FUNCTIONS =======================
//

// ----------------------------------------------------------
// CCMSEmailOperation::NewL
// ----------------------------------------------------------
//
CCMSEmailOperation* CCMSEmailOperation::NewL(
        const RArray<CCmsContactFieldItem::TCmsContactField>& aContactFieldTypesFilter,
        const RPointerArray<CCmsContactField>& aItemArray,
        const TCmsContactor::TCMSCommLaunchFlag aCommLaunchFlag )
    {
    CCMSEmailOperation* self = new (ELeave) CCMSEmailOperation(
        aContactFieldTypesFilter, 
        aItemArray,
        aCommLaunchFlag );
    CleanupStack::PushL ( self);
    self->ConstructL ();
    CleanupStack::Pop ( self);
    return self;
    }

// ----------------------------------------------------------
// CCMSEmailOperation::CCMSEmailOperation
// ----------------------------------------------------------
//
inline CCMSEmailOperation::CCMSEmailOperation(
        const RArray<CCmsContactFieldItem::TCmsContactField>& aContactFieldTypesFilter,
        const RPointerArray<CCmsContactField>& aItemArray,
        const TCmsContactor::TCMSCommLaunchFlag aCommLaunchFlag ) :
    CCMSOperationHandlerBase(aContactFieldTypesFilter, aItemArray, aCommLaunchFlag)
    {
    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSEmailOperation::CCMSEmailOperation()"));

    return;
    }

// --------------------------------------------------------------------------
// CCMSEmailOperation::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CCMSEmailOperation::ConstructL()
    {
    BaseConstructL ();
    return;
    }

// ----------------------------------------------------------
// CCMSEmailOperation::ExecuteLD
// ----------------------------------------------------------
//
void CCMSEmailOperation::ExecuteLD()
    {
    CMS_DP (KCMSContactorLoggerFile, CMS_L ( "CCMSEmailOperation::ExecuteLD()"));

    // "D" function semantics
    CleanupStack::PushL ( this);

    PrepareDataL (R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT,
            R_QTN_CCA_POPUP_EMAIL, CCmsContactFieldItem::ECmsDefaultTypeEmail);

    //Execute the operation
    TRAPD(err, StartOperationL(LaunchDialogL()))
    if ( KErrNone != err )
        {
        CMS_DP(KCMSContactorLoggerFile, CMS_L( "CCMSEmailOperation::ExecuteLD(): Err: %d"),err);
        }

    CleanupStack::PopAndDestroy (this);
    CMS_DP(KCMSContactorLoggerFile, CMS_L( "CCMSEmailOperation::ExecuteLD(): Done."));
    return;
    }

// ----------------------------------------------------------
// CCMSEmailOperation::StartOperationL
// ----------------------------------------------------------
//
void CCMSEmailOperation::StartOperationL(
        const CCmsContactFieldItem& aContactFieldItem)
    {
    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSEmailOperation::StartOperationL()"));

    __ASSERT_DEBUG (NULL != &aContactFieldItem,
                        Panic (EPanicPreCond_StartOperationL));

    TDesC* namestring = GetNameStringLC ();

    CCmsMsgEditors::ExecuteL (KSenduiMtmSmtpUid, aContactFieldItem.Data (),
            *namestring);

    CleanupStack::PopAndDestroy (namestring);

    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSEmailOperation::StartOperationL(): Done."));
    return;
    }

// End of file


