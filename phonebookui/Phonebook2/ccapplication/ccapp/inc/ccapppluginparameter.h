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
* Description:  Parameter class between CCApp and actual plugins
*
*/


#ifndef C_CCAPPPLUGINPARAMETER_H
#define C_CCAPPPLUGINPARAMETER_H

#include "ccappheaders.h"

class CCCAppView;

/**
 *  This class is the concrete implementation of 
 *  MCCAppPluginParameter-parameter class.
 * 
 *  If specific parameter is not supported by certain version
 *  of this class, NULL is returned.
 * 
 *  @code
 *   ?good_class_usage_example(s) todo!
 *  @endcode
 *
 *  @lib CCA application
 *  @since S60 v5.0
 */
class CCCAppPluginParameter: 
	public CBase, 
	public MCCAppPluginParameter
    {

public:

    /**
     * Two-phased constructor.
     */
    static CCCAppPluginParameter* NewL( CCCAppView& aAppView );

    /**
     * Destructor.
     */
    virtual ~CCCAppPluginParameter();

public:

    // Interface methods
    
    /**
     * From MCCAppPluginParameter
     */
    TInt Version();

    /**
     * From MCCAppPluginParameter
     */
    MCCAParameter& CCAppLaunchParameter();    
    
    /**
     * From MCCAppPluginParameter
     */
    const RArray<TInt>& ReservedKeys();
    
private:

    /**
     * Constructor.
     */
    CCCAppPluginParameter( CCCAppView& aAppView );

private: // data

    /**
     * Version information
     */
    TInt iVersion;

    /**
     * Reference to AppView
     */
    CCCAppView& iAppView;

    };


#endif // C_CCAPPPLUGINPARAMETER_H
