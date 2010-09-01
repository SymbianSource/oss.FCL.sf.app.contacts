/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:        ?Description
*
*/









// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "BCTestPbkUIUtil.h"

#include <RPbkViewResourceFile.h>
#include <CPbkViewState.h>
#include <coemain.h> 
#include <CPbkContactItem.h>
#include <CPbkContactEngine.h>
#include <CPbkFieldInfo.h>
#include <CPbkFieldsInfo.h>
#include <eikenv.h> 
#include <eikappui.h> 
#include <s32file.h>
#include <PbkIconInfo.h>
#include <COEMAIN.H>
#include <barsread.h>
#include <aknsutils.h> 
#include <GULICON.H>
#include <e32def.h>

//  INTERNAL INCLUDES
#include "CPbkViewState30.h"
#include "CPbkFieldData.h"//Not found in SDK, for testing copied in test folder

// CONSTANTS
_LIT(KExternalizeFileName,"c:\\MT_CPbkViewStateExternalize.dat");
_LIT(KNewLFileName,"c:\\MT_CPbkViewStateNewLFile.dat");

const TContactItemId KItemId2 = 2;
const TContactItemId KItemId5 = 5;
const TContactItemId KItemId6 = 6;
const TContactItemId KItemId8 = 8;

const TContactItemId KSelectedItemId = 5;
const TInt KIndexId = 99;
const TInt KIndexNotSet = -1;

// CPbkViewState Uid used with the view server
// PbkUID.h is not public, for testing redefined here
#define KPbkViewStateUid_Test 0x101f4ccf 
    

enum TPanicCodes
    {
    KPanicUnspportedFieldType = 1
    };
    


// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CBCTestPbkUIUtil::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CBCTestPbkUIUtil::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CBCTestPbkUIUtil::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CBCTestPbkUIUtil::RunMethodL( 
    CStifItemParser& aItem ) 
    {

     TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "NewLtest", CBCTestPbkUIUtil::MT_CPbkViewState_NewLL ),
        ENTRY( "NewLCtest", CBCTestPbkUIUtil::MT_CPbkViewState_NewLCL ),
        ENTRY( "Uidtest", CBCTestPbkUIUtil::MT_CPbkViewState_UidL),
        ENTRY( "TDataTypeenumtest", CBCTestPbkUIUtil::MT_CPbkViewState_TDataTypeEnumL),
        ENTRY( "TDataTypeenumtest", CBCTestPbkUIUtil::MT_CPbkViewState_TFlagsEnumL),
        ENTRY( "SetandGetFocusedContactIdtest", CBCTestPbkUIUtil::MT_CPbkViewState_SetGetFocusedContactIdL ),
        ENTRY( "SetandGetTopContactIdtest", CBCTestPbkUIUtil::MT_CPbkViewState_SetGetTopContactIdL ),
        ENTRY( "SetandGetMarkedContactIdstest", CBCTestPbkUIUtil::MT_CPbkViewState_SetGetMarkedContactIdsL ),
        ENTRY( "SetandGetFocusedFieldIndextest", CBCTestPbkUIUtil::MT_CPbkViewState_SetGetFocusedFieldIndexL ),
        ENTRY( "SetandGetTopFieldIndextest", CBCTestPbkUIUtil::MT_CPbkViewState_SetGetTopFieldIndexL ),
        ENTRY( "SetandGetFieldDataArraytest", CBCTestPbkUIUtil::MT_CPbkViewState_SetGetFieldDataArrayL ),
        ENTRY( "Resettest", CBCTestPbkUIUtil::MT_CPbkViewState_ResetL ),
        ENTRY( "SetandGetParentContactIdtest", CBCTestPbkUIUtil::MT_CPbkViewState_SetGetParentContactIdL ),
        ENTRY( "SetandGetFlagstest", CBCTestPbkUIUtil::MT_CPbkViewState_SetGetFlagsL ),
        ENTRY( "PackLandUnpackLtest", CBCTestPbkUIUtil::MT_CPbkViewState_PackLAndUnpackLL ),
        ENTRY( "PackLCtest", CBCTestPbkUIUtil::MT_CPbkViewState_PackLCL ),
        ENTRY( "InternalizeLtest", CBCTestPbkUIUtil::MT_CPbkViewState_InternalizeLL ),
        ENTRY( "ExternalizeLtest", CBCTestPbkUIUtil::MT_CPbkViewState_ExternalizeLL ),
        ENTRY( "operator==test", CBCTestPbkUIUtil::MT_CPbkViewState_operatorL ),
        ENTRY( "NewL(RReadStream)test", CBCTestPbkUIUtil::MT_CPbkViewState_NewL_StreamL ),
        ENTRY( "NewLC(RReadStream)test", CBCTestPbkUIUtil::MT_CPbkViewState_NewLC_StreamL ),
        ENTRY( "NewL(TDesC8)test", CBCTestPbkUIUtil::MT_CPbkViewState_NewL_TDesCL ),
        ENTRY( "NewLC(TDesC8)test", CBCTestPbkUIUtil::MT_CPbkViewState_NewLC_TDesCL ),
        ENTRY( "TestconstructionwithCCoeEnv", CBCTestPbkUIUtil::RunConstructContainerWithCoeEnvTestL ),
        ENTRY( "TestconstructionwithoutCCoeEnv", CBCTestPbkUIUtil::RunConstructContainerWithoutCoeEnvTestL ),
        ENTRY( "TestfindingTPbkIconInfo", CBCTestPbkUIUtil::RunFindTestL ),
        ENTRY( "Testloadingbitmap", CBCTestPbkUIUtil::RunLoadBitmapLTestL ),
        ENTRY( "Testgettingiconid", CBCTestPbkUIUtil::RunPbkIconInfoIdTestL ),
        ENTRY( "Testgettingbitmapfilename", CBCTestPbkUIUtil::RunMbmFileNameTestL ),
        ENTRY( "Testgettingiconid", CBCTestPbkUIUtil::RunIconIdTestL ),
        ENTRY( "Testgettingicon'smaskid", CBCTestPbkUIUtil::RunMaskIdTestL ),
        ENTRY( "Testgettingicon'sskinid", CBCTestPbkUIUtil::RunSkinIdTestL ),
        ENTRY( "Testcreatingbitmapandmaskforanicon", CBCTestPbkUIUtil::RunCreateIconLTestL ),
        ENTRY( "Testcreatingbitmapandmaskforanicon", CBCTestPbkUIUtil::RunCreateIconLCTestL ),
        ENTRY( "OpenResourcefile", CBCTestPbkUIUtil::RunLoadResourceTestL ),
        ENTRY( "SizeofClassnotlessthaninv30", CBCTestPbkUIUtil::RunTest_ClassSizeL ),
        
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

