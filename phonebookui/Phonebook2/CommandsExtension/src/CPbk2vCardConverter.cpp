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
* Description:  Phonebook 2 vCard converter.
*
*/


#include "CPbk2vCardConverter.h"

// Phonebook 2
#include "Pbk2SendCmdUtils.h"
#include <CPbk2AttachmentFile.h>
#include <pbk2commands.rsg>
#include <pbk2cmdextres.rsg>
#include <MPbk2ContactLinkIterator.h>

// Virtual Phonebook
#include <CVPbkVCardEng.h>
#include <MVPbkStoreContact.h>
#include <MPbk2ContactNameFormatter.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactFieldData.h>
#include <CVPbkContactManager.h>
#include <MVPbkFieldType.h>
#include <TVPbkFieldTypeMapping.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactFieldDateTimeData.h>
#include <MVPbkContactFieldBinaryData.h>
#include <MVPbkContactFieldUriData.h>
#include <MVPbkContactLink.h>

// System includes
#include <barsread.h>
#include <coemain.h>

// Debugging headers
#include <Pbk2Debug.h>

/// Unnamed namespace for local functions
namespace {

// CONSTANTS
_LIT(KPbk2vCardFileExtension, ".vcf");

// LOCAL FUNCTIONS


/**
 * Creates an empty vCard file from the given contact.
 *
 * @param aFs               File server session handle.
 * @param aContact          The contact.
 * @param aNameFormatter    Contact name formatter.
 * @return  Created vCard file.
 */

CPbk2AttachmentFile* CreateEmptyvCardFileL
        ( RFs& aFs, const MVPbkStoreContact& aContact,
          MPbk2ContactNameFormatter& aNameFormatter )
    {
    // Create the file name using contact's name
    HBufC* name = aNameFormatter.GetContactTitleL
        ( aContact.Fields(),
          MPbk2ContactNameFormatter::EPreserveLeadingSpaces );
    CleanupStack::PushL(name);
    HBufC* fileNameBuf = HBufC::NewLC(
        name->Length() + KPbk2vCardFileExtension().Length());
    TPtr fileName = fileNameBuf->Des();
    fileName = *name;
    fileName.Append(KPbk2vCardFileExtension);

    // Create attachment file object
    CPbk2AttachmentFile* result = CPbk2AttachmentFile::NewL
        (fileName, aFs, EFileWrite|EFileStream|EFileShareExclusive);

    CleanupStack::PopAndDestroy(2);  // fileNameBuf, name
    return result;
    }

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicLogic_AddFieldToContactL = 1,
    EPanicInvalidStorageType,
    EPanicPostCond_PrepareContactL
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2vCardConverter");
    User::Panic(KPanicText,aReason);
    }
#endif // _DEBUG


}  /// namespace


// --------------------------------------------------------------------------
// CPbk2vCardConverter::CPbk2vCardConverter
// --------------------------------------------------------------------------
//
inline CPbk2vCardConverter::CPbk2vCardConverter
        (RFs& aFs, CVPbkContactManager& aEngine, CVPbkVCardEng& aVCardEngine,
        MPbk2ContactNameFormatter& aNameFormatter) :
            CActive(EPriorityIdle),
            iFs(aFs), iEngine(aEngine), iVCardEngine(aVCardEngine),
            iNameFormatter(aNameFormatter), iField(NULL),
            iObserver(NULL), iOpIndex(0), iFieldLevelOperation(EFalse),
            iNextDriveTried(EFalse)
    {
    }

// --------------------------------------------------------------------------
// CPbk2vCardConverter::~CPbk2vCardConverter
// --------------------------------------------------------------------------
//
CPbk2vCardConverter::~CPbk2vCardConverter()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2vCardConverter, destructor (0x%x)"), this);
    Cancel();
    if (iVcardFiles)
        {
        iVcardFiles->ResetAndDestroy();
        delete iVcardFiles;
        }
    delete iVCardFile;
    delete iRetrieveOperation;
    delete iVCardContact;
    iFileWriteStream.Close();
    delete iContactOperation;
    iContacts.Reset(); // iContacts does not own its items
    }

