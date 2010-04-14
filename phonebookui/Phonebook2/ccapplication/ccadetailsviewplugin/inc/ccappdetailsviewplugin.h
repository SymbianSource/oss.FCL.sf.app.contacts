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
* Description:  Class implementing CCCAppViewPluginBase interface
 *
*/

#ifndef C_CCAPPDETAILSVIEWPLUGIN_H
#define C_CCAPPDETAILSVIEWPLUGIN_H

#include <ccappviewpluginbase.h>

#include <MVPbkContactStoreListObserver.h>
#include <RPbk2LocalizedResourceFile.h>
#include <mccappviewpluginbase2.h>

#include <mccacontactobserver.h>

class CAknIcon;
class CPbk2ApplicationServices;
class MVPbkContactStoreList;
class CCCAppDetailsViewMenuHandler;
class MCCAParameter;
class CCCAppCmsContactFetcherWrapper;
class CCAContactorService;
class MVPbkContactLink;

/**
 *  Class implementing CCCAppViewPluginBase interface. This is
 *  the main class and controlling the other classes of the plugin.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ccappdetailsviewplugin.dll
 */
class CCCAppDetailsViewPlugin :
    public CCCAppViewPluginBase,
    public MVPbkContactStoreListObserver,
    public MCCAppPluginsContactObserver,
    public MCCAppViewPluginBase2
{

public:

    /**
     * Two-phased constructor.
     */
    static CCCAppDetailsViewPlugin* NewL();

    /**
     * Destructor.
     */
    ~CCCAppDetailsViewPlugin();

public: // From MCCAppPluginsContactObserver

    void NotifyPluginOfContactEventL();

private: // From CAknView

    void HandleStatusPaneSizeChange();

private: // From CCCAppViewPluginBase

    void PreparePluginViewL(MCCAppPluginParameter& aPluginParameter );

    void ProvideBitmapL(TCCAppIconType aIconType, CAknIcon& aIcon);

    void NewContainerL();

    void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

    void HandleCommandL(TInt aCommand);

    TUid Id() const;

    void DoActivateL(
        const TVwsViewId& aPrevViewId,
        TUid aCustomMessageId,
        const TDesC8& aCustomMessage );

    void DoDeactivate();

    TBool CheckVisibilityL(MCCAppPluginParameter& aPluginParameter);
    
    TAny* CCCAppViewPluginBaseExtension( TUid aExtensionUid );

public: // new

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
     * Handles Edit Request.
     */
    void HandleEditRequestL();

    /**
     * Handles Delete Request.
     */
    void HandleDeleteRequestL();

private: // new

    /**
     * Helper function for reading and
     * loading resources needed by plugin.
     *
     */
    void PrepareViewResourcesL();

    /**
     * Creates menuhandler if not created already.
     */
    void EnsureMenuHandlerCreatedL();

private:

    CCCAppDetailsViewPlugin();

    void ConstructL();

    void GetContactDataL();

private: // From MVPbkContactStoreListObserver

    void OpenComplete();

    void StoreReady(MVPbkContactStore& aContactStore );

    void StoreUnavailable(MVPbkContactStore& aContactStore, TInt aReason );

    void HandleStoreEventL(MVPbkContactStore& aContactStore, TVPbkContactStoreEvent aStoreEvent );

public:
    /*
     * Sets the title to the plugin
     * @param aTitle - title to be set
     */
    void SetTitleL( const TDesC& aTitle );

    /**
     * Sets the CBA labels.
     * @param aEditable Boolean that tells if contact is editable or not
     */
    void UpdateMSKinCbaL( TBool aEditable );

private:

    /* Sets the Visibilty of the Plugin to true
     * and also calls ShowPlugin which will enable this plugin to be visible in the CCA
     */
    void MakeMeVisibleL();

    /* Check whether the contact info passed to this view is really valid or not
     * if not valid dont set the visibility of plugin to true
     *
     */
    void CheckValidityOfContactInfoL();

    /**
     * Callback function for CIdle object that calls PrepareAllPluginsL().
     *
     * @param aContainer
     * @return KErrnone always
     */
    static TInt CheckVisibilityCallbackL( TAny* aContainer );

    /**
     * Initiates the preparation of the details view plugins in the background
     *
     */
    void CheckVisibilityInBackgroundL();

    /**
     * Helper function for getting the CPbk2ApplicationServices instance.
     * This function creates the CPbk2ApplicationServices if not available
     *
     * @return - Reference to CPbk2ApplicationServices
     */
    CPbk2ApplicationServices& GetPbk2AppServicesL();
    
public:
    
    /**
     * Return pointer to contactor service.
     */
    CCAContactorService* ContactorService();
    
public:		//From MCCAppViewPluginBase2
    
    TBool PluginBusy();

private: // data

    /**
     * Pointer to menuhandler.
     * Own.
     */
    CCCAppDetailsViewMenuHandler* iMenuHandler;

    /**
     * Pointer to Application services.
     * Own.
     * Dont use this directly, use GetPbk2AppServicesL() API to
     * get the instance of CPbk2ApplicationServices&
     */
    CPbk2ApplicationServices* iAppServices;

    /**
     *
     */
    MVPbkContactStoreList* iStoreList;

    /**
     *
     */
    MCCAParameter* iLaunchParameter;
    /**
     *
     */
    TInt iFocusedListIndex;

    /**
     *
     */
    CActiveSchedulerWait iSchedulerWait;

    /// Own: Phonebook 2 commands dll resource file
    RPbk2LocalizedResourceFile iCommandsResourceFile;

    /// Own: Phonebook 2 UI controls dll resource file
    RPbk2LocalizedResourceFile iUiControlsResourceFile;

    /// Own: Phonebook 2 common UI dll resource file
    RPbk2LocalizedResourceFile iCommonUiResourceFile;

    //Boolean to decide whether a Plugin is Visible or not in the CCA
    //Default value is EFalse - this Plugin is NotVisible by default
    TBool iVisible;

    //BG task for checking the visibility of this details view plugin
    CIdle* iBgTaskForCheckingVisibility;

    /// Not own: CMS is used through CmsHandler to (re)fetch contact data fields
    CCCAppCmsContactFetcherWrapper* iCmsWrapper;
    
	/// Own: Pointer to contactor service.
    CCAContactorService* iContactorService;
    
    ///Contact Link - Owns
    MVPbkContactLink* iLink;

};

#endif // C_CCAPPDETAILSVIEWPLUGIN_H

// End of File
