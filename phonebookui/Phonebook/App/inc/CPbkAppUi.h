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
*       Phonebook application UI class. 
*
*/


#ifndef __CPbkAppUi_H__
#define __CPbkAppUi_H__


//  INCLUDES
#include <CPbkAppUiBase.h>
#include <coemain.h>
#include "RPbkViewResourceFile.h"
#include <MPbkAppUiExtension.h>


//  FORWARD DECLARATIONS
class CAknNavigationControlContainer;
class CPbkDocument;
class CPbkApplication;
class CPbkCompressUiImpl;
class CEikImage;
class MPbkCommandHandler;
class CPbkFFSCheck;
class CPbkAppGlobals;
class CBCardEngine;
class CPbkViewNavigator;
class MPbkAppUiExtension;
class MPbkCommand;


//  CLASS DECLARATION 

/**
 * Phonebook application UI class. 
 * An object of this class is created by the Symbian OS
 * framework by a call to CPbkDocument::CreateAppUiL(). 
 * The application UI object creates and owns the application's
 * views and handles system commands selected from the menu.
 */
class CPbkAppUi : 
        public CPbkAppUiBase,
        public MCoeForegroundObserver
    {
    public:  // Constants and types
        static const TUid KPbkNamesListViewUid;
        static const TUid KPbkGroupsListViewUid;
        static const TUid KPbkGroupMembersListViewUid;
        static const TUid KPbkContactInfoViewUid;
        static const TUid KPbkBusinessCardViewUid;
        static const TUid KPbkPhotoViewUid;
        static const TUid KPbkPersonalInfoViewUid;

    public:  // Constructors and destructor
        /**
         * Constructor.
         */
        CPbkAppUi();

        /**
         * Destructor.
         */
        ~CPbkAppUi();

    public:  // New functions
        /**
         * Returns the Phonebook document object.
         */
        CPbkDocument* PbkDocument() const;

        /**
         * Returns the Phonebook application object.
         */
        CPbkApplication* PbkApplication() const;

        /**
         * Gets reference to this app's default navi pane control.
         */
        CAknNavigationControlContainer* NaviPane();

        /**
         * Call to exit Phonebook in correct way.
         */
        void ExitL();
        
        /**
         * Call to force exit in Phonebook.
         */
        void Exit();

        /**
         * Returns ETrue if this application is running on foreground, EFalse otherwise.
         * Running on foreground means that the user has not pressed exit.
         */
        TBool IsRunningForeground() const;       
                          
        /**
         * Sends application to the background
         */
        void SendAppToBackgroundL();

    public:  // from CPbkAppUiBase
        void ActivatePhonebookViewL
            (TUid aViewId, const CPbkViewState* aViewState=NULL);
        void ActivatePreviousViewL(const CPbkViewState* aViewState = NULL);
        CViewActivationTransaction* HandleViewActivationLC
            (const TUid& aViewId, const TVwsViewId& aPrevViewId,
            const TDesC* aTitlePaneText, 
            const CEikImage* aContextPanePicture,
            TUint aFlags=EUpdateAll);
		CPbkAppGlobalsBase* AppGlobalsL();
        void FFSClCheckL(const MPbkCommandHandler& aCommandHandler,
			TInt aBytesToWrite = 0);

    private: // from CAknViewAppUi
	    void HandleCommandL(TInt aCommand);
	    void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);
        void ConstructL();
	    TKeyResponse HandleKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	    void HandleForegroundEventL(TBool aForeground);
		void HandleResourceChangeL(TInt aType);
        void HandleWsEventL(const TWsEvent& aEvent, CCoeControl* aDestination);

    private:  // from MPbkKeyEventHandler
        TBool PbkProcessKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

    private: // from MCoeForegroundObserver
    	void HandleGainingForeground();
    	void HandleLosingForeground();

    private: // Command handlers (use "Cmd" prefix)
        void CmdExitL();

    private: // Implementation
        void CreateEngineL();
        void ReadViewGraphL();
        void CreateViewsL();
        TBool IsPhonebookView(const TVwsViewId& aViewId) const;
        class CViewActivationTransactionImpl;
        friend class CViewActivationTransactionImpl;

    private: // data
        /// Ref: this app's default navi pane
        CAknNavigationControlContainer* iNaviPane;
        /// Own: Phonebook view dll resource file
        RPbkViewResourceFile iViewResourceFile;
        /// Own: View navigation graph
        CDigViewGraph* iViewGraph;
        /// Own: view navigator
        CPbkViewNavigator* iViewNavigator;
        /// Own: Phonebook compress UI
        CPbkCompressUiImpl* iCompressUi;
        /// Own: FFS space checker
        CPbkFFSCheck* iFFSCheck;
        /// Own: this is the only instance of this class
        CPbkAppGlobals* iAppGlobals;
        /// Own: appui extension
        MPbkAppUiExtension* iAppUiExtension;
        /// Own: Application running on foreground indicator        
        TBool iIsRunningForeground;
        /// Own: End key exit flag
        TBool iEndKeyExit;
        
    };


#endif  // __CPbkAppUi_H__
            

// End of File
