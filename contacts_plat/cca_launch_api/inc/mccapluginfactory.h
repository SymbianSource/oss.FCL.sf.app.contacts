/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface class for instantiating views from CCA plugin.
*
*/
#ifndef MCCAPLUGINFACTORY_H
#define MCCAPLUGINFACTORY_H

class CCCAppViewPluginBase;

/**
 * Factory for creating tab views from a single plugin.
 */
class MCcaPluginFactory
    {
    public:
        
        /**
         * Number of tab views available.
         * @return Tab view count.
         */
        virtual TInt TabViewCount() = 0;
        
        /**
         * Creates a tab view.
         * @param aIndex Index specifying which tab view to create.
         * @return The newly created tab view. Ownership is transferred.
         */
        virtual CCCAppViewPluginBase* CreateTabViewL( TInt aIndex ) = 0;
    
        /**
         * Extension point for this interface.
         * @param aExtensionUid UID that identifies the extension type.
         * @return The requested extension. If it is not available, NULL is returned.
         */
        virtual TAny* CcaPluginFactoryExtension( 
                TUid /*aExtensionUid*/ ) { return NULL; }
    
    public:
        virtual ~MCcaPluginFactory() {}
    };

#endif //MCCAPLUGINFACTORY_H
