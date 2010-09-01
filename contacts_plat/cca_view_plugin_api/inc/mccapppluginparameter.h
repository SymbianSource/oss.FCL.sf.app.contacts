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


#ifndef M_MCCAPPPLUGINPARAMETER_H
#define M_MCCAPPPLUGINPARAMETER_H

class MCCAParameter;

/**
 *  This class defines the API parameters between the CCA-application
 *  and the plugin responsible showing the view.
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
class MCCAppPluginParameter
    {
public:
    
    /**
     * Getter of parameter version information. Plugin should
     * use this to identify the parameters included in this
     * class.
     *
     * Current version is 1
     *
     * @return version of the plugin
     */
    virtual TInt Version() = 0;
    
    /**
     * Getter for CCApplication launching parameter
     * containing for example getters for contact link.
     * Ownership is not transferred
     *
     * @since S60 v5.0     
     * @return CCApp launch parameter
     */
    virtual MCCAParameter& CCAppLaunchParameter() = 0;
    
    /**
     * Getter for keys reserved for CCApplication usage.
     * Ownership is not transferred
     *
     * @since S60 v5.0     
     * @return array of reserved keys
     */
    virtual const RArray<TInt>& ReservedKeys() = 0;

    /**
     * Returns an extension point for this interface or NULL.
     * @param aExtensionUid Uid of extension
     * @return Extension point or NULL
     */
    virtual TAny* CCAppPluginParameterExtension( 
            TUid /*aExtensionUid*/ ) { return NULL; }
    };


#endif // M_MCCAPPPLUGINPARAMETER_H
