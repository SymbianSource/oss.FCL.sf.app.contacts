/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MXIMPIdentity API object implementation.
*
*/



//#include "t_testsingleton.h"
#include "ccappcommlauncherheaders.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// T_CTestSingleton::InstanceL()
// ---------------------------------------------------------------------------
//
T_CTestSingleton* T_CTestSingleton::InstanceL()
    {
    T_CTestSingleton* self = NULL;
    TAny* tlsPtr = Dll::Tls();
    if ( NULL == tlsPtr )
        {
        self = new( ELeave ) T_CTestSingleton;
        CleanupStack::PushL( self );
        self->ConstructL( );
        User::LeaveIfError( Dll::SetTls( self ) );
        CleanupStack::Pop( self );
        }
    else
        {
        self = static_cast<T_CTestSingleton*>( tlsPtr );
        ++self->iRefCount;
        }
    return self;
    }
// ---------------------------------------------------------------------------
// T_CTestSingleton::~T_CTestSingleton()
// ---------------------------------------------------------------------------
//
void T_CTestSingleton::Release()
    {

    TAny* tlsPtr = Dll::Tls();
 
    T_CTestSingleton* self = static_cast<T_CTestSingleton*>( tlsPtr );
    if ( 0 == --self->iRefCount )
        {
        Dll::FreeTls();
        delete self;
        }
    }
// ---------------------------------------------------------------------------
// T_CTestSingleton::~T_CTestSingleton()
// ---------------------------------------------------------------------------
//
T_CTestSingleton::~T_CTestSingleton()
    {
    }


// ---------------------------------------------------------------------------
// T_CTestSingleton::T_CTestSingleton()
// ---------------------------------------------------------------------------
//
T_CTestSingleton::T_CTestSingleton()
    {
    iValue = KTestNormal;
    iRefCount = 1;
    }


// ---------------------------------------------------------------------------
// T_CTestSingleton::ConstructL()
// ---------------------------------------------------------------------------
//
void T_CTestSingleton::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// T_CTestSingleton::SetValue()
// ---------------------------------------------------------------------------
//
void T_CTestSingleton::SetValue( TInt aValue )
    {
    T_CTestSingleton* self = static_cast<T_CTestSingleton*>( Dll::Tls() );
    self->iValue = aValue;
    TInt err = Dll::SetTls( static_cast<TAny*>( self ) );
    if ( KErrNone != err )
        {
        // 
        }
    self = NULL;
    }

// ---------------------------------------------------------------------------
// T_CTestSingleton::GetValue()
// ---------------------------------------------------------------------------
//      
TInt T_CTestSingleton::GetValue( )
    {
    return iValue;
    }
    
    
