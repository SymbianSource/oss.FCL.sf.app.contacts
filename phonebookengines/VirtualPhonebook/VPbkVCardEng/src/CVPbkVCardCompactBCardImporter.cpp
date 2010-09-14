/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Virtual Phonebook VCard compact BCard importer
*
*/



// System includes
#include <versit.h>

#include "CVPbkVCardData.h"
#include "CVPbkVCardCompactBCardImporter.h"
#include "VPbkVCardEngError.h"
#include "CVPbkVCardFieldTypeProperty.h"
#include <vpbkeng.rsg>

// From Virtual Phonebook engine
#include <MVPbkContactFieldTextData.h>
#include <MVPbkFieldType.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkStoreContactField.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactCopyPolicy.h>
#include <MVPbkContactOperationBase.h>


#ifdef _DEBUG
using namespace VPbkVCardEngError;
#endif // _DEBUG

namespace 
    {
    enum TVPbkBCardImporterState
        {
        EGetLine,
        EStartTitleCopy,
        EAddAddress,
        EStop
        };
    
    _LIT(KBusinessCardText, "Business Card");   // Business card signature
    _LIT(KCloseParenthesis, ")");
    _LIT(KTelText, "tel ");
    _LIT(KFaxText, "fax ");
    _LIT(KSeparatorText, ", ");
    const TText KOpenParenthesis = '(';
    // Assume compact business card is corrupted if single field length
    // exceeds 200 bytes. This value is not officially specified anywhere.
    const TInt KBufferSize = 200;
    const TInt KStartPosition = 4;
    const TInt KLabelStartPosition = 5;
    const TInt KLabelValueDelimiterLength = 2;
    const TInt KGranularity( 2 );

    // Comapct business card fixed field line numbers.
    enum TVPbkCBCFieldLines
        {
        ECBCSignatureLine = 1,
        ECBCNameLine,
        ECBCCompanyLine,
        ECBCTitleLine
        };
    } // namespace

CVPbkVCardCompactBCardImporter::CVPbkVCardCompactBCardImporter(
    RPointerArray<MVPbkStoreContact>& aImportedContacts,
    MVPbkContactStore& aTargetStore,
    RReadStream &aSourceStream,
    CVPbkVCardData& aData )
    :   CActive( EPriorityStandard ),
        iImportedContacts( aImportedContacts ),
        iTargetStore(aTargetStore),
        iData( aData ),
        iSource( aSourceStream )
    {
    }

CVPbkVCardCompactBCardImporter* CVPbkVCardCompactBCardImporter::NewL(
    RPointerArray<MVPbkStoreContact>& aImportedContacts,
    MVPbkContactStore& aTargetStore, 
    RReadStream &aSourceStream,
    CVPbkVCardData& aData )
	{
    CVPbkVCardCompactBCardImporter* self = new (ELeave)
        CVPbkVCardCompactBCardImporter(
            aImportedContacts,
            aTargetStore,
            aSourceStream,
            aData );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
	}

void CVPbkVCardCompactBCardImporter::ConstructL()
    {
    CActiveScheduler::Add(this);
    iAddressArray = new( ELeave ) CDesCArrayFlat( KGranularity );
    iCopyPolicy = &iData.CopyPolicyL( iTargetStore.StoreProperties().Uri() );
    }

CVPbkVCardCompactBCardImporter::~CVPbkVCardCompactBCardImporter()
    {
    Cancel();
    delete iAddToFieldOperation;
    delete iCurContact;
    delete iAddressArray;
    delete iValue;
    delete iLabel;
    delete iLine;
    }

void CVPbkVCardCompactBCardImporter::StartL()
    {
    iCurContact = iTargetStore.CreateNewContactLC();
    CleanupStack::Pop();
    iState = EGetLine;
    IssueRequest();
    }

void CVPbkVCardCompactBCardImporter::SetObserver( 
        MVPbkImportOperationObserver& aObserver )
    {
    iObserver = &aObserver;
    }
    
