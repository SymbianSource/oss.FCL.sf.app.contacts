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
* Description:  A class responsible handling the views
 *
*/



#ifndef MCCAPPENGINE_H_
#define MCCAPPENGINE_H_

#include <mccacontactobserver.h>

// FORWARD DECLARATIONS
class MCCAParameter;

/**
*	CCA engine class interface
*
*	CCA engine handles the creation and loading of the plugins.
* This class also handles switching of the plugin views. With
* this interface clients can recieve contact notifications via
* MCCAppPluginsContactObserver
*/
class MCCAppEngine
		{
public:
		/*
     * CCApp engine is notified by a plugin of a contact event
     * 
     * @since S60 v5.0
     */     
     virtual void CCAppContactEventL() = 0;
     
    /**
     * Adds observer to start receiving MCCAppContactChangeObserver-events.
     * 
     * @since S60 v5.0
     * @param aObserver an observer
     */    
     virtual void AddObserverL( MCCAppPluginsContactObserver& aObserver ) = 0;
    
    /**
     * Removes observer from observer queue
     *
     * @since S60 v5.0
     * @param aObserver an observer
     */    
     virtual void RemoveObserver( MCCAppPluginsContactObserver& aObserver ) = 0;
     
     /**
     	* The CCA parameter.
     	* Plugins should not modify this parameter. They do not own it
     	*
     	* @since S60 v5.0
     	*/      
     virtual MCCAParameter& Parameter() = 0;

    /**
     * Shows the plugin in CCApplication.
     *
     * @since S60 v5.0
     * @param aUid Id of the plugin to be shown.
     */    
    virtual void ShowPluginL( TUid aUid ) = 0;

    /**
     * Hides the plugin from CCApplication.
     *
     * @since S60 v5.0
     * @param aUid Id of the plugin to be hidden.
     */    
    virtual void HidePluginL( TUid aUid ) = 0;

    /**
     * Returns an extension point for this interface or NULL.
     * @param aExtensionUid Uid of extension
     * @return Extension point or NULL
     */
    virtual TAny* CcaAppEngineExtension( 
            TUid /*aExtensionUid*/ ) { return NULL; }    
    
    
     };

#endif /*MCCAPPENGINE_H_*/
