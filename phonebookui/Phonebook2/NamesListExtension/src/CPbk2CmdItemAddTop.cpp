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
* Description:  UI control Command item for "Add to Top".
*
*/


#include "CPbk2CmdItemAddTop.h"

// Pbk2
#include "CPbk2AddFavoritesVisibility.h"
#include <Pbk2Commands.hrh>
#include <MPbk2AppUi.h>
#include <Pbk2InternalUID.h>
#include "Pbk2NamesListExUID.h"
#include <pbk2exnameslistres.rsg>
#include <TPbk2IconId.h>
#include "pbk2nameslistexiconid.hrh"
#include "pbk2contactuicontroldoublelistboxextension.h"

// General
#include <StringLoader.h>

CPbk2CmdItemAddTop* CPbk2CmdItemAddTop::NewLC()
	{
	CPbk2CmdItemAddTop* self = new (ELeave) CPbk2CmdItemAddTop;
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

void CPbk2CmdItemAddTop::ConstructL()
    {
    iAddFavoritesVisibility = 
        CPbk2AddFavoritesVisibility::NewL(CommandId(), 
        Phonebook2::Pbk2AppUi()->ApplicationServices());

    // Formatting is automatically done by LoadL
    iText = StringLoader::LoadL( R_QTN_PHOB_ADD_TOP_CONTACTS );
    }
    
CPbk2CmdItemAddTop::CPbk2CmdItemAddTop() 
	{
	}

CPbk2CmdItemAddTop::~CPbk2CmdItemAddTop()
	{
    delete iText; // Fix ETKI-7NADZC
	delete iAddFavoritesVisibility;
	}

TPtrC CPbk2CmdItemAddTop::NameForUi() const
	{
	return iText->Des();
	}

TAny* CPbk2CmdItemAddTop::ControlCmdItemExtension( TUid aExtensionUid )
    {
    if( aExtensionUid == TUid::Uid( KPbk2ControlCmdItemExtensionUID ) ) 
        {
        return static_cast<MPbk2DoubleListboxCmdItemExtension*>( this );
        }
    return NULL;
    }

void CPbk2CmdItemAddTop::FormatDataL( MPbk2DoubleListboxDataElement& aDataElement )
    {
    TPbk2IconId iconId ( TUid::Uid( KPbk2NamesListExtPluginUID2 ),
                         EPbk2EceAddToFovoritesThumbnailIconId );
    aDataElement.SetIconId( MPbk2DoubleListboxDataElement::EThumbnail, iconId );
    }

const MVPbkContactLink* CPbk2CmdItemAddTop::Link()
    {
    return NULL;
    }

TBool CPbk2CmdItemAddTop::IsEnabled() const
	{
	return iAddFavoritesVisibility->Visibility();	
	}

void CPbk2CmdItemAddTop::SetEnabled( TBool aEnabled )
	{
	iAddFavoritesVisibility->SetVisibility( aEnabled );	
	}

TInt CPbk2CmdItemAddTop::CommandId() const
	{
	return EPbk2CmdAddFavourites;
	}

void CPbk2CmdItemAddTop::SetVisibilityObserver(MPbk2CmdItemVisibilityObserver* aObserver)
    {
        // iAddFavoritesVisibility can have only 1 observer
        // setting an observer again would panic
        iAddFavoritesVisibility->SetVisibilityObserver(aObserver);
    }
// end of file
