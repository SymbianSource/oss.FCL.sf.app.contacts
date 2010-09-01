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
*     Memory selection setting page for Phonebook 2.
*
*/


#include "CPbk2MemorySelectionSettingPage.h"

// system includes
#include <akncheckboxsettingpage.h>

namespace
    {
    #ifdef _DEBUG
    enum TPanicCode
        {
        EPanic_InvalidIndex = 1
        };
        
    void Panic( TPanicCode aCode )
        {
        _LIT( KPanicCat, "CPbk2MemorySelectionSettingPage" );
        User::Panic( KPanicCat, aCode );
        }
    #endif // _DEBUG
    }


CPbk2MemorySelectionSettingPage::CPbk2MemorySelectionSettingPage(
        TInt aResourceID, 
        CSelectionItemList* aItemArray ) :
    CAknCheckBoxSettingPage( aResourceID, aItemArray ), 
    iItemsArray( aItemArray )
    {
    }

CPbk2MemorySelectionSettingPage::~CPbk2MemorySelectionSettingPage()
    {
    }

void CPbk2MemorySelectionSettingPage::UpdateSettingL()   
    {
    CAknCheckBoxSettingPage::UpdateSettingL();
    UpdateCbaL();
    }
   
void CPbk2MemorySelectionSettingPage::UpdateCbaL()
    {
	TBool validLabels = EFalse;
    if ( InvalidDataCbaResourceId() != DefaultCbaResourceId() )
		{
		// Check that atleast one of the items is marked	
	    const TInt count( iItemsArray->Count() );
        for ( TInt i(0); i < count; ++i )
            {
            if ( iItemsArray->At( i )->SelectionStatus() )
                {
                validLabels = ETrue;
                break;
                }
            }	   	        	            
		}

    TInt selectedItem( ListBoxControl()->CurrentItemIndex() );    
    
    __ASSERT_DEBUG( selectedItem > KErrNotFound 
        && selectedItem < iItemsArray->Count(), Panic( EPanic_InvalidIndex ) );
        
    if ( iItemsArray->At( selectedItem )->SelectionStatus() )
        {
        Cba()->SetCommandSetL( DefaultCbaResourceId() );    
        }
    else
        {
        if ( validLabels )
            {
            Cba()->SetCommandSetL( R_AVKON_SOFTKEYS_OK_CANCEL__MARK );
            }
        else
            {
            Cba()->SetCommandSetL( InvalidDataCbaResourceId() );
            }
        }
    Cba()->DrawDeferred();
    }


void CPbk2MemorySelectionSettingPage::HandleControlEventL(
        CCoeControl* /*aControl*/,  
        TCoeEvent  aEventType )
    {
    if ( aEventType == EEventStateChanged )
        {
        UpdateCbaL();
        }
    }

void CPbk2MemorySelectionSettingPage::ProcessCommandL( TInt aCommandId )    
    {
    switch( aCommandId )
        {
        case EAknSoftkeyUnmark: // FALLTHROUGH
        case EAknSoftkeyMark:
            {
            // Map these events to select event.
            aCommandId = EAknSoftkeySelect;
            break;
            }
        default:;
        }
    CAknCheckBoxSettingPage::ProcessCommandL( aCommandId );
    }
    
// End of File