// --------------------------------------------------------------------------
// CPbk2vCardConverter::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2vCardConverter::ConstructL()
    {
    CActiveScheduler::Add(this);
    iVcardFiles = new(ELeave) CPbk2AttachmentFileArray(1);
    }

// --------------------------------------------------------------------------
// CPbk2vCardConverter::CPbk2vCardConverter
// --------------------------------------------------------------------------
//
CPbk2vCardConverter* CPbk2vCardConverter::NewL
        ( RFs& aFs, CVPbkContactManager& aEngine,
          CVPbkVCardEng& aVCardEngine,
          MPbk2ContactNameFormatter& aNameFormatter )
    {
    CPbk2vCardConverter* self = new(ELeave) CPbk2vCardConverter
        ( aFs,aEngine,aVCardEngine, aNameFormatter );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2vCardConverter::ConvertContactL
// --------------------------------------------------------------------------
//
void CPbk2vCardConverter::ConvertContactL
        ( const TArray<MVPbkStoreContact*> aContacts,
          const MVPbkBaseContactField* aField, TInt aDataToSend,
          MPbk2vCardConverterObserver& aObserver )
    {
    iField = aField;
    iFieldLevelOperation = ETrue;
    ConvertContactsL(aContacts, aDataToSend, aObserver);
    }

// --------------------------------------------------------------------------
// CPbk2vCardConverter::ConvertContactsL
// --------------------------------------------------------------------------
//
void CPbk2vCardConverter::ConvertContactsL
        ( const TArray<MVPbkStoreContact*> aContacts,
          TInt aDataToSend, MPbk2vCardConverterObserver& aObserver )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2vCardConverter::ConvertContactsL(0x%x)"), this);
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2vCardConverter::ConvertContactsL, Contacts Count=%d"), 
        aContacts.Count());
    Reset();
    iObserver = &aObserver;
    iDataToSend = aDataToSend;
    for (TInt i = 0; i < aContacts.Count(); ++i)
        {
        iContacts.AppendL(aContacts[i]);
        }

    Start();
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2vCardConverter::ConvertContactsL, end (0x%x)"), this);
    }

// --------------------------------------------------------------------------
// CPbk2vCardConverter::ConvertContactsL
// --------------------------------------------------------------------------
//
void CPbk2vCardConverter::ConvertContactsL
        ( MPbk2ContactLinkIterator& aIterator, TInt aDataToSend,
          MPbk2vCardConverterObserver& aObserver )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2vCardConverter::ConvertContactsL, dataToSend=%d"), aDataToSend);
    Reset();
    iObserver = &aObserver;
    iDataToSend = aDataToSend;
    iIterator = &aIterator;
    Start();
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2vCardConverter::ConvertContactsL, end (0x%x)"), this);
    }


// --------------------------------------------------------------------------
// CPbk2vCardConverter::FileNames
// --------------------------------------------------------------------------
//
MDesC16Array& CPbk2vCardConverter::FileNames() const
    {
    return ( *iVcardFiles );
    }

