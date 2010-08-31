/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#include <QPluginLoader>
#include <QtGui>
#include <cntuiextensionfactory.h>
#include "cntextensionmanager.h"

const char *CNT_UI_EXTENSION_PLUGIN_DIRECTORY = "/resource/qt/plugins/contacts/extensions/";

CntExtensionManager::CntExtensionManager( QObject* aParent ):
    mPluginsLoaded(false)
{
    setParent(aParent);
}

CntExtensionManager::~CntExtensionManager()
{
    unloadExtensionPlugins();
}


int CntExtensionManager::pluginCount()
{
    loadExtensionPlugins();
    return mPlugins.count();
}
    
CntUiExtensionFactory* CntExtensionManager::pluginAt( int index)
{
    loadExtensionPlugins();
    QPluginLoader* pluginLoader = mPlugins[index];
    QObject *plugin = pluginLoader->instance();
    if (plugin)
    {
        return qobject_cast<CntUiExtensionFactory*>(plugin);
    }
    return NULL;
}

CntUiExtensionFactory* CntExtensionManager::servicePlugin(int serviceId)
{
    foreach(QPluginLoader* pluginLoader, mPlugins )
    {
        QObject *plugin = pluginLoader->instance();
        if (plugin)
        {
            CntUiExtensionFactory* interfacePlugin = qobject_cast<CntUiExtensionFactory*>(plugin);
            if(interfacePlugin && interfacePlugin->hasService(serviceId))
            {
                return interfacePlugin;
            }
        }
    }
    return NULL;
}

void CntExtensionManager::loadExtensionPlugins()
{
    if(mPluginsLoaded)
    {
        // Plugins already loaded, just return.
        return;
    }
    // TBD: extension should be loaded when they are first time needed.
    // This is needed due to performance impact on contacts startup.
    // Also framework should provide means to load new plugins
    // in runtime. It is very likely use case that new plugins will be
    // installed in runtime while contacts is running => new plugin should
    // be loaded when it is added to file system (assuming that this
    // extension API will be open for 3rd party plugins)
    
    QDir pluginsDir(CNT_UI_EXTENSION_PLUGIN_DIRECTORY);
    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        // Create plugin loader
        QPluginLoader* pluginLoader = new QPluginLoader( 
                pluginsDir.absoluteFilePath(fileName));
        if ( pluginLoader->load() )
        {
            QObject *plugin = pluginLoader->instance();
            if (plugin)
            {
                CntUiExtensionFactory* interface = qobject_cast<CntUiExtensionFactory*>(plugin); 
                
                if ( interface )
                {
                    mPlugins.append(pluginLoader);
                }
                // If plugin loader did not load our plugin, unload the loader
                else
                {
                    pluginLoader->unload();
                }
            }
        }
    }
    mPluginsLoaded = true;
}

void CntExtensionManager::unloadExtensionPlugins()
{
    // Unload plugins and clear plugin array
    foreach (QPluginLoader* pluginLoader, mPlugins)
    {
        pluginLoader->unload();
    }   
    mPlugins.clear();  
    mPluginsLoaded = false;  
}

