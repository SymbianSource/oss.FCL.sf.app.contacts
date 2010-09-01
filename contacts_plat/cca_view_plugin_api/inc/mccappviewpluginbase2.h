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
* Description:  Extension for plugin view base class of ECE's CCA-application
*
*/

#ifndef __MCCAPPVIEWPLUGINBASE2_H__
#define __MCCAPPVIEWPLUGINBASE2_H__

// CLASS DECLARATION
/**
 * This class is an extension to CCCAppViewPluginBase.
 * See documentation of CCCAppViewPluginBase from header
 * ccappviewpluginbase.h
 * 
 * You can access this extension by calling
 * CCCAppViewPluginBase->CCCAppViewPluginBaseExtension
 */
class MCCAppViewPluginBase2
    {
    public: // Interface

        /**
         * @return ETrue if plugin is busy with some service request processing
         *         EFalse otherwise
         */
        virtual TBool PluginBusy() = 0;
    
    protected: // Protected destructor
        ~MCCAppViewPluginBase2()
            {}
    };

#endif // MCCAPPVIEWPLUGINBASE2_H

// End of File