// --------------------------------------------------------------------------
// CPbk2vCardConverter::Reset
// --------------------------------------------------------------------------
//
void CPbk2vCardConverter::Reset()
    {
    iIterator = NULL;
    iVcardFiles->ResetAndDestroy();
    iDataToSend = ESendAllData;
    if (!iFieldLevelOperation)
        {
        iField = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2vCardConverter::AttachmentFileArray
// --------------------------------------------------------------------------
//
CPbk2AttachmentFileArray& CPbk2vCardConverter::AttachmentFileArray()
    {
    return *iVcardFiles;
    }

// --------------------------------------------------------------------------
// CPbk2vCardConverter::RunL
// --------------------------------------------------------------------------
//
void CPbk2vCardConverter::RunL()
    {
    if ( iIterator && iIterator->HasNext() )
        {
        HandleNextContactL( NULL );
        }
    else if (iOpIndex < iContacts.Count())
        {
        HandleNextContactL( iContacts[iOpIndex] );
        ++iOpIndex;
        }
    else
        {
        iObserver->ConversionDone(iOpIndex);
        }
    }

// --------------------------------------------------------------------------
// CPbk2vCardConverter::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2vCardConverter::DoCancel()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2vCardConverter::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2vCardConverter::RunError(TInt aError)
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2vCardConverter::RunError=%d"), aError);
    Reset();
    iObserver->ConversionError(aError);
    return aError;
    }

// --------------------------------------------------------------------------
// CPbk2vCardConverter::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2vCardConverter::VPbkSingleContactOperationComplete
        ( MVPbkContactOperationBase& /*aOperation*/ ,
          MVPbkStoreContact* aContact )
    {
    if ( iRetrieveOperation )
        {
        delete iRetrieveOperation;
        iRetrieveOperation = NULL;
        TRAPD( err, HandleNextContactL( aContact ) );
        delete aContact;
        aContact = NULL;
        if ( err != KErrNone )
            {
            CCoeEnv::Static()->HandleError( err );        
            }
        }
    else
        {
        delete iContactOperation;
        iContactOperation = NULL;

        // Delete temp contact
        delete iVCardContact;
        iVCardContact = NULL;

        FinalizeVCardExport();

        IssueRequest();
        }
    }

// --------------------------------------------------------------------------
// CPbk2vCardConverter::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2vCardConverter::VPbkSingleContactOperationFailed
        ( MVPbkContactOperationBase& /*aOperation*/, TInt aError )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
      ("CPbk2vCardConverter::VPbkSingleContactOperationFailed (0x%x)"), this);
    if ( iRetrieveOperation )
        {
        delete iRetrieveOperation;
        iRetrieveOperation = NULL;
        }
    else
        {
        delete iContactOperation;
        iContactOperation = NULL;

        iFileWriteStream.Close();

        if ( iNextDriveTried )
            {
            delete iVCardFile;
            iVCardFile = NULL;
			RunError( aError );
            }
        else
            {
            TRAPD( err, 
 	        	{
				// Try to switch next drive if previous failed
               iVCardFile->SwitchDriveL();
               iFileWriteStream.Attach( iVCardFile->File() );
               iContactOperation = iVCardEngine.ExportVCardL
                    ( iFileWriteStream, *iVCardContact, *this );
                }); // TRAPD
                
            if ( err != KErrNone )
           		{
            	RunError( aError );
            	}
            	
            iNextDriveTried = ETrue;
            }
        }
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2vCardConverter::VPbkSingleContactOperationFailed, end"), this);
    }

// --------------------------------------------------------------------------
// CPbk2vCardConverter::CreatevCardFileL
// Creates a file containing vCard of aContact and attaches the file to
// internal list of files.
// --------------------------------------------------------------------------
//
void CPbk2vCardConverter::CreatevCardFileL(MVPbkStoreContact* aContact)
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2vCardConverter::CreatevCardFileL (0x%x)"), this);
        
    // Check first, is the contact item empty
    // If the contact is not empty, file it so it gets sent
    if (!Pbk2SendCmdUtils::IsContactEmpty(aContact))
        {
        iVCardFile = CreateEmptyvCardFileL(iFs, *aContact, iNameFormatter);
        // Create temp handle, so that iVCardFile stays valid until we close
        // it. Stream will take care of closing tempHandle when closing stream
        RFile tempHandle;
        tempHandle.Duplicate( iVCardFile->File() );
        iFileWriteStream.Attach( tempHandle );

        iContactOperation =
            iVCardEngine.ExportVCardL(iFileWriteStream, *aContact, *this);
        }
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2vCardConverter::CreatevCardFileL, end (0x%x)"), this);
    }

