/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 select field dialog for assign service.
*
*/


#include "CPbk2AssignSelectFieldDlg.h"

// Phonebook 2
#include "MPbk2SelectFieldProperty.h"
#include "CPbk2KeyEventDealer.h"
#include <MPbk2ExitCallback.h>

// System includes
#include <avkon.hrh>
#include <eikcmobs.h>
#include <eikappui.h>

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_ResetWhenDestroyed = 1
    };

void Panic( TInt aReason )
    {
    _LIT( KPanicText, "CPbk2AssignSelectFieldDlg" );
    User::Panic( KPanicText, aReason );
    }

#endif // _DEBUG

} /// namespace

// --------------------------------------------------------------------------
// CPbk2AssignSelectFieldDlg::CPbk2AssignSelectFieldDlg
// --------------------------------------------------------------------------
//
CPbk2AssignSelectFieldDlg::CPbk2AssignSelectFieldDlg
        ( MPbk2ExitCallback& aExitCallback ):
            iExitCallback( aExitCallback )
    {
    }

// --------------------------------------------------------------------------
// CPbk2AssignSelectFieldDlg::~CPbk2AssignSelectFieldDlg
// --------------------------------------------------------------------------
//
CPbk2AssignSelectFieldDlg::~CPbk2AssignSelectFieldDlg()
    {
    // Tells ExecuteLD this object has been destroyed
    if ( iDestroyedPtr )
        {
        *iDestroyedPtr = ETrue;
        }

    if ( iSelfPtr )
        {
        *iSelfPtr = NULL;
        }

    delete iDealer;
    }

// --------------------------------------------------------------------------
// CPbk2AssignSelectFieldDlg::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2AssignSelectFieldDlg::ConstructL()
    {
    iDealer = CPbk2KeyEventDealer::NewL( *this );
    }

// --------------------------------------------------------------------------
// CPbk2AssignSelectFieldDlg::NewL
// --------------------------------------------------------------------------
//
CPbk2AssignSelectFieldDlg* CPbk2AssignSelectFieldDlg::NewL
        ( MPbk2ExitCallback& aExitCallback )
    {
    CPbk2AssignSelectFieldDlg* self =
        new ( ELeave ) CPbk2AssignSelectFieldDlg( aExitCallback );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2AssignSelectFieldDlg::ExecuteLD
// --------------------------------------------------------------------------
//
TInt CPbk2AssignSelectFieldDlg::ExecuteLD
        ( MPbk2SelectFieldProperty& aProperty )
    {
    CleanupStack::PushL( this );

    iProperty = &aProperty;

    aProperty.PrepareL();

    TBool thisDestroyed = EFalse;
    iDestroyedPtr = &thisDestroyed;

    TInt result = aProperty.ExecuteL();

    if ( thisDestroyed )
        {
        // This object has been destroyed
        CleanupStack::Pop( this );
        }
    else
        {
        iProperty = NULL;
        // If end key has been pressed the property dialog
        // returns with KErrCancel. If also abort is ordered we
        // must change the return value to KErrAbort
        if ( iExitRecord.IsSet( EAbortOrdered ) && result == KErrCancel )
            {
            result = KErrAbort;
            }

        CleanupStack::PopAndDestroy( this );
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2AssignSelectFieldDlg::RequestExitL
// --------------------------------------------------------------------------
//
void CPbk2AssignSelectFieldDlg::RequestExitL( TInt aCommandId )
    {
    if ( iProperty )
        {
        iProperty->Cancel( aCommandId );
        iProperty = NULL;
        }

    if ( aCommandId == EEikBidCancel )
        {
        // End key exit
        iExitRecord.Set( EAbortApproved );
        ExitApplication( EAknCmdExit );
        }
    else
        {
        delete this;
        }
    }

// --------------------------------------------------------------------------
// CPbk2AssignSelectFieldDlg::ForceExit
// --------------------------------------------------------------------------
//
void CPbk2AssignSelectFieldDlg::ForceExit()
    {
    if ( iProperty )
        {
        // Command id does not matter much, it shouldn't however be
        // EEikBidCancel since that indicates end key and causes
        // application exit
        iProperty->Cancel( EAknSoftkeyClose );
        iProperty = NULL;
        }

    delete this;
    }

// --------------------------------------------------------------------------
// CPbk2AssignSelectFieldDlg::ResetWhenDestroyed
// --------------------------------------------------------------------------
//
void CPbk2AssignSelectFieldDlg::ResetWhenDestroyed
        ( MPbk2DialogEliminator** aSelfPtr )
    {
    __ASSERT_DEBUG( !aSelfPtr || *aSelfPtr == this,
        Panic( EPanicPreCond_ResetWhenDestroyed ) );

    iSelfPtr = aSelfPtr;
    }

// --------------------------------------------------------------------------
// CPbk2AssignSelectFieldDlg::Pbk2ProcessKeyEventL
// --------------------------------------------------------------------------
//
TBool CPbk2AssignSelectFieldDlg::Pbk2ProcessKeyEventL
        ( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TBool ret = EFalse;

    if ( aType == EEventKey && aKeyEvent.iCode == EKeyEscape )
        {
        // We need to catch end key pressed events for quering
        // the client is it ok to exit
        iExitRecord.Set( EAbortOrdered );

        // If exit callback returned EFalse, the exit is cancelled
        if ( !iExitCallback.OkToExitL( EEikBidCancel ) )
            {
            iExitRecord.Clear( EAbortApproved );
            }

        ret = ETrue;
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2AssignSelectFieldDlg::ExitApplication
// --------------------------------------------------------------------------
//
void CPbk2AssignSelectFieldDlg::ExitApplication( TInt aCommandId )
    {
    if ( iExitRecord.IsSet( EAbortOrdered ) &&
         iExitRecord.IsSet( EAbortApproved ) )
        {
        CEikAppUi* appUi = static_cast<CEikAppUi*>
            ( CCoeEnv::Static()->AppUi() );
        MEikCommandObserver* cmdObs =
            static_cast<MEikCommandObserver*>( appUi );

        // Dialog is closed so there is nothing to do if
        // ProcessCommandL leaves. Of course it shouldn't leave in
        // practice because it's exit command.
        TRAP_IGNORE( cmdObs->ProcessCommandL( aCommandId ) );
        }
    }

// End of File
