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
* Description:  Phonebook 2 copy from Phonebook to SIM command object.
*
*/


// INCLUDE FILES
#include "CPsu2CopyFromPbkCmd.h"

// Phonebook 2
#include "CPsu2NumberQueryDlg.h"
#include "CPsu2CopyToSimFieldInfoArray.h"
#include "CPsu2CharConv.h"
#include "CPsu2UIExtensionPlugin.h"

#include <MPbk2ContactUiControl.h>
#include <MPbk2CommandObserver.h>
#include <CPbk2FetchDlg.h>
#include <MPbk2FetchDlgObserver.h>
#include <CPbk2FieldPropertyArray.h>
#include <Pbk2UIControls.rsg>
#include <Pbk2USimUIRes.rsg>
#include <TPbk2AddressSelectParams.h>
#include <CPbk2AddressSelect.h>
#include <Pbk2DataCaging.hrh>
#include <MPbk2ContactNameFormatter.h>
#include <MPbk2DialogEliminator.h>
#include <Pbk2ProcessDecoratorFactory.h>
#include <Pbk2Commands.rsg>
#include <CPbk2StoreProperty.h>
#include <CPbk2StorePropertyArray.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>
#include <Pbk2CommonUi.rsg>

// Virtual Phonebook
#include <MVPbkStoreContact.h>
#include <MVPbkContactLink.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreInfo.h>
#include <MVPbkStoreContactField.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkContactLinkArray.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkFieldType.h>
#include <TVPbkFieldVersitProperty.h>
#include <MVPbkContactFieldTextData.h>

// System includes
#include <coemain.h>
#include <barsread.h>
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <exterror.h>
#include <akntitle.h>
#include <aknnotewrappers.h>
#include <charconv.h>


/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG

enum TPanicCode
    {
    EPanic_Pbk2FetchCompletedL = 1
    };

void Panic( TPanicCode aPanic )
    {
    _LIT( KPanicCat, "CPsu2CopyFromPbkCmd" );
    User::Panic( KPanicCat, aPanic );
    }

#endif // _DEBUG

/**
 * Checks is the store full.
 *
 * @param aStore    The store to check.
 * @return  ETrue if the store is full, EFalse otherwise.
 */
inline TBool StoreFullL( MVPbkContactStore& aStore )
    {
    const MVPbkContactStoreInfo& storeInfo = aStore.StoreInfo();
    return storeInfo.MaxNumberOfContactsL() <= storeInfo.NumberOfContactsL();
    }

/**
 * Show error info note
 *
 * @param aResourceId    The numeric ID of the resource to construct info note.
 * @param aStoreName    store name to be shown in the info note   
 */
void ShowInformationNoteL( TInt aResourceId, const TDesC* aStoreName ) 
    {
    HBufC* prompt = NULL;
    
    if( aStoreName )
        {
        prompt = StringLoader::LoadL( aResourceId , *aStoreName );
        }
    else
        {
        prompt = StringLoader::LoadL( aResourceId );
        }

    if ( prompt )
        {
        CleanupStack::PushL(prompt);
        // This is a waiting dialog because the editor might be
        // used from the application server and the information note will
        // disappear if the application server closes before the
        // note timeout has expired, thus causing blinking
        CAknInformationNote* noteDlg = new(ELeave) CAknInformationNote(ETrue);
        noteDlg->ExecuteLD(*prompt);
        CleanupStack::PopAndDestroy(); // prompt
        }
    }

void ShowStoreFullNoteL
        ( const MVPbkContactStore& aTargetStore,
          const CPbk2StorePropertyArray& aStoreProperties )
    {
    // Fetch store name
    TVPbkContactStoreUriPtr uri = aTargetStore.StoreProperties().Uri();

    const CPbk2StoreProperty* storeProperty =
        aStoreProperties.FindProperty( uri );

    const TDesC* storeName = NULL;
    if ( storeProperty )
        {
        storeName = &storeProperty->StoreName();
        }
    else
        {
        storeName = &uri.UriDes();
        }
    ShowInformationNoteL( R_QTN_PHOB_NOTE_STORE_FULL, storeName );
    }
