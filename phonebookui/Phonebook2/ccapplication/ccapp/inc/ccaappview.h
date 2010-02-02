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
* Description:  A class responsible handling the views
 *
*/


#ifndef C_CCAAPPVIEW_H
#define C_CCAAPPVIEW_H

#include <e32base.h>                             
#include <coedef.h>
#include <w32std.h>

#include <mccacontactobserver.h>
#include <mccappengine.h>

// FORWARD DECLARATIONS
class CCCAppStatusPaneHandler;
class CCCAAppAppUi;
class MVPbkStoreContact;
class MCCAppPluginsContactObserver;

/**
 *  CCCAppView
 *
 *  @code
 *  @endcode
 *
 *  @lib ccaapp
 *  @since S60 v5.0
 */
class CCCAppView : 
    public CBase,
    public MCCAppContactFieldDataObserver,
    public MCCAppEngine
	{
#ifdef __CCAPPUNITTESTMODE
    friend class ut_CCCAppView;
#endif// __CCAPPUNITTESTMODE  
    
public:
	/**
	 * Two-phased constructor.
	 */
    static CCCAppView* NewL( CCCAAppAppUi& aAppUi );
    
    /**
     * Destructor.
     */
    ~CCCAppView();

    
public: // From MCCAppEngine
    
    /**
     * From MCCAppEngine
     */ 
    void CCAppContactEventL();
    
    /**
     * From MCCAppEngine
     */ 
    void AddObserverL( MCCAppPluginsContactObserver& aObserver );
    
    /**
     * From MCCAppEngine
     */ 
    void RemoveObserver( MCCAppPluginsContactObserver& aObserver );
    
    /**
     * From MCCAppEngine
     */ 
    MCCAParameter& Parameter();

    /**
     * From MCCAppEngine
     */ 
    void ShowPluginL( TUid aUid ); 

    /**
     * From MCCAppEngine
     */ 
    void HidePluginL( TUid aUid );    

private:// from MCCAppContactFieldDataObserver

	/**
	 * from MCCAppContactFieldDataObserver
	 */
    void ContactFieldDataObserverNotifyL( 
        MCCAppContactFieldDataObserver::TParameter& aParameter );    

	/**
	 * from MCCAppContactFieldDataObserver
	 */
    void ContactFieldDataObserverHandleErrorL( 
        TInt aState, TInt aError );

private:
	/**
	 * Constructor.
	 */        
    CCCAppView( CCCAAppAppUi& aAppUi );    
    void ConstructL();

public: // Implementation

	/**
     * Activates the plugin view.
     * AppUi exits in case it receives error.
	 *
	 * @since S60 v5.0
	 * @param aUid is the uid of the plugin preferred to be visible 1st
	 * @return possible error code
	 */
    TInt StartView( const TUid aUid );    

	/**
	 * Changes the plugin view.
     * AppUi exits in case it receives error.
	 *
	 * @since S60 v5.0
	 * @param aForward next/previous plugin
     * @return possible error code
	 */
    TInt ChangeView( TBool aForward );

    /**
     * TabChangedL is called after the tab is
     * succesfully changed by CAknTabGroup.
     * TabChangedL activates corresponding plugin.
     *
     * @since S60 v5.0
     * @param aIndex of plugin to be activated
     */    
    void TabChangedL( TInt aIndex );
    
    /**
	 * Returns the AppUi reference
	 *
	 * @since S60 v5.0
	 * @return reference to CCCAAppAppUi
	 */    
    CCCAAppAppUi& AppUi();

	/**
	 * Returns the Eikon Enviroment reference
	 *
	 * @since S60 v5.0
	 * @return reference to CEikonEnv
	 */ 
    CEikonEnv& EikonEnv();

	/**
	 * Returns the CCCAppPluginLoader reference
	 *
	 * @since S60 v5.0
	 * @return reference to CCCAppPluginLoader
	 */ 
    CCCAppPluginLoader& PluginLoader();

    /**
     * Tries to handle most of the cases when plugin 
     * leaves in critical point. Panics are out of our
     * reach.
     * 
     * In practise "bad boy" is removed from plugin 
     * array and the 1st plugin in the array is then 
     * activated.
     * 
     * Handles the cases if leave occurs in one of the 
     * following plugin methods:
     * - NewL / ConstructL
     * - PreparePluginViewL
     * - NewContainerL
     * - DoActivateL
     * 
     * DoActivateL call happens in Avkon fw and by
     * default the last working view is set to be
     * active. To keep the functionality similar
     * (and simple) in most cases, CIdle callback
     * ResetViewL() is used.
     * 
     * Since recovering should not be everyday use case,
     * implementation is a compromise. Recovering 
     * functionality is not recursive. If the plugin
     * failing is the 1st one, application is closed. 
     *
     * @since S60 v5.0
     */
    void RecoverFromBadPluginL();
    
    /**
     * @return ETrue if active plug-in is busy with some processing,
     *         EFalse otherwise
     */
    TBool CurrentPluginBusy();

private: //New

	/**
	 * Activates plugin itself
	 *
	 * @since S60 v5.0
	 * @param aPluginDataInFocus is the data of plugin to be activated
	 */ 
    void ActivatePluginL( CCCAppPluginData& aPluginDataInFocus );

	/**
	 * Updates the titlepane based on contact data notifications.
	 * See also ContactFieldDataObserverNotifyL.
	 *
	 * @since S60 v5.0
	 * @param aContactField is contact data field
	 */
    void ContactFieldFetchedNotifyL( 
        CCmsContactField& aContactField );       

    /**
     * Activates the plugin view. Leaves in case of errors.
     *
     * @since S60 v5.0
     * @param aUid is the uid of the plugin preferred to be visible 1st
     */    
    void StartViewL( const TUid aUid );    

    /**
     * Changes the plugin view. Leaves in case of errors.
     *
     * @since S60 v5.0
     * @param aForward next/previous plugin
     */    
    void ChangeViewL( TBool aForward );    
    
    /**
     * CIdle-callback for resetting the view.
     * See RecoverFromBadPluginL.
     *
     * @since S60 v5.0
     */
    static TInt ResetViewL( TAny* aSelf );    

    /**
     * Check other plugin visibilities.
     *
     * @since S60 v5.0
     */    
    void CheckOtherPluginVisibilitiesL();    

    /**
     * Check other plugin visibilities.
     *
     * @since S60 v5.0
     * @param aPluginToFocus 
     */    
    void StartPluginL(TInt aPluginToFocus);    

    /**
     * Shows plugin.
     *
     * @since S60 v5.0
     * @param aUid 
     */    
    void PerformShowPluginL( TUid aUid ); 
    
    /**
     * Hides plugin.
     *
     * @since S60 v5.0
     * @param aUid 
     */    
    void PerformHidePluginL( TUid aUid );    
    
    /**
     * Callback function for CIdle object that calls ConstructDelayedL().
     *
     * @since S60 v5.0
     * @param aContainer 
     */    
    static TInt ConstructDelayedCallbackL( TAny* aContainer );

    /**
     * Performs time consuming construction operations once.
     *
     * @since S60 v5.0
     */    
    void DoConstructDelayedL();    
    
    
    /**
     * Performs the synch between the tabs & the Visible Plugins
     *
     * @since S60 v5.0
     */
    void SynchPluginVisibilityWithTabAfterDelayedStartL();

private: //Data

	/**
	 * Status pane handler.
	 * Own.  
	 */    
    CCCAppStatusPaneHandler* iPaneHandler; 

    /**
     * Reference to AppUi
     * Not own.
     */
    CCCAAppAppUi& iAppUi;

    /**
     * Pointer to pluginloader
     * Own.
     */
    CCCAppPluginLoader* iPluginLoader;

    /**
     * Boolean for monitoring is view change currently in progress.
     * Own.
     */
    TBool iViewChangeInProgress;
     
    /**
     * Pointer to CMS contact fetcher wrapper
     * Own.
     */      
     CCCAppCmsContactFetcherWrapper* iCmsWrapper;

     /**
      * Pointer to CIdle in cases of recovering
      * Own.
      */      
     CIdle* iViewResetter;
     
     /**
      * Pointer to observers.
      * Not owed.
      */
     RPointerArray<MCCAppPluginsContactObserver> iObservers;

     /**
      * Members and pointers for functionality to control delayed
      * lazy plugin initialisation during construction.
      */
     CIdle* iDelayedConstructor;     
     TBool iDelayedStarting;
     
     enum TDelayState
         {
         EDelayedCreateTabs = 0,
         EDelayedCheckVisibilities,
         EDelayedCheckVisibilitiesOk,
         EDelayedFinish
         };
     TInt iDelayState;
     TInt iDelayedPluginVisibilityCheck;
     TBool iDelayedVisibilityChanged;
     
     //has a list of plugins which needs tobe synched
     RArray <TUid> iPluginUidToBeSynchd;
	};

#endif // C_CCAAPPVIEW_H

