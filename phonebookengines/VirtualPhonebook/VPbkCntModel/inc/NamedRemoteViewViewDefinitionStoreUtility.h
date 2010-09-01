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
* Description:  The virtual phonebook named remote view utility.
*
*/


#ifndef NAMEDREMOTEVIEWVIEWDEFINITIONSTOREUTILITY_H
#define NAMEDREMOTEVIEWVIEWDEFINITIONSTOREUTILITY_H

// INCLUDES
#include <e32base.h>
#include <cntviewbase.h>

namespace VPbkCntModel {

// FORWARD DECLARATIONS
class CContactStore;


// CLASS DECLARATION

/**
 * Named remote view view definition utility.
 */
NONSHARABLE_CLASS( NamedRemoteViewViewDefinitionStoreUtility )
    {
    public: // interface
        /**
         * Configures the remote view definitions.
         * @param aViewName Remote view name.
         * @param aSortOrder Sort order.
         * @param aContactTypes view preferences.
         */
        static void SetNamedRemoteViewViewDefinitionL(
            const TDesC& aViewName, 
            const RContactViewSortOrder& aSortOrder, 
            TContactViewPreferences aContactTypes);

    private: // implementation
        static void DoSetNamedRemoteViewViewDefinitionL(
            const TDesC& aViewName, 
            const RContactViewSortOrder& aSortOrder, 
            TContactViewPreferences aContactTypes);

    };

} // namespace VPbkCntModel

#endif // NAMEDREMOTEVIEWVIEWDEFINITIONSTOREUTILITY_H

// End of File
