/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CCAPPMYCARDCONTAINER_H
#define CCAPPMYCARDCONTAINER_H

// INCLUDES
#include <eiklbo.h>
#include <ccappviewpluginakncontainer.h>
#include "ccappmycard.h"
#include "ccappmycardimageloader.h"
#include <MPbk2ContactUiControl.h>
#include <MPbk2ClipListBoxText.h>
#include <aknlongtapdetector.h>
#include "ccappmycardheadercontrol.h"

#include "ccappstatuscontrol.h"

class CCCAppMyCardPlugin;
class CCCAppMyCardHeaderControl;
class CAknsBasicBackgroundControlContext;
class CAknFormDoubleGraphicStyleListBox;
class CSpbContactDataModel;
class CCCAppStatusControl;
class CPbk2ApplicationServices;
class CSpbContentProvider;
class MCCAViewLauncher;
class CCCAExtensionFactory;
class CCCaFactoryExtensionNotifier;
class CPbk2IconArray;

/**
 * Class implementing CCAppMyCard -container for UI controls
 *
 *
 *  @lib ccappmycardplugin.dll
 *  @since S60 9.2
 */
class CCCAppMyCardContainer :
    public CCCAppViewPluginAknContainer,
    public MMyCardObserver,
    public MMyCardImageLoaderObserver,
    public MEikListBoxObserver,
    public MPbk2ContactUiControl,
    public MCCAStatusControlObserver,
    public MMyCardHeaderControlObserver,
    public MAknLongTapDetectorCallBack,
    public MPbk2ClipListBoxText
    {
public: // constructor and destructor

    /**
     * CCCAppMyCardContainer.
     *
     * @param aPlugin reference
     */
    CCCAppMyCardContainer(
        CCCAppMyCardPlugin& aPlugin );

    /**
     * ~CCCAppMyCardContainer
     */
    ~CCCAppMyCardContainer();

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
    void SizeChanged();

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
    TTypeUid::Ptr MopSupplyObject( TTypeUid aId );

    /**
     * From CCoeControl
     */
    TKeyResponse OfferKeyEventL(
        const TKeyEvent& aKeyEvent,
        TEventCode aType );

    /**
     * From CCoeControl
     */
    void GetHelpContext(TCoeHelpContext& aContext) const;
    
    /**
     * From MMyCardObserver
     */
    void MyCardEventL( MMyCardObserver::TEvent aEvent );

    /**
     * From MMyCardImageLoaderObserver
     */
    void ThumbnailReady( CFbsBitmap* aThumbnail );
    
    /**
     * From MMyCardImageLoaderObserver
     */
    void ThumbnailLoadError( TInt aError );

    /**
     * From MEikListBoxObserver
     */
    void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );

    /**
     *  From MCCAStatusControlObserver
     */
    void StatusClickedL();

    /*
     * From MMyCardHeaderControlObserver
     */
    void MyCardHeaderControlClickL( TPoint aPos );
    
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
    * From CoeControl
    * Overridden to receive pointer events
    */
    void HandlePointerEventL( const TPointerEvent& aPointerEvent );

    /**
     * From MPbk2ClipListBoxText
     */
    TBool ClipFromBeginning(
        TDes& aBuffer,
        TInt aItemIndex,
        TInt aSubCellNumber);

    
public: // New

    /**
     * Getter for plugin reference.
     *
     * @return Plugin reference
     * @since S60 v5.0
     */
    CCCAppMyCardPlugin& Plugin();


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
    void ProcessCommandL( TInt aCommandId ) const;

    /*
     *  Checks the ecom implementations available and lauches the first
     *  with correct UID
     */
    MCCAViewLauncher* CreateViewLauncherL( );

private: // Constructors

    /**
     * ConstructL
     */
    void ConstructL();
    
    /**
    *  Sets the name to mycard header control in correct format
    */ 
    void SetNameForHeaderControlL();
    
    /*
     * Sets the correct place for Image selection popup menu
     */
    void PosToScreenCoordinates( CCoeControl* aControl, TPoint& aPos );

    /*
     * Gets called when ExtensionFactory plugin is installed/removed
     */
    static TInt CheckExtensionFactoryL(TAny* aPtr);
    
    /*
     * Hides/shows the status control based on Extensionfactory plugins
     * availability.
     */
    void DoCheckExtensionFactoryL();

    inline void ResolveNameOrderL();
    
private: // Data

    /**
     * Ref to plugin.
     * Not own.
     */
    CCCAppMyCardPlugin& iPlugin;

    /**
     * Background for skinning
     * Own.
     */
    CAknsBasicBackgroundControlContext* iBackground;

    /**
     * Header UI control of MyCard view
     * Own.
     */
    CCCAppMyCardHeaderControl* iHeaderCtrl;

    /**
     * Details list UI control of MyCard view
     * Own.
     */
    CAknFormDoubleGraphicStyleListBox* iListBox;

    /**
     * Reference to listbox model
     * Not own.
     */
    CSpbContactDataModel& iModel;

    /**
     * Contact image loader
     * Own.
     */
    CCCAppMyCardImageLoader* iImageLoader;

    /// Ref: command in execution
	MPbk2Command* iCommand;
	
    /**
     * Used for launching the Ecom plugins
     * Own.
     */    
	MCCAViewLauncher* iViewLauncher;
	
	TBool iControlLink;

   /** 
	  * Contact image selection stylus popup menu 
	  * Own. 
	  */  
	CAknStylusPopUpMenu* iImageSelectionPopup;
	
	/*
	 * Flag for checking whether or not an user image is set to MyCard
	 */
	TBool iMyCardImageSet;
	
	/*
	 * Used for hiding/showing the StatusControl
	 * Own.
	 */
	CCCaFactoryExtensionNotifier* iFactoryExtensionNotifier;

	TInt iNameOrder;
	
	/**
     * iLongTapDetector
     * Own.
     */
    CAknLongTapDetector* iLongTapDetector;  
    
    /** 
      * Contact detail selection stylus popup menu 
      * Own. 
      */  
    CAknStylusPopUpMenu* iDetailsPopup;
    
    /*
     * Flag for checking if Stylys opened 
     */
    TBool iLongTapHandled;    
    
    /**
     * Not own. Listbox's icon array
     */
    CPbk2IconArray* iIconArray;
    };

#endif // CCAPPMYCARDCONTAINER_H

// End of File
