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
* Description:  Phonebook 2 Tab group container extension modifier.
*
*/


#ifndef CPBK2TABGROUPCONTAINEREXTENSIONMODIFIER_H
#define CPBK2TABGROUPCONTAINEREXTENSIONMODIFIER_H

//  INCLUDES
#include <e32base.h>
#include <MPbk2TabGroupContainerCallback.h>

// FORWARD DECLARATIONS
class MPbk2AppUiExtension;

// CLASS DECLARATION

/**
 * Phonebook 2 Tab group container extension modifier.
 */
class CPbk2TabGroupContainerExtensionModifier : 
        public CBase,
        public MPbk2TabGroupContainerCallback
    {
    public: // Constructors and destructor
        /**
         * Creates a new instance of this class.
         *
         * @param aAppUiExtension   App UI Extension inteface.
         */
        static CPbk2TabGroupContainerExtensionModifier* NewLC(
                MPbk2AppUiExtension& aAppUiExtension);

        /**
         * Destructor.
         */
        ~CPbk2TabGroupContainerExtensionModifier();

    private: // From MPbk2TabGroupContainerCallback
         void CreateViewNodeTabL(
                const CPbk2ViewNode& aNode, 
                CAknTabGroup& aAknTabGroup,
                CPbk2IconInfoContainer& aTabIcons,
                TInt aViewCount );

    private: // Implementation
        CPbk2TabGroupContainerExtensionModifier(
                MPbk2AppUiExtension& aAppUiExtension);
        void ConstructL();

    private: // Data
        ///Ref: Application UI Extension
        MPbk2AppUiExtension& iAppUiExtension;

    };

#endif // CPBK2TABGROUPCONTAINEREXTENSIONMODIFIER_H

// End of File