/*******************************************************************************
* SETUP METHODS
*******************************************************************************/    


void CBCTestPbkUIUtil::EmptySetup(  )
    {
    }

void CBCTestPbkUIUtil::SetupL(  )
    {
    iViewState = CPbkViewState::NewL();  
    }

void CBCTestPbkUIUtil::Teardown(  )
    {
    delete iViewState;
    iViewState = NULL; 
    }

void CBCTestPbkUIUtil::EmptyTeardown(  )
    {
    }


/*******************************************************************************
* HELPER METHODS
*******************************************************************************/    
    
    
/****************************************************************
* File created here will be used with NewL(RWriteStream) 
* and NewL(TDesC) tests
****************************************************************/
void CBCTestPbkUIUtil::CreateFileForTestingL(  )
    {
    CPbkViewState* viewState = CPbkViewState::NewL();  
    CleanupStack::PushL( viewState );

    RFileWriteStream fileWriteStream;
    fileWriteStream.PushL();//to CleanupStack
    
    //data for adding to state
    TInt flags = 0x10101010;
    CContactIdArray* contactIds = CContactIdArray::NewL();
    CleanupStack::PushL( contactIds );
    contactIds->AddL( KItemId2 ); 
    contactIds->AddL( KItemId5 ); 
    contactIds->AddL( KItemId6 ); 
    contactIds->AddL( KItemId8 );     
    CPbkFieldDataArray* array = CPbkFieldDataArray::NewL();
    CleanupStack::PushL( array );    

    //
    //Writing full state to file
    //

    viewState->SetFocusedContactId( KSelectedItemId );    
    viewState->SetTopContactId( KSelectedItemId );
    viewState->SetMarkedContactIds( CContactIdArray::NewL( contactIds ) );
    viewState->SetFocusedFieldIndex( KIndexId );
    viewState->SetTopFieldIndex( KIndexId );
    CleanupStack::Pop( array );
    viewState->SetFieldDataArray( array );
    viewState->SetParentContactId( KSelectedItemId );
    viewState->SetFlags( flags );

    TInt fileCreated = fileWriteStream.Replace( iFs, KNewLFileName, EFileWrite );
    ASSERT(fileCreated == KErrNone)    ;    
    viewState->ExternalizeL( fileWriteStream );
    fileWriteStream.CommitL();

    CleanupStack::PopAndDestroy( contactIds );

    fileWriteStream.Close();
    fileWriteStream.Pop();
    
    CleanupStack::PopAndDestroy( viewState );
    
    }    
    
    
    
    
/*******************************************************************************
* TESTS
*******************************************************************************/    


/****************************************************************
* Testing NewL
****************************************************************/
TInt CBCTestPbkUIUtil::MT_CPbkViewState_NewLL(  )
    {
    CPbkViewState* viewState = CPbkViewState::NewL();
    delete viewState;
    viewState = NULL; 
    return KErrNone;     
    }

/****************************************************************
* Testing NewLC
****************************************************************/
TInt CBCTestPbkUIUtil::MT_CPbkViewState_NewLCL(  )
    {
    CPbkViewState* viewState = CPbkViewState::NewLC() ;
    CleanupStack::Pop( viewState );
    delete viewState;
    viewState = NULL; 
    return KErrNone;      
    }

/****************************************************************
* Testing NewL(RReadStream& aStream) NOTE: 
****************************************************************/
TInt CBCTestPbkUIUtil::MT_CPbkViewState_NewL_StreamL(  )
    {
    
    CreateFileForTestingL();
    
    RFileReadStream fileReadStream;
    fileReadStream.PushL();//To CleanupStack       
    
    TInt fileOpened = fileReadStream.Open( iFs, KNewLFileName, EFileRead ); 
    ASSERT(fileOpened == KErrNone);      
        
    CPbkViewState* viewState = CPbkViewState::NewL( fileReadStream );
    
    fileReadStream.Close();
    fileReadStream.Pop();
    delete viewState;
    viewState = NULL; 
    return KErrNone;        
       
    }

/****************************************************************
* Testing NewLC(RReadStream& aStream) 
****************************************************************/
TInt CBCTestPbkUIUtil::MT_CPbkViewState_NewLC_StreamL(  )
    {
    CreateFileForTestingL();
    RFileReadStream fileReadStream;
    fileReadStream.PushL();//To CleanupStack       
    
    TInt fileOpened = fileReadStream.Open( iFs, KNewLFileName, EFileRead ); 
    ASSERT( fileOpened == KErrNone );              
        
    CPbkViewState* viewState = CPbkViewState::NewLC( fileReadStream ) ;
    
    CleanupStack::Pop( viewState ); 
    
    fileReadStream.Close();
    fileReadStream.Pop();
    delete viewState;
    viewState = NULL; 
    return KErrNone;        
       
    }

