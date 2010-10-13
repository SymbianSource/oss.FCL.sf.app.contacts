/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Represents a pool element in the contacts cache.
*
*/

// INCLUDE FILES
#include "CPcsDebug.h"
#include "CPcsPoolElement.h"
#include "CPsData.h"
#include "CPcsAlgorithm2Utils.h"

// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CPcsPoolElement::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CPcsPoolElement* CPcsPoolElement::NewL(CPsData& aPsData)
{
	CPcsPoolElement* self = new ( ELeave ) CPcsPoolElement();
	CleanupStack::PushL( self );
	self->ConstructL(aPsData);
	CleanupStack::Pop( self );
	return self;
} 

// ----------------------------------------------------------------------------
// CPcsPoolElement::CPcsPoolElement
// Constructor
// ----------------------------------------------------------------------------
CPcsPoolElement::CPcsPoolElement()
{
}

// ----------------------------------------------------------------------------
// CPcsPoolElement::ConstructL
// 2nd Phase Constructer
// ----------------------------------------------------------------------------
void CPcsPoolElement::ConstructL(CPsData& aPsData)
{
     iPsData = &aPsData;
}

// ----------------------------------------------------------------------------
// CPcsPoolElement::~CPcsPoolElement
// Destructor
// ----------------------------------------------------------------------------
CPcsPoolElement::~CPcsPoolElement()
{
	// Do not delete the PsData in the destructor
	// It is deleted separately, since it is used at multiple locations 
}

// ----------------------------------------------------------------------------
// CPcsPoolElement::GetPsData
// 
// ----------------------------------------------------------------------------
CPsData* CPcsPoolElement::GetPsData()
{
	return iPsData;
}

// ----------------------------------------------------------------------------
// CPcsPoolElement::CompareByData
// Calls CPsData::CompareByData to compare CPsData objects
// 
// ----------------------------------------------------------------------------
TInt CPcsPoolElement::CompareByData ( const CPcsPoolElement& aObject1,  const CPcsPoolElement& aObject2 )
{
  CPsData *data1 = const_cast<CPcsPoolElement&> (aObject1).GetPsData();
  CPsData *data2 = const_cast<CPcsPoolElement&> (aObject2).GetPsData();  
  return (CPcsAlgorithm2Utils::CompareDataBySortOrder(*(data1), *(data2)));
}

// CPcsPoolElement::IsDataMatch
// 
// ----------------------------------------------------------------------------		
TBool CPcsPoolElement::IsDataMatch (TInt aIndex)
{
	TReal val;
	Math::Pow(val, 2, aIndex);

	return(iDataMatchAttribute & (TUint8)val);	
}

// ----------------------------------------------------------------------------
// CPcsPoolElement::SetDataMatch
// 
// ----------------------------------------------------------------------------
void CPcsPoolElement::SetDataMatch(TInt aIndex)
{
	TReal val;
	Math::Pow(val, 2, aIndex);

	iDataMatchAttribute |= (TUint8)val;	
}

// ----------------------------------------------------------------------------
// CPcsPoolElement::ClearDataMatchAttribute
// 
// ----------------------------------------------------------------------------
void CPcsPoolElement::ClearDataMatchAttribute()
{
	iDataMatchAttribute = 0x0;
}


// End of file

