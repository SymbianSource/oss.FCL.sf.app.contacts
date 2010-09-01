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
* Description:    Predefined contacts engine (state machine)
*
*/

// System includes
#include <cntdb.h>                  // CContactDatabase
#include <centralrepository.h>      // CRepository
#include <centralrepository.h>      // CRepository
#include <f32file.h>                // RFs 
#include <CVPbkContactLinkArray.h>  // CVPbkContactLinkArray

// User includes
#include "pdcdata.h"            // CPdcData


// Constants

/**
 * Cenrep key for the predefined contacts
 */
const TInt KCRUidPredefinedContacts = 0x2000BEE5;

/**
 * Default file location index in cenrep file
 */
const TInt KPdcFileLocation = 2;

/**
 * Pdc data file name
 */
_LIT( KPdcDataFileName, "c:\\private\\2000BEE5\\pdcdatafile");
_LIT( KPdcDataFilePath, "c:\\private\\2000BEE5\\");
/** 
 * Pdc data file major version number
 */ 
const TInt  KPdcMajorVersion = 1;

/** 
 * Pdc data file minor version number
 */ 
const TInt  KPdcMinorVersion = 0;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPdcData::NewLC
// Symbian 1st phase constructor
// @param    aFs    file system
// @param    aContactIds ids of any predefined contacts that have been added.
// @return Self pointer to CPdcData pushed to
// the cleanup stack.
// ---------------------------------------------------------------------------
//
CPdcData* CPdcData::NewL( RFs& aFs, CVPbkContactLinkArray& aLinkArray )
    {
    CPdcData* self = new( ELeave ) CPdcData( aFs, aLinkArray );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CPdcData::~CPdcData
// Destructor
// ---------------------------------------------------------------------------
//
CPdcData::~CPdcData()
    {
    delete iRepository;
    delete iLinkBuffer;
    }

// ---------------------------------------------------------------------------
// CPdcData::CPdcData
// C++ constructor
// @param    aFs    file system
// @param    aLinkArray  links to contacts that have been added.
// ---------------------------------------------------------------------------
//
CPdcData::CPdcData( RFs& aFs, CVPbkContactLinkArray& aLinkArray )
    : iFs( aFs ), iLinkArray( aLinkArray )
    {
    }

// ---------------------------------------------------------------------------
//  CPdcData::ConstructL
//  Second-phase constructor
// ---------------------------------------------------------------------------
//
void CPdcData::ConstructL()
    {   
    iRepository = CRepository::NewL( TUid::Uid( KCRUidPredefinedContacts  ) );
    }
    
// ---------------------------------------------------------------------------
// CPdcData::ContactsUpToDateL() 
// Checks if the predefined contacts need to be added.
// @return ETrue if the predefined contacts need to be added
// ---------------------------------------------------------------------------
//
TBool CPdcData::ContactsUpToDateL()
    {
    TBool upDate = ETrue;
    
    // Does the pdcdata file exist?
    RFile pdcDataFile;
    
    TInt error = pdcDataFile.Open( iFs, KPdcDataFileName,
            EFileRead | EFileStreamText );
    CleanupClosePushL( pdcDataFile );

    if ( error == KErrNone )
        {
         // Open a read stream and internalise the file
        RFileReadStream stream( pdcDataFile );
        stream.PushL();
        
        InternalizeL( stream );
    
        stream.Pop();
        stream.Close();            
        }  
    else 
        {  
        // If the pdc file doe not exist, it indicates that
        // this is the first run, a firmware upgrade or a
        // file system format. Hence the predefined contacts 
        // are added. Leave if it is a different error.
        if ( error != KErrNotFound && error != KErrPathNotFound )
            {
            User::Leave( error );
            }
        }

    CleanupStack::PopAndDestroy(); // pdcDataFile
    
    // Get timestamp from cenrep
    return upDate;
    }
      
// ---------------------------------------------------------------------------
// CPdcData::StoreL() 
// Stores the ids of anycontacts that have been added. Plus the most recent 
// info about the firmware etc if needed 
// ---------------------------------------------------------------------------
//  
void CPdcData::StoreL() 
    {   
    // Write out the new details
    RFile pdcDataFile;
    TInt error = pdcDataFile.Replace( iFs, KPdcDataFileName, EFileWrite );
    if ( error ==  KErrPathNotFound )
       {
       // Create the path if it can not be found
 	   User::LeaveIfError( iFs.MkDir( KPdcDataFilePath ) );
 	   error = pdcDataFile.Replace( iFs, KPdcDataFileName, EFileWrite );
       }
    // If there has been an error leave
    User::LeaveIfError( error );
    
    CleanupClosePushL( pdcDataFile );
    
     // Open a write stream and externalise the file
    RFileWriteStream stream( pdcDataFile );
    stream.PushL();
    
    // Externalise the data to the stream.
    ExternalizeL( stream );

    // Tidy up
    stream.CommitL();
    stream.Pop();
    stream.Close();    
    CleanupStack::PopAndDestroy(); // pdcDataFile
    }
    
// ---------------------------------------------------------------------------
// CPdcData::GetFileLocationL
// Gets the file location of the predefined contacts from the central 
// repository.
// @return  location of the predefined contacts. 
// ---------------------------------------------------------------------------
//
HBufC* CPdcData::GetFileLocationL()
    { 
    // Get the filename
    TFileName fileName;
    TInt error = iRepository->Get( KPdcFileLocation, fileName );
    User::LeaveIfError( error );
    
    HBufC* fileLocation = fileName.AllocL();
    
    return fileLocation;
    } 

// ---------------------------------------------------------------------------
// CPdcData::InternalizeL
// Reads in the list of added contacts and any other persisted data such as
// timestamp, firmware id.
// @param aStream   readstream to read data from
// ---------------------------------------------------------------------------
//
void CPdcData::InternalizeL( RReadStream& aStream )
    {
    // Read the major and minor version numbers in case there is a future 
    // BC break that may need to be detected.
    TInt major = aStream.ReadInt8L();
    TInt minor = aStream.ReadInt8L();
    
    ASSERT( major == KPdcMajorVersion && minor == KPdcMinorVersion );
    
    // Get the size of the list of contact links and store it in 
    // a buffer
    TInt bufferSize = aStream.ReadUint16L();
    iLinkBuffer = HBufC8::NewL( bufferSize );
    TPtr8 bufferPtr = iLinkBuffer->Des();
    aStream >> bufferPtr;
    bufferPtr.SetLength( bufferSize );
    }

// ---------------------------------------------------------------------------
// CPdcData::ExternalizeL
// Writes out the added contacts list and any other the persisted data such 
// as timestamp, firmware id
// 
// @param aStream   writestream to write data to
// ---------------------------------------------------------------------------
//  
void CPdcData::ExternalizeL( RWriteStream& aStream ) const
    {
    // Write out the major and minor version numbers in case there is a future 
    // BC break that may need to be detected.
    aStream.WriteInt8L( KPdcMajorVersion );
    aStream.WriteInt8L( KPdcMinorVersion );
    
    // Write out the list of contacts, first pack the link list into
    // a buffer
    HBufC8* buf = iLinkArray.PackLC();
    
    // Write the size of the descriptor, followed by the buffer
    aStream.WriteUint16L(buf->Length());
    aStream << *buf;
    
    // tidy up buffer
    CleanupStack::PopAndDestroy(); // buf
    }
    
// ---------------------------------------------------------------------------
// CPdcData::LinkArrayBuffer
// Gets the buffer of contact links
// @return  buffer of contact links
// ---------------------------------------------------------------------------
// 
HBufC8* CPdcData::LinkArrayBuffer()
    {
    return iLinkBuffer;
    }
