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
*     The main UI container element of the Speeddial Control Utility 
*     that contains the 3x3 grid of speeddial entries.
*
*/





// INCLUDE FILES
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <uikon/eikctrlstatus.h>
#endif
#include <eikdialg.h>
#include <avkon.hrh>
#include <aknlists.h>
#include <aknkeys.h>

#include <eikapp.h>
#include <eikbtgpc.h>
#include <eikmenub.h>

#include <centralrepository.h>
#include <telvmbxsettingscrkeys.h>
#include <voicemailboxdomaincrkeys.h>
#include <eiklbx.h>
#include <gulicon.h>
#include <eikclbd.h>
#include <CPbkContactChangeNotifier.h>
#include <CPbkContactEngine.h>      // phonebook engine
#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>
#include <SpdCtrl.rsg>
#include "SpdiaControl.hrh"
#include "SpdiaControl.h"
#include "SpdiaGridDlg.h"
#include "SpdiaGrid.h"
#include "Speeddial.laf"

const TUint KShortcutKey0('0');
const TInt KMaxIndex(8);
const TInt KNullIndexData(-1);
// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CSpdiaGridDlg::NewL()
// 
// ---------------------------------------------------------
CSpdiaGridDlg* CSpdiaGridDlg::NewL(TInt& aDial, const CSpdiaControl& aControl)
    {
    CSpdiaGridDlg* self = new (ELeave) CSpdiaGridDlg(aDial, aControl);
    return self;
    }

// ---------------------------------------------------------
// CSpdiaGridDlg::~CSpdiaGridDlg()
// 
// ---------------------------------------------------------
CSpdiaGridDlg::~CSpdiaGridDlg()
    {
    delete iPbkNotifier;
    }

// ---------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CSpdiaGridDlg::CSpdiaGridDlg(TInt& aDial, const CSpdiaControl& aControl): iDial(aDial)
    {
    iControl = CONST_CAST(CSpdiaControl*, &aControl);
    iCbaID = R_AVKON_SOFTKEYS_BACK;
    }

// ----------------------------------------------------
// CSpdiaGridDlg::PreLayoutDynInitL
//
// ----------------------------------------------------
//
void CSpdiaGridDlg::PreLayoutDynInitL()
    {
    iGrid = STATIC_CAST(CSpdiaGrid*, Control(ESpdGridItem));

    CDesCArray* itemArray = STATIC_CAST(CDesCArray*,
                            iGrid->Model()->ItemTextArray());

    itemArray->Reset();
    iControl->CreateDataL(*iGrid);       //

    // Obsever
    CPbkContactEngine* pbkEngine = iControl->PbkEngine();
    iPbkNotifier = pbkEngine->CreateContactChangeNotifierL(this);
    }

// ----------------------------------------------------
// CSpdiaGridDlg::PostLayoutDynInitL
//
// ----------------------------------------------------
//
void CSpdiaGridDlg::PostLayoutDynInitL()
    {
    iControl->SetLayout(Rect());
    if ( AknLayoutUtils::LayoutMirrored() )
        {
        iGrid->SetCurrentDataIndex(0);
        LoadCbaL();
        }
    HandleResourceChange( KEikDynamicLayoutVariantSwitch );
    }

// ---------------------------------------------------------
// CSpdiaGridDlg::CreateCustomControlL
//
// ---------------------------------------------------------
//
SEikControlInfo CSpdiaGridDlg::CreateCustomControlL(TInt aControlType)
    {
    SEikControlInfo ctrlInfo;
    ctrlInfo.iControl=NULL;
    ctrlInfo.iTrailerTextId=0;
    ctrlInfo.iFlags = EEikControlHasEars;
    if (aControlType == ESpdiaGrid)
        {
        ctrlInfo.iControl = CSpdiaGrid::NewL(*iControl);
        }
    return ctrlInfo;
    }
