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
*     Implementation of MLogsEventArray
*
*/


#ifndef __CLogsEventArray_H_
#define __CLogsEventArray_H_

//  INCLUDES
#include <e32base.h>
#include "MLogsEventArray.h"
#include "MLogsModel.h"

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MLogsEvent;
// CLASS DECLARATION

 /**
  * Concrete MlogsEventArray of TEventType.
  *
  * @param TEventType    concrete event type. Must be derived from MLogsEvent.
  */
template<class TEventType>
class CLogsEventArray : public CBase, public MLogsEventArray
    {
    public:  // Constructors and destructor
        
		/**
         * EPOC constructor. 
         *
         * @param aGranularity Initial granularity of the event array
         * @return model object
         */
		static CLogsEventArray<TEventType>* NewL( TInt aGranularity );
        
        /**
         *  Destructor
         */
		~CLogsEventArray();

    private:
        /**
         * C++ constructor. 
         *
         * @param aGranularity Initial granularity of the event array
         */
        CLogsEventArray( TInt aGranularity );

    public:  // New functions
		/**
		 * Append a new event of TEventType to event array  
		 *
		 * @param aEvent  event to be added to this array.
		 */
        ///// void AppendL(const TEventType& aEvent);   //FIXME: remove this 

    public:  // from MLogsEventArray
        TInt Count() const ;

        const MLogsEvent& At(TInt aIndex) const;

        MLogsEvent& At(TInt aIndex);

        /**
         * @param aEvent    event to add to this array. The concrete event type 
         *                  must always be same as TEventType.
         */
        void AppendL( const MLogsEvent& aEvent );

        void Delete( TInt aIndex );

        void Reset();

        void Compress();
      
    private:  // Data

        /// Own: Event array
		//CArrayFixSeg<TEventType> iArray;  
        CArrayPtrSeg<const MLogsEvent> iArray;  //Fix for EKKJ-6EGAKC. Changed to CArrayPtrSeg  
    };

template<class TEventType>
CLogsEventArray<TEventType>::CLogsEventArray( TInt aGranularity ) :
	iArray( aGranularity )
    {
    }

template<class TEventType>
CLogsEventArray<TEventType>* CLogsEventArray<TEventType>::NewL( TInt aGranularity )
    {
    CLogsEventArray<TEventType>* self = 
		new (ELeave) CLogsEventArray( aGranularity  );
    return self;
    }

template<class TEventType>
CLogsEventArray<TEventType>::~CLogsEventArray()
    {
	Reset();
    }

///// FIXME: remove this 
///// template<class TEventType>
///// void CLogsEventArray<TEventType>::AppendL(const TEventType& aEvent)
///// 	{
///// 	iArray.AppendL( aEvent );
/////   }

template<class TEventType>
void CLogsEventArray<TEventType>::AppendL( const MLogsEvent& aEvent )
	{
    iArray.AppendL( &aEvent ); 
	}

template<class TEventType>
TInt CLogsEventArray<TEventType>::Count() const 
	{ 
	return iArray.Count(); 
	}

template<class TEventType>
const MLogsEvent& CLogsEventArray<TEventType>::At(TInt aIndex) const
	{ 
    return *(iArray.At( aIndex )); 
	}

template<class TEventType>
MLogsEvent& CLogsEventArray<TEventType>::At(TInt aIndex)
	{ 
    return CONST_CAST(MLogsEvent&, (*(iArray.At( aIndex ))));  
	}

template<class TEventType>
void CLogsEventArray<TEventType>::Delete( TInt aIndex )
	{
    const MLogsEvent* event = iArray[aIndex];	
    delete event;                //delete the owned object
	iArray.Delete( aIndex );     //delete array entry (containing pointer)
	}

template<class TEventType>
void CLogsEventArray<TEventType>::Reset()
	{
/*****
	for( int i=0; i<iArray.Count(); i++ )
		{
		iArray[i].Reset();
		}
	iArray.Reset(); //Thsi function deletes all elements from the array and frees the memory allocated to the array buffer.
*******/
    iArray.ResetAndDestroy();  //Destroys all objects whose pointers form the elements of the array, before resetting the array.
	}

template<class TEventType>
void CLogsEventArray<TEventType>::Compress()
	{
	iArray.Compress();
	}

#endif

// End of File  __CLogsEventArray_H_
