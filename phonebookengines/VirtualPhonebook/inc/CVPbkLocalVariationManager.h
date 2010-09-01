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
* Description:  Virtual Phonebook Local variation manager.
*
*/


#ifndef CVPBKLOCALVARIATIONMANAGER_H
#define CVPBKLOCALVARIATIONMANAGER_H

// INCLUDE FILES
#include <e32base.h>
#include <VPbkVariant.hrh>

// FORWARD DECLARATIONS
class CRepository;


// CLASS DECLARATION

/**
 * Virtual Phonebook Local variation manager.
 */
class CVPbkLocalVariationManager :
        public CBase
    {
    public:
        /**
         * Creates a new instance of this class.
         */
        IMPORT_C static CVPbkLocalVariationManager* NewL();

        /**
         * Destructor.
         */
		~CVPbkLocalVariationManager();

        /**
         * Used to check is the feature defined by aFeature on or off.
         * @param aFeature the feature to check
         * @return ETrue if the feature is on, EFalse otherwise
         */
		IMPORT_C TBool LocallyVariatedFeatureEnabled(TVPbkLocalVariantFlags aFeature);

	private: // implementation
		CVPbkLocalVariationManager();
		void ConstructL();
        TInt DoGetLocalVariationFlags();

	private: // data
        // Own: Central repository
        CRepository* iRepository;
        /// Own: local variation flags
        TInt iLocalVariationFlags;

    };

#endif // CVPBKLOCALVARIATIONMANAGER_H
            
// End of File
