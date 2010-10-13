/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:       
*           Methods for Email address selection dialog.
*
*/








// INCLUDE FILES

#include    <CPbkContactItem.h>
#include    <StringLoader.h>        // StringLoader
#include    <MT_CPbkPhoneNumberSelectBase.rsg>
#include    <eikon.hrh>

#include    "CPhoneNumberSelectBase.h"

// ================= MEMBER FUNCTIONS =======================

CPhoneNumberSelectBase::CPhoneNumberSelectBase( CMTPbkPhoneNumberSelectBase& aSelectBase )
: iSelectBase( &aSelectBase )
	{
	}

CPhoneNumberSelectBase::TParams::TParams
        (const CPbkContactItem& aContact) :
        TBaseParams(aContact,aContact.DefaultPhoneNumberField() )
    {
    }

TBool CPhoneNumberSelectBase::ExecuteLD(TParams& aParams)
    {
    return CPbkAddressSelect::ExecuteLD(aParams);
    }

CPhoneNumberSelectBase::~CPhoneNumberSelectBase()
	{
    delete iQueryTitle;
	}

/**
 * Makes phone number query return selection if Call key is pressed.
 */ 

EXPORT_C TKeyResponse CPhoneNumberSelectBase::PbkControlKeyEventL
    (const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    if (aType == EEventKey && aKeyEvent.iCode == EKeyPhoneSend)
        {
        // Event is Send key, tell field selection dialog to accept current selection
        AttemptExitL(ETrue);
        return EKeyWasConsumed;
        }
    return EKeyWasNotConsumed;
    }

EXPORT_C const TDesC& CPhoneNumberSelectBase::QueryTitleL()
    {
    if ( !iQueryTitle )
        {
        HBufC* title = ContactItem().GetContactTitleL();
        CleanupStack::PushL(title);
        iQueryTitle = StringLoader::LoadL( R_QUERY_TITLE, *title);
        CleanupStack::PopAndDestroy(title);
        }
    return *iQueryTitle;
    }

EXPORT_C TInt CPhoneNumberSelectBase::QuerySoftkeysResource() const
    {
    return R_CBA;
    }


//  End of File