// --------------------------------------------------------------------------
// CPbk2vCardConverter::PrepareContactL
// --------------------------------------------------------------------------
//
MVPbkStoreContact* CPbk2vCardConverter::PrepareContactL
        ( MVPbkStoreContact* aContact )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2vCardConverter::PrepareContactL (0x%x)"), this);
    MVPbkStoreContact* vCardContact = NULL;
    // Create a temporary contact
    vCardContact = aContact->ParentStore().CreateNewContactLC();
    // Copy currently focused field's data to a dummy contact
    FillTemporaryContactL(*vCardContact, *aContact, *iField);

    __ASSERT_DEBUG(vCardContact, Panic(EPanicPostCond_PrepareContactL));

    CleanupStack::Pop(); // vCardContact
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2vCardConverter::PrepareContactL, end (0x%x)"), this);
    return vCardContact;
    }

// --------------------------------------------------------------------------
// CPbk2vCardConverter::FillTemporaryContactL
// --------------------------------------------------------------------------
//
void CPbk2vCardConverter::FillTemporaryContactL
        ( MVPbkStoreContact& aDestItem,
          const MVPbkStoreContact& aSourceItem,
          const MVPbkBaseContactField& aDataField ) const
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2vCardConverter::FillTemporaryContactL (0x%x)"), this);
    if (iDataToSend == ESendAllData)
        {
        TInt fieldCount = aSourceItem.Fields().FieldCount();
        PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2vCardConverter::FillTemporaryContactL, fieldCount=%d"), 
        fieldCount);
        
        const MVPbkStoreContactFieldCollection& fieldSet =
            aSourceItem.Fields();
        for (TInt i=0; i < fieldCount; ++i)
            {
            const MVPbkStoreContactField& field = fieldSet.FieldAt(i);
            AddFieldToContactL(aDestItem, field);
            }
        }
    else if (iDataToSend == ESendAllDataWithoutPicture)
        {
        TInt fieldCount = aSourceItem.Fields().FieldCount();
        const MVPbkStoreContactFieldCollection& fieldSet =
            aSourceItem.Fields();
        const MVPbkFieldType& thumbnailFieldType =
                Pbk2SendCmdUtils::ReadFieldTypeFromResL(
                    R_THUMBNAIL_FIELD_TYPE, iEngine.FieldTypes());            
        const MVPbkFieldType& imageFieldType =
                    Pbk2SendCmdUtils::ReadFieldTypeFromResL(
                        R_IMAGE_FIELD_TYPE, iEngine.FieldTypes());                    
        for (TInt i=0; i < fieldCount; ++i)
            {
            const MVPbkStoreContactField& field = fieldSet.FieldAt(i);
            // Do not add thumbnail or image            
            if (!Pbk2SendCmdUtils::IsFieldMatching(
                    field, thumbnailFieldType, iEngine.FieldTypes()) &&
                !Pbk2SendCmdUtils::IsFieldMatching(
                        field, imageFieldType, iEngine.FieldTypes()))
                {                                
                AddFieldToContactL(aDestItem, field);
                }
            }
        }
    else // ESendCurrentItem
        {
        // Actual single data field
        AddFieldToContactL(aDestItem, aDataField);

        // According to specification lastname & firstname
        // (when existing) must be added when sending single item
        const MVPbkBaseContactField* lastName =
            Pbk2SendCmdUtils::FindFieldL(
                aSourceItem, R_LNAME_FIELD_TYPE, iEngine.FieldTypes());
        if (lastName)
            {
            if (!lastName->FieldData().IsEmpty())
                {
                AddFieldToContactL(aDestItem, *lastName);
                }
            }

        const MVPbkBaseContactField* firstName =
            Pbk2SendCmdUtils::FindFieldL(
                aSourceItem, R_FNAME_FIELD_TYPE, iEngine.FieldTypes());
        if (firstName)
            {
            if (!firstName->FieldData().IsEmpty())
                {
                AddFieldToContactL(aDestItem, *firstName);
                }
            }
        }
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
       ("CPbk2vCardConverter::FillTemporaryContactL, end (0x%x)"), this);
    }

