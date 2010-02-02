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
* Description:  Implementation of MyCard plugin
*
*/

#ifndef CCAPPMYCARDPLUGIN_H
#define CCAPPMYCARDPLUGIN_H

#include <ccappviewpluginbase.h>
#include <mccappviewpluginbase2.h>
#include <coeutils.h>
#include <eikmobs.h>

// Forward declaration
class CCCAppMyCard;
class CAiwServiceHandler;
class CCCAppMyCardContainer;
class CPbk2CommandHandler;


/**
 *  Class implementing CCCAppViewPluginBase interface. This is
 *  the main class and controlling the other classes of the plugin.
 *
 *
 *  @lib ccappmycardplugin.dll
 *  @since S60 9.2
 */
NONSHARABLE_CLASS( CCCAppMyCardPlugin ) :
    public CCCAppViewPluginBase,
    public MCCAppViewPluginBase2
    {
// for testing
friend class CUT_CCAppMyCardPlugin;

public:

    /**
     * Two-phased constructor.
     */
    static CCCAppMyCardPlugin* NewL();

    /**
     * Destructor.
     */
    ~CCCAppMyCardPlugin();

public: // New methods
    
    /**
     * Get access to MyCard instance
     * 
     * @return MyCard
     */
    CCCAppMyCard& MyCard();
    
    /*
     * Edit my card fields
     * 
     * @param aFocusedFieldIndex  field to be focused in editor
     */
    void EditL( TInt aFocusedFieldIndex );
    
public: 

// from base class CCCAppViewPluginBase

    /**
     * From CCCAppViewPluginBase
     */
    void PreparePluginViewL(
        MCCAppPluginParameter& aPluginParameter );

    /**
     * From CCCAppViewPluginBase
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
    
    /**
     * From MCCAppViewPluginBase2
     */
    TBool PluginBusy();
    
public: // MEikMenuObserver
     void ProcessCommandL(TInt aCommandId);

private:
    
    /*
     * Send business card created from my card
     */
    void SendBusinessCardL();
    
    /*
     * Removes the MyCard image
     */
    void RemoveImageCmdL(); 
    
    /*
     *  Add image to MyCard
     */
    void AddImageCmdL(); 
    
    /*
     *  Change MyCard image
     */
    void ChangeImageCmdL(); 
    
    /*
     * View MyCard image
     */
    void ViewImageCmdL(); 
    
    /*
     * Creates command handler if not created, and returns pointer to it.
     */
    CPbk2CommandHandler* CommandHandlerL();
    
      
private: // constructors
    
    /**
     * Constructor
     */
    CCCAppMyCardPlugin();

    /**
     * Constructor
     */
    void ConstructL();

private: // data
    
    /// Own. Contact card manager
    CCCAppMyCard* iMyCard;
    
    /// Own. Phonebook 2 UIControls dll resource file
    RCoeResourceLoader iPbk2UiControlResource;
    
    /// Own: Phonebook 2 common UI dll resource file
    RCoeResourceLoader iCommonUiResourceFile;
    
    /// Own: Phonebook 2 commands dll resource file
    RCoeResourceLoader iCommandsResourceFile;
    
    /// Not own. Reference to own container. Owned in base class.
    CCCAppMyCardContainer* iOwnContainer;
    
    // Own. Command handler
    CPbk2CommandHandler* iCommandHandler;
    };

#endif // CCAPPMYCARDPLUGIN_H

// End of File
