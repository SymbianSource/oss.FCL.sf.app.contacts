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
* Description: UI status control
*
*/


#ifndef CCAPPSTATUSCONTROL_H_
#define CCAPPSTATUSCONTROL_H_

// INCLUDES
#include <coecntrl.h>
#include <spbcontentprovider.h>
#include <ccaextensionfactory.h>


// FORWARD DECLARATIONS
class CEikImage;
class CEikLabel;
class CFbsBitmap;
class CAknIcon;
class MVPbkContactLink;
class CAknsFrameBackgroundControlContext;
class MTouchFeedback;

// CLASS DECLARATION
/**
 * Class for observing click events to status control
 * 
 * since s60 9.2  
 **/
class MCCAStatusControlObserver
{
public:	
	virtual void StatusClickedL() = 0;

protected:
    virtual ~MCCAStatusControlObserver() {}
};

/**
 * Class implements the status control
 *
 *  @lib ccapputil.lib
 *  @since S60 9.2
 */
NONSHARABLE_CLASS( CCCAppStatusControl ) : 
    public CCoeControl, 
    public MSpbContentProviderObserver,
    public MCCAStatusProviderObserver
    {
public: // Construction & destruction
	
	enum TContactType
            {
            ENormalContact = 0x01,
            EMyCardContact = 0x02
            };

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CCCAppStatusControl* NewL( 
            CSpbContentProvider& aContentProvider, 
            MCCAStatusControlObserver& aObserver,
            CCCAppStatusControl::TContactType aContactType );

    /**
     * Destructor.
     */
    ~CCCAppStatusControl();       

public: // New implementation
        
    /**
	 * Sets the link object 
	 * 
	 * @aLink Link to current contact
	 */
    IMPORT_C void SetContactLinkL( 
            MVPbkContactLink& aLink );

    /**
     * Change buttons state between pressed and released.
     * 
     * @param aPressed ETrue to make the button pressed
     */
    void SetPressed( TBool aPressed );
     
    /**
     * Set the default status icon 
	 * 	 
	 * @aDefaultStatusIcon Default status icon
	 */
    IMPORT_C void SetDefaultStatusIconL( CGulIcon* aDefaultStatusIcon );
     
    /**
	 * Set the default status text 
	 * 	 
	 * @aDefaultStatusText Default status text
	 */
    IMPORT_C void SetDefaultStatusTextL( HBufC* aDefaultStatusText );     
    
    /**
	 * Set the CCA statusprovider 
	 * 	 
	 * @aCCAStatusProvider ECom status provider
	 */
    IMPORT_C void SetStatusProvider( MCCAStatusProvider* aCCAStatusProvider );
         
public: // From CCoeControl
     
    /**
     * From CCoeControl
     */
    TInt CountComponentControls() const;

    /**
     * From CCoeControl
     */
    CCoeControl* ComponentControl( TInt aIndex ) const;

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
    
    /**
     * From CoeControl
     */
    void Draw( const TRect& aRect ) const;

public: // From MSpbContentProviderObserver
    
    /**
     * From MSpbContentProviderObserver
     */
    void ContentUpdated( 
        MVPbkContactLink& aLink, 
        MSpbContentProviderObserver::TSpbContentEvent aEvent );           
    
public: // From MCCAStatusProviderObserver
    
	/*
	 * From MCCAStatusProviderObserver
	 */
    void StatusEvent(
		MCCAStatusProviderObserver::TCCAStatusProviderObserverEvent aEvent,
		const MVPbkContactLink* aLink = NULL );
		
private: // New functions    

    /**
     * Wraps the status text to fit into the space allocated for it.
     */
    TInt RewrapStatusTextL();

    /**
     * Show default status icon and text
     */
    void ShowDefaultContentL();
    
    /**
     * Handle status update event
     * 
     * @param aLink contact which has the status updated
     * @param aEvent 
     */
    void DoStatusUpdateL( 
        MVPbkContactLink& aLink, 
        MSpbContentProviderObserver::TSpbContentEvent aEvent );           

    /*
     * Update contact status from ECom plugin
     * @param aLink contact which has the status updated
     */
    void UpdateCCAStatusL( MVPbkContactLink* aLink );
    
    void SetVariableLayouts( TInt aOption );
    
    inline void RewrapStatusTextToArrayL( 
            TDes& aStatusText, 
            CArrayFix<TPtrC>& aTxtArray );
    
protected: // construction

    /**
     * Constructor for performing 1st stage construction
     */
    CCCAppStatusControl( 
        CSpbContentProvider& aContentProvider, 
        MCCAStatusControlObserver& aObserver,
        CCCAppStatusControl::TContactType aContactType );

    /**
     * Constructor for performing 2nd stage construction
     */
    void ConstructL();

protected: // enums
    
    /**
     * Internal states
     */
    enum TStatusControlState
        {
        // Show nothing
        EStateUndefined,
        // Default content shown
        EStateDefaultContent,
        // Status content shown
        EStateStatusContent
        };
    
protected: // data

    /**
     * Own. Status service icon control
     */
    CEikImage* iStatusImage;

    /**
     * Own. Status text label control
     */
    CEikLabel* iStatusLabel1;
    CEikLabel* iStatusLabel2;

    /**
	* Own. The status service bitmap
	*/
    CGulIcon* iStatusIcon;

    /**
     * The status icon size on UI
     */
    TSize iStatusIconSize;

    /**
     * Own. Current status text
     */
    HBufC* iStatusText;
    
    /**
	 * Own. Default status text
	 */
	HBufC* iDefaultStatusText;
        
    /**
	* Ref, Not owned. Status provider for updating the status and icon.
	*/
	CSpbContentProvider& iContentProvider;
    
    /**
	* Observes clikcs to statustext
	*/    
    MCCAStatusControlObserver& iObserver;
    
    /// Own.
    MVPbkContactLink* iLink;             
    
    // Own.
    CGulIcon* iDefaultIcon;              
        
    /**
     * Own. Button background graphics context (button effect)
     */
    CAknsFrameBackgroundControlContext* iBgContext;
    
    /**
     * Flag for pressed down state
     */
    TBool iPressed;
    
    /**
     * Not own (singleton). Tactile feedback interface
     */
    MTouchFeedback* iTouchFeedBack;
    
    /*
     * Current control state.
     */
    TStatusControlState iState;
        
    // Own. ECOM plugin tatus provider
    MCCAStatusProvider* iCCAStatusProvider;
    
    // Current contact type.
    CCCAppStatusControl::TContactType iContactType;
    };

#endif // CCAPPSTATUSCONTROL_H_