/****************************************************************
* Testing NewL(const TDesC8& aBuf) 
****************************************************************/
TInt CBCTestPbkUIUtil::MT_CPbkViewState_NewL_TDesCL(  )
    {
    CreateFileForTestingL();
    
    RFileReadStream fileReadStream;
    fileReadStream.PushL();//To CleanupStack       
    
    TInt fileOpened = fileReadStream.Open( iFs, KNewLFileName, EFileRead ); 
    ASSERT(fileOpened == KErrNone);
    
    TBuf8<10000> buf;
    fileReadStream.ReadL( buf, fileReadStream.Source()->SizeL() );
    
    CPbkViewState* viewState = CPbkViewState::NewL( buf );
    
    fileReadStream.Close();
    fileReadStream.Pop();
    delete viewState;
    viewState = NULL;  
    return KErrNone;         
    }

/****************************************************************
* Testing NewLC(const TDesC8& aBuf) 
****************************************************************/
TInt CBCTestPbkUIUtil::MT_CPbkViewState_NewLC_TDesCL(  )
    {
    CreateFileForTestingL();
    
    RFileReadStream fileReadStream;
    fileReadStream.PushL();//To CleanupStack       
    
    
    TInt fileOpened = fileReadStream.Open( iFs, KNewLFileName, EFileRead ); 
    ASSERT(fileOpened == KErrNone);
    
    TBuf8<10000> buf;
    fileReadStream.ReadL( buf, fileReadStream.Source()->SizeL() );
       
    CPbkViewState* viewState = CPbkViewState::NewLC( buf ) ;

    CleanupStack::Pop( viewState ); 

    
    fileReadStream.Close();
    fileReadStream.Pop();
    delete viewState;
    viewState = NULL;  
    return KErrNone; 
    }


/****************************************************************
* Testing Uid
****************************************************************/
TInt CBCTestPbkUIUtil::MT_CPbkViewState_UidL(  )
    {
    SetupL();
    // PbkUID.h is not public, uid for testing redefined
    ASSERT( KPbkViewStateUid_Test == iViewState->Uid( ).iUid );
    Teardown(); 
    return KErrNone; 
    }


/****************************************************************
* Testing TDataType enum
****************************************************************/
TInt CBCTestPbkUIUtil::MT_CPbkViewState_TDataTypeEnumL()
    {
    ASSERT( TDataType30( EEnd ) == 
        CPbkViewState::TDataType( EEnd ) );
    ASSERT( TDataType30( EFocusedContactId ) == 
        CPbkViewState::TDataType( EFocusedContactId ) );
    ASSERT( TDataType30( ETopContactId ) == 
        CPbkViewState::TDataType( ETopContactId ) );
    ASSERT( TDataType30( EMarkedContactIds ) == 
        CPbkViewState::TDataType( EMarkedContactIds ) );
    ASSERT( TDataType30( EFocusedFieldIndex ) == 
        CPbkViewState::TDataType( EFocusedFieldIndex ) );
    ASSERT( TDataType30( ETopFieldIndex ) == 
        CPbkViewState::TDataType( ETopFieldIndex ) );
    ASSERT( TDataType30( EFieldDataArray ) == 
        CPbkViewState::TDataType( EFieldDataArray ) );
    ASSERT( TDataType30( EParentContactId ) == 
        CPbkViewState::TDataType( EParentContactId ) );
    ASSERT( TDataType30( EFlags ) == 
        CPbkViewState::TDataType( EFlags ) ); 
    return KErrNone; 
    }

/****************************************************************
* Testing TFlags enum
****************************************************************/
TInt CBCTestPbkUIUtil::MT_CPbkViewState_TFlagsEnumL()
    {
    ASSERT( TFlags30( ENullFlags ) == 
        CPbkViewState::TFlags( ENullFlags ) );
    ASSERT( TFlags30( EFocusFirst ) == 
        CPbkViewState::TFlags( EFocusFirst ) );
    ASSERT( TFlags30( EFocusLast ) == 
        CPbkViewState::TFlags( EFocusLast ) );
    ASSERT( TFlags30( EInitialized ) == 
        CPbkViewState::TFlags( EInitialized ) );
    ASSERT( TFlags30( ESendToBackground ) == 
        CPbkViewState::TFlags( ESendToBackground ) ); 
    return KErrNone; 
    }


/****************************************************************
* SetFocusedContactId and FocusedContactId
****************************************************************/
TInt CBCTestPbkUIUtil::MT_CPbkViewState_SetGetFocusedContactIdL(  )
    {
    CPbkViewState* viewState = CPbkViewState::NewL();
    CleanupStack::PushL( viewState );
    viewState->SetFocusedContactId( KSelectedItemId ) ;
    const TContactItemId itemId = viewState->FocusedContactId();
    ASSERT( itemId == KSelectedItemId );
    CleanupStack::Pop( viewState );
    delete viewState;
    viewState = NULL;      
    return KErrNone; 
    }

/****************************************************************
* SetTopContactId and TopContactId
****************************************************************/
TInt CBCTestPbkUIUtil::MT_CPbkViewState_SetGetTopContactIdL(  )
    {
    CPbkViewState* viewState = CPbkViewState::NewL();
    CleanupStack::PushL( viewState );
    viewState->SetTopContactId( KSelectedItemId );
    const TContactItemId itemId = viewState->TopContactId();
    ASSERT( itemId == KSelectedItemId );
    CleanupStack::Pop( viewState );
    delete viewState;
    viewState = NULL;      
    return KErrNone; 
    }

