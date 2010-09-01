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
* Description:  Container for icon choice decision data.
*/

// --------------------------------------------------------------------------
// CCustomIconDecisionData::CCustomIconDecisionData
// --------------------------------------------------------------------------
//
inline CCustomIconDecisionData::CCustomIconDecisionData():
        iContext( KNullUid )
    {
    }

// --------------------------------------------------------------------------
// CCustomIconDecisionData::~CCustomIconDecisionData
// --------------------------------------------------------------------------
//
inline CCustomIconDecisionData::~CCustomIconDecisionData()
    {
    delete iTime;
    delete iPhoneNumber;
    }

// --------------------------------------------------------------------------
// CCustomIconDecisionData::Context
// --------------------------------------------------------------------------
//
inline TUid CCustomIconDecisionData::Context() const
    {
    return iContext;
    }

// --------------------------------------------------------------------------
// CCustomIconDecisionData::Time
// --------------------------------------------------------------------------
//
inline const TTime* CCustomIconDecisionData::Time() const
    {
    return iTime;
    }

// --------------------------------------------------------------------------
// CCustomIconDecisionData::PhoneNumber
// --------------------------------------------------------------------------
//
inline TPtrC CCustomIconDecisionData::PhoneNumber() const
    {
    if( iPhoneNumber )
        {
        return *iPhoneNumber;
        }
    return KNullDesC();
    }

// --------------------------------------------------------------------------
// CCustomIconDecisionData::ContactLinkRef
// --------------------------------------------------------------------------
//
inline const MVPbkContactLink* CCustomIconDecisionData::ContactLinkRef() const
    {
    return iContactLinkRef;
    }

// --------------------------------------------------------------------------
// CCustomIconDecisionData::SetContext
// --------------------------------------------------------------------------
//
inline void CCustomIconDecisionData::SetContext( TUid aUid )
    {
    iContext = aUid; 
    }

// --------------------------------------------------------------------------
// CCustomIconDecisionData::SetTime
// --------------------------------------------------------------------------
//
inline void CCustomIconDecisionData::SetTime( TTime* aTime )
    {
    delete iTime;
    iTime = aTime;
    }

// --------------------------------------------------------------------------
// CCustomIconDecisionData::SetPhoneNumberL
// --------------------------------------------------------------------------
//
inline void CCustomIconDecisionData::SetPhoneNumberL
        ( const TDesC& aPhoneNumber )
    {
    HBufC* newNumber = aPhoneNumber.AllocL();
    delete iPhoneNumber;
    iPhoneNumber = newNumber;
    }

// --------------------------------------------------------------------------
// CCustomIconDecisionData::SetContactLinkRef
// --------------------------------------------------------------------------
//
inline void CCustomIconDecisionData::SetContactLinkRef
        ( const MVPbkContactLink* aLink )
    {
    iContactLinkRef = aLink;
    }

// End of File