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
* Description:  PCS general debug class 
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <CPcsDefs.h>
#include "CPcsDebug.h"
#include "CPsQueryItem.h"
#include "CPsQuery.h"


// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CPcsDebugWrapper::__LatencyMarkStart
// Marks the start time for latency measurement
// ----------------------------------------------------------------------------
EXPORT_C void CPcsDebugWrapper::__LatencyMarkStartL(TRefByValue<const TDesC> str) 
{
    CPcsDebugArr* dbgArr = 0;
    
	// Check thread local storage:
	if ( Dll::Tls() == NULL )
	{	 
		dbgArr = CPcsDebugArr::NewL();
		User::LeaveIfError ( Dll::SetTls( dbgArr ) );
	}
	else
	{
	  	dbgArr = static_cast<CPcsDebugArr*>( Dll::Tls() );
	}

	CPcsDebug *dbg = CPcsDebug::NewL();
	dbg->Mark(str);
    
    dbgArr->Push(*dbg);
}

// ----------------------------------------------------------------------------
// CPcsDebugWrapper::__LatencyMarkEnd
// Marks the end time for latency measurement
// Displays the difference from Latency Mark start
// ----------------------------------------------------------------------------
EXPORT_C void CPcsDebugWrapper::__LatencyMarkEnd(TRefByValue<const TDesC> str) 
{
    CPcsDebugArr* dbgArr = 0;
    
	// Check thread local storage:
	if ( Dll::Tls() == NULL )
	{	 
	  	return;
	}
	else
	{
	  	dbgArr = static_cast<CPcsDebugArr*>( Dll::Tls() );
	}

    if ( dbgArr->IsEmpty() )
    {
        delete dbgArr;
        Dll::SetTls( NULL );
		return;
    }
    else
    {
    	CPcsDebug* dbg = dbgArr->Pop();
		dbg->UnMark(str);
		delete dbg;			  	
    }
    
    if ( dbgArr->IsEmpty() )
    {
        delete dbgArr;
        Dll::SetTls( NULL );
    }
}

// ----------------------------------------------------------------------------
// CPcsDebug::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
EXPORT_C CPcsDebug* CPcsDebug::NewL()
{
    CPcsDebug* self = new (ELeave) CPcsDebug();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
}

// ----------------------------------------------------------------------------
// CPcsDebug::ConstructL
// Two Phase Construction
// ----------------------------------------------------------------------------
void CPcsDebug::ConstructL()
{
}

// ----------------------------------------------------------------------------
// CPcsDebug::Mark
// Marks the start time for latency measurement
// ----------------------------------------------------------------------------
void CPcsDebug::Mark(TRefByValue<const TDesC> str,...)
{
	VA_LIST list;
    VA_START(list, str);

    // Print to log file
    TBuf<255> buf;
    buf.FormatList(str, list);
    PRINT1 ( _L("#### [%S] Latency Measurement Start ####"), &buf );
    
	startTime.HomeTime();
}

// ----------------------------------------------------------------------------
// CPcsDebug::UnMark
// Marks the end time for latency measurement
// Displays the difference from Latency Mark start
// ----------------------------------------------------------------------------
void CPcsDebug::UnMark(TRefByValue<const TDesC> str,...)
{
	endTime.HomeTime();
	VA_LIST list;
    VA_START(list, str);

    // Print to log file
    TBuf<255> buf;
    buf.FormatList(str, list);
	
	TTimeIntervalMicroSeconds diff = endTime.MicroSecondsFrom(startTime);
	TInt mytime = (diff.Int64()) / 1000;

	PRINT2 ( _L("#### [%S] Latency Measurement End, Time taken = %d (ms) ####"), &buf, mytime );
}

// ----------------------------------------------------------------------------
// CPcsDebug::PrintQueryL
// Prints the query as array of query items (query items cannot be spaces)
// Used only for debugging
// ----------------------------------------------------------------------------
EXPORT_C void CPcsDebug::PrintQueryL(const TDesC& aPreTxt, CPsQuery& aQuery)
{
    for ( TInt i = 0; i < aQuery.Count(); i++ )
    {
        CPsQueryItem& item = aQuery.GetItemAtL(i);
        TUint inputKey = item.Character().GetUpperCase();
        TBuf<2> buffer;
        buffer.Format(_L("%c"), inputKey);
        switch ( item.Mode() )
        {
            case ENonPredictive:
                PRINT3 ( _L("%SQuery[%d].{Character.Up=%S, Mode=ENonPredictive}"),
                         &aPreTxt, i, &buffer);
                break;
            case EPredictiveDefaultKeyboard:
                PRINT3 ( _L("%SQuery[%d].{Character.Up=%S, Mode=EPredictiveDefaultKeyboard}"),
                         &aPreTxt, i, &buffer);
                break;
            case EPredictiveItuT:
                PRINT3 ( _L("%SQuery[%d].{Character.Up=%S, Mode=EPredictiveItuT}"),
                         &aPreTxt, i, &buffer);
                break;
            case EPredictiveQwerty:
                PRINT3 ( _L("%SQuery[%d].{Character.Up=%S, Mode=EPredictiveQwerty}"),
                         &aPreTxt, i, &buffer);
                break;
            default:
                PRINT4 ( _L("%SQuery[%d].{Character.Up=%S, Mode=%d (Error}"),
                         &aPreTxt, i, &buffer, item.Mode());
                break;
        }
    }
}

