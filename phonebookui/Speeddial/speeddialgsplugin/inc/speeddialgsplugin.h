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
* Description:     Container for the Speeddial sub-folder
*
*/





#ifndef SPEEDDIALGSPLUGIN_H
#define SPEEDDIALGSPLUGIN_H

// Includes
#include <aknview.h>
#include <eikclb.h>
#include <gsparentplugin.h>
#include <gsfwviewuids.h>
#include <gsplugininterface.h>
#include <AknServerApp.h> 
// Classes referenced
class CAknViewAppUi;
class RConeResourceLoader;
class CAknLaunchAppService;
class CAknNullService;
// CLASS DECLARATION

/**
* CSpeeddialGsPlugin view class.
*
*/
class CSpeeddialGsPlugin : public CGSPluginInterface,
                                    public MAknServerAppExitObserver
    {
    public: // Constructors and destructor

        /**
        * Symbian OS two-phased constructor
        * @return
        */
        static CGSPluginInterface* NewL( TAny* aInitParams );

        /**
        * Destructor.
        */
        ~CSpeeddialGsPlugin();

    public: // From CAknView

        /**
        * See base class.
        */
        TUid Id() const;

        /**
        * See base class.
        */
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );
        /**
        * See base class.
        */
        void DoDeactivate();
        
        /**
        * From CGSPluginInterface.
        * @param aSelectionType selection type.
        */
        void HandleSelection( const TGSSelectionTypes aSelectionType );

    public: // From CGSParentPlugin

        /**
        * See base class.
        */
        TUid UpperLevelViewUid();

        /**
        * See base class.
        */
        TGSListboxTypes ListBoxType();
        
        /**
        * See base class.
        */
        void GetHelpContext( TCoeHelpContext& aContext );

    public: // From CGSPluginInterface

        /**
        * See base class.
        */
        void GetCaptionL( TDes& aCaption ) const;

        /**
        * See base class.
        */
        TInt PluginProviderCategory() const;

        /**
        * @see CGSPluginInterface header file.
        */
        CGulIcon* CreateIconL( const TUid aIconType );
        
         /**
        * From CGSPluginInterface.
        * @return this plugin's item type.
        */
        TGSListboxItemTypes ItemType();
        
        void HandleServerAppExit( TInt aReason );

    protected: // New
        /**
        * C++ default constructor.
        */
        CSpeeddialGsPlugin();
        
        void CSpeeddialGsPlugin::LaunchEasAppL();

    protected:
        /**
        * Symbian OS default constructor.
        */
        void ConstructL();

    private: // Data

        CEikonEnv* env;
        TInt       iResId;
        CAknNullService*  iNullService;
    };

#endif //SPEEDDIALGSPLUGIN_H

// End of File