// ---------------------------------------------------------
// CSpdiaGridDlg::OkToExitL
//
// ---------------------------------------------------------
//
TBool CSpdiaGridDlg::OkToExitL(TInt aButtonId)    // Pressed button id
    {
    TBool result(ETrue);
    if (aButtonId == EAknSoftkeyOk)
        {
        TInt vmbxSupport = 0;
        CRepository* vmbxSupported = CRepository::NewL( KCRUidVideoMailbox );
        vmbxSupported->Get( KVideoMbxSupport, vmbxSupport );
        delete vmbxSupported;
        
        TInt vmbxPos;
        CRepository* vmbxKey2 = CRepository::NewL( KCRUidTelVideoMailbox );
        vmbxKey2->Get( KTelVideoMbxKey, vmbxPos );
        TInt vdoIndex( iControl->Index( vmbxPos ) );
        delete vmbxKey2;
        
        iDial = iControl->Number(iGrid->CurrentDataIndex());
        TInt index(iGrid->CurrentDataIndex());

        if ( iControl->VMBoxPosition() == index || ( vmbxSupport && vdoIndex == index ) )
            {
            result = EFalse;
            }
        }
    return result;
    }

// ---------------------------------------------------------
// CSpdiaGridDlg::OfferKeyEventL
//
// ---------------------------------------------------------
//
TKeyResponse CSpdiaGridDlg::OfferKeyEventL
            ( const TKeyEvent& aKeyEvent, 
             TEventCode aType )
    {
    TBool keyConsumed( EFalse );
    if ( iGrid != NULL )
        {
        TChar code( aKeyEvent.iCode );

        TInt language = User::Language();
        // When Arabic and Hebrew input is in use, the Arabic and Hebrew browsing order
        // is not followed. Instead, the browsing order is the same as in Western variants.
        if ( language == ELangUrdu /*|| language == ELangArabic*/ )
            {
            if ( code == EKeyLeftArrow )
                {
                TInt index = iGrid->CurrentDataIndex();
                if ( index == KMaxIndex )
                    {
                    iGrid->SetCurrentDataIndex( 0 );
                    LoadCbaL();
                    return EKeyWasConsumed; 
                    }
                }
            if ( code == EKeyRightArrow )
                {
                TInt index = iGrid->CurrentDataIndex();
                if ( index == 0 )
                    {
                    iGrid->SetCurrentDataIndex(KMaxIndex);
                    LoadCbaL();
                    return EKeyWasConsumed; 
                    }
                }	
            }
        else
            {
            if ( code == EKeyRightArrow )
                {
                TInt index = iGrid->CurrentDataIndex();
                if(index == KMaxIndex)
                    {
                    iGrid->SetCurrentDataIndex(0);
                    LoadCbaL();
                    return EKeyWasConsumed; 
                    }
                }
            if ( code == EKeyLeftArrow )
                {
                TInt index = iGrid->CurrentDataIndex();
                if ( index == 0 )
                    {
                    iGrid->SetCurrentDataIndex(KMaxIndex);
                    LoadCbaL();
                    return EKeyWasConsumed; 
                    }
                }
            }

        if ( aKeyEvent.iScanCode == EStdKeyYes )
            {
            return EKeyWasConsumed;
            }
        TKeyResponse exitCode(
            iGrid->OfferKeyEventL( aKeyEvent, aType) );
        if ( exitCode != EKeyWasNotConsumed )
            {
            if ( code.IsDigit() )
                {
                if ( aKeyEvent.iCode - KShortcutKey0 > 0 )
                    {
                    iGrid->SetCurrentDataIndex(
                            iControl->Index(aKeyEvent.iCode - KShortcutKey0));
                    LoadCbaL();
                    keyConsumed = ETrue;
                    }
                }
            else if (aKeyEvent.iCode == EKeyLeftArrow ||
                aKeyEvent.iCode == EKeyRightArrow ||
                aKeyEvent.iCode == EKeyUpArrow ||
                aKeyEvent.iCode == EKeyDownArrow)
                {
                LoadCbaL();
                keyConsumed = ETrue;
                }

            if ( aKeyEvent.iCode == EKeyEnter )
                {
                TryExitL(EAknSoftkeyOk);
                keyConsumed = ETrue;
                }
            else if (aKeyEvent.iCode == EKeyEscape)
                {
                TryExitL(EAknSoftkeyBack);
                keyConsumed = ETrue;
                }
            }
        }

    if ( keyConsumed )
        {
        return EKeyWasConsumed;
        }

    return CEikDialog::OfferKeyEventL(aKeyEvent, aType);
    }

