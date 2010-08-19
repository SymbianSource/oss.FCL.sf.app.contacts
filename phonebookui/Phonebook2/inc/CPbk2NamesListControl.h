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
* Description:  Phonebook 2 names list control.
*
*/


#ifndef CPBK2NAMESLISTCONTROL_H
#define CPBK2NAMESLISTCONTROL_H

// INCLUDES
#include <coecntrl.h>
#include <MPbk2ContactUiControl.h>
#include <MPbk2ContactUiControl2.h>
#include <MVPbkContactViewObserver.h>
#include <MPbk2PointerEventInspector.h>
#include <MPbk2ContactUiControlUpdate.h>
#include <MPbk2UiControlCmdItem.h>
#include <mpbk2commanditemupdater.h>
#include <eiksbobs.h>
#include <aknsfld.h>
#include <eiklbo.h>
#include "CPbk2ControlContainer.h"
#include <aknmarkingmodeobserver.h>

// FORWARD DECLARATIONS
class MPbk2NamesListState;
class CPbk2ContactViewListBox;
class CPbk2NamesListStateFactory;
class CVPbkContactManager;
class MPbk2ContactNameFormatter;
class MPbk2ControlObserver;
class CPbk2StorePropertyArray;
class CAknSearchField;
class CPbk2ThumbnailLoader;
class MPbk2FilteredViewStack;
class CPbk2UiControlEventSender;
class CPbk2FilteredViewStackObserver;
class CPbk2ListBoxSelectionObserver;
class MPbk2ContactUiControlExtension;
class CPbk2PredictiveSearchFilter;
class CPbk2ContactViewDoubleListBox;
class MVPbkContactLinkArray;
class CPbk2NamesListControlBgTask;
class CPbk2ThumbnailManager;
class CPbk2HandleMassUpdate;

// CLASS DECLARATION

/**
 * Phonebook 2 names list control.
 * Responsible for delegating control calls to control state objects,
 * selecting correct state object based on events etc. indications,
 * and notifying observers about control events. In other words
 * this class manages the names list control framework and performs
 * the high level operations itself but delegates the detailed operations
 * to names list state classes.
 */
