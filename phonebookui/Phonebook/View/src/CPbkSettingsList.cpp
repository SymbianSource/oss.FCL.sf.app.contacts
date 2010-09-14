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
*       Provides methods for Settings list for Phonebook.
*
*/


// INCLUDE FILES
#include "CPbkSettingsList.h"			// This class
#include "Cpbknameordersettingitem.h"
#include <PbkView.hrh>
#include <pbkview.rsg>
#include <CPbkContactEngine.h>

/// Unnamed namespace for local definitons
namespace {

#ifdef _DEBUG
enum TPanicCode
    {
	EPanicPreCond_CreateSettingItemL,
	EPanicPreCond_SettingValue,
	EPanicPreCond_ChangeValue
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkSettingsList");
    User::Panic(KPanicText, aReason);
    }
#endif  // _DEBUG


} // namespace



// ================= MEMBER FUNCTIONS =======================

// CPbkSettingsList
inline CPbkSettingsList::CPbkSettingsList(CPbkContactEngine& aEngine)
	: iEngine(aEngine)
    {
    }


CPbkSettingsList* CPbkSettingsList::NewL(CPbkContactEngine& aEngine)
    {
    CPbkSettingsList* self = new(ELeave) CPbkSettingsList(aEngine);
    CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
    return self;
    }


inline void CPbkSettingsList::ConstructL()
    {
	// Construct from resources
	ConstructFromResourceL(R_PBK_SETTINGS_LIST);
    }


CPbkSettingsList::~CPbkSettingsList()
    {
	// iNameOrderSettingItem is deleted by the base class, since
	// all setting items belong to an array (owned by base class)
	// which is reseted at base class destructor
    }


CAknSettingItem* CPbkSettingsList::CreateSettingItemL(TInt aSettingId)
	{
    __ASSERT_DEBUG(!iNameOrderSettingItem,
		Panic(EPanicPreCond_CreateSettingItemL));

	CAknSettingItem* settingItem = NULL;

	switch (aSettingId)
		{
		case EPbkSettingNameOrder:
			{
			iDefaultNameOrderValue = iEngine.NameDisplayOrderL();
			TBool useComma = !iEngine.NameSeparator().Eos();
			
			// Map the Engine's NameOrder- and UseComma-values to 
			// Settings-dialog's NameOrder-value.	    			
			switch (iDefaultNameOrderValue)
		        {
		        case CPbkContactEngine::EPbkNameOrderLastNameFirstName:
			        {
			        if (useComma)
			            {
			            iDefaultNameOrderValue = 
			                EPbkSettingNameOrderLastSeparatorFirst;
			            }
			        else
			            {
			            iDefaultNameOrderValue = 
			                EPbkSettingNameOrderLastSpaceFirst;
			            }
			        break;
			        }
			    case CPbkContactEngine::EPbkNameOrderFirstNameLastName:
			        {
			        iDefaultNameOrderValue = 
			            EPbkSettingNameOrderFirstSpaceLast;
			        break;
			        }

			    default:
			        break;
		        }

		    settingItem = new (ELeave) CPbkNameOrderSettingItem(aSettingId,
			    iDefaultNameOrderValue,iEngine);
		    delete iNameOrderSettingItem;
		    iNameOrderSettingItem = NULL;
		    iNameOrderSettingItem = (CPbkNameOrderSettingItem*) settingItem;
			break;
			}
		default:
			{
			// Do nothing
			break;
			}
		}
	return settingItem;
	}

TInt CPbkSettingsList::SettingValueL(TInt aSettingId)
	{
    __ASSERT_DEBUG(iNameOrderSettingItem,
		Panic(EPanicPreCond_SettingValue));

	switch (aSettingId)
		{
		case EPbkSettingNameOrder:
			{
			iNameOrderSettingItem->StoreL();
			return iNameOrderSettingItem->Value();
			}
		default:
			{
			return KErrNotFound;
			}
		}
	}

void CPbkSettingsList::ChangeValueL(TInt aSettingId)
	{
    __ASSERT_DEBUG(iNameOrderSettingItem,
		Panic(EPanicPreCond_ChangeValue));

	switch (aSettingId)
		{
		case EPbkSettingNameOrder:
			{
			iNameOrderSettingItem->ChangeValueL();
			LoadSettingsL();
			ListBox()->DrawDeferred();
			break;
			}
		default:
			{
			// Do nothing
			break;
			}
		}
	}

void CPbkSettingsList::SizeChanged()
    {
    CAknSettingItemList::SizeChanged();
    if (ListBox()) 
        {
        ListBox()->SetRect(Rect());
        }
    }

//  End of File
