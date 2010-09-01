/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handling status pane.
 *
*/


#ifndef __CCAAPPSTATUSPANEHANDLER_H__
#define __CCAAPPSTATUSPANEHANDLER_H__

//INCLUDES
#include <e32base.h>                             

//FORWARD DECLARATIONS
class CCCAAppAppUi;
class CAknTabGroup; 
class CAknNavigationDecorator;

//CONSTS
const TInt KCCAppMinPluginCountForTabs = 2;
const TInt KCCAppMinPluginCountForMaxTabs = 3;

/**
 *  CCCAppStatusPaneHandler
 *  Class for handling status pane.
 *
 *  @code
 *   
 *  @endcode
 *
 *  @lib ccaapp
 *  @since S60 v5.0
 */
class CCCAppStatusPaneHandler : 
    public CBase,
    public MAknTabObserver
	{
#ifdef __CCAPPUNITTESTMODE
    friend class UT_CCAappStatusPaneHandler;
#endif// __CCAPPUNITTESTMODE  	
	
public:
	/**
	 * Two-phased constructor.
	 */
    static CCCAppStatusPaneHandler* NewL( CCCAppView& aView );

    /**
     * Destructor.
     */
    ~CCCAppStatusPaneHandler();

private:
	/**
	 * Constructor.
	 */    
    CCCAppStatusPaneHandler( CCCAppView& aView );
    void ConstructL();

private:// from MAknTabObserver

    /**
     * from MAknTabObserver
     */
    void TabChangedL( TInt aIndex );    
    
public: // Implementation

	/**
	 * Creates tabs if there are KCCAppMinPluginCountForTabs (or more) plugins
	 *
	 * @since S60 v5.0
	 */
    void CreateTabsL();

	/**
	 * Set given tab active if iTabGroup available 
	 *
	 * @since S60 v5.0
	 * @param aTabIndex: Activates given tab. 
	 */
    void ActivateTab( TInt aTabIndex ); 

    /**
     * Set next/previous tab active if iTabGroup available 
     *
     * @since S60 v5.0
     * @param aForward next/previous
     */    
    void ActivateTabL( TBool aForward );

	/**
	 * Returns current tab index. If no iTabGroup available
	 * returns KErrNotFound.
	 *
	 * @since S60 v5.0
	 * @return current tab index.
	 */
    TInt ActiveTabIndex(); 

	/**
	 * Counts tabs. If no iTabGroup available
     * returns KErrNotFound.
	 *
	 * @since S60 v5.0
	 * @return count of tab items. 
	 */
    TInt TabCount(); 
    
    /**
     * Helper Function to identify whether a Tab has been
     * Created or not. If there is only 1 plugin, then the tab 
     * will not be created. If there is more than 1Plugin interested 
     * in the CCAParam then Tabs will be created and made visible
     * Use this function to check the availability of the tab
     * and pass the tab related functionalities like
     * switching between tabs
     *
     * @since S60 v5.0
     * @return ETrue - If Tab Created and Visible 
     *          EFalse - Otherwise 
     */
    TBool IsTabAvailable();
    
private: //Implementation 
    
    /**
     * Helper function for adding bitmaps.
     *
     * @since S60 v5.0
     * @param index of plugin 
     */
    void AddBitmapsL(TInt aPlugin);

    /**
     * Creates navipane
     *
     * @since S60 v5.0
     */    
    void CreateNavipaneL();

    /**
     * Adds a plugin into navipane.
     *
     * @since S60 v5.0
     * @param index of plugin 
     */
    void AddTabL( TInt aPlugin );    
    
private: //Data

	/**
	 * Decorated tab-group.
	 * Own.  
	 */
    CAknNavigationDecorator* iDecoratedTabGroup;

	/**
	 * Tab-group.
	 * Not own. Ownership transferred to NavigationDecoration  
	 */
     CAknTabGroup* iTabGroupRef; 

	/**
	 * Reference to view -class
	 * Not own. 
	 */     
     CCCAppView& iView;
     
     //Tab is available or not ETrue/EFalse
     TBool iTabAvailability;
	};

#endif // __CCAAPPSTATUSPANEHANDLER_H__

