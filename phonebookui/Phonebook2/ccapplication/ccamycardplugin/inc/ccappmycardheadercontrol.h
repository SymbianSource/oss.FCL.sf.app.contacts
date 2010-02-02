/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header UI control of the mycard plugin
*
*/


#ifndef CCCAPPMYCARDHEADERCONTROL_H
#define CCCAPPMYCARDHEADERCONTROL_H

// INCLUDES
#include <coecntrl.h>

// FORWARD DECLARATIONS
class CEikImage;
class CEikLabel;
class CFbsBitmap;
class CAknIcon;
class CCCAppMyCardPlugin;
class CAknStylusPopUpMenu;
class CCCAppStatusControl;
class CSpbContentProvider;
class CPbk2ApplicationServices;
class MCCAStatusControlObserver;

// CLASS DECLARATION
/**
 * MMyCardHeaderControlObserver Observer class
 */
class MMyCardHeaderControlObserver
    {
public:
     /**
     * Notification method to report MyCardHeaderControl events to observers
	 * @param const TPointerEvent& aPointerEvent
     */
    virtual void MyCardHeaderControlClickL( TPoint aPos ) = 0;

protected:
    virtual ~MMyCardHeaderControlObserver() {}
    };

/**
 * Class implementing the header part control of mycardplugin
 *
 *  @lib ccappmycardplugin.dll
 *  @since S60 9.2
 */
class CCCAppMyCardHeaderControl : public CCoeControl
    {
public: // Construction & destruction
    /**
     * Two-phased constructor.
     */
    static CCCAppMyCardHeaderControl* NewL( 
            MCCAStatusControlObserver& aObserver );
    
    /**
     * Destructor.
     */
    ~CCCAppMyCardHeaderControl();

public: // New implementation
    
    /**
     * Sets the bitmap with mask shown in the header portrait.
     * Ownership of bitmap and mask is transferred.
     * 
     * @param aImage new bitmap
     * @param aMask mask for the bitmap
     */
    void SetPortraitIcon( CFbsBitmap* aImage, CFbsBitmap* aMask );
    
    /**
     * Sets the bitmap shown in the header portrait. 
     * Takes ownership of the bitmap even if leaves.
     * 
     * @param aImage new bitmap
     */
    void SetPortraitBitmapL( CFbsBitmap* aImage );
    
    void SetLabel1TextL( const TDesC& aTxt );
    void SetLabel2TextL( const TDesC& aTxt );
    
    inline CCCAppStatusControl* StatusControl();
    
public: // Functions from base classes 

    /**
     * From CCoeControl
     */
    TInt CountComponentControls() const;
    
    /**
     * From CCoeControl
     */
    CCoeControl* ComponentControl(TInt aIndex) const;

    /**
     * From CoeControl
     */
    void SizeChanged();

    /**
     * From CoeControl
    */
    void HandlePointerEventL( const TPointerEvent& aPointerEvent );

    /**
     * From CoeControl
    */
    void SetContainerWindowL( const CCoeControl& aContainer );
    
    void AddObserverL( MMyCardHeaderControlObserver* aObserver );
        
protected: // construction 

    /**
     * Constructor for performing 1st stage construction
     */
    CCCAppMyCardHeaderControl();

    /**
     * Constructor for performing 2nd stage construction
     */
    void ConstructL( MCCAStatusControlObserver& aObserver );
    
private:

    void NotifyObservers( TPoint aPoint );
   
protected: // data

    /**
     * Own. Portrait image control
     */
    CEikImage* iPortraitImage;

    /**
     * Own. Name label control
     */    
    CEikLabel* iLabel1;
    CEikLabel* iLabel2;

    /**
     * Own. The portrait bitmap
     */
    CFbsBitmap* iPortraitBitmap;

    /**
     * Own. The portrait mask
     */
    CFbsBitmap* iPortraitMask;
    
    /**
     * The portrait bitmap size on UI
     */
    TSize iPortraitImageSize;
    
    /** 
      * Contact image selection stylus popup menu 
      * Own. 
      */  
    CAknStylusPopUpMenu* iImageSelectionPopup;
        
    /*
     * Observer notification
     */
    MMyCardHeaderControlObserver* iMyCardHeaderControlObserver;
    
    /*
     * observer array
     */
    RPointerArray<MMyCardHeaderControlObserver> iObserverArray;
    
    /**
     * Control to show the status icon and the text
     */
    CCCAppStatusControl* iStatusControl;

    /**
     * ContentProvider
     * Own.
     */
    CSpbContentProvider* iProvider;

    /**
     * Own. 
     */
    CPbk2ApplicationServices* iAppServices;
    
    };

inline CCCAppStatusControl* CCCAppMyCardHeaderControl::StatusControl()
    {
    return iStatusControl;
    }

#endif // CCCAPPMYCARDHEADERCONTROL_H
