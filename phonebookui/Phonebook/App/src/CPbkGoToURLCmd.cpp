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
*      Go to URL command object.Opens browser to the URL address
*		saved in contact details.
*
*/


// INCLUDE FILES
#include "CPbkGoToURLCmd.h"
#include <CPbkContactItem.h>
#include <CPbkURLAddressSelect.h>
#include <PbkView.rsg>
#include <SchemeHandler.h>
#include <aknnotewrappers.h>
#include <pbkdebug.h>
// ================= MEMBER FUNCTIONS =======================

// Default constructor
CPbkGoToURLCmd::CPbkGoToURLCmd
        (const CPbkContactItem& aContact, 
        const TPbkContactItemField* aFocusedField) :
		iContact(aContact), 
        iFocusedField(aFocusedField)
	{
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkGoToURLCmd::CPbkGoToURLCmd(0x%x)"), this);
    }


// Static constructor
CPbkGoToURLCmd* CPbkGoToURLCmd::NewL
        (const CPbkContactItem& aContact, 
        const TPbkContactItemField* aFocusedField)
	{
    CPbkGoToURLCmd* self = new(ELeave)
		CPbkGoToURLCmd(aContact, aFocusedField);
    return self;
    }

// Destructor
CPbkGoToURLCmd::~CPbkGoToURLCmd()
    {
	delete iHandler;

    // Tells ExecuteLD this object has been destroyed
    if(iDestroyedPtr)
        {
		*iDestroyedPtr = ETrue;
		}

    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkGoToURLCmd::~CPbkGoToURLCmd(0x%x)"), this);
	}

void CPbkGoToURLCmd::ExecuteLD()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkGoToURLCmd::ExecuteLD(0x%x)"), this); 

   	CleanupStack::PushL(this);

    TBool thisDestroyed = EFalse;
    iDestroyedPtr = &thisDestroyed;

	// Show URL address selection dialog, if necessary
	CPbkURLAddressSelect* selectDlg = new(ELeave) CPbkURLAddressSelect;
	CPbkURLAddressSelect::TParams params(iContact);
    params.SetFocusedField(iFocusedField);
    params.SetUseDefaultDirectly(EFalse);	// no defaults in case of URL

    TPtrC address;
    address.Set(selectDlg->ExecuteLD(params));

    if (address.Length() > 0)
		{
        // All ok, launch browser
		TRAPD(err, DoGoToUrlL(address));
		switch(err)
			{
			case KErrNone:
				{
                // Fix cleanup stack, this object is deleted later
                // in NotifyExit
                CleanupStack::Pop(); // this
                break;
         		}

			case KErrArgument:      // FALLTHROUGH
            case KErrNotFound:
				{
				// The URL was not in valid form
				CAknNoteWrapper * note = new(ELeave) CAknNoteWrapper;
				note->ExecuteLD(R_QTN_PHOB_NOTE_INVALID_URL);
                // Fix cleanup stack
                if (!thisDestroyed)
                    {
                    CleanupStack::PopAndDestroy(); // this
                    }
                else
                    {
                    CleanupStack::Pop(); // this
                    }
				break;
				}

			default:
				{
				// Some other error occurred
				if (thisDestroyed)
                    {
                    // Avoid double deletion
                    CleanupStack::Pop(); // this
                    }
				User::Leave(err);
                break;
				}
			}
		}
    else
        {
        CleanupStack::PopAndDestroy(this);
        }
    }

void CPbkGoToURLCmd::DoGoToUrlL(const TDesC& aUrl)
	{
	delete iHandler;
	iHandler = NULL;

	// Launch browser via SchemeHandler, the following leaves
	// with KErrArgument if the URL is not in valid form
	iHandler = CSchemeHandler::NewL(aUrl);
	iHandler->Observer(this);
	iHandler->HandleUrlStandaloneL();
	}

void CPbkGoToURLCmd::HandleServerAppExit(TInt aReason)
    {
    MAknServerAppExitObserver::HandleServerAppExit(aReason);
    delete this;
    }

//  End of File  
