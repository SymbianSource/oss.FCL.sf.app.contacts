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
* Description:  Phonebook 2 tab group container callback interface.
*
*/


#ifndef MPBK2TABGROUPCONTAINERCALLBACK_H
#define MPBK2TABGROUPCONTAINERCALLBACK_H

// INCLUDE FILES
#include <e32std.h>

// CLASS DECLARATION
class CPbk2ViewNode;
class CAknTabGroup;
class CPbk2IconInfoContainer;

/**
 * Phonebook 2 tab group container callback interface.
 * Callback is responsible for creating the view nodes tab
 * to the Avkon tab group and adding the tab icon infos to 
 * the icon info container.
 * The implementation must take into consideration the view count.
 */
class MPbk2TabGroupContainerCallback
    {
    public: // Interface

        /**
         * Creates the view node tab.
         *
         * @param aNode         View graph node.
         * @param aAknTabGroup  Avkon tab group.
         * @param aTabIcons     Tab icon info container.
         * @param aViewCount    Amount of views in the tab.
         */
        virtual void CreateViewNodeTabL(
                const CPbk2ViewNode& aNode, 
                CAknTabGroup& aAknTabGroup,
                CPbk2IconInfoContainer& aTabIcons,
                TInt aViewCount ) = 0;

    protected: // Disabled functions
        virtual ~MPbk2TabGroupContainerCallback()
                {}
    };

#endif // MPBK2TABGROUPCONTAINERCALLBACK_H

// End of File
