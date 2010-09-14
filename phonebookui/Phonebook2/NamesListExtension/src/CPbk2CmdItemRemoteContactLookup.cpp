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
* Description:  UI control Command item for "Search from Remote".
*
*/


#include "CPbk2CmdItemRemoteContactLookup.h"

// Pbk2
#include "CPbk2RemoteContactLookupVisibility.h"
#include <Pbk2Commands.hrh>
#include <MPbk2AppUi.h>
#include <pbk2exnameslistres.rsg>

// General
#include <StringLoader.h>

CPbk2CmdItemRemoteContactLookup* CPbk2CmdItemRemoteContactLookup::NewLC()
	{
	CPbk2CmdItemRemoteContactLookup* self = new (ELeave) CPbk2CmdItemRemoteContactLookup;
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

void CPbk2CmdItemRemoteContactLookup::ConstructL()
    {
    iRemoteContactLookupVisibility = 
        CPbk2RemoteContactLookupVisibility::NewL(CommandId(), 
        Phonebook2::Pbk2AppUi()->ApplicationServices());

    // Formatting is automatically done by LoadL
    iText = StringLoader::LoadL( R_QTN_PHOB_SEARCH_FROM_REMOTE );
    }
    
CPbk2CmdItemRemoteContactLookup::CPbk2CmdItemRemoteContactLookup() 
	{
	}

CPbk2CmdItemRemoteContactLookup::~CPbk2CmdItemRemoteContactLookup()
	{
    delete iText; 
	delete iRemoteContactLookupVisibility;
	}

TPtrC CPbk2CmdItemRemoteContactLookup::NameForUi() const
	{
	return iText->Des();
	}

TBool CPbk2CmdItemRemoteContactLookup::IsEnabled() const
	{
    if(iRemoteContactLookupVisibility) 
        {
        return iRemoteContactLookupVisibility->Visibility();	
	    }
    else
	    {
		return EFalse;
		}	
	}

void CPbk2CmdItemRemoteContactLookup::SetEnabled( TBool aEnabled )
	{
    if(iRemoteContactLookupVisibility) 
        {
        iRemoteContactLookupVisibility->SetVisibility( aEnabled );	
	    }
	}

TInt CPbk2CmdItemRemoteContactLookup::CommandId() const
	{
	return EPbk2CmdRcl;
	}

void CPbk2CmdItemRemoteContactLookup::SetVisibilityObserver(MPbk2CmdItemVisibilityObserver* aObserver)
    {
    // iRemoteContactLookupVisibility can have only 1 observer
    // setting an observer again would panic
    if(iRemoteContactLookupVisibility) 
        {
        iRemoteContactLookupVisibility->SetVisibilityObserver(aObserver);
        }
    }
// end of file
