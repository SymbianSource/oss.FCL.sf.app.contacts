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
*       Provides methods for Settings list dialog for Phonebook.
*
*/


// INCLUDE FILES
#include "CPbkSettingsDlg.h"		// This class
#include "CPbkSettingsList.h"
#include <PbkView.hrh>
#include "PbkUID.h"
#include "PbkViewUtils.h"

#include <PbkView.rsg>
#include <CPbkContactEngine.h>
#include <eikspane.h>
#include <akntitle.h>
#include <aknnavide.h>
#include <barsread.h>
#include <featmgr.h>
#include <AknWaitDialog.h>
#include <aknappui.h>    //iAvkonAppUi
#include <StringLoader.h>
#include <cshelp/phob.hlp.hrh>

/// Unnamed namespace for local definitons
namespace {

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_SetupPanesL,
	EPanicPreCond_SaveTitlePaneL,
	EPanicPreCond_ProcessCommandL,
	EPanicPreCond_OfferKeyEventL
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkSettingsDlg");
    User::Panic(KPanicText, aReason);
    }
#endif  // _DEBUG

} // namespace


// ================= MEMBER FUNCTIONS =======================

/**
 * Default constructor.
 */
CPbkSettingsDlg::CPbkSettingsDlg(CPbkContactEngine& aEngine) :
    iEngine(aEngine), iUnderlyingViewReady(ETrue)
	{
	}

/**
 * Destructor.
 */
CPbkSettingsDlg::~CPbkSettingsDlg()
	{
	delete iSettingsList;
	delete iNaviDecorator;
    delete iWaitNote;

    // Restore the titlepane text
    if (iTitlePane && iStoredTitlePaneText)
        {
        //iTitlePane takes ownership of iStoredTitlePaneText
        iTitlePane->SetText(iStoredTitlePaneText);
        }

    if (iAllContactsView)
        {
        iAllContactsView->Close(*this);
        }

	iEikonEnv->RemoveFromStack(this);
	}

EXPORT_C CPbkSettingsDlg* CPbkSettingsDlg::NewL
        (CPbkContactEngine& aEngine)
	{
	CPbkSettingsDlg* self = new(ELeave) CPbkSettingsDlg(aEngine);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
	}

void CPbkSettingsDlg::ConstructL()
	{
	CAknDialog::ConstructL(R_PBK_SETTINGS_MENUBAR);
	SaveTitlePaneL();

	if (!iAllContactsView)
		{
		iAllContactsView = &iEngine.AllContactsView();
		// Register to receive view events
		iAllContactsView->OpenL(*this);
		}
			
	}

EXPORT_C void CPbkSettingsDlg::ExecuteLD()
	{		
	CAknDialog::ExecuteLD(R_PBK_SETTINGS_DIALOG);
	}

void CPbkSettingsDlg::PreLayoutDynInitL()
	{
	SetupPanesL();
	iEikonEnv->EikAppUi()->AddToStackL(this, ECoeStackPriorityDialog);
	CreateSettingsListL();
	}


/**
 * Creates the settings list.
 */
void CPbkSettingsDlg::CreateSettingsListL()
	{
	delete iSettingsList;
	iSettingsList = NULL;
	iSettingsList = CPbkSettingsList::NewL(iEngine);
	iSettingsList->SetMopParent(this);
	iSettingsList->MakeVisible(ETrue);
	iSettingsList->ActivateL();
	iSettingsList->DrawDeferred();
	}

void CPbkSettingsDlg::ProcessCommandL
        (TInt aCommandId)
    {
    __ASSERT_DEBUG(iSettingsList, Panic(EPanicPreCond_ProcessCommandL));

	// Hide all menus
	HideMenu();

    // Process view-specific commands
    switch (aCommandId) 
        {
        case EEikCmdExit:   // FALLTHROUGH
        case EAknCmdExit:   // Exit calling application
			{
            CEikAppUi* appUi = iEikonEnv->EikAppUi();
            // Close this dialog first
            delete this;
            // Exit application
            static_cast<MEikCommandObserver*>(appUi)->ProcessCommandL(aCommandId);
			break;
			}

		case EPbkCmdEditSetting:
			{
			TInt count = iSettingsList->SettingItemArray()->Count();
			for (TInt index = 0; index < count; ++index)
				{
				if (iSettingsList->SettingItemArray()->At(index)->Identifier()
						== EPbkSettingNameOrder)
					{
					iSettingsList->SettingItemArray()->
                        At(index)->EditItemL(ETrue);
                    iSettingsList->DrawDeferred();
                    SaveSettingsL();
					}
				}
			break;
			}

        default:
            {
            // Command not handled here, forward it to the app ui.
            iEikonEnv->EikAppUi()->HandleCommandL(aCommandId);
            break;
            }
        }
    }