// --------------------------------------------------------------------------
// CPbk2vCardConverter::AddFieldToContactL
// --------------------------------------------------------------------------
//
void CPbk2vCardConverter::AddFieldToContactL
        ( MVPbkStoreContact& aDestItem,
          const MVPbkBaseContactField& aSourceField ) const
    {
    const MVPbkFieldType* fieldType = aSourceField.BestMatchingFieldType();
    if ( fieldType )
        {
        MVPbkStoreContactField* dstField =
            aDestItem.CreateFieldLC( *fieldType );

        __ASSERT_DEBUG( dstField, Panic( EPanicLogic_AddFieldToContactL ) );

        switch (dstField->FieldData().DataType())
            {
            case EVPbkFieldStorageTypeText:
                {
                TPtrC data = MVPbkContactFieldTextData::Cast(
                                aSourceField.FieldData()).Text();
                MVPbkContactFieldTextData::Cast(dstField->FieldData())
                    .SetTextL(data);
                break;
                }
            case EVPbkFieldStorageTypeUri:
                {
                TPtrC data = MVPbkContactFieldUriData::Cast(
                                aSourceField.FieldData()).Uri();
                MVPbkContactFieldUriData::Cast(dstField->FieldData())
                    .SetUriL(data);
                break;
                }
            case EVPbkFieldStorageTypeDateTime:
                {
                TTime time = MVPbkContactFieldDateTimeData::Cast(
                                aSourceField.FieldData()).DateTime();
                MVPbkContactFieldDateTimeData::Cast(dstField->FieldData())
                    .SetDateTime(time);
                break;
                }
            case EVPbkFieldStorageTypeBinary:
                {
                TPtrC8 data = MVPbkContactFieldBinaryData::Cast(
                    aSourceField.FieldData()).BinaryData();
                MVPbkContactFieldBinaryData::Cast(dstField->FieldData())
                    .SetBinaryDataL(data);
                break;
                }
            default:
                {
                __ASSERT_DEBUG(EFalse, Panic(EPanicInvalidStorageType));
                break;
                }
            }
        aDestItem.AddFieldL(dstField);
        CleanupStack::Pop();
        }
    }

// --------------------------------------------------------------------------
// CPbk2vCardConverter::Start
// --------------------------------------------------------------------------
//
void CPbk2vCardConverter::Start()
    {
    iOpIndex = 0;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2vCardConverter::HandleNextContactL
// --------------------------------------------------------------------------
//
void CPbk2vCardConverter::HandleNextContactL( MVPbkStoreContact* aContact )
    {
    if ( aContact )
        {
        if (iVCardContact)
            {
            delete iVCardContact;
            iVCardContact = NULL;
            }
        iVCardContact = PrepareContactL(aContact);
        CreatevCardFileL(iVCardContact);
        }
    else
        {
        MVPbkContactLink* link = iIterator->NextL();
        CleanupDeletePushL( link );
        iRetrieveOperation = iEngine.RetrieveContactL( *link, *this );
        CleanupStack::PopAndDestroy(); // link
        }
    }

// --------------------------------------------------------------------------
// CPbk2vCardConverter::IssueRequest
// --------------------------------------------------------------------------
//
void CPbk2vCardConverter::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }

// --------------------------------------------------------------------------
// CPbk2vCardConverter::FinalizeVCardExport
// --------------------------------------------------------------------------
//
void CPbk2vCardConverter::FinalizeVCardExport()
    {
   	PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2vCardConverter::FinalizeVCardExport (0x%x)"), this);
    TRAPD( err, 
        {
        // commit stream after exportvcard has completed
        iFileWriteStream.CommitL();
        iFileWriteStream.Close();
        iVcardFiles->AppendL(iVCardFile); // Takes ownership
        iVCardFile = NULL;
        }); //TRAPD
        
    if ( err != KErrNone )
        {
        RunError( err );
        }
            
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2vCardConverter::FinalizeVCardExport, end (0x%x)"), this);
    }

//  End of File
