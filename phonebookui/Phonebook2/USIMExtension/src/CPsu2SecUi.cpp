/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Encapsulates the usage of s60 security UI component
*
*/



// INCLUDE FILES
#include "CPsu2SecUi.h"

#include <MVPbkSimPhone.h>
#include <secuisecuritysettings.h> // CSecuritySettings
#include <secui.h> // TSecUi

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPsu2SecUi::CPsu2SecUi
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPsu2SecUi::CPsu2SecUi(MVPbkSimPhone& aPhone)
:   iPhone(aPhone)
    {
    }

// -----------------------------------------------------------------------------
// CPsu2SecUi::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPsu2SecUi::ConstructL()
    {
    TSecUi::InitializeLibL();
    iSecuritySettings = CSecuritySettings::NewL();
    iPhone.OpenL(*this);
    }

// -----------------------------------------------------------------------------
// CPsu2SecUi::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPsu2SecUi* CPsu2SecUi::NewL(MVPbkSimPhone& aPhone)
    {
    CPsu2SecUi* self = new( ELeave ) CPsu2SecUi(aPhone);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CPsu2SecUi::~CPsu2SecUi()
    {
    iPhone.Close(*this);
    delete iSecuritySettings;
    TSecUi::UnInitializeLib();
    }

// -----------------------------------------------------------------------------
// CPsu2SecUi::ActivateFDNL
// -----------------------------------------------------------------------------
//
void CPsu2SecUi::ActivateFDNL()
    {
    TBool isActive = IsFDNActive();
    if ( !isActive )
        {
        iSecuritySettings->SetFdnModeL();
        if ( IsFDNActive() )
            {
            // Activity has been changed which means that PIN2 was inserted
            // correctly
            iPin2Confirmed = ETrue;
            }
        }
    }

// -----------------------------------------------------------------------------
// CPsu2SecUi::DeactivateFDNL
// -----------------------------------------------------------------------------
//
void CPsu2SecUi::DeactivateFDNL()
    {
    TBool isActive = IsFDNActive();
    if ( isActive )
        {
        iSecuritySettings->SetFdnModeL();
        if ( !IsFDNActive() )
            {
            // Activity has been changed which means that PIN2 was inserted
            // correctly
            iPin2Confirmed = ETrue;
            }
        }
    }

// -----------------------------------------------------------------------------
// CPsu2SecUi::ConfirmPin2L
// -----------------------------------------------------------------------------
//
TBool CPsu2SecUi::ConfirmPin2L()
    {
    if ( !iPin2Confirmed )
        {
        iPin2Confirmed = iSecuritySettings->AskPin2L();
        }

    return iPin2Confirmed;
    }

// -----------------------------------------------------------------------------
// CPsu2SecUi::AskPin2L
// -----------------------------------------------------------------------------
//
TBool CPsu2SecUi::AskPin2L()
    {
    return iSecuritySettings->AskPin2L();
    }

// -----------------------------------------------------------------------------
// CPsu2SecUi::IsFDNActive
// -----------------------------------------------------------------------------
//
TBool CPsu2SecUi::IsFDNActive()
    {
    MVPbkSimPhone::TFDNStatus fdnStatus = iPhone.FixedDialingStatus();
    if ( fdnStatus == MVPbkSimPhone::EFdnIsActive ||
         fdnStatus == MVPbkSimPhone::EFdnIsPermanentlyActive )
        {
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CPsu2SecUi::PhoneOpened
// -----------------------------------------------------------------------------
//
void CPsu2SecUi::PhoneOpened(MVPbkSimPhone& /*aPhone*/)
    {
    }

// -----------------------------------------------------------------------------
// CPsu2SecUi::PhoneOpened
// -----------------------------------------------------------------------------
//
void CPsu2SecUi::PhoneError(MVPbkSimPhone& /*aPhone*/,
    TErrorIdentifier /*aIdentifier*/, TInt /*aError*/)
    {
    }

// -----------------------------------------------------------------------------
// CPsu2SecUi::ServiceTableUpdated
// -----------------------------------------------------------------------------
//
void CPsu2SecUi::ServiceTableUpdated(TUint32 /*aServiceTable*/)
    {
    }

// -----------------------------------------------------------------------------
// CPsu2SecUi::FixedDiallingStatusChanged
// -----------------------------------------------------------------------------
//
void CPsu2SecUi::FixedDiallingStatusChanged(
        TInt /*aFDNStatus*/ )
    {
    }

// -----------------------------------------------------------------------------
// CPsu2SecUi::Reset
// -----------------------------------------------------------------------------
//
void CPsu2SecUi::Reset()
    {
    iPin2Confirmed = EFalse;
    }
//  End of File