TKeyResponse CPbkSettingsDlg::OfferKeyEventL
        (const TKeyEvent& aKeyEvent, 
        TEventCode aType)
    {
    __ASSERT_DEBUG(iSettingsList, Panic(EPanicPreCond_OfferKeyEventL));

	// Handle ok key
    if (aType == EEventKey && aKeyEvent.iCode == EKeyOK)
        {
        // Now that there are more than two choices, we must open the
        // selection popup
        TKeyResponse response = 
            iSettingsList->OfferKeyEventL(aKeyEvent, aType);
        SaveSettingsL();
        return response;
        }

    if (aType == EEventKey && aKeyEvent.iCode == EKeyEscape)
        {
		AttemptExit();
		return EKeyWasConsumed;
        }

	// Let the base class handle the event
	return CAknDialog::OfferKeyEventL(aKeyEvent, aType);
    }

void CPbkSettingsDlg::AttemptExit()
	{
    delete this;
	}

/**
 * Setups the title and navi pane.
 */
void CPbkSettingsDlg::SetupPanesL()
    {
    __ASSERT_DEBUG(!iNaviDecorator && iTitlePane,
		Panic(EPanicPreCond_SetupPanesL));

    // Read and display the new text in title pane
	HBufC* titleText = iCoeEnv->AllocReadResourceLC
		(R_QTN_PHOB_SET_TITLE);
	iTitlePane->SetTextL(*titleText);
	CleanupStack::PopAndDestroy(); // titleText

	// Setup navi pane
	CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
    if (statusPane && statusPane->PaneCapabilities
			(TUid::Uid(EEikStatusPaneUidNavi)).IsPresent())
        {
		// Get pane
        CAknNavigationControlContainer* naviPane = 
            static_cast<CAknNavigationControlContainer*> 
            (statusPane->ControlL(TUid::Uid(EEikStatusPaneUidNavi)));
        TResourceReader reader;
        iCoeEnv->CreateResourceReaderLC(reader, R_PBK_SETTINGS_NAVILABEL);

		// Create new navi decorator
        iNaviDecorator = naviPane->ConstructNavigationDecoratorFromResourceL
			(reader);
        naviPane->PushL(*iNaviDecorator);
        CleanupStack::PopAndDestroy();  // reader
		}
    }

/**
 * Saves title pane.
 */
void CPbkSettingsDlg::SaveTitlePaneL()
    {
    __ASSERT_DEBUG(!iTitlePane && !iStoredTitlePaneText,
		Panic(EPanicPreCond_SaveTitlePaneL));

    CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
    if (statusPane &&  statusPane->PaneCapabilities
			(TUid::Uid(EEikStatusPaneUidTitle)).IsPresent())
        {
		// Store the title pane
        CAknTitlePane* titlePane = static_cast<CAknTitlePane*>
			(statusPane->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));

		delete iTitlePane;
		iTitlePane = NULL;
        iTitlePane = titlePane;

		// Store the old text
		iStoredTitlePaneText = titlePane->Text()->AllocL();
        }
    }

/**
 * Saves settings.
 */
