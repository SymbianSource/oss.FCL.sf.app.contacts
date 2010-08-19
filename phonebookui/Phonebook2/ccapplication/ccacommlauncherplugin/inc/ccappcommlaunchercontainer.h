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
* Description:  Class implementing CCCAppViewPluginAknContainer
*
*/


#ifndef __CCAPPCOMMALAUNCHERCONTAINER_H
#define __CCAPPCOMMALAUNCHERCONTAINER_H
// INCLUDES
#include "ccappcommlauncherheaders.h"
#include <Pbk2Commands.hrh>
#include "ccappstatuscontrol.h"
#include <aknlongtapdetector.h>
#include <akninputblock.h>

class CAknsBasicBackgroundControlContext;
class CAknPreviewPopUpController;
class CCCAppCommLauncherPlugin;
class CCCAppCommLauncherHeaderControl;
class CCCAppCommLauncherPbkCmd;
class CCCAppStatusControl;
class CPbk2ApplicationServices;
class CSpbContentProvider;
class MCCAViewLauncher;
class CCCAExtensionFactory;
class CCCAppCommLauncherCustomListBox;
class CCCaFactoryExtensionNotifier;
class CAknInputBlock;

/**
 * Class implementing CCAppCommLauncher -container for controls
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ccappcommlauncherplugin.dll
 *  @since S60 v5.0
 */
