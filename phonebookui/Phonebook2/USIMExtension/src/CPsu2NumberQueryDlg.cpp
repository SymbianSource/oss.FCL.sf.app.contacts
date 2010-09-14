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
* Description:  A dialog for asking fixed dialling phone number.
*
*/



// INCLUDE FILES
#include "CPsu2NumberQueryDlg.h"
#include <pbk2usimuires.rsg>
#include <pbk2uicontrols.rsg>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPsu2NumberQueryDlg::CPsu2NumberQueryDlg
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPsu2NumberQueryDlg::CPsu2NumberQueryDlg( 
    TDes& aNumberBuffer )
    : CAknTextQueryDialog( aNumberBuffer )
    {
    }

// -----------------------------------------------------------------------------
// CPsu2NumberQueryDlg::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPsu2NumberQueryDlg::ConstructL( const TDesC& aSourceNumber )
    {
    TDes& dialogData = Text();
    dialogData.Copy( aSourceNumber.Left( dialogData.MaxLength() ) );
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( 
        dialogData );
    }

// -----------------------------------------------------------------------------
// CPsu2NumberQueryDlg::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPsu2NumberQueryDlg* CPsu2NumberQueryDlg::NewL( 
    const TDesC& aSourceNumber, TDes& aNumberBuffer )
    {
    CPsu2NumberQueryDlg* self = 
        new( ELeave ) CPsu2NumberQueryDlg( aNumberBuffer );
    CleanupStack::PushL( self );
    self->ConstructL( aSourceNumber );
    CleanupStack::Pop( self );
    return self;
    }


// Destructor
CPsu2NumberQueryDlg::~CPsu2NumberQueryDlg()
    {
    }

// -----------------------------------------------------------------------------
// CPsu2NumberQueryDlg::UpdateLeftSoftKeyL
// -----------------------------------------------------------------------------
//
void CPsu2NumberQueryDlg::UpdateLeftSoftKeyL()
    {
    const TChar questionMark('?');

    CAknQueryControl* control = QueryControl();
    if ( control )
        {
        TDes& dialogData = Text(); 
        control->GetText( dialogData );       
        // Number is ok if it's not empty and it doesn't contain question mark
        if ( dialogData.Length() > 0 && 
             dialogData.Locate( questionMark ) == KErrNotFound )
            {
            iNumberAccepted = ETrue;
            MakeLeftSoftkeyVisible( ETrue );
            MakeMiddleSoftkeyVisibleL( ETrue );
            }
        else
            {
            iNumberAccepted = EFalse;
            MakeLeftSoftkeyVisible( EFalse );
            MakeMiddleSoftkeyVisibleL( EFalse );
            }
        }
    }

// -----------------------------------------------------------------------------
// CPsu2NumberQueryDlg::OfferKeyEventL
// -----------------------------------------------------------------------------
//
TKeyResponse CPsu2NumberQueryDlg::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
    TEventCode aType )
    {
    TKeyResponse ret;
    if ( aType == EEventKey && aKeyEvent.iCode == EKeyPhoneSend )
        {
        if( iNumberAccepted )
            {
            TryExitL( EAknSoftkeyOk );
            }     
        ret = EKeyWasConsumed;
        }
    else
        {
        ret = CAknTextQueryDialog::OfferKeyEventL( aKeyEvent, aType );
        }
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CPsu2NumberQueryDlg::ExecuteLD
// -----------------------------------------------------------------------------
//
TBool CPsu2NumberQueryDlg::ExecuteLD()
    {
    TBool result = ETrue;
    // A stack reference to data must be used because this instance is deleted
    // after ExecuteLD
    TDes& dialogData = Text();
    SetMaxLength( Text().MaxLength() );
    if ( CAknTextQueryDialog::ExecuteLD( R_PSU2_FDN_NUMBERQUERY_DIALOG ) )
        {
        AknTextUtils::ConvertDigitsTo( dialogData, EDigitTypeWestern );
        }
    else
        {
        result = EFalse;
        }
    return result;
    }
    
// -----------------------------------------------------------------------------
// CPsu2NumberQueryDlg::MakeMiddleSoftkeyVisibleL
// -----------------------------------------------------------------------------
//    
void CPsu2NumberQueryDlg::MakeMiddleSoftkeyVisibleL( TBool aVisible )
	{
	TInt resId( KErrNotFound );
	if ( aVisible )
		{
		resId = R_PBK2_SOFTKEYS_CALL_CANCEL_CALL;
		}
	else
		{
		resId = R_PBK2_SOFTKEYS_EMPTY_CANCEL_EMPTY;
		}
	ButtonGroupContainer().SetCommandSetL( resId );
	ButtonGroupContainer().DrawDeferred();
	}
	
//  End of File
