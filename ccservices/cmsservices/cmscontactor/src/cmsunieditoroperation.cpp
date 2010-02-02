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
* Description:  Unieditor operation. 
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
        _LIT(KPanicText, "CCMSUniEditorOperation");
        User::Panic (KPanicText, aReason);
        }
#endif // _DEBUG


    } /// namespace


// ================= MEMBER FUNCTIONS =======================
//

// ----------------------------------------------------------
// CCMSUniEditorOperation::NewL
// ----------------------------------------------------------
//
CCMSUniEditorOperation* CCMSUniEditorOperation::NewL(
        const RArray<CCmsContactFieldItem::TCmsContactField>& aContactFieldTypesFilter,
        const RPointerArray<CCmsContactField>& aItemArray,
        const TCmsContactor::TCMSCommLaunchFlag aCommLaunchFlag )
    {
    CCMSUniEditorOperation* self = new (ELeave) CCMSUniEditorOperation( 
        aContactFieldTypesFilter, 
        aItemArray,
        aCommLaunchFlag );
    CleanupStack::PushL ( self);
    self->ConstructL ();
    CleanupStack::Pop ( self);
    return self;
    }

// ----------------------------------------------------------
// CCMSUniEditorOperation::CCMSUniEditorOperation
// ----------------------------------------------------------
//
inline CCMSUniEditorOperation::CCMSUniEditorOperation(
        const RArray<CCmsContactFieldItem::TCmsContactField>& aContactFieldTypesFilter,
        const RPointerArray<CCmsContactField>& aItemArray,
        const TCmsContactor::TCMSCommLaunchFlag aCommLaunchFlag ) :
    CCMSOperationHandlerBase(aContactFieldTypesFilter, aItemArray, aCommLaunchFlag)
    {
    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSUniEditorOperation::CCMSUniEditorOperation()"));

    return;
    }

// --------------------------------------------------------------------------
// CCMSUniEditorOperation::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CCMSUniEditorOperation::ConstructL()
    {
    BaseConstructL ();
    return;
    }

// ----------------------------------------------------------
// CCMSUniEditorOperation::ExecuteLD
// ----------------------------------------------------------
//
void CCMSUniEditorOperation::ExecuteLD()
    {
    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSUniEditorOperation::ExecuteLD()"));

    // "D" function semantics
    CleanupStack::PushL ( this);

    PrepareDataL (R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT,
            R_QTN_CCA_POPUP_MESSAGE, CCmsContactFieldItem::ECmsDefaultTypeMms);

    //Execute the operation
    TRAPD(err, StartOperationL(LaunchDialogL()))
    if ( KErrNone != err )
        {
        CMS_DP(KCMSContactorLoggerFile, CMS_L( "CCMSUniEditorOperation::ExecuteLD(): Err: %d"),err);
        }

    CleanupStack::PopAndDestroy (this);

    CMS_DP(KCMSContactorLoggerFile, CMS_L( "CCMSUniEditorOperation::ExecuteLD(): Done."));
    return;
    }

// ----------------------------------------------------------
// CCMSUniEditorOperation::StartOperationL
// ----------------------------------------------------------
//
void CCMSUniEditorOperation::StartOperationL(
        const CCmsContactFieldItem& aContactFieldItem)
    {
    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSUniEditorOperation::StartOperationL()"));

    __ASSERT_DEBUG (NULL != &aContactFieldItem,
                        Panic (EPanicPreCond_StartOperationL));

    TDesC* namestring = GetNameStringLC ();

    CCmsMsgEditors::ExecuteL (KSenduiMtmUniMessageUid,
            aContactFieldItem.Data (), *namestring);

    CleanupStack::PopAndDestroy (namestring);

    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSUniEditorOperation::StartOperationL(): Done."));
    return;
    }

// End of file


