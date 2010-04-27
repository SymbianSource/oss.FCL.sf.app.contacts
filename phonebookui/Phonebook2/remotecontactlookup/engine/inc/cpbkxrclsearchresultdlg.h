/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of the class CPbkxRclSearchResultDlg.
*
*/


#ifndef CPBKXRCLSEARCHRESULTDIALOG_H
#define CPBKXRCLSEARCHRESULTDIALOG_H

#include <coemain.h>

#include "cpbkxrclbasedlg.h"
#include "pbkxrclengineconstants.h"
#include "fsccontactactionmenudefines.h"  //"pbkxcontactactionmenudefines.h"

class CAknNavigationControlContainer;
class CAknNavigationDecorator;
class CAknTitlePane;
class CContactCard;
class CPbkContactEngine;
class CPbkxRclActionServiceWrapper;
class CPbkxContactActionMenu;
class CGulIcon;
class CFscContactActionMenu;
class CAknLongTapDetector;
class CIdle;
/**
* Dialog for showing remote contact lookup search results.
*
* Offers menu options and acion menu to initiate actions for contacts.
*/
class CPbkxRclSearchResultDlg : 
    public CPbkxRclBaseDlg, 
    public MCoeForegroundObserver,
    public MEikListBoxObserver, 
    public MFsActionMenuPositionGiver
    {                          
public: // constructor and destructor
    
    /**
    * Creates new object and returns pointer to it.
    *
    * @param aIndex Index which user selected from the list (probably not used?)
    * @param aArray Array of items.
    * @param aCommand Command observer for observing menu commands.
    * @param aContactItems Contact items to be shown on the dialog.
    * @param aContactEngine Contact engine for creating pbk contact items.
    * @param aActionService Action service wrapper.
    * @param aNaviText Navi text to be displayed.
    * @param aContactSelectorEnabled Flag indicating whether RCL is in contact
    *                                selector mode or not.
    * @return Created object.
    */
    static CPbkxRclSearchResultDlg* NewL(
        TInt &aIndex,
        CDesCArray* aArray,
        MEikCommandObserver* aCommand,
        RPointerArray<CContactCard>& aContactItems,
        CPbkContactEngine& aContactEngine,
        CPbkxRclActionServiceWrapper& aActionService,
        TBool aContactSelectorEnabled,
        TBool aMoreThanMaxResults);
        
    /**
    * Destructor.
    */
    virtual ~CPbkxRclSearchResultDlg();

public: // methods from base classes

    /**
    * Hides menu items that are not usable.
    *
    * @param aResourceId Menu resource id.
    * @param aMenu Menu pane.
    */
    virtual void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane *aMenuPane );

    /**
    * Gets help context for this view.
    *
    * @param aContext On return, contains help context information
    */
    virtual void GetHelpContext( TCoeHelpContext& aContext ) const;
 
    /**
    * Dialog is popped on foreground.
    */
    virtual void HandleGainingForeground();

    /**
    * Dialog is popped background.
    */
    virtual void HandleLosingForeground();
    
    /**
    * From MEikListBoxObserver
    * Handles listbox pointer events
    */
    void HandleListBoxEventL(
            CEikListBox *aListBox, TListBoxEvent aEventType );


public: // new methods

    /**
    * Executes dialog and destroys it after dialog is closed.
    */
    void ExecuteLD();

    /**
    * Returns index of the currently selected item.
    *
    * @return Index of the currently selected item.
    */
    TInt CurrentItemIndex() const;

    /**
    * Sets index of the currently selected item.
    *
    * @param aIndex Index to be set.
    */
    void SetCurrentItemIndexL( TInt aIndex );
    
    /**
    * Sets current contact to action service.
    *
    * This affect menu item visibility.
    */
    void SetCurrentContactToActionServiceL();
    
    /**
    * Closes dialog.
    */
    void Close();
    
    /**
    * Updates search result dialog.
    *
    * @param aNaviText New navi text.
    */
    void UpdateDialogL();
    
    /**
    * Sends business card of the currently selected contact.
    */
    void SendBusinessCardL();

    //Fix for: PKAO-7NNCJ2
    /**
    * Informs dialog that info dialog is visible.
    */
    void InfoDlgVisible( TBool aVisible );
    
    /**
     * Set the internal flag telling wether last search found more than
     * maximum number of results. 
     * 
     * @param aMoreThanMax ETrue\EFalse
     */
    void SetMoreThanMaxResults(TBool aMoreThanMax);
    
    /**
     * The state of the internal flag telling wether last search
     * found more than maximum number of results.
     * 
     * @return TBool ETrue\EFalse
     */
    TBool MoreThanMaxResults();

protected: // redefinition of functions of the base class

    /**
    * From CAknSelectionListDialog.
    *
    * Handles key events.
    *
    * @param aKeyEvent Key event.
    * @param aType Type of the key event.
    * @return Key response (handled/not handled).
    */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

    TBool OkToExitL( TInt aButtonId );
    