/****************************************************************
* SetMarkedContactIds and MarkedContactIds
****************************************************************/
TInt CBCTestPbkUIUtil::MT_CPbkViewState_SetGetMarkedContactIdsL(  )
    {
    SetupL();
    CContactIdArray* contactIds = CContactIdArray::NewL();
    CleanupStack::PushL( contactIds );
    
    // define first some items to mark
    contactIds->AddL( KItemId2 ); 
    contactIds->AddL( KItemId5 ); 
    contactIds->AddL( KItemId6 ); 
    contactIds->AddL( KItemId8 ); 

    iViewState->SetFocusedContactId( KSelectedItemId );
    
    TRAPD( err, iViewState->SetMarkedContactIds( 
        CContactIdArray::NewL( contactIds ) ) );
    ASSERT( err == KErrNone );        
    
    CContactIdArray* ret = iViewState->MarkedContactIds();
    
    ASSERT( contactIds->Count() == ret->Count() );
    ASSERT( contactIds->Find( KItemId2 ) == ret->Find( KItemId2 ) );
    ASSERT( contactIds->Find( KItemId5 ) == ret->Find( KItemId5 ) );
    ASSERT( contactIds->Find( KItemId6 ) == ret->Find( KItemId6 ) );
    ASSERT( contactIds->Find( KItemId8 ) == ret->Find( KItemId8 ) );
 
    CleanupStack::Pop( contactIds );
    delete contactIds;
    contactIds = NULL; 
    Teardown(); 
    return KErrNone; 
    }

/****************************************************************
* SetFocusedFieldIndex and FocusedFieldIndex
****************************************************************/
TInt CBCTestPbkUIUtil::MT_CPbkViewState_SetGetFocusedFieldIndexL(  )
    {
    SetupL();
    iViewState->SetFocusedFieldIndex( KIndexId );
    TInt itemId = iViewState->FocusedFieldIndex();
    ASSERT( itemId == KIndexId );
    Teardown(); 
    return KErrNone; 
    }

/****************************************************************
* SetTopFieldIndex and TopFieldIndex
****************************************************************/
TInt CBCTestPbkUIUtil::MT_CPbkViewState_SetGetTopFieldIndexL(  )
    {
    SetupL();
    iViewState->SetTopFieldIndex( KIndexId );
    TInt fieldId = iViewState->TopFieldIndex();
    ASSERT( fieldId == KIndexId );
    Teardown(); 
    return KErrNone; 
    }

/****************************************************************
* SetFieldDataArray and FieldDataArray
****************************************************************/
TInt CBCTestPbkUIUtil::MT_CPbkViewState_SetGetFieldDataArrayL(  )
    {
    SetupL();
    //CPbkFieldData.h not found in SDK, for testing copied in test folder
    //CPbkFieldDataArray is defined in there
    
    CPbkFieldDataArray* array = CPbkFieldDataArray::NewL();    
    iViewState->SetFieldDataArray( array );
    CPbkFieldDataArray* ret = iViewState->FieldDataArray();
    ASSERT( array == ret );    

    //Setting again, so replacement will be tested also
    CPbkFieldDataArray* array2 = CPbkFieldDataArray::NewL();    
    iViewState->SetFieldDataArray( array2 );
    CPbkFieldDataArray* ret2 = iViewState->FieldDataArray();
    ASSERT( array2 == ret2 );    
	Teardown(); 
    return KErrNone; 
    }

/****************************************************************
* Reset
****************************************************************/
TInt CBCTestPbkUIUtil::MT_CPbkViewState_ResetL(  )
    {
    SetupL();
    //Setting datas    
    iViewState->SetFocusedContactId( KSelectedItemId );
    iViewState->SetTopContactId( KSelectedItemId );
    iViewState->SetFocusedFieldIndex( KIndexId );
    iViewState->SetTopFieldIndex( KIndexId );
    // define first some items to mark
    CContactIdArray* contactIds = CContactIdArray::NewL();
    CleanupStack::PushL( contactIds );
    contactIds->AddL( KItemId2 ); 
    contactIds->AddL( KItemId5 ); 
    contactIds->AddL( KItemId6 ); 
    contactIds->AddL( KItemId8 );
    const TInt count = contactIds->Count();
    iViewState->SetMarkedContactIds(CContactIdArray::NewL( contactIds ) );
    CPbkFieldDataArray* array = CPbkFieldDataArray::NewL();//calling Reset() will delete array
    iViewState->SetFieldDataArray( array );
    iViewState->SetParentContactId( KSelectedItemId );
    
    //Verify that all data to be resetted is set
    ASSERT( iViewState->FocusedContactId() == KSelectedItemId);
    ASSERT( iViewState->TopContactId() == KSelectedItemId );
    ASSERT( iViewState->FocusedFieldIndex() == KIndexId );
    ASSERT( iViewState->TopFieldIndex() == KIndexId );
    ASSERT( iViewState->MarkedContactIds()->Count() == count );
    ASSERT( iViewState->FieldDataArray() == array );
    
    ASSERT( iViewState->ParentContactId() == KSelectedItemId );
    
    //reset
    iViewState->Reset();
    
    //verify that all data was resetted
    ASSERT( iViewState->FocusedContactId() == KNullContactId );
    ASSERT( iViewState->TopContactId() == KNullContactId );
    ASSERT( iViewState->FocusedFieldIndex() == KIndexNotSet );
    ASSERT( iViewState->TopFieldIndex() == KIndexNotSet );
    ASSERT( !iViewState->MarkedContactIds() );// == NULL
    ASSERT( !iViewState->FieldDataArray() );// == NULL
    ASSERT( iViewState->ParentContactId() == KNullContactId );

    CleanupStack::Pop( contactIds );
    delete contactIds;
    contactIds = NULL;  
    Teardown();   
    return KErrNone; 
    }

/****************************************************************
* SetParentContactId and ParentContactId
****************************************************************/
TInt CBCTestPbkUIUtil::MT_CPbkViewState_SetGetParentContactIdL(  )
    {
    SetupL();
    iViewState->SetParentContactId( KSelectedItemId );
    TContactItemId parentId = iViewState->ParentContactId();
    ASSERT( parentId == KSelectedItemId ); 
    Teardown();       
    return KErrNone; 
    }

/****************************************************************
* SetFlagsL and FlagsL
****************************************************************/
TInt CBCTestPbkUIUtil::MT_CPbkViewState_SetGetFlagsL(  )
    {
    SetupL();
    TInt flags = 0x10101010;
    iViewState->SetFlags( flags );
    TInt ret = iViewState->Flags();
    ASSERT( flags == ret );    
    Teardown();    
    return KErrNone; 
    }