class CPbk2NamesListControl : public CCoeControl,
                              public MPbk2ContactUiControl,
                              public MPbk2ContactUiControl2,
                              public MVPbkContactViewObserver,
                              public MPbk2PointerEventInspector,
                              public MCoeControlObserver,
                              public MPbk2ContactUiControlUpdate,
                              public MEikScrollBarObserver,
                              public MAdaptiveSearchTextObserver,
                              public MEikListBoxObserver,
                              public MPbk2ControlContainerForegroundEventObserver,
                              public MPbk2CommandItemUpdater,
                              public MVPbkContactViewObserverExtension, 
                              public MAknMarkingModeObserver
    {
    public: // Enumerations

        /// The names list states
        enum TPbk2NamesListState
            {
            /// Startup state
            EStateStartup = 0,
            /// Not ready state
            EStateNotReady,
            EStateEmpty,
            EStateReady,
            EStateFiltered,
            EStateHidden
            };
        
        /// The names list Background Events
        enum TPbk2NamesListBgEvents
            {
            /// Startup state
            EStateBgTaskEmpty = 0,
            /// Mark & Unmark States
            EStateSaveMarkedContacts,
            EStateRestoreMarkedContacts            
            };

    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aResourceId       Id of the resource where the control
         *                          is defined in.
         * @param aContainer        This control's container.
         * @param aManager          Virtual Phonebook contact manager.
         * @param aView             The contact view.
         * @param aNameFormatter    Name formatter.
         * @param aStoreProperties  An array containg properties
         *                          for the stores that are shown.
         * @return A new instance of this class.
         */
        IMPORT_C static CPbk2NamesListControl* NewL(
                TInt aResourceId,
                const CCoeControl* aContainer,
                CVPbkContactManager& aManager,
                MVPbkContactViewBase& aView,
                MPbk2ContactNameFormatter& aNameFormatter,
                CPbk2StorePropertyArray& aStoreProperties );
        
        /**
        * Creates a new instance of this class.
        *
        * @param aResourceId       Id of the resource where the control
        *                          is defined in.
        * @param aContainer        This control's container.
        * @param aManager          Virtual Phonebook contact manager.
        * @param aView             The contact view.
        * @param aNameFormatter    Name formatter.
        * @param aStoreProperties  An array containg properties
        *                          for the stores that are shown.
        * @param aThumbManager       Thumbnail manager for social phonebook    //TODO
        * @return A new instance of this class.
        */
       IMPORT_C static CPbk2NamesListControl* NewL(
               TInt aResourceId,
               const CCoeControl* aContainer,
               CVPbkContactManager& aManager,
               MVPbkContactViewBase& aView,
               MPbk2ContactNameFormatter& aNameFormatter,
               CPbk2StorePropertyArray& aStoreProperties,
               CPbk2ThumbnailManager* aThumbManager );

        /**
         * Constructor. Defined as public for custom control needs.
         *
         * @param aContainer        This control's container.
         * @param aManager          Virtual Phonebook contact manager.
         * @param aView             The contact view.
         * @param aNameFormatter    Name formatter.
         * @param aStoreProperties  An array containg properties
         *                          for the stores that are shown.
         */
        IMPORT_C CPbk2NamesListControl(
                const CCoeControl* aContainer,
                CVPbkContactManager& aManager,
                MVPbkContactViewBase& aView,
                MPbk2ContactNameFormatter& aNameFormatter,
                CPbk2StorePropertyArray& aStoreProperties );

        /**
         * Destructor.
         */
        ~CPbk2NamesListControl();

    public: // Interface

        /**
         * Adds an observer to this control.
         *
         * @param aObserver     The observer to add.
         */
        IMPORT_C void AddObserverL(
                MPbk2ControlObserver& aObserver );

        /**
         * Removes an observer from this control.
         *
         * @param aObserver     The observer to remove.
         */
        IMPORT_C void RemoveObserver(
                MPbk2ControlObserver& aObserver );

        /**
         * Resets the control.
         * -Clears marks
         * -Resets the find box
         * -Sets focus to the first contact
         * -Removes the thumbnail
         * -Updates the state of the control asynchronously according to
         *  the Virtual Phonebook view state
         */
        IMPORT_C void Reset();

        /**
         * Sets the new view for the control. This will cause
         * the control to reset its state according to state of aView
         * asynchronously.
         */
        IMPORT_C void SetViewL( MVPbkContactViewBase& aView );

        /**
         * Sets the new view for the control. This will cause
         * the control to reset its state according to state of aView
         * asynchronously. Control takes given view's ownership.
         */
        IMPORT_C void GiveViewL( MVPbkContactViewBase* aView );

        /**
         * Use this method to block pointer events from this control.
         */
        IMPORT_C void AllowPointerEvents( TBool aAllow );

        /**
         * Set the group contact link for curently open group. 
         *    
         * @param aGroupLinktoSet  The input group contact link
         */
         IMPORT_C   void SetCurrentGroupLinkL( MVPbkContactLink* aGroupLinktoSet);
         /**
         * Fetches the current view
         *
         * @return  Current view.
         */
        MVPbkContactViewBase& View();
        
        /**
         * Foreground event handling function. 
         * @param aForeground - Indicates the required focus state of the control.
         * Needs to be called when there is a Foreground Event trigerred for the View.
         */
        
        IMPORT_C void HandleViewForegroundEventL( TBool aForeground );

        /**
         * For special case: Call this function to skip showing the 
         * blocking progress note when mass update is going on in nameslistview.
         * When done, MassUpdateSkipProgressNote(EFalse) must always be
         * called to reset back to original state (even if a Leave would happen in between) 
         * @param aSkip - ETrue if progress note should not be displayed. EFalse reset to normal.
         */
        IMPORT_C void MassUpdateSkipProgressNote( TBool aSkip );
        
        /**
         * Set the iOpeningCca flag. 
         * @param aIsOpening - Indicates user is opening cca or not.
         */
        IMPORT_C void SetOpeningCca( TBool aIsOpening );
        
    public: // From CCoeControl
        void MakeVisible(
                TBool aVisible );
        TKeyResponse OfferKeyEventL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );
        void ConstructFromResourceL(
                TResourceReader& aReader );
        void ActivateL();
        TInt CountComponentControls() const;
        CCoeControl* ComponentControl(
                TInt aIndex ) const;
        void FocusChanged(
                TDrawNow aDrawNow );
        void SizeChanged();
        void Draw(
                const TRect& aRect ) const;
        void HandleResourceChange(
                TInt aType );
        void HandlePointerEventL(
                const TPointerEvent& aPointerEvent );

    public: // From MPbk2ContactUiControl
        MPbk2ContactUiControl* ParentControl() const;
        TInt NumberOfContacts() const;
        const MVPbkBaseContact* FocusedContactL() const;
        const MVPbkViewContact* FocusedViewContactL() const;
        const MVPbkStoreContact* FocusedStoreContact() const;
        void SetFocusedContactL(
                const MVPbkBaseContact& aContact );
        void SetFocusedContactL(
                const MVPbkContactBookmark& aContactBookmark );
        void SetFocusedContactL(
                const MVPbkContactLink& aContactLink );
        TInt FocusedContactIndex() const;
        void SetFocusedContactIndexL(
                TInt aIndex );
        TInt NumberOfContactFields() const;
        const MVPbkBaseContactField* FocusedField() const;
        TInt FocusedFieldIndex() const;
        void SetFocusedFieldIndex(
                TInt aIndex );
        TBool ContactsMarked() const;
        MVPbkContactLinkArray* SelectedContactsL() const;
        MVPbkContactLinkArray* SelectedContactsOrFocusedContactL() const;
        MPbk2ContactLinkIterator* SelectedContactsIteratorL() const;
        CArrayPtr<MVPbkContactStore>* SelectedContactStoresL() const;
        void ClearMarks();
        void SetSelectedContactL(
                TInt aIndex,
                TBool aSelected );
        void SetSelectedContactL(
                const MVPbkContactBookmark& aContactBookmark,
                TBool aSelected );
        void SetSelectedContactL(
                const MVPbkContactLink& aContactLink,
                TBool aSelected );
        
        void DynInitMenuPaneL(
                TInt aResourceId,
                CEikMenuPane* aMenuPane ) const;
        void ProcessCommandL (
                TInt aCommandId ) const;
        void UpdateAfterCommandExecution();
        TInt GetMenuFilteringFlagsL() const;
        CPbk2ViewState* ControlStateL() const;
        void RestoreControlStateL(
                CPbk2ViewState* aState );
        const TDesC& FindTextL();
        void ResetFindL();
        void ShowThumbnail();
        void HideThumbnail();
        void SetBlank(
                TBool aBlank );
        void RegisterCommand(
                MPbk2Command* aCommand );
        void SetTextL(
                const TDesC& aText );
        TAny* ContactUiControlExtension(TUid aExtensionUid );       
           
   public: // From MPbk2ContactUiControl2
        TInt CommandItemCount() const;
        const MPbk2UiControlCmdItem& CommandItemAt( TInt aIndex ) const;
        const MPbk2UiControlCmdItem* FocusedCommandItem() const;
        void DeleteCommandItemL( TInt aIndex );
        void AddCommandItemL(MPbk2UiControlCmdItem* aCommand, TInt aIndex);
    
    public: // From MVPbkContactViewObserver
        void ContactViewReady(
                MVPbkContactViewBase& aView );
        void ContactViewUnavailable(
                MVPbkContactViewBase& aView );
        void ContactAddedToView(
                MVPbkContactViewBase& aView,
                TInt aIndex,
                const MVPbkContactLink& aContactLink );
        void ContactRemovedFromView(
                MVPbkContactViewBase& aView,
                TInt aIndex,
                const MVPbkContactLink& aContactLink );
        void ContactViewError(
                MVPbkContactViewBase& aView,
                TInt aError,
                TBool aErrorNotified );
        TAny* ContactViewObserverExtension(TUid aExtensionUid );
                  
    private: // From MVPbkContactViewUpdateObserver        
          void FilteredContactRemovedFromView( MVPbkContactViewBase& aView );
                  
    public: // From MPbk2PointerEventInspector
        TBool FocusedItemPointed();
        TBool FocusableItemPointed();
        TBool SearchFieldPointed();

    public: // From MCoeControlObserver
        void HandleControlEventL(
                CCoeControl* aControl,
                TCoeEvent aEventType );

    public: // From MPbk2ContactUiControlUpdate
        void UpdateContact(
                const MVPbkContactLink& aContactLink );
    
    public: // From MPbk2CommandItemUpdater
        void UpdateCommandItem( TInt aCommandId );

    public: // from MEikScrollBarObserver
         void HandleScrollEventL(CEikScrollBar* aScrollBar, TEikScrollEvent aEventType);
    public: // from MEikListBoxObserver
        void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );
    public: // from MAdaptiveSearchTextObserver

        void AdaptiveSearchTextChanged( CAknSearchField* aSearchField );

    public: // From MAknMarkingModeObserver 
            
        /**
         * This method is called when marking mode is activated or deactivated.
         * 
         * @param aActivated @c ETrue if marking mode was activate, @c EFalse
         *                   if marking mode was deactivated.
         */
        void MarkingModeStatusChanged( TBool aActivated );

        /**
         * This method is called just before marking mode is closed. Client can 
         * either accept or decline closing.
         * 
         * @return @c ETrue if marking mode should be closed, otherwise @c EFalse.
         */
        TBool ExitMarkingMode() const;
        
    private: // Implementation
        
        /**
         * Constructor.
         *
         * @param aContainer        This control's container.
         * @param aManager          Virtual Phonebook contact manager.
         * @param aView             The contact view.
         * @param aNameFormatter    Name formatter.
         * @param aStoreProperties  An array containg properties
         *                          for the stores that are shown.
         * @param aThumbManager     Thumbnail manager for social phonebook
         */
        CPbk2NamesListControl(
                const CCoeControl* aContainer,
                CVPbkContactManager& aManager,
                MVPbkContactViewBase& aView,
                MPbk2ContactNameFormatter& aNameFormatter,
                CPbk2StorePropertyArray& aStoreProperties,
                CPbk2ThumbnailManager* aThumbManager );
        
        void ConstructL(
                TInt aResourceId);
        void Redraw();
        void DoHandleContactViewReadyL(
                MVPbkContactViewBase& aView );
        void DoHandleContactAdditionL(
                TInt aIndex );
        void DoHandleContactRemovalL(
                TInt aIndex );
        void SelectAndChangeReadyStateL();
        void RelayoutControls();
        void HandleTopViewChangedL(
                MVPbkContactViewBase& aOldView );
        void HandleTopViewUpdatedL();
        void HandleBaseViewChangedL();
        void HandleViewStackError(
                TInt aError );
        void HandleContactAddedToBaseView(
                MVPbkContactViewBase& aBaseView,
                TInt aIndex,
                const MVPbkContactLink& aContactLink );
        void DeleteIfAlreadyAdded(const MPbk2UiControlCmdItem* aCommand); // Fix ETKI-7NADZC
        

    private: //from MPbk2ControlContainerForegroundEventObserver
        void HandleForegroundEventL( TBool aForeground );
    
    public: //For Storing/Restoring/Clearing the Marked Contacts
        void StoreMarkedContactsAndResetViewL();
        void RestoreMarkedContactsL();
        void ClearMarkedContactsInfo();
        
    public:
        inline void SetMarkingMode( TBool aActived )
            {
            iMarkingModeOn = aActived;
            }
        
        inline TBool GetMarkingMode() const
            {
            return iMarkingModeOn;
            }
        
    private: // Data
        /// Ref: Current state
        MPbk2NamesListState* iCurrentState;
        /// Own: State factory
        CPbk2NamesListStateFactory* iStateFactory;
        /// Own: List box
        CPbk2ContactViewListBox* iListBox;
        /// Own: Observers of this object
        RPointerArray<MPbk2ControlObserver> iObservers;
        /// Own: Previous view count
        TInt iPrevCount;
        /// Own: Find box
        CAknSearchField* iFindBox;
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iContactManager;
        /// Ref: All contacts view
        MVPbkContactViewBase* iBaseView;
        /// Ref: The view that is used by the control
        MPbk2FilteredViewStack* iViewStack;
        /// Ref: Control container
        const CCoeControl*  iContainer;
        /// Ref: Name formatter
        MPbk2ContactNameFormatter& iNameFormatter;
        /// Ref: The store's UI properties
        CPbk2StorePropertyArray& iStoreProperties;
        /// Own: Thumbnail loader
        CPbk2ThumbnailLoader* iThumbnailLoader;
        /// Ref: Command in Execution
        MPbk2Command* iCommand;
        /// Own: A control event sender
        CPbk2UiControlEventSender* iEventSender;
        /// Own: An observer for the filtered view stack
        CPbk2FilteredViewStackObserver* iStackObserver;
        /// Own: An observer for listbox selection mode changes
        CPbk2ListBoxSelectionObserver* iListBoxSelectionObserver;
        /// Own: Flag for owning the base view
        TBool iOwnBaseView;
        /// Own: UI extension
        MPbk2ContactUiControlExtension* iUiExtension;
        /// flag whether pointer moved or not for the thumbnail
        TBool iDidMove;
        /// Defines whether this control handles pointer events or not.
        TBool iAllowPointerEvents;
        /// Command items. Owns the array members.
        RPointerArray<MPbk2UiControlCmdItem> iCommandItems; 
        /// Own: Search filter
        CPbk2PredictiveSearchFilter* iSearchFilter;
        //Flag to indicate Feature manager initilization
        TBool iFeatureManagerInitilized;
        
        // Own/Ref (see below): Thumbnail manager
        CPbk2ThumbnailManager* iThumbManager;
        // Wheter this control owns the thumbnail manager (iThumbManager) or not 
        TBool iOwnThumbManager;
        //OWN: double list box "handle"
        CPbk2ContactViewDoubleListBox* iDoubleListBox;
        
        //Own: Selected/Marked Contacts
        MVPbkContactLinkArray* iSelectedLinkArray;
           
        //Own: Background Task Handler     
        CPbk2NamesListControlBgTask* iBgTask;
        //Own: Mass update checker/handler
        CPbk2HandleMassUpdate* iCheckMassUpdate;
        //Own: Open Cca is in progress
        TBool iOpeningCca;
        // Flag to indicate marking mode is active
        TBool iMarkingModeOn;
    };

#endif // CPBK2NAMESLISTCONTROL_H

// End of File
