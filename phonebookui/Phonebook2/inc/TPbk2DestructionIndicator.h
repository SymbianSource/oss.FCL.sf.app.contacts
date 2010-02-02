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
* Description:  Phonebook 2 smart destruction indicator.
*
*/


#ifndef TPBK2DESTRUCTIONINDICATOR_H
#define TPBK2DESTRUCTIONINDICATOR_H

// CLASS DECLARATION

/**
 * Phonebook 2 smart destruction indicator.
 * Responsible for reseting the destruction indicator pointer in case
 * the destruction does not happen. This class is meant to be used
 * in classes that launch waiting dialogs. If application is killed
 * during the dialog the dialog code will return and run the code of the
 * deleted object. In that case the deleted object has to perform some checks
 * to find out if it is deleted. This class is used in that process to
 * help to perform that task safely.
 */
class TPbk2DestructionIndicator
    {
    public: // Construction and destruction

        /**
         * Constructor.
         *
         * @param aDestructionIndicator     Pointer to a destruction
         *                                  indicator.
         * @param aDestroyedPtr             Reference to a pointer pointing
         *                                  to given destruction indicator.
         */
        TPbk2DestructionIndicator(
                TBool* aDestructionIndicator,
                TBool*& aDestroyedPtr );

        /**
         * Destructor.
         */
        ~TPbk2DestructionIndicator();

    private: // Data
        /// Ref: Destruction indicator
        TBool* iDestructionIndicator;
        /// Ref: Points to the destruction indicator
        TBool*& iDestroyedPtr;
    };

// INLINE IMPLEMENTATION

// --------------------------------------------------------------------------
// TPbk2DestructionIndicator::TPbk2DestructionIndicator
// --------------------------------------------------------------------------
//
inline TPbk2DestructionIndicator::TPbk2DestructionIndicator
        ( TBool* aDestructionIndicator, TBool*& aDestroyedPtr ) :
            iDestructionIndicator( aDestructionIndicator ),
            iDestroyedPtr( aDestroyedPtr )
    {
    }

// --------------------------------------------------------------------------
// TPbk2DestructionIndicator::~TPbk2DestructionIndicator
// --------------------------------------------------------------------------
//
inline TPbk2DestructionIndicator::~TPbk2DestructionIndicator()
    {
    if ( !*iDestructionIndicator )
        {
        // Reset the pointer to destruction indicator if destruction
        // has not happened (iDestructionIndicator == EFalse)
        iDestroyedPtr = NULL;
        }
    }

#endif // TPBK2DESTRUCTIONINDICATOR_H

// End of File
