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
*           Phonebook contact editor new contact strategy.
*
*/


// INCLUDE FILES
#include "CPbkContactEditorNewContact.h"
#include <coemain.h>
#include <cntdef.h>
#include <coehelp.h>
#include <eikmenup.h>
#include <bldvariant.hrh>
#include <featmgr.h>
#include <PbkView.rsg>
#include "CPbkContactEngine.h"
#include <CPbkFFSCheck.h>
#include <avkon.hrh>
#include <cshelp/phob.hlp.hrh>


/// State flag values 
enum TStateFlag
    {
    EInitialized,
    EContactSaved,
    EModified
    };

// ================= MEMBER FUNCTIONS =======================

inline CPbkContactEditorNewContact::CPbkContactEditorNewContact
        (CPbkContactEngine& aEngine, CPbkContactItem& aContactItem) :
		iEngine(aEngine), iContactItem(aContactItem),
		iContactId(KNullContactId)
	{
	}

inline void CPbkContactEditorNewContact::ConstructL()
	{
    CCoeEnv* coeEnv = CCoeEnv::Static();
	iFFSCheck = CPbkFFSCheck::NewL(coeEnv);
	iDefaultTitle = coeEnv->AllocReadResourceL(R_QTN_PHOB_TITLE_NEW_ENTRY);
	}

CPbkContactEditorNewContact* CPbkContactEditorNewContact::NewL
        (CPbkContactEngine& aEngine,
        CPbkContactItem& aContactItem)
	{
	CPbkContactEditorNewContact* self = new(ELeave) CPbkContactEditorNewContact(aEngine, aContactItem);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CPbkContactEditorNewContact::~CPbkContactEditorNewContact()
	{
	delete iDefaultTitle;
    delete iFFSCheck;
	}

TContactItemId CPbkContactEditorNewContact::SaveContactL()
	{
    // Check that free FFS space is above critical level before saving to
    // database
    if (iStateFlags.IsSet(EModified) && !iStateFlags.IsSet(EContactSaved) &&
        iStateFlags.IsSet(EInitialized) && iFFSCheck->FFSClCheckL())
        {
        iContactId = iEngine.AddNewContactL(iContactItem, ETrue);
		iStateFlags.Set(EContactSaved);
		iStateFlags.Clear(EModified);
		}
    return iContactId;
	}

void CPbkContactEditorNewContact::GetHelpContext
        (TCoeHelpContext& aContext) const
	{
    aContext.iContext = KPHOB_HLP_ADD_ENTRY;
    }

void CPbkContactEditorNewContact::SetStateModified()
    {
    iStateFlags.Set(EModified);
    }

void CPbkContactEditorNewContact::SetStateInitialized()
    {
    iStateFlags.Set(EInitialized);
    }

void CPbkContactEditorNewContact::DeleteContactL()
	{
	iStateFlags.Set(EContactSaved);
	}

const TDesC& CPbkContactEditorNewContact::DefaultTitle() const
	{
	return *iDefaultTitle;
	}

void CPbkContactEditorNewContact::DynInitMenuPaneL
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
