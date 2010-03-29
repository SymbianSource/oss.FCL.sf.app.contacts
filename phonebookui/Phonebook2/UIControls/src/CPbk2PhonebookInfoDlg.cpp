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
* Description:  A dialog the shows the memory status of different stores
*
*/


// INCLUDE FILES
#include "CPbk2PhonebookInfoDlg.h"

#include "CPbk2StoreInfoItemBuilder.h"
#include "MPbk2StoreInfoUiItem.h"
#include <Pbk2UIControls.rsg>
#include <CPbk2StorePropertyArray.h>
#include <CPbk2StoreProperty.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>
#include <CVPbkContactStoreUriArray.h>
#include <TVPbkContactStoreUriPtr.h>
#include <aknPopup.h>
#include <aknlists.h>
#include <StringLoader.h>


/// Unnamed namespace for local definitions
namespace
    {
    // LOCAL CONSTANTS AND MACROS
    _LIT( KSeparator, "\t" );    
    _LIT( KSpace, " " );
    }


// ================= MEMBER FUNCTIONS =======================

// inline because only one call site
inline CPbk2PhonebookInfoDlg::CPbk2PhonebookInfoDlg()
    {
    }


CPbk2PhonebookInfoDlg::~CPbk2PhonebookInfoDlg()
    {
	if(iDestroyed)
		{
		*iDestroyed = ETrue;
		}
	delete iListBox;
	if (iInfoItems)
	    {
	    iInfoItems->ResetAndDestroy();
	    delete iInfoItems;
	    }
    }

// inline because only one call site
inline void CPbk2PhonebookInfoDlg::ConstructL()
    {
	iListBox = new(ELeave) CAknSingleHeadingPopupMenuStyleListBox;
	iPopupList = CAknPopupList::NewL(iListBox,
								R_AVKON_SOFTKEYS_OK_EMPTY__OK,
                                AknPopupLayouts::EMenuGraphicHeadingWindow);
	iListBox->ConstructL(iPopupList, EAknListBoxViewerFlags);
    
    iListBox->CreateScrollBarFrameL(ETrue);
	iListBox->ScrollBarFrame()->SetScrollBarVisibilityL
        (CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
    }

EXPORT_C CPbk2PhonebookInfoDlg* CPbk2PhonebookInfoDlg::NewL()
    {
    CPbk2PhonebookInfoDlg* self = new(ELeave) CPbk2PhonebookInfoDlg;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

EXPORT_C void CPbk2PhonebookInfoDlg::ExecuteLD(
        CVPbkContactStoreUriArray& aStoreUris)
    {
	CleanupStack::PushL(this);
	TBool thisDestroyed(EFalse);
	iDestroyed = &thisDestroyed;
	
	SetDataL(Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager(),
        aStoreUris);
	// Set title
	SetTitleL(aStoreUris);
	
	
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

void CPbk2PhonebookInfoDlg::SetTitleL( CVPbkContactStoreUriArray& aStoreUris )
	{
	HBufC* title = NULL;
	const TInt oneStore = 1;
    if ( aStoreUris.Count() == oneStore )
        {
        title = StringLoader::LoadLC( R_QTN_PHOB_QTL_PB_INFO_SELECTED );
        }
    else
        {
        title = StringLoader::LoadLC( R_QTN_PHOB_QTL_PB_INFO_ALL );
        }
	
    iPopupList->SetTitleL( *title );
    CleanupStack::PopAndDestroy( title ); // title
	}

void CPbk2PhonebookInfoDlg::SetDataL(CVPbkContactManager& aContactManager,
        CVPbkContactStoreUriArray& aStoreUris)
    {
    CPbk2StoreInfoItemBuilder* itemBuilder = CPbk2StoreInfoItemBuilder::NewL(
        aContactManager, Phonebook2::Pbk2AppUi()->ApplicationServices().
            StoreProperties());
    CleanupStack::PushL(itemBuilder);
    
    const TInt oneStore = 1;
    if (aStoreUris.Count() == oneStore)
        {
        iInfoItems = itemBuilder->BuildSingleStoreItemsL(aStoreUris[0]);
        }
    else
        {
        iInfoItems = itemBuilder->BuildAllStoresItemsL(aStoreUris);
        }
    
    const TInt count = iInfoItems->Count();
	CDesCArray* array = new(ELeave) CDesCArrayFlat(count);
    CleanupStack::PushL(array);
    
    for (TInt i = 0; i < count; ++i)
        {
        MPbk2StoreInfoUiItem& uiItem = *iInfoItems->At(i);
        HBufC* itemBuf = HBufC::NewLC(uiItem.HeadingText().Length() +
            uiItem.ItemText().Length() + KSeparator().Length() + KSpace().Length());
        TPtr ptr(itemBuf->Des());
        ptr.Append(KSeparator);
        ptr.Append(uiItem.HeadingText());
        ptr.Append(KSpace);
        ptr.Append(uiItem.ItemText());
        // Convert digits
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion(ptr);
        array->AppendL(*itemBuf);
        CleanupStack::PopAndDestroy(itemBuf);
        }
    
    iListBox->Model()->SetOwnershipType(ELbmOwnsItemArray);
    iListBox->Model()->SetItemTextArray(array);
    iListBox->DrawNow();
    CleanupStack::Pop(array);
    CleanupStack::PopAndDestroy(itemBuilder);
    }

//  End of File
