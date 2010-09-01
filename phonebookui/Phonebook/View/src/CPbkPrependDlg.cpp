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
* Description: 
*       Provides methods for Prepend dialog.
*
*/


// INCLUDE FILES
#include "CPbkPrependDlg.h"  // This class' declaration


// ================= MEMBER FUNCTIONS =======================

inline CPbkPrependDlg::CPbkPrependDlg(TDes& aDataText) :
    CAknTextQueryDialog(aDataText)
    {
    }

EXPORT_C CPbkPrependDlg* CPbkPrependDlg::NewL(TDes& aDataText)
    {
    CPbkPrependDlg* self = new(ELeave) CPbkPrependDlg(aDataText);
    return self;
    }

CPbkPrependDlg::~CPbkPrependDlg()
    {
    }

TBool CPbkPrependDlg::OkToExitL(TInt aButtonId)
    {
    TBool result(EFalse);
    
    if (aButtonId == EKeyPhoneSend)
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

TKeyResponse CPbkPrependDlg::OfferKeyEventL
        (const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    TKeyResponse ret(EKeyWasNotConsumed);
    
    if (aKeyEvent.iCode == EKeyPhoneSend)
        {
        // If send key pressed, exit and make the call (from calling class)
        TryExitL(EKeyPhoneSend);
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
