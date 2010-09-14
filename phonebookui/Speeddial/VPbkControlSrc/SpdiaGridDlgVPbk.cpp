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

#include <eiklbx.h>
#include <gulicon.h>
#include <eikclbd.h>

#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>
#include <spdctrl.rsg>
#include "SpdiaControl.hrh"
#include "speeddialprivate.h"
#include "SpdiaGridDlgVPbk.h"
#include "SpdiaGridVPbk.h"
#include "Speeddial.laf"

const TUint KShortcutKey0('0');
const TInt KMaxIndex(8);
// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CSpdiaGridDlgVPbk::NewL()
// 
// ---------------------------------------------------------
CSpdiaGridDlgVPbk* CSpdiaGridDlgVPbk::NewL(TInt& aDial, const CSpeedDialPrivate& aControl)
	{
	CSpdiaGridDlgVPbk* self = new (ELeave) CSpdiaGridDlgVPbk(aDial, aControl);
	return self;
	}

// ---------------------------------------------------------
// CSpdiaGridDlgVPbk::~CSpdiaGridDlgVPbk()
// 
// ---------------------------------------------------------
CSpdiaGridDlgVPbk::~CSpdiaGridDlgVPbk()
    {
    
    }

// ---------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------
//
CSpdiaGridDlgVPbk::CSpdiaGridDlgVPbk(TInt& aDial, const CSpeedDialPrivate& aControl): iDial(aDial)
    {
    iControl = CONST_CAST(CSpeedDialPrivate*, &aControl);
    iCbaID = R_AVKON_SOFTKEYS_BACK;
    }

// ----------------------------------------------------
// CSpdiaGridDlgVPbk::PreLayoutDynInitL
//
// ----------------------------------------------------
//
void CSpdiaGridDlgVPbk::PreLayoutDynInitL()
    {
    iGrid = STATIC_CAST(CSpdiaGridVPbk*, Control(ESpdGridItem));

    CDesCArray* itemArray = STATIC_CAST(CDesCArray*,
                            iGrid->Model()->ItemTextArray());

    itemArray->Reset();
    iControl->CreateDataL(*iGrid);   

    // Set the Obsever
    // CPbkContactEngine* pbkEngine = iControl->PbkEngine();
    // iPbkNotifier = pbkEngine->CreateContactChangeNotifierL(this);
    }

// ----------------------------------------------------
// CSpdiaGridDlgVPbk::PostLayoutDynInitL
//
// ----------------------------------------------------
//
void CSpdiaGridDlgVPbk::PostLayoutDynInitL()
    {
    iControl->SetLayout(Rect());
    if(AknLayoutUtils::LayoutMirrored())
	{
	iGrid->SetCurrentDataIndex(0);
	LoadCbaL();
	}
	HandleResourceChange(KEikDynamicLayoutVariantSwitch);
    }

// ---------------------------------------------------------
// CSpdiaGridDlgVPbk::CreateCustomControlL
//
// ---------------------------------------------------------
//
SEikControlInfo CSpdiaGridDlgVPbk::CreateCustomControlL(TInt aControlType)
    {
    SEikControlInfo ctrlInfo;
    ctrlInfo.iControl=NULL;
    ctrlInfo.iTrailerTextId=0;
    ctrlInfo.iFlags = EEikControlHasEars;
    if (aControlType == ESpdiaGrid)
        {
        ctrlInfo.iControl = CSpdiaGridVPbk::NewL(*iControl);
        }
    return ctrlInfo;
    }
// ---------------------------------------------------------
// CSpdiaGridDlgVPbk::OkToExitL
//
// ---------------------------------------------------------
//
TBool CSpdiaGridDlgVPbk::OkToExitL(TInt aButtonId)    // Pressed button id
    {
    TBool result(ETrue);
    if (aButtonId == EAknSoftkeyOk)
        {
         iDial = iControl->Number(iGrid->CurrentDataIndex());
         TInt index(iGrid->CurrentDataIndex());
        if (iControl->VMBoxPosition() == index)
            {
            result = EFalse;
            }
        }
    return result;
    }

