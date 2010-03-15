/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include "cpbk2openmycardcmd.h"
#include "cpbk2mycard.h"
#include <CPbk2NamesListControl.h>
#include <MPbk2CommandObserver.h>
#include <MVPbkBaseContact.h>
#include <ccappmycardpluginuids.hrh>
#include <spbcontactdatamodel.h>
#include <pbk2exnameslistres.rsg>
#include <CPbk2FieldPropertyArray.h>
#include <CPbk2StoreSpecificFieldPropertyArray.h>
#include <CPbk2PresentationContact.h>
#include <CPbk2StorePropertyArray.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactStore.h>

//Cca
#include <mccaparameter.h>
#include <mccaconnection.h>
#include <ccafactory.h>

#include <s32mem.h>

// Debugging headers
#include <Pbk2Debug.h>
#include <Pbk2Profile.h>

// --------------------------------------------------------------------------
// CPbk2OpenMyCardCmd::CPbk2OpenMyCardCmd
// --------------------------------------------------------------------------
//
CPbk2OpenMyCardCmd::CPbk2OpenMyCardCmd(
    MPbk2ContactUiControl* aUiControl,
    CPbk2MyCard* aMyCard,
    MCCAConnection*& aCCAConnection ) :
        CActive( EPriorityStandard ),
        iUiControl( aUiControl ),
        iConnectionRef(aCCAConnection),
        iViewUid( TUid::Uid( KCCAMyCardPluginImplmentationUid ) ), // default mycard view
        iMyCard( aMyCard )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2OpenMyCardCmd::~CPbk2OpenMyCardCmd
// --------------------------------------------------------------------------
//
CPbk2OpenMyCardCmd::~CPbk2OpenMyCardCmd()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2OpenMyCardCmd(%x)::~CPbk2OpenMyCardCmd()"), this);
    Cancel();

    if( iUiControl )
        {
        iUiControl->RegisterCommand( NULL );
        }
    delete iPresentationContact;
    delete iSpecificFieldProperties;
    delete iFieldProperties;
    }

