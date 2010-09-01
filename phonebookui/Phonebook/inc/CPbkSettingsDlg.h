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
*     Settings list dialog for Phonebook.
*
*/


#ifndef __CPbkSettingsDlg_H__
#define __CPbkSettingsDlg_H__

// INCLUDES
#include <AknDialog.h>
#include <cntviewbase.h>

// FORWARD DECLARATIONS
class CPbkSettingsList;
class CAknTitlePane;
class CAknNavigationDecorator;
class CPbkContactEngine;
class TCoeHelpContext;
class CAknWaitDialog;

// CLASS DECLARATION

/**
 * Settings list dialog.
 */
class CPbkSettingsDlg :
		public CAknDialog,
		private MContactViewObserver
	{
	public: // interface
        /**
         * Creates a new instance of this class.
		 * @param aEngine reference to Phonebook engine
         */
		IMPORT_C static CPbkSettingsDlg* NewL(CPbkContactEngine& aEngine);

        /**
         * Executes the settings dialog.
         */
		IMPORT_C void ExecuteLD();

	private: // from CAknDialog
		void PreLayoutDynInitL();
		void ProcessCommandL(TInt aCommandId);
		TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,
			TEventCode aType);
		TBool OkToExitL(TInt aButtonId);
        void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
        void FocusChanged( TDrawNow aDrawNow );
    private:  // from MContactViewObserver
    	void HandleContactViewEvent
            (const CContactViewBase& aView,const TContactViewEvent& aEvent);
            
    private:  // from CCoeControl
        void SizeChanged();
	    void HandleResourceChange(TInt aType);
	    
	private: // implementation
		CPbkSettingsDlg(CPbkContactEngine& aEngine);
		void ConstructL();
		~CPbkSettingsDlg();
		void CreateSettingsListL();
		void AttemptExit();
		void SaveTitlePaneL();
		void SetupPanesL();
		void SaveSettingsL();
		void GetHelpContext(TCoeHelpContext& aContext) const;
		TRect ReadScreenResourceL(TInt aResourceId);

	private: // data
		/// Ref: phonebook engine
		CPbkContactEngine& iEngine;
		/// Own: settings list
		CPbkSettingsList* iSettingsList;
        /// Own: title pane text to restore on exit
        HBufC* iStoredTitlePaneText;
        /// Ref: application's title pane
        CAknTitlePane* iTitlePane;
        /// Own: navi pane control
        CAknNavigationDecorator* iNaviDecorator;
        /// Ref: all contacts view
        CContactViewBase* iAllContactsView;
		/// Own: underlying view state
		TBool iUnderlyingViewReady;
        /// Own: wait note wrapper
        CAknWaitDialog* iWaitNote;
        /// For testing only
        friend class CPbkSettingsDlg_TestAccess;
	};

#endif // __CPbkSettingsDlg_H__

// End of File
