/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 names list application view.
*
*/


#ifndef CPBK2NAMESLISTEXVIEW_H
#define CPBK2NAMESLISTEXVIEW_H

// INCLUDES
#include <e32base.h>
#include <coecobs.h>
#include <MPbk2UIExtensionView.h>
#include <MPbk2ControlObserver.h>
#include <MPbk2StoreConfigurationObserver.h>
#include <MPbk2MenuCommandObserver.h>
#include <mpbk2uiextensionview2.h>
#include <akntoolbarobserver.h>
#include <eiklbo.h>
#include <coemain.h>

#include <Pbk2Commands.hrh> 

// FORWARD DECLARATIONS
template<class ControlType> class CPbk2ControlContainer;
class CPbk2UIExtensionView;
class MPbk2ContactUiControl;
class CPbk2NamesListControl;
class MVPbkContactViewBase;
class MPbk2ContactNameFormatter;
class MPbk2CommandHandler;
class CPbk2ViewState;
class MPbk2PointerEventInspector;
class CPbk2IconInfoContainer;
class CVPbkContactManager;
class CPbk2StoreConfiguration;
class MCCAParameter;
class MCCAConnection;
class CPbk2ContactUiControlProxy;
class CAknStylusPopUpMenu;
class CPbk2MyCard;
class CSpbContentProvider;
class CPbk2ContextLaunch;

// CLASS DECLARATION

/**
 * Phonebook 2 names list application view.
 * Responsible for implementing Phonebook application view
 * interface for names list view.
 */
