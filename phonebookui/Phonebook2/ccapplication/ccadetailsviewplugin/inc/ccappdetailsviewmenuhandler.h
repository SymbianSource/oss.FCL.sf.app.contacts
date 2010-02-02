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
* Description:  Class implementing ccappdetailsviewplugin menu functionality
*
*/


#ifndef C_CCAPPDETAILSVIEWMENUHANDLER_H
#define C_CCAPPDETAILSVIEWMENUHANDLER_H

#include <mccappengine.h>

class CCCAppDetailsViewPlugin;
class CAiwServiceHandler;
class CPbk2CommandHandler;
class CMnProvider;

/**
 *  Class implementing ccappdetailsviewplugin menu functionality
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ccappdetailsviewplugin.dll
 */
class CCCAppDetailsViewMenuHandler :
    public CBase,
    public MAiwNotifyCallback
{
public:

    /**
     * Two-phased constructor.
     */
    static CCCAppDetailsViewMenuHandler* NewL(CCCAppDetailsViewPlugin& aPlugin);

    /**
     * Destructor.
     */
    ~CCCAppDetailsViewMenuHandler();

public: // From CCCAppViewPluginBase

    void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

    void HandleCommandL(TInt aCommand);

public: // From MAiwNotifyCallback

    TInt HandleNotifyL(
        TInt aCmdId,
        TInt aEventId,
        CAiwGenericParamList& aEventParamList,
        const CAiwGenericParamList& aInParamList);

public:

    MCCAppEngine& AppEngine()
    {
        return *iAppEngine;
    }

    void SetAppEngine(MCCAppEngine* aAppEngine)
    {
        iAppEngine = aAppEngine;
    }
    
    /**
     * Resets the state in case of details view deactivation
     */
    void Reset();

    /**
     * Activates the state in case of details view activation
     */
    void Activate();

private: // new

    /**
     *
     */
    TBool IsFocusedFieldTypeL(TInt aSelectorResId);

    /**
     *
     */
    TBool HasContactFieldTypeL(TInt aSelectorResId);

    /**
     *
     */
    void PrepareAiwMenusL();

private: // Commands for Options-menu items

    void DoEditCmdL();
    void DoViewImageCmdL();
    void DoSendBusinessCardCmdL();
    void DoTopContactCmdL();
    void DoDeleteCmdL();
    void DoCopyCmdL();
    void DoCopyDetailCmdL();
    void DoAssignSpeeddialCmdL();
    void DoUnassignSpeeddialCmdL();
    void DoSetDefaultCmdL();
    void DoPrependCmdL();
    void DoCopyAddressCmdL();   
    void DoCopyNumberCmdL();
	void DoShowOnMapCmdL();
    TBool HasAddressFieldL();
    TBool IsAddressValidatedL();

    /**
     * Handlers for Aiw-based commands
     *
     */
    //void DoAiwCommandL( TInt aCmdId, TInt aServiceId );

private: // constructors

    CCCAppDetailsViewMenuHandler(CCCAppDetailsViewPlugin& aPlugin);

    void ConstructL();

private: // data

    /**
     * Pointer to class handling the view
     * Not own.
     */
    CCCAppDetailsViewPlugin& iPlugin;

    /**
     * Pointer to AIW service handler
     * Own.
     */
    CAiwServiceHandler* iAiwServiceHandler;

    /**
     * Pointer to Pbk2 command handler.
     * Own.
     */
    CPbk2CommandHandler* iCommandHandler;

    /**
     *
     */
    MCCAppEngine* iAppEngine;

    /**
     * Map View Provider
     * Own.
     */
    CMnProvider* iMapViewProvider;

};

#endif // C_CCAPPDETAILSVIEWMENUHANDLER_H

// End of File