/// Tasks
enum TNextTask
    {
    EPsu2ContactFetch,
    EPsu2QueryAddress,
    EPsu2CopyContact,
    EPsu2Commit,
    EPsu2Complete
    };

_LIT( KPSu2InternationalPrefix, "+" );

} /// namespace

// --------------------------------------------------------------------------
// CPsu2CopyFromPbkCmd::CPsu2CopyFromPbkCmd
// --------------------------------------------------------------------------
//
CPsu2CopyFromPbkCmd::CPsu2CopyFromPbkCmd(
        MVPbkContactStore& aTargetStore,
        MPbk2ContactUiControl& aUiControl,
        CPsu2UIExtensionPlugin* aPsu2UIExensionPlugin) :
    CActive( EPriorityStandard ),
    iTargetStore( aTargetStore ),
    iUiControl( &aUiControl ),
    iPsu2UIExensionPlugin( aPsu2UIExensionPlugin )
    {
    }

// --------------------------------------------------------------------------
// CPsu2CopyFromPbkCmd::~CPsu2CopyFromPbkCmd()
// --------------------------------------------------------------------------
//
CPsu2CopyFromPbkCmd::~CPsu2CopyFromPbkCmd()
    {
    Cancel();
    delete iCopyOperation;
    delete iSourceStoreContact;
    delete iTargetStoreContact;
    delete iRetrieveOperation;
    delete iStoreContactField;
    delete iSimFieldInfos;
    delete iDecorator;   
    delete iTitlePaneText;
    }

