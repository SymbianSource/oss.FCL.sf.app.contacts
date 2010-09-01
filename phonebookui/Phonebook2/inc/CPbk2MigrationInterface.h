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
* Description:  Plug-in interface for Phonebook 2 migration support.
*
*/


#ifndef CPBK2MIGRATIONINTERFACE_H
#define CPBK2MIGRATIONINTERFACE_H

// INCLUDES
#include <e32base.h>
#include <ecom/ecom.h>
#include <Pbk2InternalUID.h>

// FORWARD DECLARATIONS
class CPbk2ViewState;
class CVPbkContactManager;


// CLASS DECLARATION

/**
 * Plug-in interface for Phonebook 2 migration support.
 *
 * @internal This interface is internal to Phonebook2.
 */
class CPbk2MigrationInterface : public CBase
    {
    public: // Constructor and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aContactManager   Virtual Phonebook contact manager.
         * @return  A new instance of this class.
         */
        static CPbk2MigrationInterface* NewLC(
                CVPbkContactManager& aContactManager );

        /**
         * Destructor.
         */
        ~CPbk2MigrationInterface();

    public: // Interface

        /**
         * Converts the data in a CPbkViewState format descriptor
         * to a CPbk2ViewState object.
         *
         * @param aCustomMessage    Custom message containing a descriptor
         *                          in Phonebook 1 view state format.
         * @param aViewState        View state object in Phonebook 2 format.
         */
        virtual void ConvertViewStateL(
                const TDesC8& aCustomMessage,
                CPbk2ViewState& aViewState ) = 0;

    protected: // Implementation
        CPbk2MigrationInterface();

    private: // Data
        /// Own: Destructor id key
        TUid iDtor_ID_Key;
    };

// INLINE FUNCTIONS

// --------------------------------------------------------------------------
// CPbk2MigrationInterface::CPbk2MigrationInterface
// --------------------------------------------------------------------------
//
inline CPbk2MigrationInterface::CPbk2MigrationInterface()
    {
    }

// --------------------------------------------------------------------------
// CPbk2MigrationInterface::~CPbk2MigrationInterface
// --------------------------------------------------------------------------
//
inline CPbk2MigrationInterface::~CPbk2MigrationInterface()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }

// --------------------------------------------------------------------------
// CPbk2MigrationInterface::NewLC
// --------------------------------------------------------------------------
//
inline CPbk2MigrationInterface* CPbk2MigrationInterface::NewLC(
        CVPbkContactManager& aContactManager )
    {
    TAny* ptr = REComSession::CreateImplementationL(
            TUid::Uid( KPbk2MigrationSupportImplementationUID ),
            _FOFF( CPbk2MigrationInterface, iDtor_ID_Key ),
            static_cast<TAny*>( &aContactManager ) );

    CPbk2MigrationInterface* impl =
            static_cast<CPbk2MigrationInterface*>( ptr );
    CleanupStack::PushL( impl );
    return impl;
    }

#endif // CPBK2MIGRATIONINTERFACE_H

// End of File
