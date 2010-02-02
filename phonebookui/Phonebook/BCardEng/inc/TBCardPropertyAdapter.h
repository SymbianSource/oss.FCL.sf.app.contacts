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
*    CPbkFieldInfo export type <-> MPbkVcardProperty adapter.
*
*/


#ifndef __TBCARDPROPERTYADAPTER_H__
#define __TBCARDPROPERTYADAPTER_H__

//  INCLUDES
#include <e32def.h> // Basic Symbian OS types
#include "MPbkVcardProperty.h"
#include "MPbkVcardParameters.h"


// FORWARD DECLARATIONS
class CParserProperty;
class CBCardFieldUidMapping;
class CBCardParserProperty;


// CLASS DECLARATION

/**
 * Maps a CParserProperty to a MPbkVcardProperty.
 */
NONSHARABLE_CLASS(TBCardPropertyAdapter) : 
        public MPbkVcardProperty,
        private MPbkVcardParameters
    {
    public:  // Constructors and destructor
        /**
         * Constructor.
		 * @param aProperty the property
		 * @param aFieldIndex index of the field
		 * @param aFieldUidMapping UID mapping of the field
         */
        TBCardPropertyAdapter
            (const CParserProperty& aProperty, 
            TInt aFieldIndex,
            const CBCardFieldUidMapping& aFieldUidMapping);

        /**
         * Destructor.
         */
        ~TBCardPropertyAdapter();

     public:  // from MPbkVcardProperty
         TUid PropertyName() const;
         const MPbkVcardParameters& PropertyParameters() const;

    private: // from MPbkVcardParameters
        TInt ParameterCount() const;
        TUid ParameterAt(TInt aIndex) const;
        
    private:  // Data
		/// Ref: the property
        const CBCardParserProperty& iProperty;
		/// Own: index of the field
        const TInt iFieldIndex;
		/// Ref: UID mapping of the field
        const CBCardFieldUidMapping& iFieldUidMapping;
    };


#endif // __TBCARDPROPERTYADAPTER_H__

// End of File
