/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*          Provides phonebook card converter class methods
*
*/


// INCLUDE FILES
#include "CPbkvCardConverter.h"

#include <CPbkFieldInfo.h>
#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
#include <CPbkAttachmentFile.h>
#include <BCardEng.h>

#include <pbkdebug.h>

// Unnamed namespace for local functions
namespace {

// LOCAL FUNCTIONS

CPbkAttachmentFile* CreateEmptyvCardFileLC
        (RFs& aFs, const CPbkContactItem& aContact)
    {
    // Create the file name using contact's name
    HBufC* name = aContact.GetContactTitleL();
    CleanupStack::PushL(name);
    HBufC* fileNameBuf = HBufC::NewLC(name->Length() + KPbkvCardFileExtension().Length());
    TPtr fileName = fileNameBuf->Des();
    fileName = *name;
    fileName.Append(KPbkvCardFileExtension);

    // Create attachment file object
    CPbkAttachmentFile* result = CPbkAttachmentFile::NewL
        (fileName, aFs, EFileWrite|EFileStream|EFileShareExclusive);

    CleanupStack::PopAndDestroy(2);  // fileNameBuf, name
    CleanupStack::PushL(result);
    return result;
    }

void WritevCardFileL
        (CBCardEngine& aBcardEngine,
        RFileWriteStream& aStream, 
        CPbkContactItem& aContact)
    {
    aBcardEngine.ExportBusinessCardL(aStream, aContact);
    aStream.CommitL();
    }

void WritevCardToAttachmentFileL
        (CBCardEngine& aBcardEngine,
        CPbkAttachmentFile& vCardFile, 
        CPbkContactItem& aContact)
    {
    // Write vCard to attachment file
    TBool retry=EFalse;
    do
        {
        RFileWriteStream fileWriteStream(vCardFile.File());
        fileWriteStream.PushL();
        TRAPD(err, WritevCardFileL(aBcardEngine,fileWriteStream,aContact));
        if (err != KErrNone)
            {
            if (retry)
                {
                // Out of retry attempts
                User::Leave(err);
                }
            // If write fails, try again on different drive
            vCardFile.SwitchDriveL();
            retry = ETrue;
            }
        CleanupStack::PopAndDestroy(); // fileWriteStream
        }
    while (retry);
    }

// LOCAL DEBUG CODE
#ifdef _DEBUG
enum TPanicCode
    {
	EPanicLogic_AddFieldToContactL = 1,
    EPanicInvalidStorageType,
    EPanicPostCond_PrepareContactLC
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkvCardConverter");
    User::Panic(KPanicText,aReason);
    }
#endif // _DEBUG


}  // namespace


// ================= MEMBER FUNCTIONS =======================

inline CPbkvCardConverter::CPbkvCardConverter
        (RFs& aFs, CPbkContactEngine& aEngine, CBCardEngine& aBCardEngine) :
    iFs(aFs), iEngine(aEngine), iBCardEngine(aBCardEngine), iField(NULL)
    {
    }

inline void CPbkvCardConverter::ConstructL()
    {
    iVcardFiles = new(ELeave) CPbkAttachmentFileArray(1);
    }