/****************************************************************
* PackL and UnpackL
****************************************************************/
TInt CBCTestPbkUIUtil::MT_CPbkViewState_PackLAndUnpackLL(  )
    {
    //UnpackL function also tested through NewL / NewLC(const TDesC8& aBuf) 
    //which uses UnpackL
    
    SetupL();
    HBufC8* pack = iViewState->PackL();
    CleanupStack::PushL( pack );
    iViewState->UnpackL( *pack );
    HBufC8* pack2 = iViewState->PackL();
    CleanupStack::PushL( pack2 );
    
    ASSERT( *pack == *pack2 );
    
    CPbkViewState* state = CPbkViewState::NewL();
    CleanupStack::PushL( state );
    state->SetFocusedContactId( KSelectedItemId );
    HBufC8* diffPack = state->PackL();
    CleanupStack::PushL( diffPack );
    ASSERT( !( *diffPack == *pack ) );

    iViewState->UnpackL( *diffPack );
    HBufC8* pack3 = iViewState->PackL();
    CleanupStack::PushL(pack3);
    ASSERT( *diffPack == *pack3 );

    CleanupStack::PopAndDestroy( pack3 );
    CleanupStack::PopAndDestroy( diffPack );
    CleanupStack::PopAndDestroy( state );    
    CleanupStack::PopAndDestroy( pack2 );
    CleanupStack::PopAndDestroy( pack );
	Teardown();    
    return KErrNone; 
    }

/****************************************************************
* PackLC
****************************************************************/
TInt CBCTestPbkUIUtil::MT_CPbkViewState_PackLCL(  )
    {
    SetupL();
    HBufC8* pack = iViewState->PackLC();    
    iViewState->UnpackL( *pack );
    HBufC8* pack2 = iViewState->PackLC();    
    ASSERT( *pack == *pack2 );

    CleanupStack::PopAndDestroy( pack2 );
    CleanupStack::PopAndDestroy( pack );
    Teardown();    
    return KErrNone; 
    }

/****************************************************************
* ExternalizeL, NewL(RReadStream) and InternalizeL
* 
* File created here will be used with NewL(RWriteStream) 
* and NewL(TDesC) tests, so be sure it will stay after, or those 
* tests will fail
****************************************************************/
TInt CBCTestPbkUIUtil::MT_CPbkViewState_ExternalizeLL(  )
    {
    SetupL();
    RFileWriteStream fileWriteStream;
    RFileReadStream fileReadStream;
    fileWriteStream.PushL();//to CleanupStack
    fileReadStream.PushL();//To CleanupStack           
    
    //data for adding to state
    TInt flags = 0x10101010;
    CContactIdArray* contactIds = CContactIdArray::NewL();
    CleanupStack::PushL( contactIds );
    contactIds->AddL( KItemId2 ); 
    contactIds->AddL( KItemId5 ); 
    contactIds->AddL( KItemId6 ); 
    contactIds->AddL( KItemId8 );     
    CPbkFieldDataArray* array = CPbkFieldDataArray::NewL();
    CleanupStack::PushL( array );
    
    //
    //Writing empty state to file
    //
    
    //if file exist, will be replaced
    TInt fileCreated = fileWriteStream.Replace( iFs, KExternalizeFileName, EFileWrite );
    ASSERT( fileCreated == KErrNone )  ;      
    iViewState->ExternalizeL( fileWriteStream );
    fileWriteStream.CommitL();
    fileWriteStream.Release();//release stream to file, so it can be opened to read
        
    TInt fileOpened = fileReadStream.Open( iFs, KExternalizeFileName, EFileRead ); 
    ASSERT( fileOpened == KErrNone );      
        
    //NewL(RWriteStream) will call InternalizeL() when creating new state,
    //so Internalize will be tested too, and Externalize will be verified
    CPbkViewState* state = CPbkViewState::NewL( fileReadStream );    
    fileReadStream.Release();//release stream to file, so it can be opened to write
    
    //Orginal state vs. new state created with Externalized and readed stream
    ASSERT( *iViewState == *state );
    delete state;
    state = NULL;    




    //
    //Writing full state to file
    //

    iViewState->SetFocusedContactId( KSelectedItemId );    
    iViewState->SetTopContactId( KSelectedItemId );
    iViewState->SetMarkedContactIds( CContactIdArray::NewL( contactIds ) );
    iViewState->SetFocusedFieldIndex( KIndexId );
    iViewState->SetTopFieldIndex( KIndexId );
    CleanupStack::Pop( array );
    iViewState->SetFieldDataArray( array );
    iViewState->SetParentContactId( KSelectedItemId );
    iViewState->SetFlags( flags );

    fileCreated = fileWriteStream.Replace( iFs, KExternalizeFileName, EFileWrite );
    ASSERT(fileCreated == KErrNone)  ;      
    iViewState->ExternalizeL( fileWriteStream );
    fileWriteStream.CommitL();
    fileWriteStream.Release();//release stream to file, so it can be opened to read
        
    fileOpened = fileReadStream.Open( iFs, KExternalizeFileName, EFileRead ); 
    ASSERT( fileOpened == KErrNone );      
        
    //NewL(RWriteStream) will call InternalizeL() when creating new state,
    //so Internalize will be tested too, and Externalize will be verified
    state = CPbkViewState::NewL( fileReadStream );    
    fileReadStream.Release();//release stream to file, so it can be opened to write
    
    //Orginal state vs. new state created with Externalized and readed stream
    ASSERT( *iViewState == *state );    

    delete state;
    state = NULL;  

    CleanupStack::PopAndDestroy( contactIds );

    fileReadStream.Close();
    fileReadStream.Pop();
    fileWriteStream.Close();
    fileWriteStream.Pop();
    Teardown();    
    return KErrNone; 
    }

