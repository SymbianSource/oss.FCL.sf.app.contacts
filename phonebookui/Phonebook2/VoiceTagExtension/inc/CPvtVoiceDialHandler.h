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
*       Phonebook 2 voice dial handler.
*
*/


#ifndef CPVTVOICEDIALHANDLER_H
#define CPVTVOICEDIALHANDLER_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CVoiceDialer;
class CVoiceTagContact;
class CVPbkContactIdConverter;
class MVPbkContactLink;
class MVPbkContactStore;

// CLASS DECLARATION

/**
 * Phonebook 2 voice dial handler.
 */
class CPvtVoiceDialHandler : public CBase
    {
    public: // Constructor and destructor

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        static CPvtVoiceDialHandler* NewL();

        /**
         * Destructor.
         */
        ~CPvtVoiceDialHandler();

    public: // Interface

        /**
         * Opens a voice tag dialog for plaing back a voice tag.
         *
         * @param aLink         Link to a contact whose voice tag
         *                      is to be played.
         * @param aFieldIndex   Index of the field of the voice tag.
         */
        void PlaybackVoiceTagL(
                const MVPbkContactLink& aLink,
                TInt aFieldIndex ) const;

        /**
         * Fills voice tag contact instance aVoiceTagContact
         * with voice tagged contacts.
         *
         * @param aVoiceTagContact  Voice tag contact.
         */
        void GetContactsWithVoiceTagL(
                CVoiceTagContact& aVoiceTagContact ) const;

        /**
         * Check if there are available voice tag positions.
         *
         * @return  ETrue if there are positions, EFalse otherwise.
         */
        TBool VoiceTagAvailable() const;

    private: // Implementation
        CPvtVoiceDialHandler();
        void ConstructL();

    private: // Data
        /// Own: Voice dialer
        CVoiceDialer* iVoiceDialer;
    };

#endif // CPVTVOICEDIALHANDLER_H

//  End of File
