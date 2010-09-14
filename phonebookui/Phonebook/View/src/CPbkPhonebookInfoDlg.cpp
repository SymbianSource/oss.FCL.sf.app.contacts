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
*       Provides methods for Phonebook Info view UI control.
*
*/


// INCLUDE FILES
#include "CPbkPhonebookInfoDlg.h"

#include <pbkview.rsg>
#include <CPbkContactEngine.h>
#include <StringLoader.h>
#include <aknPopup.h>
#include <aknlists.h>
#include <StringLoader.h>


/// Unnamed namespace for local definitions
namespace
    {
    // LOCAL CONSTANTS AND MACROS
    _LIT( KNumberFormat, "%d" );
    _LIT( KSeparator, "\t" );
    const TUint KItemCount = 4;
    const TInt KKByte = 1024;
    const TUint KMaxInfoPopupWidth = 16;
    const TUint KSeparatorSize = 1;
    }


// ================= MEMBER FUNCTIONS =======================

// inline because only one call site
inline CPbkPhonebookInfoDlg::CPbkPhonebookInfoDlg
        (CPbkContactEngine& aEngine) :
    iEngine(aEngine), iContactsViewReady(EFalse),
    iGroupsViewReady(EFalse)
    {
    }


CPbkPhonebookInfoDlg::~CPbkPhonebookInfoDlg()
    {
	if(iDestroyed)
		{
		*iDestroyed = ETrue;
		}
	delete iListBox;

    if (iAllGroupsView)
        {
        iAllGroupsView->Close(*this);
        }
    if (iAllContactsView)
        {
        iAllContactsView->Close(*this);
        }
    }

