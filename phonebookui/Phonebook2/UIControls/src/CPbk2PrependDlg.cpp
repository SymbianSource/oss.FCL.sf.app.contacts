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
* Description: 
*       Provides methods for Prepend dialog.
*
*/


// INCLUDE FILES
#include "CPbk2PrependDlg.h"  // This class' declaration


// ================= MEMBER FUNCTIONS =======================

inline CPbk2PrependDlg::CPbk2PrependDlg(TDes& aDataText) :
    CAknTextQueryDialog(aDataText)
    {
    }

EXPORT_C CPbk2PrependDlg* CPbk2PrependDlg::NewL(TDes& aDataText)
    {
    CPbk2PrependDlg* self = new(ELeave) CPbk2PrependDlg(aDataText);
    return self;
    }

CPbk2PrependDlg::~CPbk2PrependDlg()
    {
    }

TBool CPbk2PrependDlg::OkToExitL(TInt aButtonId)
    {
    TBool result(EFalse);
    
    if (aButtonId == EAknSoftkeyCall)
        {
        // With send key we can exit..
        CAknQueryControl* control = QueryControl();
        if (control)
            {
            // Save the text
            control->GetText(iDataText);
            }

        result = ETrue;
        }
    else
        {
        // ..otherwise, ask from base class
        result = CAknTextQueryDialog::OkToExitL(aButtonId);
        }

    return result;
    }

TKeyResponse CPbk2PrependDlg::OfferKeyEventL
        (const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    TKeyResponse ret(EKeyWasNotConsumed);
    
    // guards to prevent second call 
    if ( ( aType == EEventKeyDown || aType == EEventKeyUp )
        && aKeyEvent.iScanCode == EStdKeyYes )
        {
        return EKeyWasConsumed;
        }
    
    if (aKeyEvent.iCode == EKeyPhoneSend)
        {
        // If send key is pressed and the text is valid ,
        // exit and make the call (from calling class)
        if ( CheckIfEntryTextOk() )
            {
            TryExitL(EAknSoftkeyCall);
            }    
        ret = EKeyWasConsumed;
        }
    else
        {
	    // Let the base class handle the event
	    ret = CAknDialog::OfferKeyEventL(aKeyEvent, aType);
        }

    return ret;
    }

//  End of File  
