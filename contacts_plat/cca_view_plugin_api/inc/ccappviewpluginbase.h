/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  View plugin base class of ECE's CCA-application
*
*/


#ifndef __CCAPPVIEWPLUGINBASE_H__
#define __CCAPPVIEWPLUGINBASE_H__

#include <e32base.h>
#include <ecom.h>
#include <ccappviewpluginaknview.h>
#include <mccaparentcleaner.h>  

class CAknIcon;
class MCCAppPluginParameter;
class CEikMenuPane;
class MCCAppEngine;

//Use this UID to access CCCAppViewPluginBase interface extension 2. Used as
//a parameter to CCCAppViewPluginBaseExtension() method.
const TUid KMCCAppViewPluginBaseExtension2Uid = { 2 };

//Uid value to request ecom implementations for legagy plugin interface. Legacy
//interface provides one ui view per one plugin. If multiple ui views need to
//be created from one plugin, then use factory interface instead.
const TUid KCCAppViewPluginBaseInterfaceUID = {0x2000B1AB};

//Uid value to request ecom implementations for multi view plugin interface. Multi
//view interface provides factory function that can be used instantiate multiple
//ui views from one plugin.
const TUid KCCAppViewFactoryInterfaceUID = {0x20022FD0};

/**
 *  View plugin base class of CCA-application view plugins
 *
 *  This classs defines the API between the CCA-application and the plugin
 *  responsible showing the certain view.
 *
 *  ConstructL of plugin implementation should be as light as possible
 *  and if complex construction is needed, asynchronous operations must
 *  be implemented.
 *
 *  Summary of interaction between CCA-application FW and plugins: 
 *  
 *  1. When launching CCApplication (implementations of these need to be fast and lightweight).
 *    -CCCAppNNNNNPlugin::NewL                      (lightweight construction of plugin)
 *    -CCCAppNNNNNPlugin::Id                        (get view uid, should be same as plugin implementation uid)
 *    -CCCAppNNNNNPlugin::ProvideBitmapLC           (request bitmap which is shown in CCA)
 *  2. Requesting initial plugin visibility in CCA  (by default plugins are shown in CCA)
 *    -CCCAppNNNNNPlugin::CheckVisibilityL          (query whether plugin is shown or kept hidden in background)  
 *  3. When moving between plugins 
 *    -CCCAppNNNNNPlugin::PreparePluginViewL        (preparing plugin to activate, called only once in plugin's lifespan)
 *    -CCCAppNNNNNPlugin::DoActivateL               (activating plugin)
 *    -CCCAppNNNNNPlugin::NewContainerL             (creating plugin control/container)
 *    -CCCAppNNNNNPlugin::DoDeactivate              (deactivating plugin)
 *    -CCCAppNNNNNContainer::~CCCAppNNNNNContainer  (deleting container when plugin is not active)
 *  4. When plugin needs to show or hide itself     (by default plugins are shown in CCA).
 *    -ShowPluginL                                  (shows plugin in CCA)
 *    -HidePluginL                                  (hides plugin from CCA)
 *
 *
 *  See details from ccappviewpluginaknview.h and ccappviewpluginakncontainer.h.
 * 
 *  @since S60 v5.0
 */
class CCCAppViewPluginBase : public CCCAppViewPluginAknView
{

public: // Constructors / destructors

    /**
     * Two-phased constructor.
     * @param aImplementationUid
     */
    static CCCAppViewPluginBase* NewL( TUid aImplementationUid );
    
    /**
     * Destructor.
     */
    virtual ~CCCAppViewPluginBase();

public: // Interface methods

    /**
     * This method is called only once during the plugin
     * lifetime. Use it to prepare/initialise plugin's
     * classes.
     *
     * Note: All the time consuming operations should be made
     *       asynchronous
     *
     * @since S60 v5.0
     * @param aPluginParameter contains parameters for plugin
     */
    virtual void PreparePluginViewL(
        MCCAppPluginParameter& aPluginParameter ); 

    /**
     * Defined CCApp icon types
     *
     *  @since S60 v5.0
     */
    enum TCCAppIconType
        {
        ECCAppTabIcon
        };

    /**
     * CCApp retrieves bitmaps from plugin through this callback.
     * 
     * Ownership is transferred to CCApplication.
     *
     * @since S60 v5.0
     * @param aIconType defines the needed bitmap type (see above)
     * @param aIcon contains the bitmap and it's mask
     */    
    virtual void ProvideBitmapL( 
        TCCAppIconType aIconType, 
        CAknIcon& aIcon );
        
   /**
    * Plugins get services from the MCCAppEngine like notifications
    * incase of the contact relocates from one store to another.
    * Ownership is not transferred. 
    *   
    * @since S60 v5.0
    * @param aAppEngine the application engine, ownership not transferred.
    */
    void SetAppEngine(MCCAppEngine* aAppEngine)
        {
        iAppEngine = aAppEngine;
        }
    
    MCCAppEngine* AppEngine()
        {
        return iAppEngine;
        }

    /**
     * CCApp retrieves visibility information from plugin through 
     * this callback. By default plugin is shown. Implement this function if 
     * plugin needs to run a check to decide whether to retain itself hidden in 
     * background. If running the check takes a lot of time, then following
     * pattern is recommended: start asynchronous process to run the check
     * and return EFalse. When asynchronous process is finished call 
     * ShowPluginL() if the plugin needs to be shown.
     * 
     * @since S60 v5.0
     * @param aPluginParameter contains parameters for plugin     
     * @return ETrue show plugin, EFalse keep plugin hidden in background.
     */        
    virtual TBool CheckVisibilityL(MCCAppPluginParameter& aPluginParameter);

    /**
     * Shows plugin when called.
     * 
     * @since S60 v5.0
     */        
    void ShowPluginL();     

    /**
     * Hides plugin when called.
     * 
     * @since S60 v5.0
     */        
    void HidePluginL();

    /** 
     * Sets a parent clearer to this instance of view. Parent clearer takes care
     * of clearing ecom instantiated views. Ownership of clearer is shared among
     * views, reference count takes care that last owner deletes it.
     * @param aParentCleaner Pointer to parent clearer instance
     * @since S60 v5.0
     */        
    void TakeSharedOwnerShip( MCcaParentCleaner* aParentCleaner );

    /**
     * Returns an extension point for this interface or NULL.
     * @param aExtensionUid Uid of extension
     * @return Extension point or NULL
     */
    virtual TAny* CCCAppViewPluginBaseExtension( 
            TUid /*aExtensionUid*/ ) { return NULL; }
        
private: // data

    /**
     * iDtor_ID_Key Instance identifier key.
     */
    TUid iDtor_ID_Key;
        
    /**
     * Reference to the cca application engine
     * not owned
     */
    MCCAppEngine* iAppEngine;
    
    /** 
     * Pointer to parent clearer instance. Parent clearer takes care
     * of clearing ecom instantiated views 
     */
    MCcaParentCleaner* iSharedParentCleaner;

    /*
     * For future use. 
     */
	TInt32 iReserved[2];
};

#include <ccappviewpluginbase.inl>

#endif // __CCAPPVIEWPLUGINBASE_H__