/****************************************************************
* InternalizeL
****************************************************************/
TInt CBCTestPbkUIUtil::MT_CPbkViewState_InternalizeLL(  )
    {
    SetupL();
    //Internalize function also tested through Externalize and 
    //NewL / NewLC(RReadStream& aStream) 
    //which uses Internalize 
    //Here testing only by creating viewState from "scratch"    
    
    RFileWriteStream fileWriteStream;
    RFileReadStream fileReadStream;
    fileWriteStream.PushL();//to CleanupStack
    fileReadStream.PushL();//To CleanupStack       


    //
    // Creating state by "self"
    // Firstly create a stream and internalize
    //
    
    TBuf8<16> param;    
    RDesWriteStream stream( param );
    
    stream.PushL();
    stream.WriteInt8L( EFocusedContactId ); 
    stream.WriteInt32L( KItemId2 );  
    stream.WriteInt8L( EFocusedFieldIndex );
    stream.WriteInt32L( KItemId8 );

    
    stream.CommitL();
    CleanupStack::PopAndDestroy( &stream );

    RDesReadStream rStream( param );
    rStream.PushL();
    
    TRAPD( err, iViewState->InternalizeL( rStream ) );
    ASSERT( err == KErrNone );
    
    rStream.Close();
    CleanupStack::PopAndDestroy( &rStream );
    
    //
    // Then externalize to file
    //
    
    TInt fileCreated = fileWriteStream.Replace( iFs, KExternalizeFileName, EFileWrite );
    ASSERT( fileCreated == KErrNone ) ;       
    iViewState->ExternalizeL( fileWriteStream );
    fileWriteStream.CommitL();
    fileWriteStream.Release();//release stream to file, so it can be opened to read
        
    //
    // Lastly internalize from file and compare
    //
        
    TInt fileOpened = fileReadStream.Open( iFs, KExternalizeFileName, EFileRead ); 
    ASSERT( fileOpened == KErrNone );      
    CPbkViewState* state = CPbkViewState::NewL( fileReadStream );
    CleanupStack::PushL( state );
    
    //compare states created from own stream and externalized/internalized state
    ASSERT( *iViewState == *state );
      
    CleanupStack::PopAndDestroy( state );
    fileReadStream.Close();
    fileReadStream.Pop();
    fileWriteStream.Close();
    fileWriteStream.Pop();
    Teardown();    
    return KErrNone; 
    }


/****************************************************************
* == Operator
****************************************************************/
TInt CBCTestPbkUIUtil::MT_CPbkViewState_operatorL(  )
    {
    SetupL();
    CPbkViewState* viewState = CPbkViewState::NewL();
    CleanupStack::PushL( viewState );
    
    //First assert when no data set
    ASSERT( *viewState == *iViewState);
    
    //Setting datas one by one, asserting during different combinations
    
    iViewState->SetFocusedContactId( KSelectedItemId );
    ASSERT(!( *viewState == *iViewState ) );
    viewState->SetFocusedContactId( KSelectedItemId );
    ASSERT( *viewState == *iViewState );
    
    iViewState->SetTopContactId( KSelectedItemId );
    ASSERT(!( *viewState == *iViewState ) );
    viewState->SetTopContactId( KSelectedItemId );
    ASSERT( *viewState == *iViewState );

    iViewState->SetFocusedFieldIndex( KIndexId );
    ASSERT( ! ( *viewState == *iViewState ) );
    viewState->SetFocusedFieldIndex( KIndexId );
    ASSERT( *viewState == *iViewState );

    iViewState->SetTopFieldIndex( KIndexId );
    ASSERT( ! ( *viewState == *iViewState ) );
    viewState->SetTopFieldIndex( KIndexId );
    ASSERT( *viewState == *iViewState );

    iViewState->SetParentContactId( KSelectedItemId );
    ASSERT(!( *viewState == *iViewState ) );
    viewState->SetParentContactId( KSelectedItemId );
    ASSERT( *viewState == *iViewState );
  
    CleanupStack::Pop( viewState );  
    delete viewState;
    viewState = NULL; 
    Teardown();    
    return KErrNone; 
    }

// ----------------------------------------------------------------------------
// CBCTestPbkUIUtil::OpenResourceFileL()
// Open resource file
// ----------------------------------------------------------------------------
RPbkViewResourceFile* CBCTestPbkUIUtil::OpenResourceFileL()
    {  
    //open resource file
    RPbkViewResourceFile*  resourceFile = new RPbkViewResourceFile( *CCoeEnv::Static() );
    if( !resourceFile->IsOpen() )
        {
        resourceFile->OpenL();
        }
       
    //ASSERT( resourceFile->IsOpen() == _L("Failed to open resource file" )); //doubt
    ASSERT(resourceFile->IsOpen());
    return resourceFile;
    }
    
// ----------------------------------------------------------------------------
// CBCTestPbkUIUtil::DummyMethod()
// Does nothing
// ----------------------------------------------------------------------------
void CBCTestPbkUIUtil::DummyMethod()
    {  
    }

///////////////////////////////////////////////////////////////////////////////
// CPbkIconInfoContainer test methods                                          //   
///////////////////////////////////////////////////////////////////////////////
    
// ----------------------------------------------------------------------------
//  CBCTestPbkUIUtil::RunConstructContainerWithCoeEnvTestL()
//  Test "CPbkIconInfoContainer::NewL" with CCoeEnv
// ----------------------------------------------------------------------------
TInt CBCTestPbkUIUtil::RunConstructContainerWithCoeEnvTestL()
    {
    RPbkViewResourceFile* resourceFile = OpenResourceFileL();
    CleanupStack::PushL( resourceFile );
    
    CPbkIconInfoContainer* container = CPbkIconInfoContainer::NewL( CCoeEnv::Static() );
    CleanupStack::PushL( container );
    ASSERT( container!= NULL );
    
    CleanupStack::PopAndDestroy( container );
    resourceFile->Close();
    CleanupStack::PopAndDestroy( resourceFile ); 
    return KErrNone; 
    }
    