// ----------------------------------------------------------------------------
// CPcsDebug::PrintQueryList
// Prints the query list as array of queries (queries cannot contain spaces)
// Used only for debugging
// ----------------------------------------------------------------------------
EXPORT_C void CPcsDebug::PrintQueryListL(const TDesC& aPreTxt, RPointerArray<CPsQuery>& aPsQueryList)
{
    for ( TInt i = 0; i < aPsQueryList.Count(); i++ )
    {
        TPtrC queryPtr = aPsQueryList[i]->QueryAsStringLC();        
        PRINT3 ( _L("%SQueryList[%d] = %S"), &aPreTxt, i, &queryPtr );
        CleanupStack::PopAndDestroy();
    }
}

// ----------------------------------------------------------------------------
// CPcsDebug::PrintMatchLoc
// Prints the array of match locations
// Used only for debugging
// ----------------------------------------------------------------------------
EXPORT_C void CPcsDebug::PrintMatchLoc(const TDesC& aPreTxt, RArray<TPsMatchLocation>& aMatchLocs)
{
    for ( TInt i = 0; i < aMatchLocs.Count(); i++ )
    {
        PRINT5 ( _L("%SMatchLoc[%d].{index=%d, length=%d, direction=%d}"),
                 &aPreTxt, i, aMatchLocs[i].index, aMatchLocs[i].length, (TInt) aMatchLocs[i].direction );
    }
}

// ----------------------------------------------------------------------------
// CPcsDebug::PrintMatchSet
// Prints the array of match sequences
// Used only for debugging
// ----------------------------------------------------------------------------
EXPORT_C void CPcsDebug::PrintMatchSet(const TDesC& aPreTxt, RPointerArray<TDesC>& aMatchSet)
{
    for ( TInt i = 0; i < aMatchSet.Count(); i++ )
    {
        PRINT4 ( _L("%SMatchSet[%d]=%S, Length=%d"), &aPreTxt, i, aMatchSet[i], aMatchSet[i]->Length() );
    }
}

// ----------------------------------------------------------------------------
// CPcsDebug::PrintMatchSet
// Prints the array of match sequences
// Used only for debugging
// ----------------------------------------------------------------------------
EXPORT_C void CPcsDebug::PrintMatchSet(const TDesC& aPreTxt, CDesCArray& aMatchSet)
{
    for ( TInt i = 0; i < aMatchSet.Count(); i++ )
    {
        PRINT4 ( _L("%SMatchSet[%d]=%S, Length=%d"), &aPreTxt, i, &aMatchSet[i], aMatchSet[i].Length() );
    }
}

// ----------------------------------------------------------------------------
// CPcsDebugArr::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
EXPORT_C CPcsDebugArr* CPcsDebugArr::NewL()
{
    CPcsDebugArr* self = new (ELeave) CPcsDebugArr();
    return self;
}

// ----------------------------------------------------------------------------
// CPcsDebugArr::Push
// Push an element into the array
// ----------------------------------------------------------------------------
EXPORT_C void CPcsDebugArr::Push(CPcsDebug& dbg)
{
    debugArray.Append(&dbg);
}

// ----------------------------------------------------------------------------
// CPcsDebugArr::Pop
// Pop an element from the array
// ----------------------------------------------------------------------------
EXPORT_C CPcsDebug* CPcsDebugArr::Pop()
{
     TInt index = debugArray.Count() - 1;
     CPcsDebug* dbg = debugArray[index];
     debugArray.Remove(index);
	 return (dbg);
}

// ----------------------------------------------------------------------------
// CPcsDebugArr::IsEmpty
// Check if array is empty
// ----------------------------------------------------------------------------
EXPORT_C TBool CPcsDebugArr::IsEmpty()
{
     if ( debugArray.Count() == 0 )
          return ETrue;
     else 
          return EFalse;
}

// ----------------------------------------------------------------------------
// CPcsDebugArr::~CPcsDebugArr
// Destructor
// ----------------------------------------------------------------------------
CPcsDebugArr::~CPcsDebugArr()
{
	debugArray.ResetAndDestroy();
}