void CVPbkVCardCompactBCardImporter::RunL()
    {
    switch (iState)
        {
        case EGetLine:
            {
            if (GetLineL())
                {
                ParseLineL();
                }
            break;
            }
        case EAddAddress:
	        {
	    	AddAddressL();
	    	break;
	        }
        case EStartTitleCopy:
            {
            StartTitleCopyL();
            break;
            }
        case EStop:
        	{
        	iObserver->ContactsImportingCompleted();
        	break;	
        	}
        default:
            {
            // Do nothing
            break;
            }
        }
    }
    
void CVPbkVCardCompactBCardImporter::DoCancel()
    {
    delete iAddToFieldOperation;
    iAddToFieldOperation = NULL;
    }
    
TInt CVPbkVCardCompactBCardImporter::RunError(TInt aError)
    {
    iObserver->ContactImportingFailed( aError );
    return KErrNone;
    }

void CVPbkVCardCompactBCardImporter::FieldAddedToContact(
        MVPbkContactOperationBase& aOperation)
    {
    if ( &aOperation == iAddToFieldOperation )
        {
        delete iAddToFieldOperation;
        iAddToFieldOperation = NULL;
        IssueRequest();
        }
    }

void CVPbkVCardCompactBCardImporter::FieldAddingFailed(
        MVPbkContactOperationBase& aOperation, TInt aError)
    {
    if ( &aOperation == iAddToFieldOperation )
        {
        delete iAddToFieldOperation;
        iAddToFieldOperation = NULL;
        if (aError == KErrNotSupported )
            {
            // Target contact didn't support field -> continue
            IssueRequest();
            }
        else if ( aError == KErrOverflow )
            {
            // Target contact is full of that field
            TRAPD( res, HandleContactSwitchL() );
            HandleError( res );
            }
        else
            {
            iObserver->ContactImportingFailed( aError );
            }
        }
    }

void CVPbkVCardCompactBCardImporter::FieldsCopiedToContact(
        MVPbkContactOperationBase& /*aOperation*/)
    {
    ++iTargetContactIndex;
    if ( iTargetContactIndex >= iImportedContacts.Count() )
        {
        iObserver->ContactsImported();
        }
    else
        {
        TRAPD( res, CopyTitleL() );
        HandleError( res );    
        }
    }

void CVPbkVCardCompactBCardImporter::FieldsCopyFailed(
        MVPbkContactOperationBase& /*aOperation*/, 
        TInt aError)
    {
    HandleError( aError );
    }
            
void CVPbkVCardCompactBCardImporter::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }
    
TBool CVPbkVCardCompactBCardImporter::GetLineL()
    {
    delete iLine; 
    iLine = NULL;
    iLine = HBufC::NewL( KBufferSize );
    TPtr line = iLine->Des();
    
    TBool result = EFalse;
    TInt retVal( GetLineFromStreamL( line, iSource ) );
    if ( retVal == KErrNone || retVal == KErrEof  )
		{
		if(line.Length() > 0)
		    {
		    ++iLineCount;
		    result = ETrue;	
		    }
	    else
		    {
		    iState = EStop;
		    IssueRequest(); 	
		    }

		 }
	if(retVal == KErrEof && line.Length() > 0 )
		{
		iState = EAddAddress;
		}
    
    return result;
    }

void CVPbkVCardCompactBCardImporter::ParseLineL()
    {
    TPtr line = iLine->Des();
    TInt resId( ResolveFieldTypeResIdL( line ) );
    
    if ( resId == KErrNotFound )
        {
        IssueRequest();
        }
    else if ( resId == R_VPBK_FIELD_TYPE_LANDPHONEGEN || 
              resId == R_VPBK_FIELD_TYPE_FAXNUMBERGEN )
        {
        // Asynchronous
        ValueAndLabelToBufL( line, resId );
        }
    else if ( resId == R_VPBK_FIELD_TYPE_ADDRLABELGEN )
        {
        // Synchronous
        ValueToArrayL( line );
        IssueRequest();
        }
    else
        {
        // Asynchronous
        ValueToBufL( line, resId );
        }
    }

void CVPbkVCardCompactBCardImporter::ValueToBufL( 
    const TDesC& aBuffer, TInt aResId )
    {
    const MVPbkFieldType* fieldType = FindFieldTypeL( aResId );
    __ASSERT_DEBUG( fieldType, 
        VPbkVCardEngError::Panic( VPbkVCardEngError::ENullFieldType ) );

    AddFieldWithValueL( *fieldType, aBuffer, NULL );
    }