// ----------------------------------------------------------------------------
//  CBCTestPbkUIUtil::RunConstructContainerWithoutCoeEnvTestL()
//  Test "CPbkIconInfoContainer::NewL" without CCoeEnv
// ----------------------------------------------------------------------------
TInt CBCTestPbkUIUtil::RunConstructContainerWithoutCoeEnvTestL()
    {
    RPbkViewResourceFile* resourceFile = OpenResourceFileL();
    CleanupStack::PushL( resourceFile );
    
    CPbkIconInfoContainer* container = CPbkIconInfoContainer::NewL( );
    CleanupStack::PushL( container );
    ASSERT( container!= NULL );
    
    CleanupStack::PopAndDestroy( container );
    resourceFile->Close();
    CleanupStack::PopAndDestroy( resourceFile ); 
    return KErrNone; 
    }

// ----------------------------------------------------------------------------
//  CBCTestPbkUIUtil::RunFindTestL()
//  Run 'CPbkIconInfoContainer::Find' testcase
// ----------------------------------------------------------------------------
TInt CBCTestPbkUIUtil::RunFindTestL()
    {
    RPbkViewResourceFile* resourceFile = OpenResourceFileL();
    CleanupStack::PushL( resourceFile );
    
    CPbkIconInfoContainer* container = CPbkIconInfoContainer::NewL();
    CleanupStack::PushL( container );
    
    // get null icon
    const TPbkIconInfo* iconInfo1 = container->Find( EPbkNullIconId );
    ASSERT( !iconInfo1 );
    
    // get real icon
    const TPbkIconInfo* iconInfo2 = container->Find( EPbkqgn_prop_checkbox_off );
    ASSERT( iconInfo2 );
    
    CleanupStack::PopAndDestroy( container );
    resourceFile->Close();
    CleanupStack::PopAndDestroy( resourceFile ); 
    return KErrNone; 
    }
    
// ----------------------------------------------------------------------------
//  CBCTestPbkUIUtil::RunLoadBitmapLTestL()
//  Run 'CPbkIconInfoContainer::LoadBitmapL' testcase
// ----------------------------------------------------------------------------
TInt CBCTestPbkUIUtil::RunLoadBitmapLTestL()
    {
    RPbkViewResourceFile* resourceFile = OpenResourceFileL();
    CleanupStack::PushL( resourceFile );
    
    CPbkIconInfoContainer* container = CPbkIconInfoContainer::NewL();
    CleanupStack::PushL( container );
         
    CGulIcon* bmp1 = container->LoadBitmapL( EPbkNullIconId );
    ASSERT( !bmp1 );
    delete bmp1;
         
    CGulIcon* bmp2 = container->LoadBitmapL( EPbkqgn_prop_checkbox_off );
    ASSERT( bmp2 );
    delete bmp2;
    
    CleanupStack::PopAndDestroy( container );
    resourceFile->Close();
    CleanupStack::PopAndDestroy( resourceFile ); 
    return KErrNone; 
    }
       
///////////////////////////////////////////////////////////////////////////////
// TPbkIconInfo test methods                                                   //   
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// CBCTestPbkUIUtil::SetupIconInfoL()
// Create TPbkIconInfo 
// ----------------------------------------------------------------------------
void CBCTestPbkUIUtil::SetupIconInfoL()
    {  
    RPbkViewResourceFile* resourceFile = OpenResourceFileL(); 
    CleanupStack::PushL( resourceFile );
    
    iContainer = CPbkIconInfoContainer::NewL( );
    
    iIconInfo = iContainer->Find( EPbkqgn_prop_checkbox_off );
    
    resourceFile->Close();
    CleanupStack::PopAndDestroy( resourceFile );
    }
        
// ----------------------------------------------------------------------------
// CBCTestPbkUIUtil::TeardownIconInfo()
// Teardown TPbkIconInfo 
// ----------------------------------------------------------------------------
void CBCTestPbkUIUtil::TeardownIconInfo()
    {
    iIconInfo = NULL;
    delete iContainer;
    iContainer = NULL;
    }
    
// ----------------------------------------------------------------------------
//  CBCTestPbkUIUtil::RunPbkIconInfoIdTestL()
//  Run 'TPbkIconInfo::PbkIconInfoId' testcase
// ----------------------------------------------------------------------------
TInt CBCTestPbkUIUtil::RunPbkIconInfoIdTestL()
    {
    SetupIconInfoL();
    //ASSERT_EQUALS( iIconInfo->PbkIconInfoId(), EPbkqgn_prop_checkbox_off );
    ASSERT( iIconInfo->PbkIconInfoId() == EPbkqgn_prop_checkbox_off );
    TeardownIconInfo(); 
    return KErrNone; 
    }
    
// ----------------------------------------------------------------------------
//  CBCTestPbkUIUtil::RunMbmFileNameTestL()
//  Run 'TPbkIconInfo::MbmFileName' testcase
// ----------------------------------------------------------------------------
TInt CBCTestPbkUIUtil::RunMbmFileNameTestL()
    {
    SetupIconInfoL();
    const TDesC& name = iIconInfo->MbmFileName();
    // just test string is not empty
    ASSERT( name.Length() > 0 );
    TeardownIconInfo(); 
    return KErrNone; 
    }
    
// ----------------------------------------------------------------------------
//  CBCTestPbkUIUtil::RunIconIdTestL()
//  Run 'TPbkIconInfo::IconId' testcase
// ----------------------------------------------------------------------------
TInt CBCTestPbkUIUtil::RunIconIdTestL()
    {
    SetupIconInfoL();
    ASSERT( iIconInfo->IconId() > 0 );
    TeardownIconInfo(); 
    return KErrNone; 
    }
    