// ---------------------------------------------------------
// CSpdiaContainer::HandleDatabaseEventL
// Handles an database event of type aEventType.
// ---------------------------------------------------------
//
void CSpdiaGridDlg::HandleDatabaseEventL(
         TContactDbObserverEvent aEvent)
    {
    switch(aEvent.iType)
        {
        case EContactDbObserverEventContactChanged:
        case EContactDbObserverEventContactDeleted:
        case EContactDbObserverEventContactAdded:
            break;
        case EContactDbObserverEventSpeedDialsChanged:
            iNeedUpdate = ETrue;
            break;
        default:
            break;
        }
    }

// ---------------------------------------------------------
// CSpdiaGridDlg::FocusChanged()
// Changes focus.  
// ---------------------------------------------------------
//
void CSpdiaGridDlg::FocusChanged(TDrawNow aDrawNow)
	{
    if (IsFocused() && aDrawNow == EDrawNow && iNeedUpdate)
        {
        TRAP_IGNORE(iControl->CreateDataL(*iGrid));
        iNeedUpdate = EFalse;
        }
	}

// ---------------------------------------------------------
// CSpdiaGridDlg::LoadCbaL()
// Loads CBA buttons.  
// ---------------------------------------------------------
//
void CSpdiaGridDlg::LoadCbaL()
    {
    TInt id(R_AVKON_SOFTKEYS_BACK);

    TInt vmbxSupport = 0;
    CRepository* vmbxSupported = CRepository::NewL( KCRUidVideoMailbox );
    vmbxSupported->Get( KVideoMbxSupport, vmbxSupport );
    delete vmbxSupported;

    TInt vmbxPos;
    CRepository*  vmbxKey2 = CRepository::NewL( KCRUidTelVideoMailbox );
    vmbxKey2->Get( KTelVideoMbxKey, vmbxPos );
    TInt vdoIndex( iControl->Index( vmbxPos ) );
    delete vmbxKey2;

    if ( iGrid->CurrentDataIndex() != iControl->VMBoxPosition() && 
        ( !vmbxSupport || iGrid->CurrentDataIndex() != vdoIndex ) )
        {
        id =  R_SPDCTRL_SOFTKEYS_ASSIGN_BACK_ASSIGN;
        }
    if (id != iCbaID)
        {
        iCbaID = id;
        ButtonGroupContainer().SetCommandSetL(id);
        ButtonGroupContainer().DrawNow();
        }
    iPrevIndex = iGrid->CurrentDataIndex();
    }

// ---------------------------------------------------------
// CSpdiaGridDlg::HandleResourceChange()
// Handle layout and skin change event.  
// ---------------------------------------------------------
//
void CSpdiaGridDlg::HandleResourceChange( TInt aType )
    {    
    TRect  mainPaneRect ;
    TRect  statusPaneRect;
    
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        CEikStatusPane* StatusPane = 
        ( ( CAknAppUi* ) CEikonEnv::Static()->EikAppUi() )->StatusPane();

        if ( Layout_Meta_Data::IsLandscapeOrientation() )
            {
            StatusPane->MakeVisible( ETrue );
            AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane,mainPaneRect );
            StatusPane->DrawNow();
            }
        else
            {
            StatusPane->MakeVisible( EFalse );
            AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mainPaneRect );
            AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EStatusPane, statusPaneRect );
            mainPaneRect.iTl = statusPaneRect.iTl;
            }	
        SetRect( mainPaneRect );
        DrawNow();
        }
    else if ( aType == KAknsMessageSkinChange )
        {
        TRAP_IGNORE( iControl->CreateGridDataL( iGrid, KNullIndexData ) );
        CAknDialog::HandleResourceChange( aType );	 	
        }
    else
        {
        CAknDialog::HandleResourceChange( aType );
        }
    }

// ---------------------------------------------------------
// CSpdiaGridDlg::HandleDialogPageEventL()
// Handles the events on the dialog page (for Touch)
// ---------------------------------------------------------
//	
void CSpdiaGridDlg::HandleDialogPageEventL(TInt /*aEventId*/)
    {
    // When clicking on grid, the function will simulate the event 
    // just like clicking on LSK
    TryExitL( EAknSoftkeyOk );
    }
// End of File
