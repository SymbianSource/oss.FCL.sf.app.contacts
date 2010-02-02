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
* Description:  Migration support for Phonebook 2.
*
*/


#ifndef CPBK2MIGRATIONSUPPORT_H
#define CPBK2MIGRATIONSUPPORT_H

// INCLUDES
#include <CPbk2MigrationInterface.h>

// FORWARD DECLARATIONS
class CVPbkContactManager;
class CVPbkContactIdConverter;

// CLASS DECLARATION

/**
 * Migration support for Phonebook 2.
 * Responsible for converting Phonebook 1 view state to
 * Phonebook 2 view state.
 */
class CPbk2MigrationSupport : public CPbk2MigrationInterface
    {
    public: // Constructor and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aParam    ECom construction parameter of type
         *                  CVPbkContactManager.
         * @return  A new instance of this class.
         */
        static CPbk2MigrationSupport* NewL(
                TAny* aParam );

        /**
         * Destructor.
         */
        ~CPbk2MigrationSupport();

    public: // From CPbk2MigrationInterface
        void ConvertViewStateL(
                const TDesC8& aCustomMessage,
                CPbk2ViewState& aViewState );

    private: // Implementation
        CPbk2MigrationSupport(
                CVPbkContactManager* aContactManager );
        void ConstructL();

    private: // Data
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iContactManager;
        /// Own: Contact id converter
        CVPbkContactIdConverter* iIdConverter;
    };

#endif // CPBK2MIGRATIONSUPPORT_H

// End of File
