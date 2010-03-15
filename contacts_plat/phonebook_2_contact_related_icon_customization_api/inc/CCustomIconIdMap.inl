/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Data container to icons and their unique identifiers.
*/

// --------------------------------------------------------------------------
// TCustomIconIdMapItem::TCustomIconIdMapItem
// --------------------------------------------------------------------------
//
inline TCustomIconIdMapItem::TCustomIconIdMapItem():
        iIconPtr( NULL ), iId( NULL )
    {
    }

// --------------------------------------------------------------------------
// TCustomIconIdMapItem::TCustomIconIdMapItem
// --------------------------------------------------------------------------
//
inline TCustomIconIdMapItem::TCustomIconIdMapItem
        ( CGulIcon* aIcon, TCustomIconId aId):
            iIconPtr( aIcon ), iId( aId )
    {
    }

// --------------------------------------------------------------------------
// CCustomIconIdMap::CCustomIconIdMap
// --------------------------------------------------------------------------
//
inline CCustomIconIdMap::CCustomIconIdMap( TInt aGranularity ):
        CArrayFixFlat<TCustomIconIdMapItem>( aGranularity )
    {
    }

// --------------------------------------------------------------------------
// CCustomIconIdMap::~CCustomIconIdMap
// --------------------------------------------------------------------------
//
inline CCustomIconIdMap::~CCustomIconIdMap()
    {
    TInt i;
    TInt count = Count();
    for( i=0; i<count; i++ )
        {
        delete At( i ).iIconPtr;
        }
    }

// --------------------------------------------------------------------------
// CCustomIconIdMap::AppendL
// --------------------------------------------------------------------------
//
inline void CCustomIconIdMap::AppendL( CGulIcon* aIcon, TCustomIconId aId  )
    {
    TCustomIconIdMapItem item( aIcon, aId );
    CArrayFixFlat<TCustomIconIdMapItem>::AppendL( item );
    }

// End of File