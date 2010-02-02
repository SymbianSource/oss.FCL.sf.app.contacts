/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Implements Virtual Phonebook generic IPC package.
*
*/


#include "VPbkIPCPackage.h"

// System includes
#include <barsread.h>
#include <s32mem.h>

/// Unnamed namespace for local definitions
namespace {

const TInt KNumberSize = 4;
const TInt KEmpty = 0;
const TInt KDouble = 2;

} /// namespace

namespace VPbkEngUtils {

// --------------------------------------------------------------------------
// VPbkIPCPackage::ExternalizeL
// --------------------------------------------------------------------------
//  
EXPORT_C void VPbkIPCPackage::ExternalizeL( 
        const TDesC8* aBuffer, 
        RDesWriteStream& aWriteStream )
    {
    if ( aBuffer )
        {        
        aWriteStream.WriteUint32L( aBuffer->Length() );
        aWriteStream.WriteL( *aBuffer );                
        }
    else
        {
        aWriteStream.WriteUint32L( KEmpty );
        }
    }

// --------------------------------------------------------------------------
// VPbkIPCPackage::ExternalizeL
// --------------------------------------------------------------------------
//     
EXPORT_C void VPbkIPCPackage::ExternalizeL( 
        const TDesC* aBuffer, 
        RDesWriteStream& aWriteStream )
    {
    if ( aBuffer )
        {        
        aWriteStream.WriteUint32L( aBuffer->Length() );
        aWriteStream.WriteL( *aBuffer );                
        }
    else
        {
        aWriteStream.WriteUint32L( KEmpty );
        }
    }

// --------------------------------------------------------------------------
// VPbkIPCPackage::InternalizeL
// --------------------------------------------------------------------------
// 
EXPORT_C void VPbkIPCPackage::InternalizeL( 
        HBufC8*& aBuffer, 
        RDesReadStream& aReadStream )
    {
    delete aBuffer;
    aBuffer = NULL;   
    
    TInt length( aReadStream.ReadUint32L() );    
    if ( length > 0 )
        {
        aBuffer = HBufC8::NewL( length );        
        TPtr8 ptr = aBuffer->Des();
        aReadStream.ReadL( ptr ,length );
        }      
    }
 
// --------------------------------------------------------------------------
// VPbkIPCPackage::InternalizeL
// --------------------------------------------------------------------------
//     
EXPORT_C void VPbkIPCPackage::InternalizeL( 
        HBufC*& aBuffer, 
        RDesReadStream& aReadStream )    
    {
    delete aBuffer;
    aBuffer = NULL;
    
    TInt length( aReadStream.ReadUint32L() );
    if ( length > 0 )
        {
        aBuffer = HBufC::NewL( length );        
        TPtr ptr = aBuffer->Des();
        aReadStream.ReadL( ptr ,length );
        }
    }

// --------------------------------------------------------------------------
// VPbkIPCPackage::InternalizeL
// --------------------------------------------------------------------------
// 
EXPORT_C void VPbkIPCPackage::InternalizeL
        ( HBufC8*& aBuffer, RDesReadStream& aReadStream,
          const TInt aPosition )
    {
    delete aBuffer;
    aBuffer = NULL;   

    // Read the non relevant parts of the package
    for ( TInt i=0; i < aPosition; ++ i )
        {
        TInt dummyLength = aReadStream.ReadUint32L();
        aReadStream.ReadL(dummyLength);
        }
    
    TInt length( aReadStream.ReadUint32L() );    
    if ( length > 0 )
        {
        aBuffer = HBufC8::NewL( length );        
        TPtr8 ptr = aBuffer->Des();
        aReadStream.ReadL( ptr ,length );
        }      
    }
 
// --------------------------------------------------------------------------
// VPbkIPCPackage::InternalizeL
// --------------------------------------------------------------------------
//     
EXPORT_C void VPbkIPCPackage::InternalizeL
        ( HBufC*& aBuffer,  RDesReadStream& aReadStream,
          const TInt aPosition )
    {
    delete aBuffer;
    aBuffer = NULL;

    // Read the non relevant parts of the package
    for ( TInt i=0; i < aPosition; ++ i )
        {
        TInt dummyLength = aReadStream.ReadUint32L();
        aReadStream.ReadL(dummyLength);
        }
    
    TInt length( aReadStream.ReadUint32L() );
    if ( length > 0 )
        {
        aBuffer = HBufC::NewL( length );        
        TPtr ptr = aBuffer->Des();
        aReadStream.ReadL( ptr ,length );
        }
    }

// --------------------------------------------------------------------------
// VPbkIPCPackage::CountPackageSize
// --------------------------------------------------------------------------
//
EXPORT_C TInt VPbkIPCPackage::CountPackageSize( const TDesC8* aBuffer )
    {
    if ( aBuffer )
        {        
        return aBuffer->Length() + KNumberSize;
        }
    return KNumberSize;        
    }

// --------------------------------------------------------------------------
// VPbkIPCPackage::CountPackagesLength
// --------------------------------------------------------------------------
//    
EXPORT_C TInt VPbkIPCPackage::CountPackageSize( const TDesC* aBuffer )
    {
    if ( aBuffer )
        { 
        return aBuffer->Length()*KDouble + KNumberSize;
        }
    return KNumberSize;        
    }

} /// namespace

// End of File
