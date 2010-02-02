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
*     Abstract interface for Event arrays
*
*/


#ifndef __MLogsEventArray_H__
#define __MLogsEventArray_H__

//  INCLUDES

// FORWARD DECLARATION

// CLASS DECLARATION
class MLogsEvent;


class MLogsEventArray
    {
	public:
    
        virtual ~MLogsEventArray() {};

    public:		// Straight encapsulation of CArrayFixFlat
        virtual TInt Count() const =0;
        virtual const MLogsEvent& At( TInt aIndex ) const =0;
        virtual       MLogsEvent& At( TInt aIndex ) =0;
        virtual void AppendL( const MLogsEvent& aEvent ) =0;
        virtual void Delete( TInt aIndex ) =0;
		virtual void Reset() =0;
		virtual void Compress() =0;

	public:		// convenience methods
		inline const MLogsEvent& operator[]( TInt aIndex ) const
            { return At( aIndex ); }
        inline MLogsEvent& operator[](TInt aIndex)
            { return At( aIndex ); }

    };

#endif

// End of File  __MLogsEventArray_H_