// inline because only one call site
inline void CPbkPhonebookInfoDlg::ConstructL()
    {
    // Attach to contact views
    CContactViewBase& allContactsView = iEngine.AllContactsView();
    allContactsView.OpenL(*this);
    iAllContactsView = &allContactsView;

    CContactViewBase& allGroupsView = iEngine.AllGroupsViewL();
    allGroupsView.OpenL(*this);
    iAllGroupsView = &allGroupsView;

	iListBox = new(ELeave) CAknSingleHeadingPopupMenuStyleListBox;
	iPopupList = CAknPopupList::NewL(iListBox,
								R_AVKON_SOFTKEYS_OK_EMPTY,
                                AknPopupLayouts::EMenuGraphicHeadingWindow);
	iListBox->ConstructL(iPopupList, EAknListBoxViewerFlags);

    // Set title
	HBufC* title = StringLoader::LoadLC(R_QTN_PHOB_QTL_PB_INFO);
	SetTitleL(*title);
	CleanupStack::PopAndDestroy(); // title

    UpdateDataL();

    iListBox->CreateScrollBarFrameL(ETrue);
	iListBox->ScrollBarFrame()->SetScrollBarVisibilityL
        (CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
    }

EXPORT_C CPbkPhonebookInfoDlg* CPbkPhonebookInfoDlg::NewL
        (CPbkContactEngine& aEngine)
    {
    CPbkPhonebookInfoDlg* self = new(ELeave) CPbkPhonebookInfoDlg(aEngine);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

EXPORT_C void CPbkPhonebookInfoDlg::ExecuteLD()
    {
	CleanupStack::PushL(this);
	TBool thisDestroyed(EFalse);
	iDestroyed = &thisDestroyed;
	
    TRAPD(err, iPopupList->ExecuteLD());
    
    // Just in case
	if(!thisDestroyed)
		{
		iPopupList = NULL;
		}

	User::LeaveIfError(err);

    if(thisDestroyed)
		{
		CleanupStack::Pop(); // this
		}
	else
		{
		CleanupStack::PopAndDestroy(); // this
		}
	iDestroyed = NULL;
    }

void CPbkPhonebookInfoDlg::HandleContactViewEvent
        (const CContactViewBase& aView,
        const TContactViewEvent& aEvent)
    {
    // It's safe to ignore errors
    TRAP_IGNORE(HandleContactViewEventL(aView,aEvent));
    }

void CPbkPhonebookInfoDlg::HandleContactViewEventL
        (const CContactViewBase& aView,
        const TContactViewEvent& aEvent)
    {
    switch (aEvent.iEventType)
        {
        case TContactViewEvent::EReady:             // FALLTHROUGH
        case TContactViewEvent::ESortOrderChanged:  // FALLTHROUGH
        case TContactViewEvent::EItemAdded:         // FALLTHROUGH
        case TContactViewEvent::EItemRemoved:
            {
            if (&aView == iAllContactsView)
                {
                iContactsViewReady = ETrue;
                }
            else if (&aView == iAllGroupsView)
                {
                iGroupsViewReady = ETrue;
                }
            UpdateDataL();
            break;
            }

        default:
            {
            if (&aView == iAllContactsView)
                {
                iContactsViewReady = EFalse;
                }
            else if (&aView == iAllGroupsView)
                {
                iGroupsViewReady = EFalse;
                }
            break;
            }
        }
    }


void CPbkPhonebookInfoDlg::SetTitleL(const TDesC &aTitle)
	{
	iPopupList->SetTitleL(aTitle);
	}

MDesCArray* CPbkPhonebookInfoDlg::ConstructDataArrayL()
	{
	CDesCArray* array = new(ELeave) CDesCArrayFlat(KItemCount);
	CleanupStack::PushL(array);

	// Names
	HBufC* contacts = ContactsEntryLC();
	array->AppendL(*contacts);
	CleanupStack::PopAndDestroy(contacts);

	// Groups
	HBufC* groups = GroupsEntryLC();
	array->AppendL(*groups);
	CleanupStack::PopAndDestroy(groups);

    // Used memory
	HBufC* usedMem = UsedMemoryEntryLC();
	array->AppendL(*usedMem);
	CleanupStack::PopAndDestroy(usedMem);

    // Free memory
	HBufC* freeMem = FreeMemoryEntryLC();
	array->AppendL(*freeMem);
	CleanupStack::PopAndDestroy(freeMem);

	CleanupStack::Pop(array); 
	return array;
	}

HBufC* CPbkPhonebookInfoDlg::ContactsEntryLC()
	{
    HBufC* dataStr = NULL;

    if (iContactsViewReady)
        {
        // Get contact count
        const TInt contacsCount = iAllContactsView->CountL();

        // Format contacts string
        HBufC* contactsText = NULL;
        if (contacsCount == 1)
            {
            contactsText = StringLoader::LoadLC(R_QTN_PHOB_PB_INFO_ONE_NAME);
            }
        else
            {
            contactsText = StringLoader::LoadLC(R_QTN_PHOB_PB_INFO_NAMES);
            }

        // Format value string
        TBuf<KMaxInfoPopupWidth> valueStr;
        valueStr.Format(KNumberFormat, contacsCount);

        // Format the string to be returned
	    dataStr = HBufC::NewL(contactsText->Length() +
            KSeparatorSize + valueStr.Length());
	    TPtr dataPtr(dataStr->Des());
	    dataPtr.Append(valueStr);
        // Convert digits
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion(dataPtr);
	    dataPtr.Append(KSeparator);
	    dataPtr.Append(*contactsText);

        CleanupStack::PopAndDestroy(contactsText);
        }
    else
        {
        dataStr = HBufC::NewL(KNullDesC().Length());
    	TPtr dataPtr(dataStr->Des());
	    dataPtr = KNullDesC;
        }

    CleanupStack::PushL(dataStr);
    return dataStr;
	}

HBufC* CPbkPhonebookInfoDlg::GroupsEntryLC()
	{
    HBufC* dataStr = NULL;

    if (iGroupsViewReady)
        {
        // Get group count
        const TInt groupCount = iAllGroupsView->CountL();

        // Format group string
        HBufC* groupsText = NULL;
        if (groupCount == 1)
            {
            groupsText = StringLoader::LoadLC(R_QTN_PHOB_PB_INFO_ONE_GROUP);
            }
        else
            {
            groupsText = StringLoader::LoadLC(R_QTN_PHOB_PB_INFO_GROUPS);
            }

        // Format value string
        TBuf<KMaxInfoPopupWidth> valueStr;
        valueStr.Format(KNumberFormat, groupCount);

        // Format the string to be returned
	    dataStr = HBufC::NewL(groupsText->Length() +
            KSeparatorSize + valueStr.Length());
	    TPtr dataPtr(dataStr->Des());
	    dataPtr.Append(valueStr);
        // Convert digits
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion(dataPtr);
	    dataPtr.Append(KSeparator);
	    dataPtr.Append(*groupsText);

        CleanupStack::PopAndDestroy(groupsText);
        }
    else
        {
        dataStr = HBufC::NewL(KNullDesC().Length());
    	TPtr dataPtr(dataStr->Des());
	    dataPtr = KNullDesC;
        }

    CleanupStack::PushL(dataStr);
    return dataStr;
	}

HBufC* CPbkPhonebookInfoDlg::UsedMemoryEntryLC()
	{
    // Format used memory amount
    HBufC* usedMemoryAmount = StringLoader::LoadLC(R_QTN_SIZE_KB,
        (iEngine.Database().FileSize() + KKByte/2) / KKByte);

    // Format used memory string
    HBufC* usedMemoryText = StringLoader::LoadLC(R_QTN_PHOB_PB_INFO_RESERVED);

    // Format the string to be returned
	HBufC* dataStr = HBufC::NewL(usedMemoryText->Length() +
        KSeparatorSize + usedMemoryAmount->Length());
	TPtr dataPtr(dataStr->Des());
	dataPtr.Append(*usedMemoryAmount);
    // Convert digits
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion(dataPtr);
	dataPtr.Append(KSeparator);
	dataPtr.Append(*usedMemoryText);

    CleanupStack::PopAndDestroy(2); // usedMemoryText, usedMemoryAmount
    CleanupStack::PushL(dataStr);
    return dataStr;
	}

HBufC* CPbkPhonebookInfoDlg::FreeMemoryEntryLC()
	{
    TVolumeInfo volInfo;
    User::LeaveIfError(CCoeEnv::Static()->FsSession().Volume(volInfo,EDriveC));

    // Format free memory amount
    HBufC* freeMemoryAmount = StringLoader::LoadLC(R_QTN_SIZE_KB,
        (I64LOW(volInfo.iFree) / KKByte));

    // Format used memory string
    HBufC* freeMemoryText = StringLoader::LoadLC(R_QTN_PHOB_PB_INFO_FREE);

    // Format the string to be returned
	HBufC* dataStr = HBufC::NewL(freeMemoryText->Length() +
        KSeparatorSize + freeMemoryAmount->Length());
	TPtr dataPtr(dataStr->Des());
	dataPtr.Append(*freeMemoryAmount);
    // Convert digits
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion(dataPtr);
	dataPtr.Append(KSeparator);
	dataPtr.Append(*freeMemoryText);

    CleanupStack::PopAndDestroy(2); // freeMemoryText, freeMemoryAmount
    CleanupStack::PushL(dataStr);
    return dataStr;
	}

void CPbkPhonebookInfoDlg::UpdateDataL()
    {
    // Construct and set data
	MDesCArray* array = ConstructDataArrayL();
    if (iListBox)
        {
	    iListBox->Model()->SetItemTextArray(array);
        iListBox->DrawNow();
        }
    }

//  End of File
