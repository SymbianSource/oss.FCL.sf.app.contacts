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
* Description:  Defines CCCAAppAppUi, class that prepares the 
*                application View(s) and handles the UI commands 
*
*/


#ifndef __CCAAPP_APPUI_H__
#define __CCAAPP_APPUI_H__

#ifdef __CCAPPUNITTESTMODE
    #include "ccappheaders.h"
#else
    #include <aknViewAppUi.h>    
#endif// __CCAPPUNITTESTMODE 
    
// FORWARD DECLARATIONS
class CCCAppView;
class CCCAParameter;
class CRepository;

/**
 *  Defines CCCAAppAppUi, class that prepares the application 
 *  view and handles the UI commands 
 *
 *  @lib ccaapp.exe
 *  @since S60 v5.0
 */  
class CCCAAppAppUi : public CAknViewAppUi
    {
#ifdef __CCAPPUNITTESTMODE
    friend class ut_CCCAAppAppUi;
#endif// __CCAPPUNITTESTMODE    

public:

    CCCAAppAppUi();
    
   /**
    * Destructor.
    */
    ~CCCAAppAppUi();

public:

    /**
     * Initializes view plugin for usage. Calls 1st 
     * PreparePluginViewL and then ActivatePluginViewL.
     */
    void InitializePluginL();
    
    /**
     * Sets the CCApp parameter
     * @param aParameter contains contact information. 
     *        Ownership is transferred.
     */        
    void SetParameter( CCCAParameter& aParameter );

    /**
     * Returns a reference to CCApp parameter
     *
     * @since S60 v5.0
     * @return CCCAParameter reference
     */        
    CCCAParameter& Parameter();

    /**
     * Returns a reference to EikonEnv
     *
     * @since S60 v5.0
     * @return CEikonEnv reference
     */        
    CEikonEnv& EikonEnv();

    /**
     * Returns an array of key ids reserved by the AppUi
     *
     * @since S60 v5.0
     * @return key id array
     */      
    RArray<TInt>& ReservedKeys();

    /**
     * Handles cases when plugin leaves in critical point.
     * See more details in CCCAppView-classes similar method.
     * (Set as virtual because this is called from the CCCAppViewPluginAknView (ccapputil.dll))
     *
     * @since S60 v5.0
     */      
    virtual void RecoverFromBadPluginL();
    
// from base class CCoeAppUi

    /**
     * From CCoeAppUi
     * (see details from header)
     *
     * @since S60 v5.0
     */
	TKeyResponse HandleKeyEventL(
	    const TKeyEvent& aKeyEvent,
	    TEventCode aType );
	    
	void HandleWsEventL( const TWsEvent &aEvent, CCoeControl *aDestination );

private:

// from base class CEikAppUi

    /**
     * From CEikAppUi
     * (see details from header)
     *
     * @since S60 v5.0
     */
    void HandleCommandL( TInt aCommand );
    
    /**
     * From CEikAppUi
     * (see details from header)
     *
     * @since S60 v5.0
     */
    void PrepareToExit();


// from base class CAknAppUi    
    
    /**
     * From CAknAppUi
     * (see details from header)
     *
     * @since S60 v5.0
     */
    TErrorHandlerResponse HandleError(
        TInt aError, 
        const SExtendedError& aExtErr,  
        TDes& aErrorText, 
        TDes& aContextText );
    
private:

    /**
     * Loads the settings from CenRep.
     * In case of error some default values are used.
     *
     * @since S60 v5.0
     */
    void LoadSettingsL();

private:

    void ConstructL();
	
public:
    /**
     * Sets the title of the View Plugin
     * @param aTitle - the title to be set for the Plugin View     
     * @since S60 v5.0
     * 
     */
    virtual void SetTitleL( const TDesC& aTitle );

private: // data

    /**
     * Pointer to tab view.
     * Own.
     */
    CCCAppView* iCCAppView;
    
    /**
     * Pointer to CCA parameter.
     * Own.
     */    
    CCCAParameter* iCCAParameter;
    
    /**
     * Keys for view navigation
     */
     TInt       iNextViewKey;
     TInt       iPrevViewKey;
     
    /**
     * Flag for settings loading
     */
     TBool      iSettingsLoaded;

     /**
      * For loading the settings. 
      * Own.
      */     
     CRepository* iRepository;

     /**
      * Ids of reserved keys
      */     
      RArray<TInt> iReservedKeys;
    };

#endif // __CCAAPP_APPUI_H__

//End Of File