class CCCAppCommLauncherContainer :
    public CCCAppViewPluginAknContainer,
    public MEikListBoxObserver,
    public MCCAppContactHandlerNotifier,
    public MCCAStatusControlObserver,
    public MAknLongTapDetectorCallBack,
    public MAknInputBlockCancelHandler
    {
public: // constructor and destructor

    /**
     * CCCAppCommLauncherContainer.
     *
     * @param aPlugin reference
     */
    CCCAppCommLauncherContainer(
        CCCAppCommLauncherPlugin& aPlugin );

    /**
     * ~CCCAppCommLauncherContainer
     */
    virtual ~CCCAppCommLauncherContainer();

public: // Functions from base classes

    /**
     * From CCoeControl
     * (see details from baseclass )
     */
    void Draw( const TRect& aRect ) const;

    /**
     * From CCoeControl
     * (see details from baseclass )
     */
    virtual void SizeChanged();
	
	/**
     * From CCoeControl
     * (see details from baseclass )
     */
	void HandleResourceChange(TInt aType);

    /**
     * From CCoeControl
     * (see details from baseclass )
     */
    TInt CountComponentControls() const;

    /**
     * From CCoeControl
     * (see details from baseclass )
     */
    CCoeControl* ComponentControl( TInt aIndex )  const;

    /**
     * From CCoeControl
     * (see details from baseclass )
     */
    TKeyResponse OfferKeyEventL(
        const TKeyEvent& aKeyEvent, TEventCode aType);

    /**
     * From CCoeControl
     * (see details from baseclass )
     */
    TTypeUid::Ptr MopSupplyObject( TTypeUid aId );

    /**
     * From CCoeControl
     * (see details from baseclass )
     */
    void FocusChanged( TDrawNow aDrawNow );

    /**
     * From CCoeControl
     * (see details from baseclass )
     */
    void HandlePointerEventL(
        const TPointerEvent& aPointerEvent );
    
    /**
   	 * Default callback function for CAknLongTapDetector member object.
   	 * Base class implementation does nothing
   	 * 
   	 * @param aPenEventLocation Long tap event location relative to parent control.
     * @param aPenEventScreenLocation Long tap event location relative to screen.
   	 */
   	void HandleLongTapEventL( const TPoint& aPenEventLocation, 
        const TPoint& aPenEventScreenLocation );

    /**
     * From MEikListBoxObserver
     * (see details from baseclass )
     */
    void HandleListBoxEventL(
        CEikListBox *aListBox, TListBoxEvent aEventType );

    /**
     * From MCCAppContactHandlerNotifier
     * (see details from baseclass )
     */
    void ContactInfoFetchedNotifyL(
        const CCmsContactFieldInfo& aContactFieldInfo );

    /**
     * From MCCAppContactHandlerNotifier
     * (see details from baseclass )
     */
    void ContactFieldFetchedNotifyL(
        const CCmsContactField& aContactField );

    /**
     * From MCCAppContactHandlerNotifier
     * (see details from baseclass )
     */
    void ContactFieldFetchingCompletedL();

    /**
     * From MCCAppContactHandlerNotifier
     * (see details from baseclass )
     */
    void ContactsChangedL( /*MVPbkContactLink& aLink*/ );

    /**
     * From MCCAppContactHandlerNotifier
     * (see details from baseclass )
     */
    void ContactPresenceChangedL( const CCmsContactField& aContactField );

    /**
     * From CCoeControl
     * (see details from baseclass )
     */
    void GetHelpContext( TCoeHelpContext& aContext ) const;

    /**
     *  From MCCAStatusControlObserver
     */
    void StatusClickedL();

public: // New

    /**
     * Getter for plugin reference.
     *
     * @return Plugin reference
     * @since S60 v5.0
     */
    CCCAppCommLauncherPlugin& Plugin();

    /**
     * To check whether there are communication methods available
     *
     * @return ETrue if available, otherwise EFalse
     * @since S60 v5.0
     */
    TBool CommMethodsAvailable();

    /**
     * Getter for currently selected comm method.
     *
     * @return contact action
     * @since S60 v5.0
     */
    VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector SelectedCommunicationMethod();
    
    /**
     * To check whether the listbox is highlight
     *
     * @return ETrue if Highlight,otherwise EFalse
     * @since S60 v5.0
    */
    const TBool IsListBoxHighlightEnabled();
    
    /**
     * Get the amount of the list box
     *
     * @return the amount of the list box.
     * @since S60 v5.0
    */
    const TInt GetListBoxItemAmount() const;    
    

private: // New

    /**
     * Creates the listbox part and model for it
     *
     * @since S60 v5.0
     */
    void CreateListboxControlL();
    
    /**
     * Creates the status control
     */
    void CreateStatusControlL();

    /**
     * Updates the number/address popup
     */
    void UpdatePopupL();

    /**
     * Helper function for determining text truncation direction for the preview popup
     *
     * @param aContactAction The communication method
     */
    AknTextUtils::TClipDirection MapCommMethodToClipDirection(
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction );

    /**
     * Calls map commands
     *
     * @param aCommandId Command id
     */
    void DoShowMapCmdL( TPbk2CommandId aCommandId );
    
    /**
     * Long tap detector
     *
     * @param none
     */
    CAknLongTapDetector& LongTapDetectorL();
    /*
    * Gets called when ExtensionFactory plugin is installed/removed
    */
    static TInt CheckExtensionFactoryL(TAny* aPtr);
    
    /*
    * Hides/shows the status control based on Extensionfactory plugins
    * availability.
    */
    void DoCheckExtensionFactoryL();

    /*
    * Launches find on map contact action
    */
    void HandleFindOnMapContactActionL();

    /*
    * Launches generic contact action 
    * @param aActionType - Contact Action
    */
    void HandleGenericContactActionL(
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aActionType );
       
    /*
    * Used to check whether a Contact has only one number for initiating VoiceCall.  
    * Fills selection to aSelectedField if only one field is possible
    * Only works with VoiceCall Contact Action Type
    * 
    * @param aActionType - Contact ActionType. Here it should be EVoiceCallSelector
    * @param aSelectedField - Data of the selected field. (Phonenumber)
    * @return - returns ETrue, if more than one field is present for the contact which is capable 
    *       of VoiceCall.   
    *       In this case we need to show the field selection dialog
    *       wherein the user will choose the number for which he wants to start VoiceCall.
    *       
    *       returns EFalse - Straight Forward. Voice call can be made directly, 
    *       since we have only once number for this contact.
    *       The number to which VoiceCall needs to be made is filled in the aSelectedField.
    *            
    */
    TBool IsVoiceCallFieldSelectionAmbiguous(
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aActionType,
        TPtrC& aSelectedField );

    /*
    * Returns true if field has only one item
    * @param CCmsContactField
    */
    TBool HasFieldOnlyOneItem( const CCmsContactField& field ) const;

    /*
    * Returns true if field is voice call type
    * @param CCmsContactField
    */
    TBool IsVoiceCallType( const CCmsContactField& field ) const;
    
    /*
    * Returns true if contact has single address
    * @param aActionType - Contact ActionType.
    * @param aHandler - Contact Handler 
    */
    TBool HasContactSingleAddress(
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aActionType,
        CCCAppCommLauncherContactHandler& aHandler );
    
    
    /*
    * Executes contact action service with field selection
    * 
    * @param aActionType - Contact ActionType.
    * @param aContactIdentifier - Contact Link
    * @param aFullName - Contact's Name
    */
    void ExecuteContactActionServiceWithFieldSelectionL( 
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aActionType,
        TDesC8& aContactIdentifier,
        TDesC& aFullName );
    
    /*
    * Executes contact actions service without field selections
    * 
    * @param aActionType - Contact ActionType.
    * @param aContactIdentifier - Contact Link
    * @param aFullName - Contact's Name
    * @param aSelectedField - Contains the number to which 
    *                   VoiceCall needs to be established.
    */
    void ExecuteContactActionServiceWithoutFieldSelectionL( 
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aActionType,
        TDesC8& aContactIdentifier,
        TDesC& aFullName,
        TDesC& aSelectedField );
    
    /*
    * From MAknInputBlockCancelHandler
    */    
    void AknInputBlockCancel();
    
    /**
    * Removes the Input Blocker and makes the contianer regain 
    * user input, key and pen
    */    
    void RemoveInputBlocker();
    
    /**
    * Blocks user input, key and pen, from reaching
    * container untill the issue of communication is complete 
    */    
    void SetInputBlockerL();

private: // Constructors

    /**
     * ConstructL
     */
    void ConstructL();

private: // Data

    /**
     * Control showing the header part of the view
     * Own.
     */
    CCCAppCommLauncherHeaderControl* iHeaderCtrl;

    /**
     * Control to show the status icon and the text
     */
    CCCAppStatusControl* iStatusControl;

    /**
     * Listbox for the "communication buttons"
     * Own.
     */
    CCCAppCommLauncherCustomListBox* iListBox;

    /**
     * Background for skinning
     * Own.
     */
    CAknsBasicBackgroundControlContext* iBackground;

    /**
     * Ref to plugin.
     * Not own.
     */
    CCCAppCommLauncherPlugin& iPlugin;
    /**
     * Model containing the launchpad data
     * Own.
     */
    CCCAppCommLauncherLPadModel* iModel;

    /**
     * Indicates if focus has been dragged around the list, helps with single tap launching
     */
    TBool iHasBeenDragged;

    /**
     * Number of rows in model
     */
    TInt iMdlRowCount;     

    /**
     * Command handler
     * Own.
     */
    CCCAppCommLauncherPbkCmd* iPbkCmd;

    /**
	 * AppServices
	 * Not owned
	 */
    CPbk2ApplicationServices* iAppServices;

	/**
	 * Used for launching the Ecom plugins
	 * Own.
	 */    
    MCCAViewLauncher* iViewLauncher;
    
    /**
	 * ContentProvider
	 * Own.
	 */
	CSpbContentProvider* iProvider;
	
	/**
	 * iLongTapDetector
	 * Own.
	 */
	CAknLongTapDetector* iLongTapDetector;	
	
    /*
     * Used for hiding/showing the StatusControl
     * Own.
     */
	CCCaFactoryExtensionNotifier* iFactoryExtensionNotifier;
	
	
	TBool iLongTap;
	
	/**
	 * Owns - Input Blocker
	 */
	CAknInputBlock* iInputBlocker;
    };

#endif // __CCAPPCOMMALAUNCHERCONTAINER_H

// End of File
