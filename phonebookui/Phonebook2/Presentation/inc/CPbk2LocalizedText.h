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
* Description:  Phonebook 2 Localized Text.
*
*/


#ifndef CPBK2LOCALIZEDTEXT_H
#define CPBK2LOCALIZEDTEXT_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class TResourceReader;

// CLASS DECLARATION

/**
 * CPbk2LocalizedText defines phonebook2 localized context sensitive text.
 * Localized text consist of context id and text. Context id defines the 
 * context, where text is used.
 *
 * Localized texts can be defined in resource files.
 *
 * @see PHONEBOOK2_LOCALIZED_TEXT
 */
NONSHARABLE_CLASS( CPbk2LocalizedText ): public CBase
    {
    public: // Construtor and destructor
    
        /**
         * Symbian constructor. Creates and initializes new 
         * CPbk2LocalizedText object. Reads information from resource
         * file. Ownership is transferred to caller.
         *
         * @param aReader Reference to resource reader containing 
         *                localized text information.
         */
        static CPbk2LocalizedText* NewL( TResourceReader& aReader );
        
        /**
         * Symbian constructor. Creates and initializes new 
         * CPbk2LocalizedText object with given parameters. 
         * Ownership is transferred to caller.
         *
         * @param aUid Localized text's context id.
         * @param aText Localized text. Ownership is transferred.
         */        
        static CPbk2LocalizedText* NewL( 
                TInt aUid, 
                HBufC* aText );
        
        /**
         * Destructor.
         */
        ~CPbk2LocalizedText();
        
    public: // Interface
        /**
         * Fetches context id. Context id defines the context where localized
         * text should be used.
         * 
         * @return Context id
         */
        TInt ContextUid() const;

        /**
         * Fetches the localized text.
         *
         * @return Text
         */
        const TDesC& Text() const;
        
    private: // Implementation
        CPbk2LocalizedText();
        CPbk2LocalizedText(
                TInt aUid, 
                HBufC* aText );
        void ConstructL( TResourceReader& aReader );
        
    private: // Data
        /// Own: Context id
        TInt iContextUid;   // WORD context
        /// Own: Localized text buffer
        HBufC* iText;       // LTEXT text
    };
    
#endif // CPBK2LOCALIZEDTEXT_H

// End of file
