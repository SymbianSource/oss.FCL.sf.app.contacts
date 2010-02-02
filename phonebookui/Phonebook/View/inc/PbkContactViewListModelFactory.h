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
*      MPbkContactViewListModel implementation factory for PbkView.dll.
*
*/


#ifndef __PbkContactViewListModelFactory_H__
#define __PbkContactViewListModelFactory_H__

//  INCLUDES
#include "MPbkContactViewListModel.h"

// FORWARD DECLARATIONS
class MPbkContactViewListModel;
class CPbkContactEngine;
class CPbkIconArray;
class CContactViewBase;

// CLASS DECLARATION

/**
 * MPbkContactViewListModel implementation factory.
 */
NONSHARABLE_CLASS(PbkContactViewListModelFactory)
    {
    public:  // Types
        /**
         * Model initialization paramters class. 
         */
        class TParams
            {
            public:  // Input parameters
                /**
                 * MANDATORY: Contact engine.
                 */
                CPbkContactEngine* iEngine;

                /**
                 * MANDATORY: Contact view.
                 */
                CContactViewBase* iView;

                /**
                 * MANDATORY: cache size.
                 */
                TInt iCacheSize;
                
                /**
                 * MANDATORY: icon array to use.
                 */
                CPbkIconArray* iIconArray;

                /**
                 * OPTIONAL: id of the empty icon in iIconArray.
                 * EPbkNullIconId if not applicable. 
                 */
                TPbkIconId iEmptyId;

                /**
                 * OPTIONAL: id of the default icon to use for all list.
                 * EPbkNullIconId to disable default setting. 
                 */
                TPbkIconId iDefaultId;

            public:  // Constructors
                /**
                 * C++ Constructor.
                 */
                TParams();

            };
    public:  // Interface
        /**
         * Creates and returns a MPbkContactViewListModel implementation.
         * @param aParams @see TParams
         */
        static MPbkContactViewListModel* CreateL(TParams& aParams);

    private:  // Disabled functions
        PbkContactViewListModelFactory();
        PbkContactViewListModelFactory(const PbkContactViewListModelFactory&);
    };

#endif // __PbkContactViewListModelFactory_H__

// End of File
