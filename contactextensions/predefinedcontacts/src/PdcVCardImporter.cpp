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
* Description:    Responsible for the importing of vCards
*
*/

// System includes
#include <s32file.h>                    // RFileReadStream
#include <f32file.h>                    // RFs 
#include <MVPbkContactStore.h>          // MVPbkContactStore
#include <CVPbkVCardEng.h>              // CVPbkVCardEng
#include <MVPbkContactOperationBase.h>  // MVPbkContactOperationBase
#include <MVPbkContactLinkArray.h>      // MVPbkContactLinkArray
#include <CVPbkContactLinkArray.h>      // CVPbkContactLinkArray
#include <MVPbkContactLink.h>           // MVPbkContactLink

// User includes
#include "PdcVCardImporter.h"
#include "pdclogger.h"

// Constants
_LIT( KVCardWildCard, "*.vcf" ); // Wild card extension for vCards

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPdcVCardImporter::NewL
// Symbian 1st phase constructor
// @param    aFs     file system
// @param    aContactStore   contacts manager
// @param    aContactStore   contacts store
// @param    aLinkArray  links to contacts added.
// @return a CPdcVCardImporter object.
// ---------------------------------------------------------------------------
//
CPdcVCardImporter* CPdcVCardImporter::NewL( RFs& aFs,
                                        CVPbkContactManager& aContactManager,
                                        MVPbkContactStore& aContactStore,
                                        CVPbkContactLinkArray& aLinkArray )
    {
    CPdcVCardImporter* self = new( ELeave ) CPdcVCardImporter( aFs,
            aContactStore, aLinkArray );
    CleanupStack::PushL( self );
    self->ConstructL( aContactManager ); 
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CPdcVCardImporter::~CPdcVCardImporter
// Destructor
// ---------------------------------------------------------------------------
//
CPdcVCardImporter::~CPdcVCardImporter()
    {
    delete iVCardEngine;
    delete iFileDirectory;
    delete iImportOperation;
    delete iVCardList;
    }

// ---------------------------------------------------------------------------
// CPdcVCardImporter::CPdcVCardImporter
// C++ constructor
// @param    aFs     file system
// @param    aContactStore   contacts store
// @param    aLinkArray  links to contacts added.
// ---------------------------------------------------------------------------
//
CPdcVCardImporter::CPdcVCardImporter( RFs& aFs,
                                          MVPbkContactStore& aContactStore,
                                          CVPbkContactLinkArray& aLinkArray )
    : CActive( EPriorityNormal),
      iContactStore( aContactStore ),
      iFs( aFs ),
      iLinkArray( aLinkArray )
    {
    }

// ---------------------------------------------------------------------------
//  CPdcXmlImporter::ConstructL
//  Second-phase constructor
//  @param    aContactManager   contacts manager
// ---------------------------------------------------------------------------
//
void CPdcVCardImporter::ConstructL( CVPbkContactManager& aContactManager )
    {
    iVCardEngine = CVPbkVCardEng::NewL( aContactManager );
    CActiveScheduler::Add( this );
    }
    
// ---------------------------------------------------------------------------
// CPdcVCardImporter::GetVCardsL  
//  Starts the syncronously importation of all the vCards in the 
//  given directory.
//  @param    aFileDirectory    directory containg vCards.
//  @param    aCallerStatus   caller's TRequestStatus
// ---------------------------------------------------------------------------
//
void CPdcVCardImporter::GetVCardsL(const TDesC& aFileDirectory,
                                     TRequestStatus& aCallerStatus )
    {
    LOGTEXT(_L("CPdcVCardImporter::GetVCardsL"));
    aCallerStatus = KRequestPending;
    
    // Get list of vCards in directory
    TFindFile fileFinder( iFs );
    TInt error = fileFinder.FindWildByDir( KVCardWildCard, 
                                                    aFileDirectory, iVCardList);
    
    // Find the number of vCards in the directory
    if ( error == KErrNotFound )
        {
        // If the are no vCards in the directory, complete the
        // caller status
        TRequestStatus* s = &aCallerStatus;
		User::RequestComplete( s, KErrNone );
        }
    else
        {
        // Store the callers TRequestStatus
        iCallerStatus = &aCallerStatus;
        
        // Store the directory
        iFileDirectory = aFileDirectory.AllocL();
        
        // Start reading in the vCards.
        SelfComplete();
        }
    }
      
// ---------------------------------------------------------------------------
// CPdcVCardImporter::ImportContactsL
// Imports the contacts contained within a vCard.
// @param   aFileName filename of vCard to import
// ---------------------------------------------------------------------------
//  
void CPdcVCardImporter::ImportContactsL( const TDesC& aFileName )
    {
    LOGTEXT(_L("CPdcVCardImporter::ImportContactsL"));
    // Create the full path to the vCard (Note: TParse uses a TBuf<256)
    // internally ).
    TParse fileAndPath;
    fileAndPath.Set( aFileName, iFileDirectory, NULL );
    const TDesC& fullName = fileAndPath.FullName();
    
    // open the vCard into a file stream
    RFileReadStream vCard;
	User::LeaveIfError( vCard.Open( iFs, fullName, EFileRead ) );
	CleanupClosePushL( vCard );

    // Import the vCard into the contacts database
    iImportOperation = iVCardEngine->ImportVCardL(
            iContactStore, vCard , *this );
	                                                       
	// Cleanup
	CleanupStack::PopAndDestroy(); // vCard
    }
    
// ---------------------------------------------------------------------------
// CPdcVCardImporter::SelfComplete
// Signals that the AO should be run again by completing the outstanding
// TRequestStatus.
// ---------------------------------------------------------------------------
//  
void CPdcVCardImporter::SelfComplete()
    {
    LOGTEXT(_L("CPdcVCardImporter::SelfComplete"));
    // Begin the process of adding of the prefined contacts to contacts
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive(); 
    }
 
// ---------------------------------------------------------------------------
// CPdcVCardImporter::DoCancel
// From CActive
// Completes the caller's TRequestStatus with KErrCancel
// ---------------------------------------------------------------------------
//  
void CPdcVCardImporter::DoCancel()
    {
    delete iImportOperation;
    iImportOperation = NULL;
    
    // If the vCard importer is cancelled, we need to complete the
    // callers TRequestStatus.
    User::RequestComplete( iCallerStatus, KErrCancel );
    }

// ---------------------------------------------------------------------------
// CPdcVCardImporter::RunL
// From CActive
// When the request is completed, the next vCard is imported. 
// ---------------------------------------------------------------------------
//
void CPdcVCardImporter::RunL()
    {
    // Import the vCard
    const TDesC& fileName = (*iVCardList)[iVCardIndex].iName;
    ImportContactsL( fileName );
    }

// ---------------------------------------------------------------------------
// CPdcVCardImporter::RunError
// From CActive
// If RunL leaves, the caller's TRequestStatus is completed with the error.
// ---------------------------------------------------------------------------
//
TInt CPdcVCardImporter::RunError( TInt aError )
    {
    LOGTEXT(_L("CPdcVCardImporter::RunError"));
    // Report the error to the caller of the getVcards function by 
    // completing the callers activeObject.
    User::RequestComplete( iCallerStatus, aError );
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CPdcVCardImporter::AddToLinkArray
// Creates copies of contact links and adds them to the contact array.
// @param   aResults array of links to the contacts items that have been 
//                   added.
// ---------------------------------------------------------------------------
//
void CPdcVCardImporter::AddToLinkArrayL( const MVPbkContactLinkArray& aResults)
    {
    // Store the links that have been added. A MVPbkContactLinkArray array
    // is read-only so copies of the links need to be made and added
    // to iLinkArray.
    TInt linkCount = aResults.Count();
    for ( TInt index = 0; index < linkCount; index++ )
        {
        const MVPbkContactLink& link = aResults[index];
        MVPbkContactLink* copiedLink = link.CloneLC();
        iLinkArray.AppendL( copiedLink );
        CleanupStack::Pop(); // copiedLink 
        }
    }

// ---------------------------------------------------------------------------
// CPdcVCardImporter::ContactsSaved
// from MVPbkContactCopyObserver
// Called when the contact has been successfully commited. If any contacts are 
// created links to them are added to the link array. The next vCard is then 
// queued for importing, if there are no more vCards to add, the caller's 
// TRequestStatus is completed.
//
// @param aOperation    The operation that this observer monitors.
// @param aResults  An array of links to copied contacts.
//                  Caller takes the ownership of the 
//                  object immediately.
// ---------------------------------------------------------------------------
//
void CPdcVCardImporter::ContactsSaved( 
        MVPbkContactOperationBase& /*aOperation*/,
        MVPbkContactLinkArray* aResults )
    {
    LOGTEXT(_L("CPdcVCardImporter::ContactsSaved"));
    delete iImportOperation;
    iImportOperation = NULL;
    
    if ( aResults )
        {
        // Store any links
        TRAP_IGNORE( AddToLinkArrayL( *aResults ) )
        
        // delete the results array
        delete aResults;
        }

    // Increase the index
    iVCardIndex++;
    
    // If last vCard has been imported,  
    // inform the caller or self complete to import the next vCard.
    if ( iVCardIndex < iVCardList->Count() )
        {
        SelfComplete();
        }
    else
        {
        User::RequestComplete( iCallerStatus, KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CPdcVCardImporter::
// from MVPbkContactCopyObserver
// Called when there was en error while saving contact(s).
// @param aOperation    The operation that this observer monitors.
// @param aError an error that occured.
// ---------------------------------------------------------------------------
//
void CPdcVCardImporter::ContactsSavingFailed( 
        MVPbkContactOperationBase& /*aOperation*/, TInt aError )
    {
    LOGTEXT2(_L("CPdcVCardImporter::ContactsSavingFailed aError = %d"), aError);
    // If last vCard has been handled, inform the caller or self complete to 
    // import the next vCard.   
        
    User::RequestComplete( iCallerStatus, aError );
    }


