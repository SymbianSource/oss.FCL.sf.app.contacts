/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements MPbk2ExtensionIconSupport API
*
*/



#ifndef CPBK2UIEXTENSIONICONSUPPORT_H
#define CPBK2UIEXTENSIONICONSUPPORT_H

//  INCLUDES
#include <e32base.h>
#include <MPbk2UIExtensionIconSupport.h>

// FORWARD DECLARATIONS
class CPbk2UIExtensionLoader;

// CLASS DECLARATION

/**
 * UI Extension icon support implementation. 
 */
NONSHARABLE_CLASS(CPbk2UIExtensionIconSupport) : 
        public CBase,
        public MPbk2UIExtensionIconSupport
    {
    public:  // Constructors and destructor
        /**
         * C++ default constructor.
         * @param aExtensionLoader  The extension loader for getting
         *        extension information.
         */
        CPbk2UIExtensionIconSupport(CPbk2UIExtensionLoader& aExtensionLoader);

        /**
         * Destructor.
         */
        ~CPbk2UIExtensionIconSupport();

    public: // from MPbk2UIExtensionIconSupport
        void AppendExtensionIconsL(CPbk2IconArray& aIconArray);
        void RefreshL(CPbk2IconArray& aIconArray);
        
    private:    // Data
        /// Ref: Phonebook2 UI Extension Loader
        CPbk2UIExtensionLoader& iExtensionLoader;

    };

#endif      // CPBK2UIEXTENSIONICONSUPPORT_H
            
// End of File
