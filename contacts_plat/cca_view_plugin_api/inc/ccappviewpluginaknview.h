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
* Description:  Plugin IF for Avkon based plugin views
*
*/

#ifndef CCAPPVIEWPLUGINAKNVIEW_H
#define CCAPPVIEWPLUGINAKNVIEW_H

#ifndef __CCAPPUTILUNITTESTMODE
#include <aknview.h>
#include <ConeResLoader.h>
#endif //__CCAPPUTILUNITTESTMODE

class CCCAppViewPluginAknContainer;

/**
 *  CAknView tailored for the needs of CCApp plugins.
 *
 *  @code
 *   Derive from CCCAppViewPluginBase, which is inherited from CCCAppViewPluginAknView.
 *
 *   class CCCAppMyPlugin : public CCCAppViewPluginBase
 *       {
 *   ...  
 *
 *  CCCAppViewPluginBase is inherited from CAknView. CAknView has pure
 *  virtual method for view id. With plugin views, this method is used
 *  to return the implementation Id. This helps to identify the views and
 *  the plugins with each other.
 * 
 *   TUid CCCAppMyPlugin::Id()const
 *       {
 *       return TUid::Uid( KCCAMyPluginImplmentationUid );
 *       }
 *     
 *   During the preparations CCCAppViewPluginBase's PreparePluginViewL() is called. It is called
 *   only once in plugin's lifespan. Use it to load resource-file containing the resources your 
 *   plugin needs. Actual loading of menus / soft keys can be left to base-class via calling 
 *   BaseConstructL() and id of AVKON_VIEW -resource struct.
 *
 *   void CCCAppMyPlugin::PreparePluginViewL( MCCAppPluginParameter& aPluginParameter )
 *       {
 *       // preparing resources for use
 *       TFileName fileName( KCCAppCommLauncherResourceFileName );
 *       BaflUtils::NearestLanguageFile( iCoeEnv->FsSession(), fileName );
 *       iResourceLoader.OpenL( fileName );
 *       // calling base-class to let it do the rest of loading       
 *       BaseConstructL( R_COMMLAUNCHER_VIEW );
 *
 *       // other plugin preparation work...
 *       }
 *
 *   When plugin view is going to be activated to the display, DoActivateL()
 *   is called. Base-class DoActivateL should be called.
 * 
 *   void CCCAppMyPlugin::DoActivateL( 
 *       const TVwsViewId& aPrevViewId,
 *       TUid aCustomMessageId,
 *       const TDesC8& aCustomMessage )
 *       {
 *       // Forward the activation-call to base-class
 *       CCCAppViewPluginAknView::DoActivateL(
 *           aPrevViewId, aCustomMessageId, aCustomMessage );
 *       // other plugin activation work ...
 *
 *  During the activation of plugin view NewContainerL() is called. Use it to
 *  construct your container-class inherited from CCCAppViewPluginAknContainer.
 *
 *    void CCCAppMyPlugin::NewContainerL()
 *       {
 *       iContainer = new (ELeave) CCCAppMyContainer();
 *       } 
 *
 *  When plugin view is going to be deactivated from the display, 
 *  DoDeactivate() is called. Base-class DoDeactivate should be called.
 *  Plugin view is deleted during the deactivation process, but container
 *  is.
 * 
 *   void CCCAppMyPlugin::DoDeactivate()
 *       {
 *       // Forward the deactivation-call to base-class
 *       CCCAppViewPluginAknView::DoDeactivate(); 
 *       // other plugin deactivation work ...
 *       }
 *  
 *  Each plugin view should show its own title in the titlePane.
 *  To Facilitate this, the Plugin View base-Class provides SetTitleL(aTitle) API.
 *  Base-class SetTitleL should be called.
 *  The new title 'aTitle' gets updated in the TitlePane of the View
 * 
 *   void CCCAppMyPlugin::SetTitleL()
 *       {
 *       // Forward the SetTitleL-call to base-class
 *       CCCAppViewPluginAknView::SetTitleL( _LIT("My Title") ); 
         }
 * 
 *  @endcode
 *
 *  @lib ccapputil.dll
 *  @since S60 v5.0
 */
class CCCAppViewPluginAknView : public CAknView
    {

    public: // Constructors & destructors

        /**
        * C++ constructor.
        */
        IMPORT_C CCCAppViewPluginAknView();

        /**
        * Destructor
        */
        IMPORT_C virtual ~CCCAppViewPluginAknView();

    protected: // From CAknView

        /**
         * Creates needed container through NewContainerL() and adds it
         * to stack. 
         * 
         * This should be called from the sub-class.
         *
         * See more details from CAknView.
         *
         * @since S60 v5.0
         */
        IMPORT_C virtual void DoActivateL( 
            const TVwsViewId& aPrevViewId,
            TUid aCustomMessageId,
            const TDesC8& aCustomMessage );

        /**
         * Deletes used container. 
         * 
         * This should be called from the sub-class.
         *
         * See more details from CAknView.
         *
         * @since S60 v5.0
         */
        IMPORT_C virtual void DoDeactivate();

        /**
         * Calls CCApplication's HandleCommandL() which takes care of
         * following events: EEikCmdExit, EAknSoftkeyBack, EAknSoftkeyExit
         * and EAknCmdHelp (will call container classes GetHelpContext-callback). 
         * 
         * This should be called from the sub-class.
         *
         * See more details from CAknView.
         *
         * @since S60 v5.0
         */
        IMPORT_C virtual void HandleCommandL( TInt aCommand );
        
    protected: // New

        /**
        * Creates container,
        *
        * @since S60 v5.0
        */    
        void CreateContainerL();

        /**
        * This is meant for sub-class to create a container. Called during DoActivateL.
        *
        * @since S60 v5.0
        */
        virtual void NewContainerL() = 0;

        /**
        * Deletes container if exists.
        *
        * @since S60 v5.0
        */
        void DeleteContainerIfExists();   

	public:
	    /**
         * Sets the title of the View Plugin
         * @param aTitle - the title to be set for the Plugin View
         * 
         * @since S60 v5.0
         * 
         */
	    IMPORT_C void SetTitleL( const TDesC& aTitle );
    protected: // Data

        /**
        * Container.
        * Owned.
        */
        CCCAppViewPluginAknContainer* iContainer;
        /**
        * Resourceloader.
        * Owned.
        */
        RConeResourceLoader iResourceLoader;        
    };

#endif // CCAPPVIEWPLUGINAKNVIEW_H
//End of file
