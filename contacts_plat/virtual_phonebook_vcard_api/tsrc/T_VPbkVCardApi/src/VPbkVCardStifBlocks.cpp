/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  STIF test module for testing VirtualPhonebook VCard API
*
*/


#include "T_VPbkVCardApi.h"

#include <barsread.h>
#include <CVPbkVCardEng.h>
#include <CVPbkContactManager.h>
#include <CVPbkContactStoreUriArray.h>
#include <TVPbkContactStoreUriPtr.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStore.h>
#include <TVPbkFieldTypeMapping.h>
#include <MVPbkContactOperationBase.h>
#include <T_VPbkVCardApi.rsg>
#include <MVPbkStoreContact.h>

_LIT( KCntDbUri, "cntdb://c:testcontacts.cdb" );
_LIT( KResourceFileName, "\\bctestlog\\testvcard.vcf" );
_LIT( KCompactBCardFileName, "\\bctestlog\\testcbcard.ccf" );    

// -----------------------------------------------------------------------------
// CVPbkVCardApi::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CT_VPbkVCardApi::Delete() 
    {
//    iImportedContacts.ResetAndDestroy();
    delete iEngine;
    iEngine = NULL;             
    if ( iContactStore )
        {
        iContactStore->Close( *this );        
        }
    iContactStore = NULL;
    delete iContactManager;
    iContactManager = NULL;
    iFs.Close();
    }

// -----------------------------------------------------------------------------
// CT_VPbkVCardApi::RunMethodL
// Run specified method. Contains also table of test methods and their names.
// -----------------------------------------------------------------------------
//
TInt CT_VPbkVCardApi::RunMethodL( CStifItemParser& aItem ) 
    {
    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "CreateVCardEng", 
        		CT_VPbkVCardApi::CreateVCardEng ),
        ENTRY( "DeleteVCardEng", 
        		CT_VPbkVCardApi::DeleteVCardEng ),
        ENTRY( "ImportVCardToStore", 
        		CT_VPbkVCardApi::ImportVCardToStore ),
        ENTRY( "ImportVCardToContacts", 
        		CT_VPbkVCardApi::ImportVCardToContacts ),
        ENTRY( "ImportBusinessCardToStore", 
        		CT_VPbkVCardApi::ImportBusinessCardToStore ),
        ENTRY( "ImportBusinessCardToContacts", 
        		CT_VPbkVCardApi::ImportBusinessCardToContacts ),
        ENTRY( "SupportsFieldType", 
        		CT_VPbkVCardApi::SupportsFieldType )
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );    
    }

// -----------------------------------------------------------------------------
// CT_VPbkVCardApi::CreateVCardEng
// -----------------------------------------------------------------------------
//
TInt CT_VPbkVCardApi::CreateVCardEng()
	{
	TRAPD( err, CreateVCardEngL() );
	return err;
	}

// -----------------------------------------------------------------------------
// CT_VPbkVCardApi::CreateVCardEngL
// -----------------------------------------------------------------------------
//
void CT_VPbkVCardApi::CreateVCardEngL()
	{
    CVPbkContactStoreUriArray* uriList = CVPbkContactStoreUriArray::NewLC();
    uriList->AppendL( TVPbkContactStoreUriPtr( KCntDbUri ) );
    
    iContactManager = CVPbkContactManager::NewL( *uriList, &iFs );    
    CleanupStack::PopAndDestroy( uriList );
    
    MVPbkContactStoreList& stores = iContactManager->ContactStoresL();
    if ( stores.Count() > 0 )
        {
        iContactStore = &( stores.At( 0 ) );         
        }   
    
    iContactStore->ReplaceL( *this );

    iEngine = CVPbkVCardEng::NewL( *iContactManager );
	}

// -----------------------------------------------------------------------------
// CT_VPbkVCardApi::DeleteVCardEng
// -----------------------------------------------------------------------------
//
TInt CT_VPbkVCardApi::DeleteVCardEng()
	{
	if(ioperation != NULL)
	    {
	    delete ioperation;
	    ioperation = NULL;
	    }
    delete iEngine;
    iEngine = NULL;             
    if ( iContactStore )
        {
        iContactStore->Close( *this );        
        }
    iContactStore = NULL;
    delete iContactManager;
    iContactManager = NULL;
    
    return 0;
	}

