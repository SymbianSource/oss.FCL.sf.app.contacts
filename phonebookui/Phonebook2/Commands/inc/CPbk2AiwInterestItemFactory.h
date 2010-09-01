/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 AIW interest item factory.
*
*/


#ifndef CPBK2AIWINTERESTITEMFACTORY_H
#define CPBK2AIWINTERESTITEMFACTORY_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CAiwServiceHandler;
class CPbk2UIExtensionManager;
class MPbk2AiwInterestItem;

// CLASS DECLARATION

/**
 * Phonebook 2 AIW interest item factory.
 * Responsible for creating AIW interest items.
 */
NONSHARABLE_CLASS(CPbk2AiwInterestItemFactory) : public CBase
    {
    public: // Constructor and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aServiceHandler   AIW Service Handler.
         * @return  A new instance of this class.
         */
        static CPbk2AiwInterestItemFactory* NewL(
                CAiwServiceHandler& aServiceHandler );

        /**
         * Destructor.
         */
        ~CPbk2AiwInterestItemFactory();

    public: // Interface

        /**
         * Creates an AIW interest item.
         *
         * @param aInterestId       Interest id.
         * @return  AIW interest item.
         */
        MPbk2AiwInterestItem* CreateInterestItemL(
                TInt aInterestId );

    private: // Implementation
        CPbk2AiwInterestItemFactory(
                CAiwServiceHandler& aServiceHandler );
        void ConstructL();

    private: // Data
        /// Ref: AIW service handler
        CAiwServiceHandler& iServiceHandler;
        /// Own: UI extension manager
        CPbk2UIExtensionManager* iExtensionManager;
    };

#endif // CPBK2AIWINTERESTITEMFACTORY_H

// End of File
