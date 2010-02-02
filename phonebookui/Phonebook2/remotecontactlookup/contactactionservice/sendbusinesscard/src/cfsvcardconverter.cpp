/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  vCard converter.
*
*/


#include "cfsvcardconverter.h"

// Phonebook 2
#include "CPbk2AttachmentFile.h"

//Cmail
#include "fssendcmdutils.h"
//Cmail

#include <pbk2rclsendbusinesscardpluginimpl.rsg>

// Virtual Phonebook
#include <CVPbkVCardEng.h>
#include <MVPbkStoreContact.h>
#include <MPbk2ContactNameFormatter.h>
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
#include <MVPbkContactOperationBase.h>

// System includes
#include <barsread.h>
#include <coemain.h>

/// Unnamed namespace for local functions
namespace {

// CONSTANTS
_LIT(KFSvCardFileExtension, ".vcf");

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
        name->Length() + KFSvCardFileExtension().Length());
    TPtr fileName = fileNameBuf->Des();
    fileName = *name;
    fileName.Append(KFSvCardFileExtension);

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
    _LIT(KPanicText, "CFscvCardConverter");
    User::Panic(KPanicText,aReason);
    }
#endif // _DEBUG


}  /// namespace


// --------------------------------------------------------------------------
// CFscvCardConverter::CFscvCardConverter
// --------------------------------------------------------------------------
//
inline CFscvCardConverter::CFscvCardConverter
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
// CFscvCardConverter::~CFscvCardConverter
// --------------------------------------------------------------------------
//
CFscvCardConverter::~CFscvCardConverter()
    {
    
    if (iVcardFiles)
        {
        iVcardFiles->ResetAndDestroy();
        delete iVcardFiles;
        }
    delete iVCardFile;
    delete iVCardContact;
    iFileWriteStream.Close();
    iContacts.Reset(); // iContacts does not own its items
    }

// --------------------------------------------------------------------------
// CFscvCardConverter::ConstructL
// --------------------------------------------------------------------------
//
inline void CFscvCardConverter::ConstructL()
    {
    CActiveScheduler::Add(this);
    iVcardFiles = new(ELeave) CPbk2AttachmentFileArray(1);
    }

