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
*       Dummy extension factory. Provides default implementations.
*
*/


#ifndef TLogsDummyExtensionFactory_H
#define TLogsDummyExtensionFactory_H


//  INCLUDES
#include "MLogsExtensionFactory.h"
#include "TLogsDummyViewExtension.h"
#include "TLogsDummyUiControlExtension.h"


// CLASS DECLARATION

/**
 * Dummy extension factory. Provides default implementations.
 */
class TLogsDummyExtensionFactory :  public MLogsExtensionFactory                                    
    {
    public: // Constructor
        /**
         * Creates a new TPLogsDummyExtensionFactory.
         */
        static TLogsDummyExtensionFactory* NewL();

        void Release();
        
    public: // From MLogsExtensionFactory
        MLogsViewExtension* CreateLogsViewExtensionL
                (CPbkContactEngine& aEngine, MLogsExtObserver* aObserver);
        MLogsUiControlExtension* CreateLogsUiControlExtensionL();

    private:
        TLogsDummyExtensionFactory();

    private: // Data
        /// Own: dummy view extension
        TLogsDummyViewExtension iDummyViewExtension;
        /// Own: dummy app ui extension
        TLogsDummyUiControlExtension iDummyUiControlExtension;    
    };

#endif
            

// End of File
