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
* Description:  Phonebook 2 SIM service table information.
*
*/


#ifndef TPSU2SERVICETABLE_H
#define TPSU2SERVICETABLE_H

// INCLUDE FILES
#include <e32std.h>

// CLASS DECLARATION

/**
 * Phonebook 2 SIM service table information.
 * Responsible for making service table request to ETel.
 */
class TPsu2ServiceTable
    {
    public: // Interface

        /**
         * Sets the service table.
         *
         * @param aTable    The new service table.
         */
        inline void SetServiceTable(
                TUint32 aTable );

        /**
         * Offers the service table.
         *
         * @return  A reference to the SIM or USIM service table.
         */
        inline const TUint32& ServiceTable() const;

        /**
         * Resets the service table.
         */
        inline void Reset();

        /**
         * Returns ETrue if all bits aFlags are on.
         *
         * @param aFlags    A set of flags (TPsu2ServiceTableFlag).
         * @return  ETrue if all the bits in aFlags are on.
         */
        inline TBool IsOn(
                TUint32 aFlags );

        /**
         * Returns ETrue if all bits in aFlags are off.
         *
         * @param aFlags    A set of flags (TPsu2ServiceTableFlag).
         * @return  ETrue if all bits in aFlags are off.
         */
        inline TBool IsOff(
                TUint32 aFlags );

    private: // Data
        /// Own: A set of flags (TPsu2ServiceTableFlag)
        TUint32 iTable;
    };

// INLINE FUNCTIONS

// --------------------------------------------------------------------------
// TPsu2ServiceTable::SetServiceTable
// --------------------------------------------------------------------------
//
inline void TPsu2ServiceTable::SetServiceTable( TUint32 aTable )
    {
    iTable = aTable;
    }

// --------------------------------------------------------------------------
// TPsu2ServiceTable::ServiceTable
// --------------------------------------------------------------------------
//
inline const TUint32& TPsu2ServiceTable::ServiceTable() const
    {
    return iTable;
    }

// --------------------------------------------------------------------------
// TPsu2ServiceTable::Reset
// --------------------------------------------------------------------------
//
inline void TPsu2ServiceTable::Reset()
    {
    iTable = 0;
    }

// --------------------------------------------------------------------------
// TPsu2ServiceTable::IsOn
// --------------------------------------------------------------------------
//
inline TBool TPsu2ServiceTable::IsOn( TUint32 aFlags )
    {
    // Return ETrue if all given flags in aFlags are on
    return (aFlags == (aFlags & iTable));
    }

// --------------------------------------------------------------------------
// TPsu2ServiceTable::IsOff
// --------------------------------------------------------------------------
//
inline TBool TPsu2ServiceTable::IsOff( TUint32 aFlags )
    {
    return (aFlags & iTable) == 0;
    }

#endif // TPSU2SERVICETABLE_H

// End of File