void CPbkSettingsDlg::SaveSettingsL()
    {
	// Get name order value
	TInt nameOrder = iSettingsList->SettingValueL(EPbkSettingNameOrder);
	TBool separatorBetweenNames = EFalse;
	
	// Map the setting value to the NameOrder system used by Phonebook engine
	if (nameOrder == EPbkSettingNameOrderLastSpaceFirst)
	    {
	    nameOrder = CPbkContactEngine::EPbkNameOrderLastNameFirstName;
	    }
	else if (nameOrder == EPbkSettingNameOrderLastSeparatorFirst)
	    {
	    nameOrder = CPbkContactEngine::EPbkNameOrderLastNameFirstName;
	    separatorBetweenNames = ETrue;
	    }
	else if (nameOrder == EPbkSettingNameOrderFirstSpaceLast)
	    {
	    nameOrder = CPbkContactEngine::EPbkNameOrderFirstNameLastName;
	    }

    // Do not change the name ordering if it wasn't really changed
    // and most importantly if the view is not ready, otherwise contacts
    // server crashes
    TChar tempChar = iEngine.NameSeparator();
	if ((iUnderlyingViewReady && nameOrder != KErrNotFound)
		&& (nameOrder != iEngine.NameDisplayOrderL() || 
		   (separatorBetweenNames == tempChar.Eos())))
		{
		// Set underlying view flag
		iUnderlyingViewReady = EFalse;

        // Show wait note
        CAknWaitDialog* waitNote = 
            new(ELeave) CAknWaitDialog(reinterpret_cast<CEikDialog**>(&iWaitNote));
        waitNote->ExecuteLD(R_QTN_GEN_NOTE_PROCESSING);
        CleanupStack::PushL(waitNote);

		// Change the name ordering
		iEngine.SetNameDisplayOrderL
			((CPbkContactEngine::TPbkNameOrder)nameOrder);
		TChar separatorChar(0);

		if (separatorBetweenNames)
		    {
		    separatorChar = PbkViewUtils::ResolveNameOrderSeparatorL(iEngine);
            }		    
	    iEngine.SetNameSeparatorL(separatorChar);

        CleanupStack::Pop(waitNote);
        iWaitNote = waitNote;
		}
    }

void CPbkSettingsDlg::HandleContactViewEvent
	(const CContactViewBase& aView, 
    const TContactViewEvent& aEvent)
    {
    if (&aView==iAllContactsView)
        {
        switch (aEvent.iEventType)
            {
			case TContactViewEvent::EReady:	// FALLTHROUGH
            case TContactViewEvent::ESortOrderChanged:
                {
				// The view is ready
				iUnderlyingViewReady = ETrue;

                // Dismiss the wait note, if any
                if (iWaitNote)
                    {
	                TRAPD(err, iWaitNote->ProcessFinishedL());
	                if (err != KErrNone)
    		            {
	    	            delete iWaitNote;
                        iWaitNote = NULL;
		                }
                    }
                break;
                }
            default:
				{
				iUnderlyingViewReady = EFalse;
                break;
				}
            }
        }
    }

TBool CPbkSettingsDlg::OkToExitL
        (TInt aButtonId)
	{
	TBool result = ETrue;

    if (aButtonId == EAknSoftkeyOptions)
        {
        // Launch menu
        DisplayMenuL();
        result = EFalse;
        }
	else if (aButtonId == EAknSoftkeyBack)
		{
		// Do not exit here, let AttempExit do it
		AttemptExit();
		result = EFalse;
        }

	return result;
	}

void CPbkSettingsDlg::DynInitMenuPaneL
        (TInt aResourceId, 
        CEikMenuPane* aMenuPane)
    {
    switch (aResourceId)
        {
        case R_PBK_SETTINGS_MENU:
            {
            if (!FeatureManager::FeatureSupported(KFeatureIdHelp)) 
                {
                // remove non-supported help from menu
                aMenuPane->SetItemDimmed(EAknCmdHelp, ETrue);
                }
            }
            break;

        default:
            {
            break;
            }
        }
    }

void CPbkSettingsDlg::GetHelpContext
        (TCoeHelpContext& aContext) const
	{
	aContext.iMajor.iUid = KPbkUID3;
	aContext.iContext = KPHOB_HLP_SETTINGS;
    }

void CPbkSettingsDlg::SizeChanged()
    {
    CAknDialog::SizeChanged();
    if (iSettingsList) 
        {
        TRect parent;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, parent);
        AknLayoutUtils::LayoutControl(
            iSettingsList, parent, AknLayout::list_gen_pane(0));
        }
    }

void CPbkSettingsDlg::HandleResourceChange(TInt aType)
    {
    CAknDialog::HandleResourceChange(aType);
    
    CEikStatusPane* statusPane = iAvkonAppUi->StatusPane();
    if (statusPane)
		{
		statusPane->HandleResourceChange(aType);
		}	
    if (iSettingsList)
		{
		iSettingsList->HandleResourceChange(aType);
		}
    }

void CPbkSettingsDlg::FocusChanged( TDrawNow aDrawNow )
    {
    CAknDialog::FocusChanged( aDrawNow );
    if( iSettingsList )
        {
        iSettingsList->SetFocus( IsFocused(), aDrawNow );
        }
    }

// End of File