class CPbk2NamesListExView : public CBase,
                             public MPbk2UIExtensionView,
                             public MPbk2ControlObserver,
                             public MPbk2StoreConfigurationObserver,
                             public MPbk2MenuCommandObserver,
                             public MAknToolbarObserver,
                             public MCoeControlObserver,
                             public MEikListBoxObserver,
                             private MCoeForegroundObserver,
                             public MPbk2UIExtensionView2
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param 
         * @return  A new instance of this class.
         */
        static CPbk2NamesListExView* NewL(
                CPbk2UIExtensionView& aView,
                CSpbContentProvider& aStatusProvider,
                MCCAConnection*& aCCAConnection );

        /**
         * Destructor.
         */
        ~CPbk2NamesListExView();

    private: // From MPbk2UIExtensionView
        void HandleCommandL(
                TInt aCommand );
        TUid Id() const;
        void HandleStatusPaneSizeChange();
        void DoActivateL(
                const TVwsViewId& aPrevViewId,
                TUid aCustomMessageId,
                const TDesC8& aCustomMessage );
        void DoDeactivate();
        void DynInitMenuPaneL(
                TInt aResourceId,
                CEikMenuPane* aMenuPane );
        void HandleForegroundEventL(
                TBool aForeground );
        CPbk2ViewState* ViewStateLC() const;
        TBool HandleCommandKeyL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );
        void HandlePointerEventL(
                const TPointerEvent& aPointerEvent );
        TInt GetViewSpecificMenuFilteringFlagsL() const;
        void HandleLongTapEventL(
                const TPoint& aPenEventLocation,
                const TPoint& aPenEventScreenLocation);
        TAny* UIExtensionViewExtension( TUid aExtensionUid );

    protected: 
        // from MEikListBoxObserver
        void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );




    private: // From MPbk2ControlObserver
        void HandleControlEventL(
                MPbk2ContactUiControl& aControl,
                const TPbk2ControlEvent& aEvent );

    public: // From MPbk2StoreConfigurationObserver
        void ConfigurationChanged();
        void ConfigurationChangedComplete();

    public: // From MPbk2MenuCommandObserver
        void PreCommandExecutionL(
                const MPbk2Command& aCommand );
        void PostCommandExecutionL(
                const MPbk2Command& aCommand );

    public:		
        TBool IsRclOnL();
        
        /**
         * Getter for MyCard instance which can be NULL. 
         * Ownership NOT transferred to callee.
         * 
         * @return Mycard object 
         */
         CPbk2MyCard* MyCard() const;

    private: // From MAknToolbarObserver
        void DynInitToolbarL(
                TInt aResourceId,
                CAknToolbar* aToolbar );
        void OfferToolbarEventL(
                TInt aCommand );

    private: // From MCoeControlObserver
        void HandleControlEventL(
                CCoeControl* aControl,
                TCoeEvent aEventType );

    private: // Implementation
        CPbk2NamesListExView( 
            CPbk2UIExtensionView& aView,
            CSpbContentProvider& aStatusProvider,
            MCCAConnection*& aCCAConnection );
        void ConstructL();
        void ConstructToolbarL();
        void CreateControlsL();
        void StoreStateL();
        void RestoreStateL();
        void UpdateCbasL();
        static TInt IdleSaveToolbarStateL(
                TAny* aSelf );
        void AttachAIWProvidersL();
        void UpdateListEmptyTextL(
                TInt aListState );
        void Reset();
        void ShowToolbarOnViewActivation(TBool aShown);        
        static TInt IdleSetEmptyTextL(
                TAny* aSelf );
        void DimItem( CEikMenuPane* aMenuPane, TInt aCmd );
        void ShowContextMenuL();
        void AddCommandItemsToContainerL();
        TBool NeedToHideToolbar(TInt aCurrentCommand);
        TBool IsPhoneMemoryUsedL() const;
        
    private: // From MCoeForegroundObserver
        void HandleGainingForeground();
        void HandleLosingForeground();

    private: // Data
        CPbk2UIExtensionView& iView;
        /// This view's control container type
        typedef CPbk2ControlContainer<CPbk2NamesListControl> CContainer;
        /// Own: This view's control container
        CContainer* iContainer;
        /// Ref: This view's control
        CPbk2NamesListControl* iControl;
        /// Ref: Pointer event inspector
        MPbk2PointerEventInspector* iPointerEventInspector;
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager* iContactManager;
        /// Ref: Reference to the name formatter
        MPbk2ContactNameFormatter* iNameFormatter;
        /// Ref: Command handler
        MPbk2CommandHandler* iCommandHandler;
        /// Own: Stored control state
        CPbk2ViewState* iControlState;
        /// Own: Control state passed as parameter
        CPbk2ViewState* iParamState;
        /// Ref: Toolbar
        CAknToolbar* iToolbar;
        /// Own: Toolbar state indicator
        TBool iToolbarVisible;
        /// Own: Toolbar state is saved with the help of an idle object
        CIdle* iIdleToolbarWarden;
        /// Own: Stylus popup menu state indicator
        TBool iStylusPopupMenuLaunched;
        /// Own: Toolbar icon container
        CPbk2IconInfoContainer* iToolbarIcons;
        /// Own: ETrue, if any selection modifier key is depressed
        TBool iSelectionModifierUsed;
        /// Ref: Phonebook store configuration
        CPbk2StoreConfiguration* iStoreConfiguration;
        /// Own: Idle empty text setter
        CIdle* iIdleTexter;
        /// Own: List state
        TInt iListState;
        /// Own: Command state, indicates one command is alerady in execution.
        TBool iCommandState;
        /// Own: Proxy between commands and contact ui control
        CPbk2ContactUiControlProxy* iControlProxy;
        /// Own: contains current command id, if under execution. 
        /// Value is -1 if no command is executing
        TInt iCurrentCommandId;
        /// Own: control visible state, indicates if the control is visible before
        /// hide in background.
        TBool iCtrlVisibleStateBeforeLosingForground;
        /// Own: indicates whether the names list view is first time activated
        TBool iFirstTimeActivated;
        /// Ref: Popup menu
        CAknStylusPopUpMenu* iPopupMenu;
        
        /// Own:
        CPbk2MyCard* iMyCard;
        // If the application is brought to the foreground too fast after being set to the
        // backgroung the find box is not initialized properly by Avkon. 
        // In practise this means tapping the appshell icon for this application BEFORE 
        // the application has completed losing foreground actions. What happens in the
        // background is that the find box setfocus is called when the phonebook appui is 
        // still in the background. 
        // This flag is used to force the find box focus to be re-initialized when the 
        // application is back in the foreground.
        TBool iNeedSetFocus;
        
        // REF: content provider
        CSpbContentProvider&  iContentProvider;
        // Not own. CCA launcher connection
        MCCAConnection*& iCCAConnection;
        /// Own: Context launcher
        CPbk2ContextLaunch* iContextLauncher;
    };

#endif // CPBK2NAMESLISTEXVIEW_H

// End of File
