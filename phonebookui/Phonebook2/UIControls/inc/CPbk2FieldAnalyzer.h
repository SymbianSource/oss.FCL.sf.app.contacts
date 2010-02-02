/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*       Provides methods for Phonebook 2 field analyzer.
*
*/


#ifndef CPBK2FIELDANALYZER_H
#define CPBK2FIELDANALYZER_H

// INCLUDES
#include <e32def.h>
#include "MPbk2FieldAnalyzer.h"

// FORWARD DECLARATIONS
class CVPbkContactManager;
class MVPbkStoreContactFieldCollection;

// CLASS DECLARATION

/**
 * Phonebook 2 field analyzer.
 */
NONSHARABLE_CLASS(CPbk2FieldAnalyzer): public MPbk2FieldAnalyzer
    {
    public: // Constructors and destructor
        /**
         * Creates a new instance of this class.
         *
         * @param aContactManager, Contact manager
         * @return  A new instance of this class.
         */
        static CPbk2FieldAnalyzer* NewL( CVPbkContactManager& aContactManager );

        /**
         * Destructor.
         */
        ~CPbk2FieldAnalyzer();
        
    public: // From MPbk2FieldAnalyzer
        TBool HasSpeedDialL(
                const MVPbkStoreContactField& aField ) const;
        TBool HasVoiceTagL(
                const MVPbkStoreContactField& aField ) const;
        TBool IsHiddenField(
                const CPbk2PresentationContactField& aField ) const;
        const TInt VoipSupportL( 
                const MVPbkBaseContactFieldCollection& aFields ) const;
        TBool IsFieldVoipCapable( 
                TInt aVpbkFieldResId, const TInt aVoipFlag ) const;
        
    public: // interface
        /**
         * Sets the fields which have a voice tag to the analyzer.
         * @param aFields   Voice tagged fields.
         */ 
        void SetVoiceTagFields(MVPbkStoreContactFieldCollection* aFields);
            
    private:
        /**
         * Constructor.
         */
        CPbk2FieldAnalyzer( CVPbkContactManager& aContactManager );

    private:
        /// Ref: Contact manager
        CVPbkContactManager& iContactManager;
        /// Own: voice tagged fields
        MVPbkStoreContactFieldCollection* iVoiceTaggedFields;
    };

#endif // CPBK2FIELDANALYZER_H

// End of File