// -----------------------------------------------------------------------------
// CT_VPbkVCardApi::StoreReady
// -----------------------------------------------------------------------------
//
void CT_VPbkVCardApi::StoreReady( MVPbkContactStore& /*aContactStore*/ )
    {    
    // Signal STIF test framework to continue processing the script file
    CScriptBase::Signal( 0 );
    }

// -----------------------------------------------------------------------------
// CT_VPbkVCardApi::StoreUnavailable
// -----------------------------------------------------------------------------
//
void CT_VPbkVCardApi::StoreUnavailable( 
        MVPbkContactStore& /*aContactStore*/,
        TInt aError )
    {    
    // Signal STIF test framework to continue processing the script file
    CScriptBase::Signal( aError );
    }

// -----------------------------------------------------------------------------
// CT_VPbkVCardApi::HandleStoreEventL
// -----------------------------------------------------------------------------
//
void CT_VPbkVCardApi::HandleStoreEventL(
        MVPbkContactStore& /*aContactStore*/, 
        TVPbkContactStoreEvent /*aStoreEvent*/ )
    {
    }

// -----------------------------------------------------------------------------
// CT_VPbkVCardApi::OpenComplete
// -----------------------------------------------------------------------------
//
void CT_VPbkVCardApi::OpenComplete()
    {    
    // Signal STIF test framework to continue processing the script file
    CScriptBase::Signal( 0 );
    }

// -----------------------------------------------------------------------------
// CT_VPbkVCardApi::ContactOperationCompleted
// -----------------------------------------------------------------------------
//
void CT_VPbkVCardApi::ContactOperationCompleted( 
        TContactOpResult /*aResult*/ )
    {    
    // Signal STIF test framework to continue processing the script file
    CScriptBase::Signal( 0 );
    }

// -----------------------------------------------------------------------------
// CT_VPbkVCardApi::ContactOperationFailed
// -----------------------------------------------------------------------------
//
void CT_VPbkVCardApi::ContactOperationFailed(
        TContactOp /*aOpCode*/, TInt aErrorCode, 
        TBool /*aErrorNotified*/ )
    {    
    // Signal STIF test framework to continue processing the script file
    CScriptBase::Signal( aErrorCode );
    }

// -----------------------------------------------------------------------------
// CT_VPbkVCardApi::VPbkSingleContactOperationComplete
// -----------------------------------------------------------------------------
//
void CT_VPbkVCardApi::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& /*aOperation*/,
        MVPbkStoreContact* )
	{
    delete ioperation;
    ioperation = NULL;
    // Signal STIF test framework to continue processing the script file
    CScriptBase::Signal( 0 );
    }

// -----------------------------------------------------------------------------
// CT_VPbkVCardApi::VPbkSingleContactOperationFailed
// -----------------------------------------------------------------------------
//
void CT_VPbkVCardApi::VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& /*aOperation*/, TInt aError )
    {    
    delete ioperation;
    ioperation = NULL;
    // Signal STIF test framework to continue processing the script file
    CScriptBase::Signal( aError );
    }

// -----------------------------------------------------------------------------
// CT_VPbkVCardApi::FieldAddedToContact
// -----------------------------------------------------------------------------
//
void CT_VPbkVCardApi::FieldAddedToContact(
        MVPbkContactOperationBase& /*aOperation*/ )
    {    
    }
        
// -----------------------------------------------------------------------------
// CT_VPbkVCardApi::FieldAddingFailed
// -----------------------------------------------------------------------------
//
void CT_VPbkVCardApi::FieldAddingFailed(
        MVPbkContactOperationBase& /*aOperation*/, TInt /*aError*/ )
    {    
    }
        
// -----------------------------------------------------------------------------
// CT_VPbkVCardApi::ContactsSaved
// -----------------------------------------------------------------------------
//
void CT_VPbkVCardApi::ContactsSaved( MVPbkContactOperationBase& /*aOp*/,
        MVPbkContactLinkArray* /*aResults*/ )
    {       
    // Signal STIF test framework to continue processing the script file
    CScriptBase::Signal( 0 );
    }
        
