/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*     Logs application "Settings" view class implementation
*
*/


#ifndef     __Logs_App_CLogsSettingsView_H__
#define     __Logs_App_CLogsSettingsView_H__

//  INCLUDES
#include <ConeResLoader.h>
#include <logcli.h>
#include <gsplugininterface.h>
#include <aknradiobuttonsettingpage.h> 

#include "CLogsBaseView.h"                  
#include "MLogsObserver.h"

#include "LogsEng.hrh"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CLogsSettingsControlContainer;
class CLogsEngine;
class MCoeForegroundObserver;
class CAknQueryDialog;

// CLASS DECLARATION

/**
 * Logs application "Settings" view class implementation.
 * Several application settings can be modified in this view.
 */
class   CLogsSettingsView : public CGSPluginInterface, 
                            public MLogsObserver, 
                            public MEikListBoxObserver,
                            public MLogsKeyProcessor,
                            public MCoeForegroundObserver    
    {
    public:  // interface
    
    
        /**
        * Creates new GS plugin having the given UID.
        * Uses Leave code KErrNotFound if implementation is not found.
        *
        * @param aImplementationUid Implementation UID of the plugin to be
        *        created.
        * @param aInitParams Plugin's initialization parameters. Make sure you know 
        *        what the plugin expects as initialization parameters. This 
        *        should be an agreement between the plugin client and the 
        *        plugin. Parameter can be used for example as sharing a common 
        *        data model between multiple plugins.
        */
        static CGSPluginInterface* NewPluginL(
            const TUid aImplementationUid,
            TAny* aInitParams);
    
        /**
         * Destructor.
         */
        ~CLogsSettingsView();

    
    private:
        /**
         * Constructor, second phase. 
         */
        void ConstructL();

        /**
         * C++ constructor
         */
        CLogsSettingsView();

        /**
         * Framework calls to init menu items.
         *
         * @param aResourceId   resource read
         * @param aMenuPane     menu pane
         */ 
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
        
        /**
         * Framework calls to init process commands
         *
         * @param aCommand
         */ 
        void ProcessCommandL( TInt aCommand );

        // from MLogsKeyProcessor
        TBool ProcessKeyEventL( const TKeyEvent& aKeyEvent,
                                TEventCode aType );
 		/**
         * Overrides the baseclass handler
         * 
         * @param aIndex	The index of the item tapped
         */                                   
        void ProcessPointerEventL( TInt aIndex);
        
        //void FocusChangedL();        

        MAknTabObserver* TabObserver();                                                  

        // from MEikListBoxObserver
        void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );

        // from MLogsModelObserver
        void StateChangedL( MLogsStateHolder* aHolder ); 

    private:  // from CAknView
        /**
         * Returns the ID of the view.
         * @return view id
         */
        TUid Id() const;

        /**
         * Called when the client rectangle of this view changes
         */
        void HandleClientRectChange();

        /**
         * Called by the framework when this view is activated.
         *
         * @param aPrevViewId      This is ID for previous view.
         * @param aCustomMessageId ID of the custom message. Not used.
         * @param aCustomMessage   custom message. Not used.
         */
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );
        
        /**
         *  Called by the framework when this view is deactivated.
         */
        void DoDeactivate();

        /*
         * Show note dialog
         * @param aResourceId, note to show.
         */
        void ShowNoteL( TInt aResourceID );


        /**
         * Is call active
         * @return ETrue if call is active, else return EFalse
         */
        // TBool CallIsActiveL();
    
    private: // From MCoeForegroundObserver.
     
        /** 
         * Called by the framework when this view gains foreground
         */                  
        void HandleGainingForeground();
        
        /**
         * Called by the framework when this view loses foreground
         */  
        void HandleLosingForeground();    

    private: // Command handlers (use "Cmd" prefix)
       
        /**
         * Manage setting log length possible changes.
         */
        void CmdLogLengthL();

        //Below: "Show Call Duration" moved General Settings application, not in use any more in Logs 
        /**
         *  Makes the change in to the "Call Duration" and handles the
         *  rules, which have been given
         *
         *  @param  aDurationItem   The selected item
         */
        // void MakeCallDurationChangeL( TBool aChange );

        /**
         * Manage setting show call duration possible changes.
         */
        // void CmdShowCallDurationPhoneL();


        /**
         *  Makes opposite selection to "Call Duration"
         */
        // void MakeCallDurationOppositeL();

    public:
        /**
         *  Log age getter
         *
         *  @return log age
         */
        TLogAge LogAge() const;
        
        /**
         *  Logs Engine getter
         *
         *  @return Logs engine
         */
        CLogsEngine* Engine();

    private:  // from CGSPluginInterface  

        /**
        * Provides caption of this plugin. This should be the
        * localized name of the Logs settings view to be shown in parent view
        * (i.e. GS applications view)
        *
        * @param aCaption pointer to Caption variable
        */
        void GetCaptionL( TDes& aCaption ) const;

        /**
        * Provides the plugin provider category. See
        * TGSPluginProviderCategory. PluginProviderCategory can be used for
        * sorting plugins.
        *
        * Default value is EGSPluginProvider3rdParty. Override this function
        * to change the category.
        *
        * @return Plugin provider category ID defined by
        *         TGSPluginProviderCategory
        */
        TInt PluginProviderCategory() const;    
        
        /**
        * Creates a new icon of desired type. Override this to provide custom
        * icons. Othervise default icon is used. Ownership of the created icon
        * is transferred to the caller.
        *
        * Icon type UIDs (use these defined constants):
        * KGSIconTypeLbxItem  -   ListBox item icon.
        * KGSIconTypeTab      -   Tab icon.
        *
        * @param aIconType UID Icon type UID of the icon to be created.
        * @return Pointer of the icon. NOTE: Ownership of this icon is
        *         transferred to the caller.
        */
        virtual CGulIcon* CreateIconL( const TUid aIconType );        

        virtual TAny* CustomOperationL( TAny* aParam1, TAny* aParam2 );

    private: // from MEikCommandObserver
        /**
         * Handles the view specific commands.
         *
         * @param aCommandId This is an id for command.
         *                   Most of the commands are forwarded to LogAppUI.
         */
         void HandleCommandL(TInt aCommand);

    
    private:  // operations

        /**
         *  Makes view's one line with the given information
         *
         *  @param  aItems          Array where to put the line information
         *  @param  aResourceText   The resource index of text to be shown
         */
        void MakeSettingLineL( CDesCArrayFlat* aItems, TInt aResourceText );

        /**
         * User log length selection "No log" query.
         *
         * @return The ETrue value indicates that log lenght "No log" 
         *         should be activated.
         */
        TBool NoLogQueryL();
        
        void SetForGS( TBool aForGS );

    private:  // data
        /// Own: This view's control container
        CLogsSettingsControlContainer* iContainer;
        CAknRadioButtonSettingPage*    iDialog;
        CAknQueryDialog* 			   iNoLogQueryDlg;
        CDesCArrayFlat*                iSettingItems; 
        
        /// Own: config info
        TLogConfig      iConfig;
        CLogsEngine*    iEngine;    
        TVwsViewId      iPrevViewId;
        CEikonEnv*      iEikEnv;                // Local eikonenv		
        TInt            iResourceFileOffset;    // Offset for eikon env.
        TBool           iForGs;
        TLogsState      iState;      
        
        TInt            iLastMaxEventAge;                  
    };

#endif  // __Logs_App_CLogsSettingsView_H__


// End of File

