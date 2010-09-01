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
*     A string array class holding only one string
*
*/


#ifndef __TSingleDesCArray_H__
#define __TSingleDesCArray_H__

// INCLUDES
#include <bamdesca.h>   // MDesCArray

// CLASS DECLARATION

/**
 * A string array class holding only one string.
 */ 
class TSingleDesCArray : public MDesCArray
    {
    public:
        /**
         * Default constructor.
         * @postcond MdcaCount()==0
         */
        inline TSingleDesCArray();

        /**
         * Constructor.
         * @param aString reference to the one and only string this array 
         *        contains
         * @postcond MdcaCount()==1 && MdcaPoint(0)==aString
         */
        inline TSingleDesCArray(const TDesC& aString);

        /**
         * Sets the one and only string this array contains to aString.
         * @postcond MdcaCount()==1 && MdcaPoint(0)==aString
         */
        inline void SetDesC(const TDesC& aString);

        /**
         * Makes this array empty.
         * @postcond MdcaCount()==0
         */
        inline void Reset();

    public:  // from MDesCArray
        TInt MdcaCount() const;
        TPtrC MdcaPoint(TInt aIndex) const;

    private:  // Implementation
        enum TPanicCode
            {
            EPanicPostCond_Constructor,
            EPanicPostCond_Constructor_TDesC,
            EPanicPostCond_SetDesC,
            EPanicPostCond_Reset,
            EPanicPreCond_MdcaPoint
            };
        static void Panic(TInt aReason);
        
    private:  // Data
		/// Ref: the string which this array contains
        const TDesC* iString;
    };


inline TSingleDesCArray::TSingleDesCArray()
    : iString(NULL)
    {
    __ASSERT_DEBUG(MdcaCount()==0, Panic(EPanicPostCond_Constructor));
    }

inline TSingleDesCArray::TSingleDesCArray(const TDesC& aString)
    : iString(&aString)
    {
    __ASSERT_DEBUG(MdcaCount()==1 && MdcaPoint(0)==aString, 
        Panic(EPanicPostCond_Constructor_TDesC));
    }

inline void TSingleDesCArray::SetDesC(const TDesC& aString)
    {
    iString = &aString;
    __ASSERT_DEBUG(MdcaCount()==1 && MdcaPoint(0)==aString, 
        Panic(EPanicPostCond_SetDesC));
    }

inline void TSingleDesCArray::Reset()
    {
    iString = NULL;
    __ASSERT_DEBUG(MdcaCount()==0, Panic(EPanicPostCond_Reset));
    }

#endif // __TSingleDesCArray_H__
            
// End of File
