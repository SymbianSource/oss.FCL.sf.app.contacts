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
* Description:  Phonebook 2 view state transformer.
*
*/

#ifndef CPBK2VIEWSTATETRANSFORMER_H
#define CPBK2VIEWSTATETRANSFORMER_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CVPbkContactManager;
class CPbk2ViewState;

// CLASS DECLARATION

/**
 * Phonebook 2 view state transformer.
 * Converts a legacy Phonebook view state to the new format.
 */
class CPbk2ViewStateTransformer : public CBase
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aContactManager   Virtual Phonebook contact manager.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2ViewStateTransformer* NewLC(
                CVPbkContactManager& aContactManager );

        /**
         * Destructor.
         */
        ~CPbk2ViewStateTransformer();

    public: // Interface

        /**
         * Converts legacy Phonebook view state to the new v2 format.
         *
         * @param aCustomMessage    Custom message containing the legacy
         *                          view state.
         * @return  Converted view state in Phonebook 2 format.
         */
        IMPORT_C CPbk2ViewState* TransformLegacyViewStateToPbk2ViewStateLC(
                const TDesC8& aCustomMessage );

    private: // Implementation
        CPbk2ViewStateTransformer(
                CVPbkContactManager& aContactManager );

    private: // Data
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iContactManager;
    };

#endif // CPBK2VIEWSTATETRANSFORMER_H

// End of File