CPbkvCardConverter* CPbkvCardConverter::NewL
        (RFs& aFs,
        CPbkContactEngine& aEngine, 
        CBCardEngine& aBCardEngine)
    {
    CPbkvCardConverter* self =
        new(ELeave) CPbkvCardConverter (aFs,aEngine,aBCardEngine);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CPbkvCardConverter::~CPbkvCardConverter()
    {
    delete iContacts;
    if (iVcardFiles)
        {
        iVcardFiles->ResetAndDestroy();
        delete iVcardFiles;
        }
    }

void CPbkvCardConverter::ConvertContactL(TContactItemId aContactId,
        TPbkContactItemField* aField, TInt aDataToSend)
    {
    Reset();
    iDataToSend = aDataToSend;
    iField = aField;
    if (!iContacts)
        {
        iContacts = CContactIdArray::NewL();
        }
    iContacts->AddL(aContactId);
    }


void CPbkvCardConverter::ConvertContactsL(const CContactIdArray& aContacts,
        TInt aDataToSend)
    {
    Reset();
    iDataToSend = aDataToSend;
    delete iContacts;
    iContacts = NULL;
    iContacts = CContactIdArray::NewL(&aContacts);
    }

MDesC16Array& CPbkvCardConverter::FileNames() const
    {
    return (*iVcardFiles);
    }

void CPbkvCardConverter::StepL()
    {
    if (iContacts && iContacts->Count() > 0)
        {
        const TInt index = iContacts->Count()-1;
        const TContactItemId contactId = (*iContacts)[index];
        iContacts->Remove(index);
        CreatevCardFileL(contactId);
        }
    }

TInt CPbkvCardConverter::TotalNumberOfSteps()
    {
    TInt ret(0);
    if (iContacts && iContacts->Count() > 0)
        {
        ret = iContacts->Count();
        }
    return ret;
    }

TBool CPbkvCardConverter::IsProcessDone() const
    {
    return (!iContacts || iContacts->Count()==0);
    }

void CPbkvCardConverter::ProcessFinished()
    {
    }

TInt CPbkvCardConverter::HandleStepError(TInt aError)
    {
    Reset();
    return aError;
    }

void CPbkvCardConverter::ProcessCanceled()
    {
    Reset();
    }

void CPbkvCardConverter::Reset()
    {
    iVcardFiles->ResetAndDestroy();
    iDataToSend = ESendAllData;
    if (iContacts)
        {
        iContacts->Reset();
        }
    iField = NULL;
    }

/**
 * Creates a file containing vCard of aContactId and attaches the file to 
 * internal list of files.
 */
void CPbkvCardConverter::CreatevCardFileL(TContactItemId aContactId)
    {
    // Prepare contact by cropping image or other fields if so desired
    CPbkContactItem* vCardContact = PrepareContactLC(aContactId);

	// Check first, is the contact item empty
	TBool emptyContact(ETrue);
	TInt fieldCount = vCardContact->CardFields().Count();
	CPbkFieldArray& fieldSet = vCardContact->CardFields();
	for (TInt i=0; i < fieldCount; ++i)
		{
		TPbkContactItemField field = fieldSet[i];
		if (!field.IsEmptyOrAllSpaces())
			{
			// There was a field, which was not empty (or all spaces),
			// that's enough for us, the contact gets sent
			emptyContact = EFalse;
			break;
			}
		}

	// If the contact is not empty, file it so it gets sent
	if (!emptyContact)
		{
		CPbkAttachmentFile* vCardFile = CreateEmptyvCardFileLC(iFs, *vCardContact);
		WritevCardToAttachmentFileL(iBCardEngine, *vCardFile, *vCardContact);
		iVcardFiles->AppendL(vCardFile);
		CleanupStack::Pop(vCardFile);
		}

    CleanupStack::PopAndDestroy(vCardContact);
    }


CPbkContactItem* CPbkvCardConverter::PrepareContactLC(TContactItemId aContactId)
    {
    CPbkContactItem* vCardContact = NULL;
    if (iDataToSend == ESendAllData)
        {
        // If all data is to be sent, just read aContactId
        // into a contact item...
        vCardContact = iEngine.ReadContactLC(aContactId);
        }
    else
        {
        // ...otherwise create a temporary contact
        vCardContact = iEngine.CreateEmptyContactL();
        CleanupStack::PushL(vCardContact);
        CPbkContactItem* sourceContact = iEngine.ReadContactLC(aContactId);

        // Copy currently focused field's data to a dummy contact
        FillTemporaryContactL(*vCardContact, *sourceContact, *iField);
        CleanupStack::PopAndDestroy(sourceContact);
        }

    __ASSERT_DEBUG(vCardContact, Panic(EPanicPostCond_PrepareContactLC));

    return vCardContact;
    }


void CPbkvCardConverter::FillTemporaryContactL(CPbkContactItem& aDestItem,
        CPbkContactItem& aSourceItem, const TPbkContactItemField& aDataField) const
    {
    if (iDataToSend == ESendAllDataWithoutPicture)
        {
	    TInt fieldCount = aSourceItem.CardFields().Count();
	    CPbkFieldArray& fieldSet = aSourceItem.CardFields();
	    for (TInt i=0; i < fieldCount; ++i)
		    {
		    TPbkContactItemField field = fieldSet[i];
            // Do not add thumbnail
            if (field.PbkFieldId() != EPbkFieldIdThumbnailImage)
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
        if ( aDataField.FieldInfo().FieldId() != EPbkFieldIdLastName )
            {
            TPbkContactItemField* lastName =
    		    aSourceItem.FindField(EPbkFieldIdLastName);
            if (lastName)
                {
                if (!lastName->IsEmptyOrAllSpaces())
                    {
                    AddFieldToContactL(aDestItem, *lastName);
                    }
                }
            }
        if ( aDataField.FieldInfo().FieldId() != EPbkFieldIdFirstName )
            {
            TPbkContactItemField* firstName =
    		    aSourceItem.FindField(EPbkFieldIdFirstName);
            if (firstName)
                {
                if (!firstName->IsEmptyOrAllSpaces())
                    {
                    AddFieldToContactL(aDestItem, *firstName);
                    }
                }
            }
        }
    }

void CPbkvCardConverter::AddFieldToContactL(
        CPbkContactItem& aDestItem, 
        const TPbkContactItemField& aSourceField) const
    {
    TPbkContactItemField* dstField =
		aDestItem.AddOrReturnUnusedFieldL(aSourceField.FieldInfo());
    __ASSERT_DEBUG(dstField, Panic(EPanicLogic_AddFieldToContactL));
    switch (dstField->StorageType())
        {
        case KStorageTypeText:
            {
            HBufC* data = aSourceField.Text().AllocL();
            dstField->TextStorage()->SetText(data); // takes ownership of data
            break;
            }
        case KStorageTypeDateTime:
            {
            TTime time = aSourceField.DateTimeStorage()->Time();
            dstField->DateTimeStorage()->SetTime(time);
            break;
            }
        default:
            {
            __ASSERT_DEBUG(EFalse, Panic(EPanicInvalidStorageType));
            break;
            }
        }    
    }

//  End of File  
