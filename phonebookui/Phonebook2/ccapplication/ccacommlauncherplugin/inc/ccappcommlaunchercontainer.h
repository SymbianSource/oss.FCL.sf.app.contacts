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
    public MAknLongTapDetectorCallBack
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
    };

#endif // __CCAPPCOMMALAUNCHERCONTAINER_H

// End of File
