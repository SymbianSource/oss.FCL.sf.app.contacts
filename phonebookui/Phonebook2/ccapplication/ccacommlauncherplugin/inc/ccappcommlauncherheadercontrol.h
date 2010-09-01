/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The header part control(s) of the commlauncher plugin UI
*
*/


#ifndef CCCAPPCOMMLAUNCHERHEADERCONTROL_H
#define CCCAPPCOMMLAUNCHERHEADERCONTROL_H

// INCLUDES
#include <eikmobs.h>
#include "ccappcommlauncherheaders.h"
#include <ccappimagedecoding.h>

// FORWARD DECLARATIONS
class CEikImage;
class CEikLabel;
class CCCAppCommLauncherImageDecoding;
class CCCAppCommLauncherHeaderTextOrder;
class CAknStylusPopUpMenu;
class CCCAppCommLauncherPbkCmd;

// CLASS DECLARATION
/**
 * Class implementing the header part control of commlauncherplugin
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ccappcommlauncherplugin.dll
 *  @since S60 v5.0
 */
class CCCAppCommLauncherHeaderControl : 
            public CCoeControl,  
            public MEikMenuObserver, 
            public MCCAppImageDecodingObserver
    {
public:
    // Construction & destruction
    /**
     * Two-phased constructor.
     */
    static CCCAppCommLauncherHeaderControl* NewL( CCCAppCommLauncherPlugin& aPlugin );
    
    /**
     * Destructor.
     */
    ~CCCAppCommLauncherHeaderControl();
    
    // Functions from base classes 
    /**
     * From CCoeControl
     * Component control handling (compound control)
     */
    TInt CountComponentControls() const;
    
    /**
     * From CCoeControl
     * Component control handling (compound control)
     */
    CCoeControl* ComponentControl(TInt aIndex) const;
    
    /**
     * From CoeControl
     * Called by framework when the view size is changed.
     */
    void SizeChanged();
    
    /**
     * From CoeControl
     * Overridden to set the container window for child controls
    */
    void SetContainerWindowL(const CCoeControl& aContainer);
    
    /**
     * From CoeControl
     * Overridden to receive pointer events
    */
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);

    /**
     * Contact data info is given through this
     */
    void ContactInfoFetchedNotifyL( 
        const CCmsContactFieldInfo& aContactFieldInfo );
    
    /**
     * Contact data is given through this
     */
    void ContactFieldFetchedNotifyL( 
            const CCmsContactField& aContactField );
    
    /**
     * Notification of completed fetching is given through this
     */
    void ContactFieldFetchingCompletedL();
    
    /**
     * Sets the bitmap shown in the header image 
     * Ownership is transferred
     * 
     * @param aBmp The bitmap
     */
    void SetBitmap(CFbsBitmap* aBmp);
    
    /**
     * Clears the texts & the image
     */
    void ClearL();
    
    /**
     * Set contact store information
     * @param aContactStore the contact store
     */
    void SetContactStoreL(TCmsContactStore aContactStore);
    
    /**
     * From CCoeControl
     * (see details from baseclass )
     */
    TKeyResponse OfferKeyEventL(
        const TKeyEvent& aKeyEvent, TEventCode aType);

    inline void SetStatusButtonVisibility( TBool aVisible )
        { iStatusButtonVisibility = aVisible; }
    
public: // MEikMenuObserver
     virtual void ProcessCommandL(TInt aCommandId);
     virtual void SetEmphasis(CCoeControl* aMenuControl,TBool aEmphasis);

private:    // from MCCAppImageDecodingObserver
     void BitmapReadyL( CFbsBitmap* aBitmap );
     
private:

    /**
     * Constructor for performing 1st stage construction
     */
    CCCAppCommLauncherHeaderControl( CCCAppCommLauncherPlugin& aPlugin );

    /**
     * Constructor for performing 2nd stage construction
     */
    void ConstructL();
    
    /**
     * Helper function for the leaving part of changing labels size
     */
    void LabelsSizeChangedL();
       
    /**
     * Updates the number/address popup
     */
    void UpdatePopupL();

    /** 
      * Converts point's relative coordinaters into absolute screen coordinates 
      */  
    void PosToScreenCoordinates( CCoeControl* aControl, TPoint& aPos );
    
    /** 
      * Launches contact image selection stylus popup menu 
      */  
    void LaunchStylusPopupL( const TPointerEvent& aPointerEvent );
    
    /** 
      * Launches view image command 
      */  
    void DoViewImageCmdL();
    
    /** 
      * Launches change image command 
      */  
    void DoChangeImageCmdL();
    
    /** 
      * Launches remove image command 
      */  
    void DoRemoveImageCmdL();
    
    /** 
      * Launches add image command 
      */  
    void DoAddImageCmdL();
    
    /** 
      * Does the configuration contain phone memory  
      */  
    TBool IsPhoneMemoryInConfigurationL();
    
    /** 
      * Displays contact image if available  
      */  
    void ProcessContactImageDisplayL();
    
    /**
     * Helper function for changing Favorite Icon size
     */
    void FavoriteIconSizeChanged();
    
    /**
     * The image control
     * Own.
     */
    CEikImage* iImage;
    
    /**
     * The bitmap
     * Own.
     */
    CFbsBitmap* iBitmap;
    CFbsBitmap* iMask;
    
    /**
     * The label controls
     * Own.
     */    
    RPointerArray<CEikLabel> iLabels;
        
    /**
     * Active listener to make asynchronic operation synchronic
     * Own.
     */    
    CCCAppImageDecoding* iImageDecoding;
    
    /**
     * Helper for ordering the texts
     * Own.
     */
    CCCAppCommLauncherHeaderTextOrder* iTextOrder;
    
    /** 
      * Contact image selection stylus popup menu 
      * Own. 
      */  
    CAknStylusPopUpMenu* iImageSelectionPopup;
    
    /** 
      * Contact image selection stylus popup menu position 
      *  
      */  
    TPoint iPos;
    
    /** 
      * Contact has an image 
      *  
      */ 
    TBool iHasContactImage;
    
    /** 
      * Command launcher to start Pbk commands 
      * Own 
      */  
    CCCAppCommLauncherPbkCmd* iPbkCmd;
    
    /**
     * Ref to plugin.
     * Not own.
     */
    CCCAppCommLauncherPlugin& iPlugin;

    /**
     * Pointers to contact fields.
     * Pointed contact fields not own.
     */    
    RArray<CCmsContactField> iCmsContactFields;
    
    /** 
      * Contact image size 
      */ 
    TSize iContactImageSize;
    
    /** 
      * Contact image path
      * Own
      */ 
    HBufC* iContactImageFullName;

    /** 
      * Contact thumbnail data
      * Own
      */ 
    HBufC8* iContactThumbnailData;

    /** 
      * Store contact image path
      * Own
      */ 
    HBufC* iStoreContactImageFullName;
    
    TBool iStatusButtonVisibility;
    
    /**
     * The Favorite Icon control
     * Own.
     */
    CEikImage* iFavContactIcon;
    };

#endif // CCCAPPCOMMLAUNCHERHEADERCONTROL_H
