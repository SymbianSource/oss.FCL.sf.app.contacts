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
*    Abstract interface for creating logs extensions.
*
*/


#ifndef MLogsExtensionFactory_H
#define MLogsExtensionFactory_H


//  INCLUDES
#include <e32std.h>  // for TUid

// CONSTANTS
_LIT( KLogsUiExtensionDllDrive, "Z:" );

_LIT( KLogsUiExtensionDllFile, "logsserviceextension.dll" );       

//  FORWARD DECLARATIONS
class MLogsViewExtension;
class MLogsUiControlExtension;
class MLogsExtObserver;
class CPbkContactEngine;


// CLASS DECLARATION

/**
 * @internal This interface is internal to Logs.
 *
 * Abstract interface for creating logs extensions. Instance of 
 * this factory should be kept as long as extensions are used, if resources
 * are needed.
 */
class MLogsExtensionFactory
    {
    public:  // Destructor

    /**
     * Cleans up the object owned through this interface.
     * 
     * @since S60 v3.2     
     */
    virtual void Release() = 0; 

    public: // Interface

        /**
         * Creates a logs view extension.
         *
         * @param aId   id of the logs view to create the extension
         *              for.
         * @param aEngine pbk engine to be provided to extensions.
         * @param aObserver observer
         * @return Logs view extension object.
         */
        virtual MLogsViewExtension* CreateLogsViewExtensionL
                ( CPbkContactEngine& aEngine, 
                MLogsExtObserver* aObserver ) = 0;
        

        /**
         * Creates a Logs UI control extension.
         * @return Logs control extension object.
         */
        virtual MLogsUiControlExtension* CreateLogsUiControlExtensionL() = 0;
    };


#endif
            

// End of File