void CVPbkVCardCompactBCardImporter::ValueToArrayL( const TDesC& aBuffer )
    {
    // Address is saved after all other fields have been saved
    iAddressArray->AppendL( aBuffer );

    if( iValueLength )
        {
        iValueLength += KSeparatorText().Length();
        }

    iValueLength += aBuffer.Length();
    }

void CVPbkVCardCompactBCardImporter::ValueAndLabelToBufL( 
    const TDesC& aBuffer, TInt aResId )
    {
    const MVPbkFieldType* fieldType = FindFieldTypeL( aResId );
    __ASSERT_DEBUG( fieldType, 
        VPbkVCardEngError::Panic( VPbkVCardEngError::ENullFieldType ) );

	// Read tel num label
	TInt position( KStartPosition );

    delete iLabel;
    iLabel = NULL;
    if( aBuffer[KStartPosition] == KOpenParenthesis )
        {
        position = aBuffer.Find( KCloseParenthesis );
        if( position == KErrNotFound )
            {
            return;
            }
        iLabel = HBufC::NewL( position - KStartPosition );
    	TPtr writebuf = iLabel->Des();
	    writebuf = KNullDesC;
        for( TInt i = KLabelStartPosition; i<position; i++ )
            {
            writebuf.Append( aBuffer[i] );
            }
		// Jump few characters
        position += KLabelValueDelimiterLength;
        }

	// Read value
    delete iValue;
    iValue = NULL;
    iValue = HBufC::NewL( aBuffer.Length() - position );
   	TPtr writebuf = iValue->Des();
    writebuf = KNullDesC;
    for( TInt i = position; i < aBuffer.Length(); i++ )
        {
        writebuf.Append( aBuffer[i] );
        }

    AddFieldWithValueL( *fieldType, *iValue, iLabel );
    }

void CVPbkVCardCompactBCardImporter::AddAddressL()
    {
    if ( iAddressArray->Count() > 0 )
        {
        // Then concatenate them with commas into one string
        // and use that as field value.
        HBufC* fielddata = HBufC::NewLC( iValueLength );
        TPtr writebuf = fielddata->Des();
        writebuf = KNullDesC;
        TInt count = iAddressArray->Count();
        for( TInt i = 0; i < count; ++i )
            {
            if( i )
                {
                writebuf.Append( KSeparatorText );
                }
            writebuf.Append( iAddressArray->MdcaPoint( i ) );
            }

        const MVPbkFieldType* fieldType = FindFieldTypeL( R_VPBK_FIELD_TYPE_ADDRLABELGEN );
        __ASSERT_DEBUG( fieldType, 
            VPbkVCardEngError::Panic( VPbkVCardEngError::ENullFieldType ) );

        AddFieldWithValueL( *fieldType, *fielddata, NULL );
        CleanupStack::PopAndDestroy( fielddata );
        }
    else
        {
        IssueRequest();
        }
        
    // Address is the last field so continue with titles after address
    // has been added, or not added
    iState = EStartTitleCopy;
    }
    
TInt CVPbkVCardCompactBCardImporter::ResolveFieldTypeResIdL( const TDesC& aBuffer )
    {
    TInt resId( KErrNotFound );
    switch ( iLineCount )
        {
        case ECBCSignatureLine:
            {
            if ( aBuffer.Match( KBusinessCardText() ) == KErrNotFound )
                {
                User::Leave( KErrNotSupported );
                }
            return resId;
            }

        case ECBCNameLine: // Name
            {
            resId = R_VPBK_FIELD_TYPE_LASTNAME;
            break;
            }

        case ECBCCompanyLine: // Company name
            {
            resId = R_VPBK_FIELD_TYPE_COMPANYNAME;
            break;
            }

        case ECBCTitleLine: // Title
            {
            resId = R_VPBK_FIELD_TYPE_JOBTITLE;
            break;
            }
        }

    if ( resId == KErrNotFound ) // Tel numbers
        {
        if ( aBuffer.Find( KTelText ) >= 0 )
            {
            resId = R_VPBK_FIELD_TYPE_LANDPHONEGEN;
            }

        else if ( aBuffer.Find( KFaxText ) >= 0 )
            {
            resId = R_VPBK_FIELD_TYPE_FAXNUMBERGEN;
            }
        }
    if ( !iEMailFetchedFlag && resId == KErrNotFound )
        {
        resId = R_VPBK_FIELD_TYPE_EMAILGEN; // Email address
        iEMailFetchedFlag = ETrue;
        }

    if ( resId == KErrNotFound )// Postal address
        {
        resId = R_VPBK_FIELD_TYPE_ADDRLABELGEN;
        }

    return resId;    
    }