// ---------------------------------------------------------
// CSpdiaGridDlgVPbk::OfferKeyEventL
//
// ---------------------------------------------------------
//
TKeyResponse CSpdiaGridDlgVPbk::OfferKeyEventL
            (const TKeyEvent& aKeyEvent, 
             TEventCode aType)
    {
    TBool keyConsumed(EFalse);
    if (iGrid != NULL)
    {
    	TChar code(aKeyEvent.iCode);
    	if(AknLayoutUtils::LayoutMirrored())
    	{
    		if(code == EKeyLeftArrow)
	    	{
		    	TInt index = iGrid->CurrentDataIndex();
		    	if(index == KMaxIndex)
		    	{
		    		iGrid->SetCurrentDataIndex(0);
		    		LoadCbaL();
		    		return EKeyWasConsumed; 
		    	}
	    	}
			if(code == EKeyRightArrow)
		    {
		    	TInt index = iGrid->CurrentDataIndex();
		    	if(index == 0)
		    	{
		    		iGrid->SetCurrentDataIndex(KMaxIndex);
		    		LoadCbaL();
		    		return EKeyWasConsumed; 
		    	}
		    }	
    	}
    	else
    	{
	    if(code == EKeyRightArrow)
	    {
	    	TInt index = iGrid->CurrentDataIndex();
	    	if(index == KMaxIndex)
	    	{
	    		iGrid->SetCurrentDataIndex(0);
	    		LoadCbaL();
	    		return EKeyWasConsumed; 
	    	}
	    }
		if(code == EKeyLeftArrow)
	    {
	    	TInt index = iGrid->CurrentDataIndex();
	    	if(index == 0)
	    	{
	    		iGrid->SetCurrentDataIndex(KMaxIndex);
	    		LoadCbaL();
	    		return EKeyWasConsumed; 
	    	}
		    }
	    }
        TKeyResponse exitCode(
            iGrid->OfferKeyEventL(aKeyEvent, aType));
        if (exitCode != EKeyWasNotConsumed)
            {
            if (code.IsDigit())
                {
                if (aKeyEvent.iCode - KShortcutKey0 > 0)
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

            if (aKeyEvent.iCode == EKeyOK)
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

    if (keyConsumed)
        {
        return EKeyWasConsumed;
        }

    return CEikDialog::OfferKeyEventL(aKeyEvent, aType);
    }

// ---------------------------------------------------------
// CSpdiaGridDlgVPbk::FocusChanged()
// Changes focus.  
// ---------------------------------------------------------
//
void CSpdiaGridDlgVPbk::FocusChanged(TDrawNow aDrawNow)
	{
    if (IsFocused() && aDrawNow == EDrawNow && iControl->UpdateFlag())
        {
        TRAP_IGNORE(iControl->CreateDataL(*iGrid));
        iControl->SetUpdateFlag(EFalse);
        }
	}

// ---------------------------------------------------------
// CSpdiaGridDlgVPbk::LoadCbaL()
// Loads CBA buttons.  
// ---------------------------------------------------------
//
void CSpdiaGridDlgVPbk::LoadCbaL()
    {
    TInt id(R_AVKON_SOFTKEYS_BACK);
    if (iGrid->CurrentDataIndex() != iControl->VMBoxPosition())
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
// CSpdiaGridDlgVPbk::HandleResourceChange()
// Handle layout and skin change event.  
// ---------------------------------------------------------
//
void CSpdiaGridDlgVPbk::HandleResourceChange(TInt aType)
	{
	TRect			   	 mainPaneRect ;
    TRect 			   	 statusPaneRect;
    
	if( aType == KEikDynamicLayoutVariantSwitch )
	 {
		if (Layout_Meta_Data::IsLandscapeOrientation())
		{
		 AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane,mainPaneRect);
		}
		else
		{
		AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane,mainPaneRect);
	 	AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EStatusPane,statusPaneRect);
	 	mainPaneRect.iTl= statusPaneRect.iTl;
		}
	 SetRect(mainPaneRect);
	 DrawNow();
	 }
	 else
	 {
	 CAknDialog::HandleResourceChange(aType);
	 }
	}
	
	// ---------------------------------------------------------
// CSpdiaGridDlg::HandleDialogPageEventL()
// Handles the events on the dialog page (for Touch)
// ---------------------------------------------------------
//	
void CSpdiaGridDlgVPbk::HandleDialogPageEventL(TInt /*aEventId*/)
    {
        //Get the current Index      
        TInt index(iGrid->CurrentDataIndex());
        //If current index is the previous index, then try opening it
        if(index == iPrevIndex)
        {
            
            TryExitL(EAknSoftkeyOk);
        }
        else //else load the cba buttons
        {
            LoadCbaL();    
        }

    }
// End of File