// --------------------------------------------------------------------------
// CPbk2OpenMyCardCmd::NewL
// --------------------------------------------------------------------------
//
CPbk2OpenMyCardCmd* CPbk2OpenMyCardCmd::NewL(
    MCCAConnection*& aCCAConnection,
    CPbk2MyCard* aMyCard,
    MPbk2ContactUiControl* aUiControl )
    {
    CPbk2OpenMyCardCmd* self = new ( ELeave ) CPbk2OpenMyCardCmd(
        aUiControl, aMyCard, aCCAConnection );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2OpenMyCardCmd::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2OpenMyCardCmd::ConstructL()
    {
    if( iUiControl )
        {
        iUiControl->RegisterCommand( this );
        }
    }

// --------------------------------------------------------------------------
// CPbk2OpenMyCardCmd::SetViewUid
// --------------------------------------------------------------------------
//
void CPbk2OpenMyCardCmd::SetViewUid( TUid aViewUid )
    {
    iViewUid = aViewUid;
    }

// --------------------------------------------------------------------------
// CPbk2OpenMyCardCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPbk2OpenMyCardCmd::ExecuteLD()
    {
    iState = ELaunching;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2OpenMyCardCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPbk2OpenMyCardCmd::ResetUiControl( MPbk2ContactUiControl& aUiControl )
    {
    if ( iUiControl == &aUiControl )
        {
        iUiControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2OpenMyCardCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPbk2OpenMyCardCmd::AddObserver( MPbk2CommandObserver& aObserver )
    {
    iCommandObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPbk2OpenMyCardCmd::LaunchCcaL
// --------------------------------------------------------------------------
//
void CPbk2OpenMyCardCmd::LaunchCcaL()
    {
    if(!iConnectionRef)
        {
        iConnectionRef = TCCAFactory::NewConnectionL();
        }

    MCCAParameter* parameter = TCCAFactory::NewParameterL();
    CleanupClosePushL( *parameter );
    parameter->SetConnectionFlag( MCCAParameter::ENormal );
    parameter->SetLaunchedViewUid( iViewUid );
    
    if( iMyCard )
        {
        // Speed up mycard launching by constructing the view model here.
        // This information is not mandatory for mycard launching however.
        CBufFlat* buffer = CBufFlat::NewL( KKilo );
        CleanupStack::PushL( buffer );
        RBufWriteStream stream( *buffer );
        CleanupClosePushL( stream );

        // create model and dump it into stream
        CSpbContactDataModel* model = CSpbContactDataModel::NewL( 
            iMyCard->ContactManager(), *CCoeEnv::Static(), 
            R_PBK2_MYCARD_FIELD_CLIP_SELECTOR );
        CleanupStack::PushL( model );

        if( iMyCard->MyCardState() == CPbk2MyCard::EExisting )
            {
            // preset contact data model to contain my cards data.
            CPbk2PresentationContact* contact = PresentationContactL();
            if( contact )
                {
                model->SetDataL( *contact, NULL );
                }
            }
        model->ExternalizeL( stream );
        CleanupStack::PopAndDestroy( 2, &stream ); // model

        // set model dump as parameter
        TPtrC8 buf( buffer->Ptr( 0 ) );
        TPtrC16 data( (TUint16*)buf.Ptr(), ( buf.Size() + 1 ) / 2 );
        parameter->SetContactDataFlag( MCCAParameter::EContactDataModel );
        parameter->SetContactDataL( data );
        CleanupStack::PopAndDestroy( buffer ); 
        }

    // Sync call
    iConnectionRef->LaunchAppL( *parameter );
    CleanupStack::Pop();// parameter is taken care by iConnectionRef

    iState = ERunning;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2OpenMyCardCmd::IssueRequest
// --------------------------------------------------------------------------
//
void CPbk2OpenMyCardCmd::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// ---------------------------------------------------------------------------
// CPbk2OpenMyCardCmd::PresentationContactL
// ---------------------------------------------------------------------------
//
CPbk2PresentationContact* CPbk2OpenMyCardCmd::PresentationContactL()
    {
    if( !iPresentationContact )
        {
        if( !iMyCard )
            {
            return NULL;
            }
            
        MVPbkStoreContact* storeContact = iMyCard->MyCardStoreContact();
        if( !storeContact )
            {
            return NULL;
            }
        
        const MVPbkContactStoreProperties& storeProperties =
            storeContact->ParentStore().StoreProperties();
        const MVPbkFieldTypeList& supportedFieldTypes =
            storeProperties.SupportedFields();
    
        if( !iFieldProperties )
            {
            iFieldProperties = CPbk2FieldPropertyArray::NewL(
                supportedFieldTypes, CCoeEnv::Static()->FsSession() );
            }
    
        // Create a field property list of the supported
        // field types of the used store
        CPbk2StorePropertyArray* pbk2StoreProperties = CPbk2StorePropertyArray::NewL();
        CleanupStack::PushL( pbk2StoreProperties );
    
        if( !iSpecificFieldProperties )
            {
            iSpecificFieldProperties = CPbk2StoreSpecificFieldPropertyArray::NewL(
                *iFieldProperties,
                *pbk2StoreProperties,
                supportedFieldTypes,
                storeContact->ParentStore() );
            }
    
        iPresentationContact = CPbk2PresentationContact::NewL( 
            *storeContact, *iSpecificFieldProperties );
        
        CleanupStack::PopAndDestroy( pbk2StoreProperties );
        }
    return iPresentationContact;
    }


// --------------------------------------------------------------------------
// CPbk2OpenMyCardCmd::IssueRequest
// --------------------------------------------------------------------------
//
void CPbk2OpenMyCardCmd::RunL()
    {
    if ( iState == ELaunching )
        {
        LaunchCcaL();   //Sync
        }
    else if ( iState == ERunning )
        {
        if ( iUiControl )
            {
            iUiControl->UpdateAfterCommandExecution();
            }
        // Asynchronous delete of this command object
        if( iCommandObserver )
            {
            iCommandObserver->CommandFinished( *this );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2OpenMyCardCmd::IssueRequest
// --------------------------------------------------------------------------
//
void CPbk2OpenMyCardCmd::DoCancel()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2OpenMyCardCmd::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2OpenMyCardCmd::RunError(TInt aError)
    {
    // Forward all errors to the framework. They will be
    // displayed on screen to the user. Typically there
    // should not be any errors.

    return aError;
    }

//  End of File