void CVPbkVCardCompactBCardImporter::AddFieldWithValueL( 
    const MVPbkFieldType& aFieldType, const TDesC& aValue, HBufC* aLabel )
    {
    __ASSERT_DEBUG( !iAddToFieldOperation, 
        VPbkVCardEngError::Panic( 
        VPbkVCardEngError::EAddToFieldOperationAlreadyExists ) );
    
    iAddToFieldOperation = iCopyPolicy->AddFieldToContactL(
        *iCurContact, aFieldType, aValue, aLabel, NULL, *this);
    }

void CVPbkVCardCompactBCardImporter::StartTitleCopyL()
    {
    MoveCurrentContactToImportsL();
    
    // If vcard was split to multiple contacts then copy
    // title fields from the first imported contact to the other contacts.
    const TInt oneContactImported = 1;
    if ( iImportedContacts.Count() <= oneContactImported )
        {
        iObserver->ContactsImported();
        }
    else
        {
        iTargetContactIndex = oneContactImported;
        CopyTitleL();    
        }
    }
        
const MVPbkFieldType* CVPbkVCardCompactBCardImporter::FindFieldTypeL( 
        TInt aFieldIndex )
    {
    return iData.SupportedFieldTypes().Find( aFieldIndex );
    }

TInt CVPbkVCardCompactBCardImporter::GetByteFromStream(
    TInt8& aByte, RReadStream& aSourceStream)
    {
    TRAPD(err, aByte = aSourceStream.ReadInt8L());
    return err;
    }

TInt CVPbkVCardCompactBCardImporter::GetLineFromStreamL(
    TDes& aBuf, RReadStream& aSourceStream)
    {
    aBuf.SetLength(0);
    TInt8 byte=0;
    TInt retVal( KErrNone );
    while( ( retVal = GetByteFromStream(byte, aSourceStream) ) == KErrNone)
        {
        if(byte == EKeyLineFeed)
            {
            break;
            }
        if(byte != EKeyEnter)
            {
            if (aBuf.Length() < aBuf.MaxLength())
                {
                aBuf.Append(byte);
                }
            else
                {
                User::Leave(KErrCorrupt);
                }
            }
        }

    return retVal;
    }

void CVPbkVCardCompactBCardImporter::HandleContactSwitchL()
    {
    // Current contact is full -> Create a new contact and
    // add the same line to that one.
    MoveCurrentContactToImportsL();
    iCurContact = iTargetStore.CreateNewContactLC();
    CleanupStack::Pop();
    if ( iLine && iLine->Length() > 0 )
        {
        // Parse same line again
        ParseLineL();
        }
    else
        {
        // End of file -> continue with address
        AddAddressL();
        }
    }

void CVPbkVCardCompactBCardImporter::CopyTitleL()
    {
    delete iAddToFieldOperation;
    iAddToFieldOperation = NULL;
    iAddToFieldOperation = iCopyPolicy->CopyTitleFieldsL(
        *iImportedContacts[0],
        *iImportedContacts[iTargetContactIndex],
        *this );
    }
    
void CVPbkVCardCompactBCardImporter::MoveCurrentContactToImportsL()
    {
    iImportedContacts.AppendL( iCurContact );
    iCurContact = NULL;
    }
    
void CVPbkVCardCompactBCardImporter::HandleError( TInt aError )
    {
    if ( aError != KErrNone )
        {
        iObserver->ContactImportingFailed( aError );
        }
    }
    TBool CVPbkVCardCompactBCardImporter::IsOwncard()
    {
    	return EFalse;
    }

// End of file
