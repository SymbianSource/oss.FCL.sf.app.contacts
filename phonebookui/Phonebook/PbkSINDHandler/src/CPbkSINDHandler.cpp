/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for CPbkSINDHandler.
*
*/


// INCLUDE FILES
#include "CPbkSINDHandler.h"
#include <VuiVoiceRecogDefs.h>  // KVoiceDialContext
#include <nssvascoreconstant.h>
#include <flogger.h>

#include "TPbkContactItemField.h"

/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS
const TInt KVoiceTagFieldIdElement = 1;

enum TPanicCode
    {
    EPanicContextNULL = 1,
    EPanicDBManagerCreationFailed,
    EPanicPreCond_NewL,
    EPanicGetContextFailed
    };

// ==================== LOCAL FUNCTIONS ====================
void Panic(TPanicCode aPanicCode)
    {
    _LIT(KPanicText, "PbkSINDHandler");
    User::Panic(KPanicText, aPanicCode);
    };
} // namespace


// ================= MEMBER FUNCTIONS =======================

inline CPbkSINDHandler::CPbkSINDHandler()
    {
    }

inline void CPbkSINDHandler::ConstructL()
    {
    TInt ret = InitializeL();
    if ( ret == KErrNone )
        {
        iInitialized = ETrue;
        }
    else
        {
        iInitialized = EFalse;
        }

    }

TInt CPbkSINDHandler::InitializeL()
    {
    // if the iVasDbManager exists, the initialization has been failed in first
    // try, and some cleaning has to be done.
    if ( iVasDbManager )
        {
        delete iVasDbManager;
        iVasDbManager = NULL;
        delete iWait;
        delete iContext;
        iWait = NULL;
        iVasDbManager = NULL;
        iContextManager = NULL;
        iTagManager = NULL;
        }

    // create Vas db manager
    iVasDbManager = CNssVASDBMgr::NewL();

    TInt err = iVasDbManager->InitializeL();

    if ( err )
        {
        // DbManager returns own error codes that are success or
        // failed, so better to return just KErrGeneral on failure
        return KErrGeneral;
        }

    // Ownership not transferred
    iContextManager = iVasDbManager->GetContextMgr();
    // Ownership not transferred
    iTagManager = iVasDbManager->GetTagMgr();

    iWait = new (ELeave) CActiveSchedulerWait();

    return KErrNone;
    }

CPbkSINDHandler* CPbkSINDHandler::NewL()
    {
    CPbkSINDHandler* self = new (ELeave) CPbkSINDHandler;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CPbkSINDHandler::~CPbkSINDHandler()
    {
    delete iVasDbManager;
    delete iWait;
    delete iContext;
    iContextManager = NULL;
    iTagManager = NULL;
    }

TInt CPbkSINDHandler::VoiceTagField(TContactItemId aContactId)
    {
    if ( !iInitialized )
        {
        // something has failed on initialization phase on first time,
        // so let's try again.
        TInt ret(KErrNone);
        TRAPD(err, ret = InitializeL());
        if ( ret == KErrNone && err == KErrNone)
            {
            iInitialized = ETrue;
            }
        else
            {
            iInitialized = EFalse;
            // cannot continue, to be tried again on next time.
            return KErrGeneral;
            }
        }

    // if the context has not been received yet, this is the
    // first time this method is called.
    if ( !iContext )
        {
        // following call is aynchronous, completed in
        // GetContextCompleted or GetContextFailed

        TInt err = iContextManager->GetContext( this, KVoiceDialContext );
        if ( err == TNssVasCoreConstant::EVasSuccess )
            {
            if ( iWait && !iWait->IsStarted() )
                {
                // Let's synchronize GetContext-method.
                iWait->Start();
                if ( !iContext )
                    {
                    // the context getting has failed.
                    // cannot continue
                    return KErrGeneral;
                    }
                }
            else
                {
                return KErrGeneral;
                }
            }
        else
            {
            return KErrGeneral;
            }
        }

    // Last parameter = position of contact id in RRD data in VAS = 0
    iErr = iTagManager->GetTagList( this, iContext, aContactId, 0);
    if ( iErr )
        {
        // GetTagList returns TNssVASErrorCodes that are positive as well as,
        // the voice tagged field ids. So those can not be used as return
        // value, return KErrGeneral.
        return KErrGeneral;
        }

    // Let's synchronize the GetTagList call
    if ( iWait && !iWait->IsStarted() )
        {
        iWait->Start();
        }
    else
        {
        return KErrGeneral;
        }

    // iErr is modifed if the GetTagList fails.
    if ( iErr == KErrNone )
        {
        return iVoiceTaggedFieldId;
        }
    else
        {
        // The iErr is positive on failure case, as well as the voicetagged field
        // identifier, so return KErrGeneral.
        return KErrGeneral;
        }
    }

void CPbkSINDHandler::GetContextCompleted(MNssContext* aContext)
    {
    __ASSERT_ALWAYS( aContext != NULL , Panic(EPanicContextNULL));
    // ownership is transferred
    iContext = aContext;

    // if the iWait has been started, the completion of GetContext method
    // has been synchronized in VoiceTagField - method. Otherwise
    // the completion has happened in background.
    if ( iWait && iWait->IsStarted() )
        {
        iWait->AsyncStop();
        }
    }

void CPbkSINDHandler::GetContextListCompleted(MNssContextListArray* /*aContextList*/)
    {
    // never comes here, defined as pure virtual in MNssGetContextClient.
    }

void CPbkSINDHandler::GetContextFailed(TNssGetContextClientFailCode aFailCode)
    {
    iErr = aFailCode;
    // if the get context has been synchronized:
    if ( iWait && iWait->IsStarted() )
        {
        iWait->AsyncStop();
        }
    }

void CPbkSINDHandler::GetTagListCompleted(MNssTagListArray* aTagList)
    {
    iVoiceTaggedFieldId = KErrNotFound;
    if ( aTagList->Count() != 0 )
        {
        MNssTag* tag = aTagList->At(0);

        if (tag)
            {
            // RRD data contains contactid as first element and field id as second element
            iVoiceTaggedFieldId = tag->RRD()->IntArray()->At( KVoiceTagFieldIdElement );
            }
        }

    // reset and delete the received tag list.
    aTagList->ResetAndDestroy();
    delete aTagList;
    aTagList = NULL;

    // continue with VoiceTagField-method.
    if ( iWait && iWait->IsStarted() )
        {
        iWait->AsyncStop();
        }
    }


void CPbkSINDHandler::GetTagFailed(TNssGetTagClientFailCode aFailCode)
    {
    iErr = aFailCode;
    if ( iWait && iWait->IsStarted() )
        {
        iWait->AsyncStop();
        }
    }

// End of file