// ----------------------------------------------------------------------------
//  CBCTestPbkUIUtil::RunMaskIdTestL()
//  Run 'TPbkIconInfo::MaskId' testcase
// ----------------------------------------------------------------------------
TInt CBCTestPbkUIUtil::RunMaskIdTestL()
    {
    SetupIconInfoL();
    ASSERT( iIconInfo->MaskId() > 0 );
    TeardownIconInfo(); 
    return KErrNone; 
    }
    
// ----------------------------------------------------------------------------
//  CBCTestPbkUIUtil::RunSkinIdTestL()
//  Run 'TPbkIconInfo::SkinId' testcase
// ----------------------------------------------------------------------------
TInt CBCTestPbkUIUtil::RunSkinIdTestL()
    {
    SetupIconInfoL();
    TAknsItemID id = iIconInfo->SkinId();
    
    ASSERT( id.iMajor > 0 );
    ASSERT( id.iMinor > 0 );
    TeardownIconInfo(); 
    return KErrNone; 
    }
    
///////////////////////////////////////////////////////////////////////////////
// PbkIconUtils test methods                                                 //   
///////////////////////////////////////////////////////////////////////////////

    
// ----------------------------------------------------------------------------
//  CBCTestPbkUIUtil::RunCreateIconLTestL()
//  Run 'PbkIconUtils::CreateIconL' testcase
// ----------------------------------------------------------------------------
TInt CBCTestPbkUIUtil::RunCreateIconLTestL()
    {
    RPbkViewResourceFile* resourceFile = OpenResourceFileL();
    CleanupStack::PushL( resourceFile );
    
    CPbkIconInfoContainer* container = CPbkIconInfoContainer::NewL();
    CleanupStack::PushL( container );
    
    const TPbkIconInfo* iconInfo = container->Find( EPbkqgn_prop_checkbox_off );
       
    CGulIcon* bmp = container->LoadBitmapL( EPbkqgn_prop_checkbox_off );
    CleanupStack::PushL( bmp );
    CFbsBitmap* bitmap;
    CFbsBitmap* mask;
    
    PbkIconUtils::CreateIconL(
                    AknsUtils::SkinInstance(),
                    bitmap,
                    mask,
                    *iconInfo
                    );
    
    ASSERT( bitmap );
    ASSERT( mask );
    
    delete bitmap;
    delete mask;
  
    CleanupStack::PopAndDestroy( bmp );  
    CleanupStack::PopAndDestroy( container );
    resourceFile->Close();
    CleanupStack::PopAndDestroy( resourceFile ); 
    return KErrNone; 
    }
    
// ----------------------------------------------------------------------------
//  CBCTestPbkUIUtil::RunCreateIconLCTestL()
//  Run 'PbkIconUtils::CreateIconLC' testcase
// ----------------------------------------------------------------------------
TInt CBCTestPbkUIUtil::RunCreateIconLCTestL()
    {
    RPbkViewResourceFile* resourceFile = OpenResourceFileL();
    CleanupStack::PushL( resourceFile );
    
    CPbkIconInfoContainer* container = CPbkIconInfoContainer::NewL();
    CleanupStack::PushL( container );
    
    const TPbkIconInfo* iconInfo = container->Find( EPbkqgn_prop_checkbox_off );
       
    CGulIcon* bmp = container->LoadBitmapL( EPbkqgn_prop_checkbox_off );
    CleanupStack::PushL( bmp );
    CFbsBitmap* bitmap;
    CFbsBitmap* mask;
    
    PbkIconUtils::CreateIconLC(
                    AknsUtils::SkinInstance(),
                    bitmap,
                    mask,
                    *iconInfo
                    );
    
    ASSERT( bitmap );
    ASSERT( mask );
    
    //CreateIconLC puts both bitmap and mask in the cleanup stack
    CleanupStack::PopAndDestroy( mask );
    CleanupStack::PopAndDestroy( bitmap );
    CleanupStack::PopAndDestroy( bmp );  
    CleanupStack::PopAndDestroy( container );
    resourceFile->Close();
    CleanupStack::PopAndDestroy( resourceFile ); 
    return KErrNone; 
    }    

///////////////////////////////////////////////////////////////////////////////
// RPbkViewResourceFile test methods                                                 //   
///////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void CBCTestPbkUIUtil::RPbkViewResourceFile_SetupL(  )
    {
    iViewResourceFile = new RPbkViewResourceFile(*CCoeEnv::Static());
    }


//----------------------------------------------------------------------------
void CBCTestPbkUIUtil::RPbkViewResourceFile_Teardown(  )
    {
    delete iViewResourceFile;
    }

TInt CBCTestPbkUIUtil::RunLoadResourceTestL()
    {
    RPbkViewResourceFile_SetupL();
//doubt asserts
    TRAPD(error1, iViewResourceFile->OpenL());
    ASSERT(error1 == 0 );

    ASSERT(iViewResourceFile->IsOpen() == 1 );

    // Resource file already open, must leave
    TRAPD(error2, iViewResourceFile->OpenL());
    ASSERT(error2);

    ASSERT(iViewResourceFile->IsOpen() == 1 );

    iViewResourceFile->Close();
    ASSERT((iViewResourceFile->IsOpen()) == 0 );
	RPbkViewResourceFile_Teardown(); 
    return KErrNone; 
    }


TInt CBCTestPbkUIUtil::RunTest_ClassSizeL()
    {
    RPbkViewResourceFile_SetupL();
    const TInt sizeof_class_RPbkViewResourceFile_30 = 12;
    TInt size = sizeof(class RPbkViewResourceFile);
    if ( size >= sizeof_class_RPbkViewResourceFile_30 )
    	ASSERT(ETrue);
    else
    	ASSERT(EFalse);
    RPbkViewResourceFile_Teardown(); 
    return KErrNone; 
    }

//  [End of File] - Do not remove