private: // methods used internally
    
   /**
    * Sets up title pane text.
    */
    void SetTitlePaneTextL();
    
    /**
    * Sets up status pane.
    */
    void SetupStatusPaneL();
    
    /**
    * Updates status pane.
    */
    void UpdateStatusPaneL();

    /**
    * Resets status pane.
    */
    void ResetStatusPane();

    /**
    * Creates list box entries.
    */
    void CreateListBoxEntriesL();

    /**
    * Creates given list box entry.
    *
    * @param aIndex Index of the list box entry.
    */
    void CreateListBoxEntryL( TInt aIndex );
    
    /**
    * Updates given list box entry.
    *
    * @param aIndex List box entry index.
    */
    void UpdateListBoxEntryL( TInt aIndex );

    /**
    * Loads navi text from resources.
    *
    * @return Navi text.
    */
    void LoadNaviTextL();

    /**
    * Returns ETrue if there is at least one call action available.
    *
    * @return ETrue if there is at least one call action available.
    */
    TBool CallActionsAvailable() const;

    /**
    * Creates and initializes action menu.
    */
    void InitializeActionMenuL();

    /**
    * Shows action menu and handles possible custom command.
    */
    void ShowActionMenuL( const TPoint& aPenEventScreenLocation );

    /**
    * Calculates action menu position based on currently selected
    * item.
    *
    * @return Action menu position.
    */
    TFscContactActionMenuPosition GetActionMenuPosition();

    /**
    * Returns ETrue if action menu is available.
    *
    * @return ETrue if action menu is available.
    */
    TBool ActionMenuAvailable() const;

    /**
    * Creates and adds save as contact action to action menu.
    */
    void CreateAndAddSaveAsContactActionL();

    /**
    * Returns ETrue if scrolling is in progress.
    *
    * Scrolling means that user is pressing up or down arrow so that
    * it creates multiple key presses.
    *
    * @return ETrue if scrolling is enabled.
    */ 
    TBool ScrollingInProgress() const;
    
    /**
    * This method is called when scrolling has stopped.
    */
    void ScrollingStoppedL();
    
    /**
     * Shows a note to user that more than max number of
     * results found and only N number of those is shown
     */
    void DisplayTooManyResultsInfoNoteL();

private: // constructors

    /**
    * Constructor.
    *
    * @param aIndex Index which user selected from the list (probably not used?)
    * @param aArray Array of items.
    * @param aCommand Command observer for observing menu commands.
    * @param aContactItems Contact items to be shown on the dialog.
    * @param aContactEngine Contact engine for creating pbk contact items.
    * @param aActionService For executing menu commands.
    * @param aContactSelectorEnabled Flag indicating whether RCL is in contact
    *                                selector mode or not.
    */ 
    CPbkxRclSearchResultDlg(
        TInt& aIndex,
        CDesCArray* aArray,
        MEikCommandObserver* aCommand,
        RPointerArray<CContactCard>& aContactItems,
        CPbkContactEngine& aContactEngine,
        CPbkxRclActionServiceWrapper& aActionService,
        TBool aContactSelectorEnabled,
        TBool aMoreThanMaxResults);
        

    /**
    * Second-phase constructor.
    *
    * @param aNaviText Navi text to be displayed.
    */
    void ConstructL();

    /**
    * Constructs menu bar.
    * 
    * @param aResourceId - menubar resource id 
    */
    void ConstructMenuBarL( TInt aResourceId );
  
private:
    
    /**
    * From CCoeControl,HandlePointerEventL.
    */
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);    
    
    
    /**
    * From MFsActionMenuPositionGiver
    */    
    TPoint ActionMenuPosition();
    
    
    CAknLongTapDetector& LongTapDetectorL();
    
    /**
     * Idle callback for the "more than 50 results" info note.
     * 
     * For smooth showing of the note, an idle object is used to
     * schedule the display of the note.
     * 
     * @param aSelf - handle for the object that made the request
     */
    static TInt TooManyResultsIdleCallbackL(TAny* aSelf );
    
private: // data
    
    // Indexes of icon array.
    enum TIconIndex
        {
        EActionMenuEnabled = 0    // Action menu enabled icon
        };

    // Contact engine.
    CPbkContactEngine& iContactEngine;

    // Contact action service wrapper. Used to execute menu commands.
    CPbkxRclActionServiceWrapper& iContactActionService;

    // Contact action menu for executing commands. Owned.
    CFscContactActionMenu* iActionMenu;
    
    // Items. Not owned.
    CDesCArray* iItems;

    // Navi pane of the current UI application. Not owned.
    CAknNavigationControlContainer* iNaviPane;
    
    // Navigation label to be shown while result dialog is shown. Owned.
    CAknNavigationDecorator* iNaviDecorator;
    
    // Contact items. Not owned.
    RPointerArray<CContactCard>& iContactItems;

    // Flag indicating if contact selector is enabled.
    TBool iContactSelectorEnabled;

    // Flag indicating whether dialog should be closed after processing user command.
    TBool iClose;

    // Selected item index.
    TInt& iSelectedItemIndex;

    // Navigation label text. Owned.
    RBuf iNaviText;

    // Icon for add recipient action menu item. Owned.
    CGulIcon* iAddRecipientIcon;

    // Flag indicating whether save as contact action menu item is
    // already added.
    TBool iSaveAsAdded;

    // Set when dialog is exiting. After that no key presses are handled.
    TBool iExit;
    
    // Flag indicating that up or down arrow is held down.
    TBool iKeyDown;

    // Counter how many keypresses has occurred since key has been pressed 
    // down.
    TInt iKeyCounter;

    //Fix for: PWAN-7MC7WA
    //Flag indicating that CAS query was performed
    TBool iIsScrollLaunched;
    
    //Fix for: PKAO-7NNCJ2
    //Flag indicating that info dialog is visible. Owned.
    TBool iIsInfoDlgVisible;
    
    //Holds the position of the Contact Action Menu
    TPoint iActionMenuPosition;
    
    //ETrue - If Contact Action Menu is being Shown
    TBool iActionMenuStarted;
	
    ///ETrue - if the more than max results note needs to be shown
    TBool iMoreThanMaxResults;
    
    // Title pane text. Owned.
    HBufC* iTitleText;
    // Original title pane text (of the caller view). Owned.
    HBufC* iOriginalTitleText;
   
    // Pointer reference to title pane. Not owned.
    CAknTitlePane* iTitlePane;
    
    /// own. idle object for scheduling the showing of the "more than 50 results" 
    /// info note
    CIdle* iIdleNote;
    };

#endif