// --------------------------------------------------------------------------
// CPsu2CopyFromPbkCmd::NewL
// --------------------------------------------------------------------------
//
CPsu2CopyFromPbkCmd* CPsu2CopyFromPbkCmd::NewL
        ( MVPbkContactStore& aTargetStore,
          MPbk2ContactUiControl& aUiControl,
          CPsu2UIExtensionPlugin* aPsu2UIExensionPlugin )
    {
    CPsu2CopyFromPbkCmd* self =
        new (ELeave) CPsu2CopyFromPbkCmd( aTargetStore, aUiControl, aPsu2UIExensionPlugin );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPsu2CopyFromPbkCmd::ConstructL
// --------------------------------------------------------------------------
//
void CPsu2CopyFromPbkCmd::ConstructL()
    {
    CActiveScheduler::Add( this );

    iSimFieldInfos = CPsu2CopyToSimFieldInfoArray::NewL
        ( Phonebook2::Pbk2AppUi()->ApplicationServices().
            ContactManager().FieldTypes(),
          Phonebook2::Pbk2AppUi()->ApplicationServices().
            ContactManager().FsSession() );
    iTargetStoreContact = iTargetStore.CreateNewContactLC();
    CleanupStack::Pop(); //iTargetStoreContact
    }

// --------------------------------------------------------------------------
// CPsu2CopyFromPbkCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPsu2CopyFromPbkCmd::ExecuteLD()
    {
    if ( StoreFullL( iTargetStore ) )
        {
        ShowStoreFullNoteL( iTargetStore,
            Phonebook2::Pbk2AppUi()->ApplicationServices().StoreProperties() );
        }
    else
        {
        iNextTask = EPsu2ContactFetch;
        IssueRequest();
        }
    }

// --------------------------------------------------------------------------
// CPsu2CopyFromPbkCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPsu2CopyFromPbkCmd::AddObserver( MPbk2CommandObserver& aObserver )
    {
    iCommandObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPsu2CopyFromPbkCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPsu2CopyFromPbkCmd::ResetUiControl( MPbk2ContactUiControl& aUiControl )
    {
    if ( iUiControl == &aUiControl )
        {
        iUiControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPsu2CopyFromPbkCmd::RunL
// --------------------------------------------------------------------------
//
void CPsu2CopyFromPbkCmd::RunL()
    {
    switch (iNextTask)
        {
        case EPsu2ContactFetch:
            {
            QueryContactL();
            break;
            }
        case EPsu2QueryAddress:
            {
            QueryAddressL();
            break;
            }
        case EPsu2CopyContact:
            {
            CopyContactL();
            break;
            }
        case EPsu2Complete: // FALLTHROUGH
        default:
            {
            if ( iFetchDialogEliminator )
                {
                iFetchDialogEliminator->RequestExitL( EEikBidOk );
                }
            if ( iCommandObserver )
                {
                iCommandObserver->CommandFinished(*this);            
                }
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPsu2CopyFromPbkCmd::DoCancel
// --------------------------------------------------------------------------
//
void CPsu2CopyFromPbkCmd::DoCancel()
    {
    // Nothing to do
    }

// --------------------------------------------------------------------------
// CPsu2CopyFromPbkCmd::RunError
// --------------------------------------------------------------------------
//
TInt CPsu2CopyFromPbkCmd::RunError( TInt aError )
    {
    CCoeEnv::Static()->HandleError( aError );
    if ( iCommandObserver )
        {
        iCommandObserver->CommandFinished( *this );        
        }
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPsu2CopyFromPbkCmd::AcceptFetchSelectionL
// --------------------------------------------------------------------------
//
MPbk2FetchDlgObserver::TPbk2FetchAcceptSelection
    CPsu2CopyFromPbkCmd::AcceptFetchSelectionL
        ( TInt aNumMarkedEntries, MVPbkContactLink& /*aLastSelection*/ )
    {
    MPbk2FetchDlgObserver::TPbk2FetchAcceptSelection ret =
        MPbk2FetchDlgObserver::EFetchNo;

    if (aNumMarkedEntries == 1)
        {
        ret = MPbk2FetchDlgObserver::EFetchYes;
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPsu2CopyFromPbkCmd::FetchCompletedL
// --------------------------------------------------------------------------
//
void CPsu2CopyFromPbkCmd::FetchCompletedL
        ( MVPbkContactLinkArray* aMarkedEntries )
    {
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;
    
    if ( iTitlePaneText )
        {
        SetTitlePaneTextL( *iTitlePaneText );        
        }


    __ASSERT_DEBUG( aMarkedEntries && aMarkedEntries->Count() > 0,
        Panic( EPanic_Pbk2FetchCompletedL ) );

    iRetrieveOperation = Phonebook2::Pbk2AppUi()->ApplicationServices().
        ContactManager().RetrieveContactL
            ( aMarkedEntries->At(0), *this );
    }

// --------------------------------------------------------------------------
// CPsu2CopyFromPbkCmd::FetchCanceled
// --------------------------------------------------------------------------
//
void CPsu2CopyFromPbkCmd::FetchCanceled()
    {
    if ( iTitlePaneText )
        {
        // cannot leave, not fatal if settings
        // title pane text fails
        TRAP_IGNORE(SetTitlePaneTextL( *iTitlePaneText ));        
        }
        
    iCommandObserver->CommandFinished( *this );
    }

// --------------------------------------------------------------------------
// CPsu2CopyFromPbkCmd::FetchAborted
// --------------------------------------------------------------------------
//
void CPsu2CopyFromPbkCmd::FetchAborted()
    {
    if ( iTitlePaneText )
        {
        // cannot leave, not fatal if settings
        // title pane text fails
        TRAP_IGNORE(SetTitlePaneTextL( *iTitlePaneText ));        
        }
        
    if ( iCommandObserver )
        {
        iCommandObserver->CommandFinished( *this );    
        }    
    }

// --------------------------------------------------------------------------
// CPsu2CopyFromPbkCmd::FetchOkToExit
// --------------------------------------------------------------------------
//
TBool CPsu2CopyFromPbkCmd::FetchOkToExit()
    {
    // FDN fetch cancel is always ok
    return ETrue;
    }

// --------------------------------------------------------------------------
// CPsu2CopyFromPbkCmd::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPsu2CopyFromPbkCmd::VPbkSingleContactOperationComplete
        ( MVPbkContactOperationBase& /*aOperation*/,
          MVPbkStoreContact* aContact )
    {
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;

    delete iSourceStoreContact;
    iSourceStoreContact = aContact;
    ProcessDismissed( KErrNone );
    }

// --------------------------------------------------------------------------
// CPsu2CopyFromPbkCmd::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPsu2CopyFromPbkCmd::VPbkSingleContactOperationFailed
        ( MVPbkContactOperationBase& /*aOperation*/, TInt /*aError*/ )
    {
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;

    iNextTask = EPsu2Complete;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPsu2CopyFromPbkCmd::ContactOperationCompleted
// --------------------------------------------------------------------------
//
void CPsu2CopyFromPbkCmd::ContactOperationCompleted
        (TContactOpResult /*aResult*/)
    {
    if ( iDecorator )
        {
        iDecorator->ProcessStopped();        
        }
    }

// --------------------------------------------------------------------------
// CPsu2CopyFromPbkCmd::ContactOperationFailed
// --------------------------------------------------------------------------
//
void CPsu2CopyFromPbkCmd::ContactOperationFailed
        ( TContactOp /*aOpCode*/, TInt aErrorCode, TBool /*aErrorNotified*/ )
    {
    iLastError = aErrorCode;

    if ( iDecorator )
        {
        iDecorator->ProcessStopped();        
        }
    }

// --------------------------------------------------------------------------
// CPsu2CopyFromPbkCmd::ProcessDismissed
// --------------------------------------------------------------------------
//
void CPsu2CopyFromPbkCmd::ProcessDismissed( TInt /*aCancelCode*/ )
    {
    // If fetch was dismissed, its time to query address
    if ( iNextTask == EPsu2ContactFetch )
        {
        iNextTask = EPsu2QueryAddress;
        }

    // Error handling: if name string contains special characters, it
    // may have to be asked again if the string wouldn't fit into SIM.
    // Otherwise just skip the error and cancel.
    if ( iLastError != KErrNone )
        {
        iLengthQuery = EFalse;
        iNextTask = EPsu2Complete;

        if ( iLastError == KErrGsm0707TextStringTooLong )
            {
            iLengthQuery = ETrue;
            iNextTask = EPsu2CopyContact;
            }
        if ( iLastError == KErrGsmCCUnassignedNumber )
            {
            ShowInformationNoteL( R_QTN_ERR_SMS_INVALID_NUMB, NULL );
            }
        iLastError = KErrNone;
        }

    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPsu2CopyFromPbkCmd::QueryContactL
// --------------------------------------------------------------------------
//
void CPsu2CopyFromPbkCmd::QueryContactL()
    {
    CPbk2FetchDlg::TParams params;
    // Launch the fetch dialog
    CPbk2FetchDlg* fetchDlg = CPbk2FetchDlg::NewL(params, *this);
    iFetchDialogEliminator = fetchDlg;
    iFetchDialogEliminator->ResetWhenDestroyed( &iFetchDialogEliminator );
    
    // save the current title pane text:
    delete iTitlePaneText;
    iTitlePaneText = NULL;
    iTitlePaneText = GetTitlePaneTextL();
    
    // set title pane text:
    HBufC* title = StringLoader::LoadLC( R_QTN_FDN_PB_FETCH_TITLE );
    SetTitlePaneTextL( *title );
    CleanupStack::PopAndDestroy(title); // stringloader string.
        
    fetchDlg->ExecuteLD();    
    }

// --------------------------------------------------------------------------
// CPsu2CopyFromPbkCmd::QueryAddressL
// --------------------------------------------------------------------------
//
void CPsu2CopyFromPbkCmd::QueryAddressL()
    {
    TResourceReader resReader;
    CCoeEnv::Static()->CreateResourceReaderLC
        (resReader, R_PBK2_PHONE_NUMBER_SELECT);

    MPbk2ApplicationServices& appServices =
        Phonebook2::Pbk2AppUi()->ApplicationServices();

    TPbk2AddressSelectParams params(
        *iSourceStoreContact,
        appServices.ContactManager(),
        appServices.NameFormatter(),
        appServices.FieldProperties(),
        resReader );

    CPbk2AddressSelect* addressSelect = CPbk2AddressSelect::NewL(params);
    addressSelect->SetCba( R_AVKON_SOFTKEYS_SELECT_BACK__SELECT );
    delete iStoreContactField;
    iStoreContactField = NULL;
    iStoreContactField = addressSelect->ExecuteLD();
    CleanupStack::PopAndDestroy(); // resReader

    if ( iStoreContactField )
        {
        iNextTask = EPsu2CopyContact;
        }
    else
        {
        if ( iPsu2UIExensionPlugin->IsEndKeyPressed() )
            {
            iPsu2UIExensionPlugin->ClearEndKeyPressedFlag();
            // User pressed the end key, ending to copy contact
            iNextTask = EPsu2Complete;
            }
        else
            {
            // User cancelled address selection, returning to fetch
            iNextTask = EPsu2ContactFetch;
            }
        }
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPsu2CopyFromPbkCmd::CopyContactL
// --------------------------------------------------------------------------
//
void CPsu2CopyFromPbkCmd::CopyContactL()
    {
    iNextTask = EPsu2Complete;
    if ( AddNameL(*iSourceStoreContact, *iTargetStoreContact) )
        {
        if ( AppendSupportedFieldL(*iStoreContactField,
                *iTargetStoreContact) )
            {
            iTargetStoreContact->CommitL(*this);
            ShowWaitNoteL();
            iNextTask = EPsu2Commit;
            }
        }

    if ( iNextTask == EPsu2Complete )
        {
        // User cancelled copy
        IssueRequest();
        }
    }

// --------------------------------------------------------------------------
// CPsu2CopyFromPbkCmd::IssueRequest
// --------------------------------------------------------------------------
//
void CPsu2CopyFromPbkCmd::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// --------------------------------------------------------------------------
// CPsu2CopyFromPbkCmd::AddNameL
// --------------------------------------------------------------------------
//
TBool CPsu2CopyFromPbkCmd::AddNameL
        ( MVPbkBaseContact& aSource, MVPbkStoreContact& aTarget )
    {
    TBool ret( ETrue );
    HBufC* title = Phonebook2::Pbk2AppUi()->ApplicationServices().
            NameFormatter().GetContactTitleOrNullL
            ( aSource.Fields(),
            MPbk2ContactNameFormatter::EPreserveLeadingSpaces );

    if ( title )
        {
        CleanupStack::PushL( title );
        MVPbkStoreContactField* field =
            aTarget.CreateFieldLC( iSimFieldInfos->SimNameType() );
        MVPbkContactFieldTextData& data =
            MVPbkContactFieldTextData::Cast( field->FieldData() );
        TPtr ptrTitle( title->Des() );
        TInt maxLength( data.MaxLength() );

        if ( ptrTitle.Length() > maxLength || iLengthQuery )
            {
            ret = QueryNameL( ptrTitle, maxLength );
            }

        if ( ret )
            {
            data.SetTextL( ptrTitle );
            aTarget.AddFieldL(field);
            CleanupStack::Pop(); // field
            }
        else
            {
            CleanupStack::PopAndDestroy(); // field
            ret = EFalse;
            }
        CleanupStack::PopAndDestroy(title);
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CPsu2CopyFromPbkCmd::AppendSupportedFieldL
// --------------------------------------------------------------------------
//
TBool CPsu2CopyFromPbkCmd::AppendSupportedFieldL
        ( MVPbkStoreContactField& aSource, MVPbkStoreContact& aTarget)
    {
    TBool ret( ETrue );
    const MVPbkFieldType* type = NULL;
    // Get the source field type
    const TInt maxPriority =
        Phonebook2::Pbk2AppUi()->ApplicationServices().
            ContactManager().FieldTypes().MaxMatchPriority();
    for (TInt k = 0; k < maxPriority && !type; ++k)
        {
        type = aSource.MatchFieldType(k);
        }

    // The field possible can be copied to the SIM
    // Get the target(=SIM) field type for source type
    const MVPbkFieldType* simType =
        iSimFieldInfos->ConvertToSimType(*type);
    // Check if the sim store supports the converted field
    const MVPbkFieldTypeList& supportedTypes =
        iTargetStore.StoreProperties().SupportedFields();
    if (simType && supportedTypes.ContainsSame(*simType))
        {
        ret = CopyFieldL(aSource, aTarget, *simType);
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CPsu2CopyFromPbkCmd::CopyFieldL
// --------------------------------------------------------------------------
//
TBool CPsu2CopyFromPbkCmd::CopyFieldL
        ( const MVPbkBaseContactField& aFieldToCopy,
          MVPbkStoreContact& aTarget, const MVPbkFieldType& aSimType )
    {
    TBool ret = ETrue;

    if (aFieldToCopy.FieldData().DataType() == EVPbkFieldStorageTypeText)
        {
        const MVPbkContactFieldTextData& sourceData =
            MVPbkContactFieldTextData::Cast(aFieldToCopy.FieldData());
        MVPbkStoreContactField* field = aTarget.CreateFieldLC(aSimType);
        MVPbkContactFieldTextData& targetData =
            MVPbkContactFieldTextData::Cast(field->FieldData());
        // Decrease 1 which is reserved for international prefix "+"
        TInt maxNumberLength( targetData.MaxLength() - 1 );
        HBufC* buf = HBufC::NewLC( targetData.MaxLength() );
        TPtr ptrBuf( buf->Des() );
        ptrBuf.Copy( sourceData.Text().Right( maxNumberLength ) );
        
        // Because there is always one space reserved for the international prefix "+", 
        // so the actual maximum digits length should be targetData.MaxLength() - 1,
        // So here use targetData.MaxLength() - 1 instead of targetData.MaxLength() to do compare to 
        // show the Number Query dialog if need
        if ( sourceData.Text().Length() > ( targetData.MaxLength() - 1 ) )
            {
            do
                {
                // Keep original data in query
                ptrBuf.Zero();
                ptrBuf.Copy( sourceData.Text().Right( maxNumberLength ) );
                ret = QueryNumberL(
                    R_QTN_PBCOP_NOTE_NUMBER_MAX_LENGTH,
                    ptrBuf, maxNumberLength, targetData.MaxLength() );
                }
            while ( ptrBuf.Length() > maxNumberLength &&
                    ptrBuf[0] != KPSu2InternationalPrefix()[0] );
            }

        if ( ret )
            {
            targetData.SetTextL( ptrBuf );
            aTarget.AddFieldL(field);
            CleanupStack::PopAndDestroy( buf );
            CleanupStack::Pop(); // field
            }
        else
            {
            CleanupStack::PopAndDestroy( buf );
            CleanupStack::PopAndDestroy(); // field
            }
        }
    return ret;
    }
        
// --------------------------------------------------------------------------
// CPsu2CopyFromPbkCmd::SetTitlePaneTextL
// --------------------------------------------------------------------------
//    
void CPsu2CopyFromPbkCmd::SetTitlePaneTextL( const TDesC& aTitle )
    {
    CEikStatusPane* statusPane =
        CEikonEnv::Static()->AppUiFactory()->StatusPane();
    if ( statusPane && statusPane->PaneCapabilities
            ( TUid::Uid( EEikStatusPaneUidTitle ) ).IsPresent() )
        {
        CAknTitlePane* titlePane = static_cast<CAknTitlePane*>(
            statusPane->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );        

        titlePane->SetTextL( aTitle ); // takes ownership of title
        }
    }

// --------------------------------------------------------------------------
// CPsu2CopyFromPbkCmd::GetTitlePaneTextL
// --------------------------------------------------------------------------
//        
HBufC* CPsu2CopyFromPbkCmd::GetTitlePaneTextL()
    {
    CEikStatusPane* statusPane =
        CEikonEnv::Static()->AppUiFactory()->StatusPane();
    if ( statusPane && statusPane->PaneCapabilities
            ( TUid::Uid( EEikStatusPaneUidTitle ) ).IsPresent() )
        {
        CAknTitlePane* titlePane = static_cast<CAknTitlePane*>(
            statusPane->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );        

        return titlePane->Text()->AllocL();
        }
    else
        {
        return NULL;
        }
    }

// --------------------------------------------------------------------------
// CPsu2CopyFromPbkCmd::QueryNameL
// --------------------------------------------------------------------------
//
TBool CPsu2CopyFromPbkCmd::QueryNameL(
        TDes& aName, TInt aMaxDataLength )
    {
    TBool ret( ETrue );
    // Not sure if the encoding for SIM contact names is UTF-8, but it is Ok
    // because it is just used here to check if there are any unicode
    // characters in the text
    CPsu2CharConv* charConv = CPsu2CharConv::NewLC(
            Phonebook2::Pbk2AppUi()->ApplicationServices().
                ContactManager().FsSession(),
            KCharacterSetIdentifierUtf8 );
    HBufC* prompt(0);
    
    if( charConv->IsUnicodeL( aName ) )
        {
        // If the name includes unicode we cannot determine the actual
        // maximum length reliably so use a note without the maximum
        // length value
        prompt = StringLoader::LoadLC(
                R_QTN_PBSAV_UNICODE_NAME_TOO_LONG );
        }
    else
        {
        prompt = StringLoader::LoadLC(
                R_QTN_PBCOP_NOTE_NAME_MAX_LENGTH, aMaxDataLength );
        }

    // Check if the name needs to be cut to fit in the editor
    if( aName.Length() > aMaxDataLength )
        {
        aName.SetLength( aMaxDataLength );
        }

    CAknTextQueryDialog* dlg =
        CAknTextQueryDialog::NewL( aName );
    dlg->SetMaxLength( aMaxDataLength );
    ret = dlg->ExecuteLD( R_PSU2_GENERAL_TEXT_QUERY, *prompt );
    CleanupStack::PopAndDestroy( prompt );
    CleanupStack::PopAndDestroy( charConv );

    return ret;
    }

// --------------------------------------------------------------------------
// CPsu2CopyFromPbkCmd::QueryNumberL
// --------------------------------------------------------------------------
//
TBool CPsu2CopyFromPbkCmd::QueryNumberL
        ( TInt aPromptResourceId, TDes& aData,
          TInt aMaxDataLength, TInt aMaxEditorLength )
    {
    TBool ret( ETrue );
    HBufC* prompt = StringLoader::LoadLC(
        aPromptResourceId, aMaxDataLength );

    if( aData.Length() > aMaxDataLength )
        {
        aData.SetLength( aMaxDataLength );
        }
    CAknTextQueryDialog* dlg =
        CAknTextQueryDialog::NewL( aData );
    dlg->SetMaxLength( aMaxEditorLength );
    ret = dlg->ExecuteLD( R_PSU2_GENERAL_NUMBER_QUERY, *prompt );
    CleanupStack::PopAndDestroy( prompt );

    return ret;
    }

// --------------------------------------------------------------------------
// CPsu2CopyFromPbkCmd::ShowWaitNoteL
// --------------------------------------------------------------------------
//
void CPsu2CopyFromPbkCmd::ShowWaitNoteL()
    {
    delete iDecorator;
    iDecorator = NULL;
    iDecorator = Pbk2ProcessDecoratorFactory::CreateWaitNoteDecoratorL
        ( R_QTN_GEN_NOTE_FETCHING, ETrue );
    iDecorator->SetObserver( *this );
    iDecorator->ProcessStartedL( 0 ); // wait note doesn't care about amount
    }

// End of File
