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
* Description:  Base interface for Phonebook 2 UI releasable objects.
*
*/


#ifndef MPBK2UIRELEASABLE_H
#define MPBK2UIRELEASABLE_H

// INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
 * Base interface for Phonebook 2 UI releasable objects.
 */
class MPbk2UiReleasable
    {
    public: // Interface

        /**
         * Pushes this object to the cleanup stack.
         */
        void PushL();

    protected: // Protected interface

        /**
         * Destructor.
         */
        virtual ~MPbk2UiReleasable()
                {};

    private: // Implementation (provided)
        friend void Release(
                MPbk2UiReleasable* aObj );
        static void CleanupRelease(
                TAny* aObj );

    private: // Implementation (derived class must implement)

        /**
         * Implement to release this object and any resources it owns.
         */
        virtual void DoRelease() =0;

        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* UiReleasableExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }
    };


// INLINE IMPLEMENTATION

// --------------------------------------------------------------------------
// MPbk2UiReleasable::PushL
// --------------------------------------------------------------------------
//
inline void MPbk2UiReleasable::PushL()
    {
    CleanupStack::PushL( TCleanupItem( CleanupRelease, this ) );
    }

// --------------------------------------------------------------------------
// MPbk2UiReleasable::Release
// --------------------------------------------------------------------------
//
inline void Release( MPbk2UiReleasable* aObj )
    {
    if ( aObj )
        {
        aObj->DoRelease();
        }
    }

// --------------------------------------------------------------------------
// MPbk2UiReleasable::CleanupRelease
// --------------------------------------------------------------------------
//
inline void MPbk2UiReleasable::CleanupRelease( TAny* aObj )
    {
    Release( static_cast<MPbk2UiReleasable*>( aObj ) );
    }

#endif // MPBK2UIRELEASABLE_H

// End of File
