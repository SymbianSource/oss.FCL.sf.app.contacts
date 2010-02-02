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
*      Phonebook field matching priority level.
*
*/


#ifndef __TPbkMatchPriorityLevel_H__
#define __TPbkMatchPriorityLevel_H__

// INCLUDES
#include <e32def.h>

// CLASS DECLARATION

/**
 * Phonebook field matching priority level.
 */
class TPbkMatchPriorityLevel
    {
    private: // Constructors
        /// Only class CPbkFieldsInfo can create instances of this class
        friend class CPbkFieldsInfo;

        /**
         * Constructor.
         * @param aMaxNumberOfLevels maximum number of matching levels
         */
        TPbkMatchPriorityLevel(TInt aMaxNumberOfLevels);

        // Default copy constructor, assignment and destructor are ok for
        // this class.

    public:  // New functions
        /**
         * Advances current priority to the next level.
         */
        void Next();

        /**
         * Returns ETrue if all priority levels have been enumerated with 
         * Next().
		 * @return ETrue if all levels have been enumerated
         */
        TBool End() const;

        /**
         * Returns the current matching priority level.
		 * @return current level
         */
        TInt CurrentLevel() const;

    private:  // Data
		/// Own: current level
        TInt iCurrentLevel;
		/// Own: maximum level
        TInt iMaxLevel;
    };


// INLINE FUNCTIONS
inline TPbkMatchPriorityLevel::TPbkMatchPriorityLevel(TInt aMaxNumberOfLevels) :
    iCurrentLevel(0), iMaxLevel(aMaxNumberOfLevels)
    {
    }

inline TBool TPbkMatchPriorityLevel::End() const
    {
    return (iCurrentLevel > iMaxLevel);
    }

inline void TPbkMatchPriorityLevel::Next()
    {
    ++iCurrentLevel;
    }

inline TInt TPbkMatchPriorityLevel::CurrentLevel() const
    {
    return iCurrentLevel;
    }


#endif // __TPbkMatchPriorityLevel_H__

// End of File
