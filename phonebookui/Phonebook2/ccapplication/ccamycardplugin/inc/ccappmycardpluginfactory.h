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
* Description:  Implementation of MyCard plugin factory
*
*/

#ifndef CCAPPMYCARDPLUGINFACTORY_H
#define CCAPPMYCARDPLUGINFACTORY_H

#include <e32def.h>
#include <e32base.h>
#include <mccapluginfactory.h>

/**
 *  MyCard class implementing MCcaPluginFactory interface.
 *
 *  @lib ccappmycardplugin.dll
 *  @since S60 9.2
 */
class CCCAppMycardPluginFactory :     
    public CBase, 
    public MCcaPluginFactory
    {
public:

    /**
     * Create interface
     */
    static MCcaPluginFactory* NewL();

    /**
     * Destructor
     */
    ~CCCAppMycardPluginFactory();

public: // From MCcaPluginFactory

    /**
     * From MCcaPluginFactory
     */
    TInt TabViewCount();

    /**
     * From MCcaPluginFactory
     */
    CCCAppViewPluginBase* CreateTabViewL( TInt aIndex );
    
protected:
    
    /**
     * Construction
     */
    CCCAppMycardPluginFactory();
    };

    
#endif // CCAPPMYCARDPLUGINFACTORY_H
