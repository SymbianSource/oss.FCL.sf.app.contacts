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
* Description:  Class implementing ccappcommlauncherplugin menu functionality
*
*/


#ifndef C_CCAPPCOMMLAUNCHERMENUHANDLER_H
#define C_CCAPPCOMMLAUNCHERMENUHANDLER_H

#include <akninputblock.h>

class CCCAppCommLauncherPlugin;
class CAiwGenericParamList;
class CAiwServiceHandler;
class CCCAppCommLauncherPbkCmd;

/**
 *  Class implementing ccappcommlauncherplugin menu functionality
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ccappcommlauncherplugin.dll
 *  @since S60 v3.2
 */
class CCCAppCommLauncherMenuHandler:
             public CBase,
             public MAknInputBlockCancelHandler,
             public MAiwNotifyCallback     
    {
#ifdef __COMMLAUNCHERPLUGINUNITTESTMODE
    friend class T_CCCAppCommLauncherMenuHandler;
#endif// __COMMLAUNCHERPLUGINUNITTESTMODE

public:

    /**
     * Two-phased constructor.
     */
    static CCCAppCommLauncherMenuHandler* NewL(
        CCCAppCommLauncherPlugin& aPlugin );

    /**
     * Destructor.
     */
    ~CCCAppCommLauncherMenuHandler();

public: // from base classes   

    /**
     * From CCCAppViewPluginBase
     */    
    void DynInitMenuPaneL(
        TInt aResourceId, 
        CEikMenuPane* aMenuPane );

    /**
     * From CCCAppViewPluginBase
     */    
    void HandleCommandL( 
        TInt aCommand );

    /**
     * From MAiwNotifyCallback
     */    
    TInt HandleNotifyL(
        TInt aCmdId,
        TInt aEventId,
        CAiwGenericParamList& aEventParamList,
        const CAiwGenericParamList& aInParamList);    
    /**
      * From MAknInputBlockCancelHandler
      */
    void AknInputBlockCancel();
    
public: // new

    /**
     * Set the store from which the contact is from
     *
     * @since S60 v5.0
     * @param aStore
     */    
    void SetContactStore( const TCmsContactStore& aStore );
    
private:

    /**
     * Prepares plugin for AIW-menu usage
     *
     * @since S60 v5.0
     */
    void PrepareAiwMenusL();

    /**
     * Avkon-like DynInitMenuPaneL for switching appropriate 
     * menu-options based on contact store type.
     *
     * @since S60 v5.0
     * @param aMenuPane as menupane structure
     */    
    void SelectDynInitMenuPaneL( CEikMenuPane* aMenuPane );
    
    /**
     * Avkon-like DynInitMenuPaneL for cases when contact is
     * found from local contact db
     *
     * @since S60 v5.0
     * @param aMenuPane as menupane structure
     */
    void PbkContactDynInitMenuPaneL( CEikMenuPane* aMenuPane );

    /**
     * Avkon-like DynInitMenuPaneL for cases when contact is
     * found from SIM
     *
     * @since S60 v5.0
     * @param aMenuPane as menupane structure
     */
    void SimContactDynInitMenuPaneL( CEikMenuPane* aMenuPane );

    /**
     * Avkon-like DynInitMenuPaneL for cases when contact is SDN contact
     *
     * @since S60 v5.0
     * @param aMenuPane as menupane structure
     */    
    void SdnContactDynInitMenuPaneL( CEikMenuPane* aMenuPane );    

    /**
     * DynInit for "Select"-menu item
     *
     * @since S60 v5.0
     * @param aMenuPane as menupane structure
     */     
    void DynInitSelectMenuItemL( CEikMenuPane* aMenuPane );
 
    /**
     * DynInit for "Select other"-menu item
     *
     * @since S60 v5.0
     * @param aMenuPane as menupane structure
     */    
    void DynInitSelectOtherMenuItemL( CEikMenuPane* aMenuPane );
    
    /**
     * Commands for Options-menu items
     *
     * @since S60 v5.0
     */
    void DoSelectCmdL( TBool aUseDefaultAddress );
    void DoEditCmdL();
    void DoSetDefaultCmdL();
    void DoShowOnMapCmdL();
    void DoDeleteCmdL();
    void DoSendBCardCmdL();
    
    /**
     * Handlers for Aiw-based commands
     *
     * @since S60 v5.0
     */
    void DoAiwCommandL( TInt aCmdId, TInt aServiceId );
    TInt PackedContactLinkArrayToInParamListLC( 
        CAiwGenericParamList& aInParamList );
    
    /**
     * Opens resource files and instanciates CPbk2ApplicationServices
     */
    void PreparePbk2ApplicationServicesL();
    
    /**
     * Avkon-like DynInitMenuPaneL for cases when contact is
     * found from xsp contact db
     *
     * @since S60 v5.0
     * @param aMenuPane as menupane structure
     */
    void XspContactDynInitMenuPaneL( CEikMenuPane* aMenuPane );

    /**
      * Cancel Input Block if it exists
      *
      * @since S60 v5.0
      */    
    void StopInputBlock();
private: // constructors

    CCCAppCommLauncherMenuHandler( CCCAppCommLauncherPlugin& aPlugin );
    void ConstructL();

private:// data

    /**
     * Pointer to class handling the view
     * Not own.
     */
    CCCAppCommLauncherPlugin& iPlugin;

    /**
     * Pointer to AIW service handler
     * Own.
     */
    CAiwServiceHandler* iAiwServiceHandler;

    /**
     * A store where this contact is stored to
     */    
    TCmsContactStore iContactStore;

    /// Own: CCCAppCommLauncherPbkCmd
    CCCAppCommLauncherPbkCmd* iPbkCmd;
    
    /// Doesnt Own: CAknInputBlock. Ownership transferred thro SetCancelDelete Call.
    CAknInputBlock *iInputBlock;
    };

#endif // C_CCAPPCOMMLAUNCHERMENUHANDLER_H