// -----------------------------------------------------------------------------
// CT_VPbkVCardApi::ContactsSavingFailed
// -----------------------------------------------------------------------------
//
void CT_VPbkVCardApi::ContactsSavingFailed(
        MVPbkContactOperationBase& /*aOperation*/, TInt aError )
    {
    // Signal STIF test framework to continue processing the script file
    CScriptBase::Signal( aError );
    }

// -----------------------------------------------------------------------------
// CT_VPbkVCardApi::ImportVCardToStore
// -----------------------------------------------------------------------------
//
TInt CT_VPbkVCardApi::ImportVCardToStore()
	{
	TRAPD( err, ImportVCardToStoreL() );
	return err;
	}

// -----------------------------------------------------------------------------
// CT_VPbkVCardApi::ImportVCardToStoreL
// -----------------------------------------------------------------------------
//
void CT_VPbkVCardApi::ImportVCardToStoreL()
    {   
    User::LeaveIfError( iSource.Open( 
    		iFs, KResourceFileName, EFileRead ) );    
    ioperation = 
	    iEngine->ImportVCardL( *iContactStore, iSource, *this );
    }

// -----------------------------------------------------------------------------
// CT_VPbkVCardApi::ImportVCardToContacts
// -----------------------------------------------------------------------------
//
TInt CT_VPbkVCardApi::ImportVCardToContacts()
	{
	TRAPD( err, ImportVCardToContactsL() );
	return err;
	}

// -----------------------------------------------------------------------------
// CT_VPbkVCardApi::ImportVCardToContactsL
// -----------------------------------------------------------------------------
//
void CT_VPbkVCardApi::ImportVCardToContactsL()
	{
    User::LeaveIfError( iSource.Open( 
    		iFs, KResourceFileName, EFileRead ) );	
    ioperation = 
	    iEngine->ImportVCardL( iImportedContacts, *iContactStore, 
	        iSource, *this );
	}

// -----------------------------------------------------------------------------
// CT_VPbkVCardApi::ImportBusinessCardToStore
// -----------------------------------------------------------------------------
//
TInt CT_VPbkVCardApi::ImportBusinessCardToStore()
	{
	TRAPD( err, ImportBusinessCardToStoreL() )
	return err;
	}

// -----------------------------------------------------------------------------
// CT_VPbkVCardApi::ImportBusinessCardToStoreL
// -----------------------------------------------------------------------------
//
void CT_VPbkVCardApi::ImportBusinessCardToStoreL()
	{
    User::LeaveIfError( iSource.Open( 
    		iFs, KCompactBCardFileName, EFileRead ) );
    ioperation = 
	    iEngine->ImportCompactBusinessCardL( *iContactStore, iSource, *this );
	}

// -----------------------------------------------------------------------------
// CT_VPbkVCardApi::ImportBusinessCardToContacts
// -----------------------------------------------------------------------------
//
TInt CT_VPbkVCardApi::ImportBusinessCardToContacts()
	{
	TRAPD( err, ImportBusinessCardToContactsL() );
	return err;
	}

// -----------------------------------------------------------------------------
// CT_VPbkVCardApi::ImportBusinessCardToContactsL
// -----------------------------------------------------------------------------
//
void CT_VPbkVCardApi::ImportBusinessCardToContactsL()
	{
    User::LeaveIfError( iSource.Open( 
        iFs, KCompactBCardFileName, EFileRead ) );	
    ioperation = 
	    iEngine->ImportCompactBusinessCardL( iImportedContacts, 
	        *iContactStore, iSource, *this );
	}

// -----------------------------------------------------------------------------
// CT_VPbkVCardApi::SupportsFieldType
// -----------------------------------------------------------------------------
//
TInt CT_VPbkVCardApi::SupportsFieldType()
	{
	TResourceReader reader;
    iEikEnv->CreateResourceReaderLC( reader, 
        R_ORGNAME_TYPE ); 

    TVPbkFieldTypeMapping typeMapping( reader );
    const MVPbkFieldTypeList& fieldTypeList = iContactManager->FieldTypes();
    const MVPbkFieldType* fieldType = typeMapping.FindMatch( fieldTypeList );
    iEngine->SupportsFieldType( *fieldType );
	
	CleanupStack::PopAndDestroy();
	
	return 0;
	}

// End of File
