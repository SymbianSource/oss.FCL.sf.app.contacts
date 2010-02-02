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
* Description:    XML importer
*
*/

// System includes
#include <s32file.h>        // RFileReadStream
#include <f32file.h>        // RFs 
#include <cntitem.h>        // CContactItem
#include <matchdata.h>      // CMatchData
#include <parser.h>         // CParser

// User includes
#include "PdcXMLImporter.h"         // CPdcXmlImporter
#include "PdcXMLContentHandler.h"   // CPdcXmlContentHandler

// Constants
_LIT( KXmlWildCard, "*.xml"); // Wildcard pattern for Xml files
_LIT8( KXmlMimeType, "text/xml" ); // XML mimie type

using namespace Xml;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPdcXmlImporter::NewL
// Symbian 1st phase constructor
// @param    aFs     file system
// @param    aContactStore   contacts store
// @param    aLinkArray  links to contacts added.
// @return a CPdcXmlImporter object.
// ---------------------------------------------------------------------------
//
CPdcXmlImporter* CPdcXmlImporter::NewL( RFs& aFs,
        MVPbkContactStore& aContactStore, CVPbkContactLinkArray& aLinkArray )
    {
    CPdcXmlImporter* self = new( ELeave ) CPdcXmlImporter( aFs,
            aContactStore, aLinkArray );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CPdcXmlImporter::~CPdcXmlImporter
// Destructor
// ---------------------------------------------------------------------------
//
CPdcXmlImporter::~CPdcXmlImporter()
    {
    Cancel();
    
    iFile.Close();
    
    delete iContentHandler;
    delete iXmlParser;
    delete iXmlMatchData;
    }

// ---------------------------------------------------------------------------
// CPdcXmlImporter::CPdcXmlImporter
// C++ constructor
// @param    aFs     file system
// @param    aContactStore   contacts store
// @param    aLinkArray  links to contacts added.
// ---------------------------------------------------------------------------
//
CPdcXmlImporter::CPdcXmlImporter( RFs& aFs, 
        MVPbkContactStore& aContactStore, CVPbkContactLinkArray& aLinkArray )
    : CActive( EPriorityNormal ),
      iContactStore( aContactStore ),
      iFs( aFs ),
      iLinkArray( aLinkArray )
    {
    }
    
// ---------------------------------------------------------------------------
//  CPdcXmlImporter::ConstructL
//  Second-phase constructor
// ---------------------------------------------------------------------------
//
void CPdcXmlImporter::ConstructL()
    {
    CActiveScheduler::Add( this );
    }
    
// ---------------------------------------------------------------------------
// CPdcXmlImporter::GetXmlContactsL 
// Opens an xml file if it exists and starts parseing the contents
// @param   aFileDirectory      directory containing xml file
// @param   aCallerStatus       iStatus of caller
// ---------------------------------------------------------------------------
//
void CPdcXmlImporter::GetXmlContactsL(const TDesC& aFileDirectory,
                                     TRequestStatus& aCallerStatus )
    {
    LOGTEXT(_L("CPdcXmlImporter::GetXmlContactsL"));
    aCallerStatus = KRequestPending;
    
    // Is there an XML document in the directory. As there should be only one
    // XML document if there are more than one, it is only the first one that
    // is parsed.
     
    // Get list of xml files in  the directory
    TFindFile fileFinder( iFs );
    
    CDir* xmlFileList = NULL;
    TInt error = fileFinder.FindWildByDir( KXmlWildCard, 
                                                    aFileDirectory, xmlFileList);
                                                    
    CleanupStack::PushL( xmlFileList );
                                                     
    // Is there a XML file in the directory
    if ( error == KErrNotFound )
        {
        // If the are no Xml file in the directory, complete the
        // caller's TRequestStatus
        TRequestStatus* s = &aCallerStatus;
		User::RequestComplete( s, KErrNone );
        }
    else
        {
        // Create the filename
        const TDesC& fileName = (*xmlFileList)[0].iName;
        TParse fileAndPath;
        fileAndPath.Set( fileName, &aFileDirectory, NULL );
        const TDesC& xmlFileName = fileAndPath.FullName();
        
        // Store the callers TRequestStatus
        iCallerStatus = &aCallerStatus;
        
        // Create match data to find the correct xml library 
        iXmlMatchData = CMatchData::NewL();
        iXmlMatchData->SetMimeTypeL(KXmlMimeType);
        iXmlMatchData->SetVariantL(_L8("libxml2"));
        
        // Create rge content handler
        iContentHandler =
                CPdcXmlContentHandler::NewL( iContactStore, iLinkArray);
        
        //transfer the pointer of ContactManager to contenthandler
        iContentHandler->SetContactManager(iContactManager);
        
        // create the xml parser
        iXmlParser = CParser::NewL( *iXmlMatchData, *iContentHandler );
        
        // Set string table
        iContentHandler->SetStringPoolL ( iXmlParser->StringPool() );   

        // Load the file
        User::LeaveIfError( 
            iFile.Open( iFs, xmlFileName, EFileRead|EFileShareReadersOnly ) );
        // Read the first block of data
        iFile.Read( iXmlData, iStatus );
        SetActive();
        }
    
    CleanupStack::PopAndDestroy( xmlFileList );
    }
  
// ---------------------------------------------------------------------------
// CPdcXmlImporter::DoCancel
// From CActive
// Completes the caller's TRequestStatus with KErrCancel
// ---------------------------------------------------------------------------
//   
void CPdcXmlImporter::DoCancel()
    {
    // If the vCard importer is cancelled, we need to complete the
    // callers TRequestStatus.
    User::RequestComplete( iCallerStatus, KErrCancel );

    // Cancel the file read operation
    iFile.ReadCancel();
    }

// ---------------------------------------------------------------------------
// CPdcXmlImporter::RunL
// From CActive
// When the request is completed, data read is past to the XML parser, and the
// next block of data is read asyncronously. If there is no more data to be
// read the caller's TRequestStatus is completed.
// ---------------------------------------------------------------------------
//
void CPdcXmlImporter::RunL()
    {
    // Leave if there is an error
    User::LeaveIfError( iStatus.Int() );
    
    if ( iXmlData.Length() )
        {
        // Parse the Xml data
        iXmlParser->ParseL( iXmlData );

        // Read the next block of data
        iFile.Read( iXmlData, iStatus );
        SetActive();
        }
    else
        {
        // Reached the end of the file. So complete 
        // the caller's TRequestStatus
		User::RequestComplete( iCallerStatus, KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// CPdcXmlImporter::RunError
// From CActive
// If RunL leaves, the caller's TRequestStatus is completed with the error.
// ---------------------------------------------------------------------------
//
TInt CPdcXmlImporter::RunError( TInt aError )
    {
    LOGTEXT(_L("CPdcXmlImporter::RunError"));
    // Report the error to the caller of the GetXmlContactsL function by 
    // completing the callers activeObject.
    User::RequestComplete( iCallerStatus, aError );
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CPdcXmlImporter::
// This method get the pointer of contactmanager from CPdcEngine
// @param aContactManager 
// ---------------------------------------------------------------------------
//     
void CPdcXmlImporter::SetContactManager(CVPbkContactManager* aContactManager)
    {
	iContactManager = aContactManager;
    }

// End of File