// --------------------------------------------------------------------------
// CFscvCardConverter::CFscvCardConverter
// --------------------------------------------------------------------------
//
CFscvCardConverter* CFscvCardConverter::NewL
        ( RFs& aFs, CVPbkContactManager& aEngine,
          CVPbkVCardEng& aVCardEngine,
          MPbk2ContactNameFormatter& aNameFormatter )
    {
    CFscvCardConverter* self = new(ELeave) CFscvCardConverter
        ( aFs,aEngine,aVCardEngine, aNameFormatter );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CFscvCardConverter::ConvertContactL
// --------------------------------------------------------------------------
//
void CFscvCardConverter::ConvertContactL
        ( const TArray<MVPbkStoreContact*> aContacts,
          const MVPbkBaseContactField* aField, TInt aDataToSend,
          MFsvCardConverterObserver& aObserver )
    {
    iField = aField;
    iFieldLevelOperation = ETrue;
    ConvertContactsL(aContacts, aDataToSend, aObserver);
    }

// --------------------------------------------------------------------------
// CFscvCardConverter::ConvertContactsL
// --------------------------------------------------------------------------
//
void CFscvCardConverter::ConvertContactsL
        ( TArray <MVPbkStoreContact*> aContacts,
          TInt aDataToSend, MFsvCardConverterObserver& aObserver )
    {
    Reset();
    iObserver = &aObserver;
    iDataToSend = aDataToSend;
    for (TInt i = 0; i < aContacts.Count(); ++i)
        {
        iContacts.AppendL(aContacts[i]);
        }

    Start();
    }

// --------------------------------------------------------------------------
// CFscvCardConverter::FileNames
// --------------------------------------------------------------------------
//
MDesC16Array& CFscvCardConverter::FileNames() const
    {
    return ( *iVcardFiles );
    }

// --------------------------------------------------------------------------
// CFscvCardConverter::Reset
// --------------------------------------------------------------------------
//
void CFscvCardConverter::Reset()
    {    
    iContacts.Reset(); // iContacts does not own its items
    iVcardFiles->ResetAndDestroy();
    iDataToSend = ESendAllData;
    if (!iFieldLevelOperation)
        {
        iField = NULL;
        }
    }

// --------------------------------------------------------------------------
// CFscvCardConverter::AttachmentFileArray
// --------------------------------------------------------------------------
//
CPbk2AttachmentFileArray& CFscvCardConverter::AttachmentFileArray()
    {
    return *iVcardFiles;
    }

// --------------------------------------------------------------------------
// CFscvCardConverter::RunL
// --------------------------------------------------------------------------
//
void CFscvCardConverter::RunL()
    {
    if (iOpIndex < iContacts.Count())
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
// CFscvCardConverter::DoCancel
// --------------------------------------------------------------------------
//
void CFscvCardConverter::DoCancel()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CFscvCardConverter::RunError
// --------------------------------------------------------------------------
//
TInt CFscvCardConverter::RunError(TInt aError)
    {
    Reset();
    iObserver->ConversionError(aError);
    return aError;
    }

// --------------------------------------------------------------------------
// CFscvCardConverter::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CFscvCardConverter::VPbkSingleContactOperationComplete
        ( MVPbkContactOperationBase& /*aOperation*/ ,
          MVPbkStoreContact* /*aContact*/ )
    {
    delete iContactOperation;
    iContactOperation = NULL;

    // Delete temp contact
    delete iVCardContact;
    iVCardContact = NULL;

    FinalizeVCardExport();

    IssueRequest();
    }

// --------------------------------------------------------------------------
// CFscvCardConverter::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CFscvCardConverter::VPbkSingleContactOperationFailed
        ( MVPbkContactOperationBase& /*aOperation*/, TInt aError )
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
        

// --------------------------------------------------------------------------
// CFscvCardConverter::CreatevCardFileL
// Creates a file containing vCard of aContact and attaches the file to
// internal list of files.
// --------------------------------------------------------------------------
//
void CFscvCardConverter::CreatevCardFileL(MVPbkStoreContact* aContact)
    {
        
    // Check first, is the contact item empty
    // If the contact is not empty, file it so it gets sent
    if (!FsSendCmdUtils::IsContactEmpty(aContact))
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
    }

// --------------------------------------------------------------------------
// CFscvCardConverter::PrepareContactL
// --------------------------------------------------------------------------
//
MVPbkStoreContact* CFscvCardConverter::PrepareContactL
        ( MVPbkStoreContact* aContact )
    {
    
    MVPbkStoreContact* vCardContact = NULL;
    // Create a temporary contact
    vCardContact = aContact->ParentStore().CreateNewContactLC();
    // Copy currently focused field's data to a dummy contact
    FillTemporaryContactL(*vCardContact, *aContact, *iField);

    __ASSERT_DEBUG(vCardContact, Panic(EPanicPostCond_PrepareContactL));

    CleanupStack::Pop(); // vCardContact
    
    return vCardContact;
    }

// --------------------------------------------------------------------------
// CFscvCardConverter::FillTemporaryContactL
// --------------------------------------------------------------------------
//
void CFscvCardConverter::FillTemporaryContactL
        ( MVPbkStoreContact& aDestItem,
          const MVPbkStoreContact& aSourceItem,
          const MVPbkBaseContactField& aDataField ) const
    {
    
    if (iDataToSend == ESendAllData)
        {
        TInt fieldCount = aSourceItem.Fields().FieldCount();
        
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
                FsSendCmdUtils::ReadFieldTypeFromResL(
                    R_FSSENDBC_THUMBNAIL_FIELD_TYPE, iEngine.FieldTypes());            
        const MVPbkFieldType& imageFieldType =
                    FsSendCmdUtils::ReadFieldTypeFromResL(
                        R_FSSENDBC_IMAGE_FIELD_TYPE, iEngine.FieldTypes());                    
        for (TInt i=0; i < fieldCount; ++i)
            {
            const MVPbkStoreContactField& field = fieldSet.FieldAt(i);
            // Do not add thumbnail or image            
            if (!FsSendCmdUtils::IsFieldMatching(
                    field, thumbnailFieldType, iEngine.FieldTypes()) &&
                !FsSendCmdUtils::IsFieldMatching(
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
            FsSendCmdUtils::FindFieldL(
                aSourceItem, R_FSSENDBC_LNAME_FIELD_TYPE, iEngine.FieldTypes());
        if (lastName)
            {
            if (!lastName->FieldData().IsEmpty())
                {
                AddFieldToContactL(aDestItem, *lastName);
                }
            }

        const MVPbkBaseContactField* firstName =
            FsSendCmdUtils::FindFieldL(
                aSourceItem, R_FSSENDBC_FNAME_FIELD_TYPE, iEngine.FieldTypes());
        if (firstName)
            {
            if (!firstName->FieldData().IsEmpty())
                {
                AddFieldToContactL(aDestItem, *firstName);
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CFscvCardConverter::AddFieldToContactL
// --------------------------------------------------------------------------
//
void CFscvCardConverter::AddFieldToContactL
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
// CFscvCardConverter::Start
// --------------------------------------------------------------------------
//
void CFscvCardConverter::Start()
    {
    iOpIndex = 0;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CFscvCardConverter::HandleNextContactL
// --------------------------------------------------------------------------
//
void CFscvCardConverter::HandleNextContactL( MVPbkStoreContact* aContact )
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
    }

// --------------------------------------------------------------------------
// CFscvCardConverter::IssueRequest
// --------------------------------------------------------------------------
//
void CFscvCardConverter::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }

// --------------------------------------------------------------------------
// CFscvCardConverter::FinalizeVCardExport
// --------------------------------------------------------------------------
//
void CFscvCardConverter::FinalizeVCardExport()
    {
   	
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
            
    
    }

//  End of File
