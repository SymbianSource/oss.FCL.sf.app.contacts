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


#ifndef __TPbkFieldInfoExportTypeAdapter_H__
#define __TPbkFieldInfoExportTypeAdapter_H__

//  INCLUDES
#include "MPbkVcardProperty.h"
#include "MPbkVcardParameters.h"

// FORWARD DECLARATIONS
class CPbkFieldInfo;

// CLASS DECLARATION

/**
 * Maps a CPbkFieldInfo object's export vCard type to a MPbkVcardProperty.
 */
class TPbkFieldInfoExportTypeAdapter : 
        public MPbkVcardProperty,
        private MPbkVcardParameters
    {
    public:  // Constructors and destructor
        /**
         * Constructor.
		 * @param aFieldInfo reference to field information
         */
        IMPORT_C TPbkFieldInfoExportTypeAdapter
			(const CPbkFieldInfo& aFieldInfo);

        /**
         * Destructor.
         */
        IMPORT_C ~TPbkFieldInfoExportTypeAdapter();

     public:  // from MPbkVcardProperty
         IMPORT_C TUid PropertyName() const;
         IMPORT_C const MPbkVcardParameters& PropertyParameters() const;

    private: // from MPbkVcardParameters
        TInt ParameterCount() const;
        TUid ParameterAt(TInt aIndex) const;
        
    private:  // Data
		/// Ref: field information
        const CPbkFieldInfo& iFieldInfo;
    };


#endif // __TPbkFieldInfoExportTypeAdapter_H__

// End of File
