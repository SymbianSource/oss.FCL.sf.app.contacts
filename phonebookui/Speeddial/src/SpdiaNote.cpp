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
* Description:     Displays the phonenumber of a speed dial entry
*                when 'View number' is chosen from the menu.
*
*/






// INCLUDE FILES
#include "SpdiaNote.h" 

// ================= MEMBER FUNCTIONS =======================
#include <aknnotecontrol.h>
#include <AknUtils.h>
#include <AknBidiTextUtils.h>
#include "Speeddial.laf" 

CSpdiaNoteDialog::CSpdiaNoteDialog()
    {
    }

CSpdiaNoteDialog::CSpdiaNoteDialog(CSpdiaNoteDialog** aDialog) 
            : iDialog(aDialog)
    {
    }

CSpdiaNoteDialog::~CSpdiaNoteDialog()
    {
    delete iPhoneNumberString;

    if (iDialog)
        {
        *iDialog = NULL;
        }
    }
void CSpdiaNoteDialog::SetPhoneNumberL(const TDesC& aNumber)
    {
    delete iPhoneNumberString;
    iPhoneNumberString = NULL;
    iPhoneNumberString = aNumber.AllocL();
    }

void CSpdiaNoteDialog::ClipPhoneNumberFromBeginningL(TInt aWidth, TInt16 fontId)
    {
    const CFont* font = iEikonEnv->NormalFont();

    if ( AknLayoutUtils::Variant() == EApacVariant )
        {
        font = ApacPlain16();
        }
	HBufC* buf = iPhoneNumberString->AllocLC();
    TPtr ptr = buf->Des();
	if( AknLayoutUtils::ScalableLayoutInterfaceAvailable() )
		{
		if ( AknTextUtils::ClipToFit(
            ptr, 
            *AknLayoutUtils::FontFromId(fontId), 
            aWidth, 
            AknTextUtils::EClipFromBeginning ) )
		    {
			NoteControl()->SetTextL( ptr, 
				SDM_VIEW_NUMBER_LINE_NUMBER_FOR_PHONE_NUMBER );
			}

		}
	else
		{
    if ( AknTextUtils::ClipToFit(
            ptr, 
            *font, 
            aWidth, 
            AknTextUtils::EClipFromBeginning ) )
        {
        NoteControl()->SetTextL( ptr, 
            SDM_VIEW_NUMBER_LINE_NUMBER_FOR_PHONE_NUMBER );
        }
		}
    CleanupStack::PopAndDestroy(); // buf
    }
