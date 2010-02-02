/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of comm launcher view plugin
*
*/


#ifndef C_CCAPPCOMMLAUNCHERPLUGIN_H
#define C_CCAPPCOMMLAUNCHERPLUGIN_H

#include <mccacontactobserver.h>

#include "ccappcommlauncherheaders.h"
#include <mccappviewpluginbase2.h>

class CCCAppCommLauncherContactHandler;
class CCCAppCommLauncherMenuHandler;
class CCAContactorService;
class CCCAppCommLauncherLSetDefault;

/**
 *  Class implementing CCCAppViewPluginBase interface. This is
 *  the main class and controlling the other classes of the plugin.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ccappcommlauncherplugin.dll
 *  @since S60 v3.2
 */
class CCCAppCommLauncherPlugin :
    public CCCAppViewPluginBase,
    public MCCAppPluginsContactObserver,
    public MCCAppViewPluginBase2
    {
#ifdef __COMMLAUNCHERPLUGINUNITTESTMODE
    friend class T_CCCAppCommLauncherPlugin;
#endif// __COMMLAUNCHERPLUGINUNITTESTMODE

public:

    /**
     * Two-phased constructor.
     */
    static CCCAppCommLauncherPlugin* NewL();

    /**
     * Destructor.
     */
    ~CCCAppCommLauncherPlugin();

public: //From MCCAppPluginsContactObserver

    void NotifyPluginOfContactEventL();

public:

// from base class CCCAppViewPluginBase

    /**
     * From CCCAppViewPluginBase
     */
    void PreparePluginViewL(
        MCCAppPluginParameter& aPluginParameter );

    /**
     * From CCCAppViewPluginBase
     * NEW API
     */
    void ProvideBitmapL(
        TCCAppIconType aIconType,
        CAknIcon& aIcon );

    /**
     * From CCCAppViewPluginBase
     */
    TAny* CCCAppViewPluginBaseExtension( TUid aExtensionUid );

// from base class CAknView

    /**
     * From CAknView
     */
    void DynInitMenuPaneL(
        TInt aResourceId,
        CEikMenuPane* aMenuPane );

    /**
     * From CAknView
     */
    void HandleCommandL(
        TInt aCommand );

    /**
     * From CAknView
     */
    void DoActivateL(
        const TVwsViewId& aPrevViewId,
        TUid aCustomMessageId,
        const TDesC8& aCustomMessage );

    /**
     * From CAknView
     */
    void DoDeactivate();

    /**
     * From CAknView
     */
    TUid Id() const;

// from base class CCCAppViewPluginAknView

    /**
     * From CCCAppViewPluginAknView
     */
    void PrepareViewResourcesL();

    /**
     * From CCCAppViewPluginAknView
     */
    void NewContainerL();
    
public: //From MCCAppViewPluginBase2
    
    TBool PluginBusy();

public:

    /**
     * Returns a pointer to preferred comm methods
     *
     * @since S60 v5.0
     * @return pointer to preferred comm method array
     */
    RArray<VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector>&
        PreferredCommMethods();

   /**
     * Classes enumerated, see ErrorNotifierL() below
     *
     * @since S60 v5.0
     */
    enum TCCAppCommLauncherPluginErrorSource
        {
        ECCAppCommLauncherContactHandler = 1
        };

   /**
     * Error handler for errors coming from differen classes
     *
     * @since S60 v5.0
     */
    void ErrorNotifierL(
        TCCAppCommLauncherPluginErrorSource aErrorSource,
        TInt aError );

    /**
     * For getting information if the contact is Top Contact
     *
     * @since S60 v5.0
     * @return TBool
     */
    TBool IsTopContactL();

    /**
     * For getting information if the contact is from the contacts db
     *
     * @since S60 v5.0
     * @return TBool
     */
    TBool IsContactL();

    /**
     * For getting reference to container-class
     *
     * @since S60 v5.0
     * @return reference to container-class
     */
    CCCAppCommLauncherContainer& Container();

    /**
     * For getting reference to contacthandler-class
     *
     * @since S60 v5.0
     * @return reference to contacthandler-class
     */
    CCCAppCommLauncherContactHandler& ContactHandler();

    /**
     * Helper function for get the reference
     * to container data.
     *
     * return container
     */
    CCCAppViewPluginAknContainer& GetContainer()
    {
        return *iContainer;
    }

    /**
     * Return pointer to contactor service.
     */
    CCAContactorService* ContactorService();
    
    void DefaultSettingComplete();

private: // new

    /**
     * Initialises the preferred communication methods array.
     *
     * @since S60 v5.0
     */
    void InitialisePreferredCommMethods();

    /**
     * Loads preferred communication methods array from CenRep.
     *
     * @since S60 v5.0
     */
    void LoadPreferredCommMethodsL();

    /**
     * Creates menuhandler if not created already.
     *
     * @since S60 v5.0
     */
    void EnsureMenuHandlerCreatedL();

private: // constructors

    CCCAppCommLauncherPlugin();
    void ConstructL();

public:
    /*
     * Sets the title to the plugin
     * @param aTitle - title to be set
     */
    void SetTitleL( const TDesC& aTitle );

    /**
     * Sets the CBA labels.
     * @param aEditable Boolean that tells if contact has communication
     * methods or not.
     */
    void UpdateMSKinCbaL( TBool aCommMethodsAvailable );

private:// data

    /**
     * Pointer to contact data fetcher.
     * Own.
     */
    CCCAppCommLauncherContactHandler* iContactHandler;
    
    CCCAppCommLauncherLSetDefault* iSetDefault;

    /**
     * Pointer to preferred communication methods.
     * In case of deactivation, this is not deleted for making the new opening faster.
     * Own.
     */
    RArray<VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector>
        iPreferredCommMethods;

    /**
     * Pointer to commlauncher's menuhandler
     * Own.
     */
    CCCAppCommLauncherMenuHandler* iMenuHandler;

    /**
     * Flag if contact must be refetched.
     */
    TBool iRefetchContact;

    /**
     * Pointer to contactor service.
     * Own.
     */
    CCAContactorService* iContactorService;
    
    CActiveSchedulerWait* iWaitFinish;

    };

#endif // C_CCAPPCOMMLAUNCHERPLUGIN_H

// End of File
