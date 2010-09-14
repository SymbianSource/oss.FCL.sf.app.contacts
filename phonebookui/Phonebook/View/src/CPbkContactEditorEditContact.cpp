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
*           Phonebook Contact editor edit contact strategy.
*
*/


// INCLUDE FILES
#include "CPbkContactEditorEditContact.h"
#include <coemain.h>
#include <coehelp.h>
#include <cntdb.h>
#include <eikmenup.h>
#include <bldvariant.hrh>
#include <featmgr.h>
#include "CPbkContactEngine.h"
#include "CPbkContactItem.h"
#include "CPbkDeleteContactQuery.h"
#include <CPbkFFSCheck.h>
#include <pbkview.rsg>
#include <cshelp/phob.hlp.hrh>


/// State flag values 
enum TStateFlag
    {
    EInitialized,
    EContactSaved,
    EModified,
	EContactDeleted
    };

// ================= MEMBER FUNCTIONS =======================

inline CPbkContactEditorEditContact::CPbkContactEditorEditContact
		(CPbkContactEngine& aEngine,
		CPbkContactItem& aContactItem) :
	iEngine(aEngine),
	iContactItem(aContactItem)
	{
	}

inline void CPbkContactEditorEditContact::ConstructL()
	{
	iFFSCheck = CPbkFFSCheck::NewL(CCoeEnv::Static());
	}

CPbkContactEditorEditContact* CPbkContactEditorEditContact::NewL
		(CPbkContactEngine& aEngine,
		CPbkContactItem& aContactItem)
	{
	CPbkContactEditorEditContact* self = new(ELeave) CPbkContactEditorEditContact(aEngine, aContactItem);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CPbkContactEditorEditContact::~CPbkContactEditorEditContact()
	{
    delete iFFSCheck;
	}

TContactItemId CPbkContactEditorEditContact::SaveContactL()
	{
	// If contact was deleted, return immediatly with KNullContactId
	if (iStateFlags.IsSet(EContactDeleted))
		{
		return KNullContactId;
		}

    // Check that free FFS space is above critical level before saving to
    // database
    if (iStateFlags.IsSet(EModified) && !iStateFlags.IsSet(EContactSaved) && 
        iStateFlags.IsSet(EInitialized) && iFFSCheck->FFSClCheckL())
        {
        iEngine.CommitContactL(iContactItem, ETrue);
        iStateFlags.Clear(EModified);		
        }
    iStateFlags.Set(EContactSaved);

    return iContactItem.Id();
	}

void CPbkContactEditorEditContact::GetHelpContext
		(TCoeHelpContext& aContext) const
	{
    aContext.iContext = KPHOB_HLP_EDIT_ENTRY;
	}

void CPbkContactEditorEditContact::SetStateModified()
    {
    iStateFlags.Set(EModified);
    }

void CPbkContactEditorEditContact::SetStateInitialized()
    {
    iStateFlags.Set(EInitialized);
    }

void CPbkContactEditorEditContact::DeleteContactL()
	{
    // Offer to delete the current contact
    const TContactItemId contactId = iContactItem.Id();
    CPbkDeleteContactQuery* dlg = CPbkDeleteContactQuery::NewLC();
    if (dlg->RunLD(iEngine, contactId))
        {
	    iEngine.CloseContactL(contactId);
		iEngine.DeleteContactL(contactId);
    	iStateFlags.Set(EContactDeleted);
		}
	}

const TDesC& CPbkContactEditorEditContact::DefaultTitle() const
	{
	return iEngine.UnnamedTitle();
	}

void CPbkContactEditorEditContact::DynInitMenuPaneL
        (TInt aResourceId, 
        CEikMenuPane* aMenuPane)
    {
    if (aResourceId == R_CONTACTEDITOR_MENUPANE)
        {
        if (!FeatureManager::FeatureSupported(KFeatureIdHelp)) 
            {
            // remove non-supported help from menu
            aMenuPane->SetItemDimmed(EAknCmdHelp, ETrue);
            }
        }
    }

//  End of File  
