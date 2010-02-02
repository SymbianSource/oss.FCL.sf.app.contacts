/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


// INCLUDE FILES
#include <StifParser.h>
#include <CPsQueryItem.h>
#include <collate.h>
#include "testsuiteinputdata.h"

// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// Two Phase constructor
// ----------------------------------------------------------------------------
CRclTestSuiteInputData* CRclTestSuiteInputData::NewL(CStifItemParser& aItem)
{
	CRclTestSuiteInputData* self = new (ELeave) CRclTestSuiteInputData();	
    CleanupStack::PushL(self);
    self->ConstructL(aItem);
    CleanupStack::Pop();
    return self;
}

// ----------------------------------------------------------------------------
// Default constructor
// ----------------------------------------------------------------------------
CRclTestSuiteInputData::CRclTestSuiteInputData()
{
}


// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
CRclTestSuiteInputData::~CRclTestSuiteInputData()
{    	
    	
    

}


// ----------------------------------------------------------------------------
// Second phase construction
// ----------------------------------------------------------------------------
void CRclTestSuiteInputData::ConstructL(CStifItemParser& /*aItem*/)
{

}


// End of file
