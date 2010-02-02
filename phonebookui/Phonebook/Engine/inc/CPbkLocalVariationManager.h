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
*     Observes shared data events.
*
*/


#ifndef __CPBKLOCALVARIATIONMANAGER_H__
#define __CPBKLOCALVARIATIONMANAGER_H__

//  INCLUDES
#include <e32base.h>
#include <PhonebookVariant.hrh>

// FORWARD DECLARATIONS
class CPbkContactEngine;
class RFs;
class MPbkGlobalSetting;

// CLASS DECLARATION

/**
 * Local variation manager.
 */
NONSHARABLE_CLASS(CPbkLocalVariationManager) :
        public CBase
    {
    public:
        /**
         * Creates a new instance of this class.
         */
		static CPbkLocalVariationManager* NewL();

        /**
         * Destructor.
         */
		~CPbkLocalVariationManager();

        /**
         * Used to check is the feature defined by aFeature on or off.
         * @param aFeature the feature to check
         * @return ETrue if the feature is on, EFalse otherwise
         */
        TBool LocallyVariatedFeatureEnabled(TPbkLocalVariantFlags aFeature);

	private: // implementation
		CPbkLocalVariationManager();
		void ConstructL();
        TInt GetLocalVariationFlags();

	private: // data
        /// Own: local variation setting
        MPbkGlobalSetting* iLocalVariationSetting;
        /// Own: local variation flags
        TInt iLocalVariationFlags;
    };

#endif // __CPBKLOCALVARIATIONMANAGER_H__
            
// End of File
