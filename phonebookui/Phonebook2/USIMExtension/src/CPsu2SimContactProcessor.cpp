/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A class that processes sim contacts into appropritate form for
*                the copying process.
*                Handles errors related to SIM contact fields
*
*/


// INCLUDE FILES
#include "CPsu2SimContactProcessor.h"

// Phonebook 2
#include "Pbk2USimUI.hrh"
#include "CPsu2CopyToSimFieldInfoArray.h"
#include "CPsu2CharConv.h"
#include <MPbk2FieldProperty.h>
#include <MPbk2ContactNameFormatter.h>
#include <Pbk2ContactFieldCopy.h>

// Virtual Phonebook
#include <TVPbkFieldTypeMapping.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactStore.h>
#include <MVPbkFieldType.h>
#include <MVPbkBaseContact.h>
#include <MVPbkStoreContact.h>
#include <MVPbkStoreContactField.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactFieldTextData.h>
#include <CVPbkFieldTypeRefsList.h>
#include <CVPbkContactFieldIterator.h>
#include <CVPbkFieldTypeRefsList.h>
#include <vpbkeng.rsg>

// System includes
#include <gsmerror.h>
#include <exterror.h>
#include <barsread.h>
#include <coemain.h>
#include <featmgr.h>

// Debugging headers
#include <Pbk2Debug.h>


/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG
enum TPanicCode
    {
    EPreCond_SplitToSimContactsL
    };

void Panic(TInt aReason)
    {
    _LIT( KPanicText, "CPsu2SimContactProcessor");
    User::Panic( KPanicText, aReason );
    }

#endif // _DEBUG

/**
 * A helper class to keep track of the number fields of certain type
 */
class TFieldTypeCounter
    {
    public: // Construction

        /**
         * Constructor.
         *
         * @param aType     Field type.
         */
        TFieldTypeCounter(
                const MVPbkFieldType& aType ) :
                    iType( aType ), iCounter( 0 )
            {}

    public: // Data
        // Ref: Field type
        const MVPbkFieldType& iType;
        // Own: Number of fields
        TInt iCounter;
    };


/**
 * Returns matching field type counter object based on given field type.
 *
 * @param aArray    Array of field type counters.
 * @param aType     Field type of interest.
 * @return  Matching field type counter.
 */
TFieldTypeCounter& FieldTypeCounterL( 
        RArray<TFieldTypeCounter>& aArray, const MVPbkFieldType& aType )
    {
    const TInt count = aArray.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (aArray[i].iType.IsSame(aType))
            {
            return aArray[i];
            }
        }
    aArray.AppendL(TFieldTypeCounter(aType));

    return aArray[count];
    }

/**
 * Checks if the contact already has maximum amount fields
 * of given field type.
 *
 * @param aMaxAmountFieldInContact  Max number of allowed fields of type.
 * @param aType                     Field type.
 * @param aArray                    Array of field type counters.
 * @return  ETrue if contact already has max amount of fields of given type.
 */
TBool CheckNumberOfFieldsL( 
        TInt aMaxAmountFieldInContact, const MVPbkFieldType& aType,
        RArray<TFieldTypeCounter>& aArray )
    {
    TBool ret( EFalse );

    TFieldTypeCounter& counter = FieldTypeCounterL( aArray, aType );

    // Compare the max amount fields to the current amount of the
    // fields in the contact
    if (counter.iCounter >= aMaxAmountFieldInContact)
        {
        ret = ETrue;
        }

    // Add one to counter of the given type
    ++counter.iCounter;

    return ret;
    }

/**
 * Checks is given field type a number type.
 *
 * @param aSimType  Field type.
 * @return  ETrue if the field is of number type.
 */
TBool IsNumberType( const MVPbkFieldType& aSimType )
    {
    TBool ret = EFalse;

    // SIM number type is always Mobile phone (general) -> EVPbkVersitNameTEL,
    // therefore a selector is not needed
    TArray<TVPbkFieldVersitProperty> props = aSimType.VersitProperties();
    if ( props.Count() > 0 && props[0].Name() == EVPbkVersitNameTEL )
        {
        ret = ETrue;
        }

    return ret;
    }

/**
 * Returns a valid number, removes e.g spaces, braces...
 *
 * @param aSource           Source descriptor.
 * @param aNumberKeyMap     Number key mapping.
 * @return  Formatted valid number.
 */
HBufC* CreateValidNumberLC( 
        const TDesC& aSource, const TDesC& aNumberKeyMap )
    {
    const TInt length = aSource.Length();
    HBufC* number = HBufC::NewLC( length );
    TPtr ptr( number->Des() );
    for ( TInt i = 0; i < length; ++i )
        {
        if ( aNumberKeyMap.Locate( aSource[i] ) != KErrNotFound )
            {
            ptr.Append( aSource[i] );
            }
        }
    return number;
    }

} /// namespace

// --------------------------------------------------------------------------
// CPsu2SimContactProcessor::CPsu2SimContactProcessor
// --------------------------------------------------------------------------
//
CPsu2SimContactProcessor::CPsu2SimContactProcessor( 
        MVPbkContactStore& aTargetStore,
        CPsu2CopyToSimFieldInfoArray& aCopyToSimFieldInfoArray,
        MPbk2ContactNameFormatter& aNameFormatter,
        const MVPbkFieldTypeList& aMasterFieldTypeList )
        :   iTargetStore( aTargetStore ),
            iCopyToSimFieldInfoArray( aCopyToSimFieldInfoArray ),
            iNameFormatter( aNameFormatter ),
            iMasterFieldTypeList( aMasterFieldTypeList ),
            iSimMaxMatchPriority( 
                aTargetStore.StoreProperties().SupportedFields().
                  MaxMatchPriority() )
    {
    }

// --------------------------------------------------------------------------
// CPsu2SimContactProcessor::~CPsu2SimContactProcessor
// --------------------------------------------------------------------------
//
CPsu2SimContactProcessor::~CPsu2SimContactProcessor()
    {
    iNewSimContacts.ResetAndDestroy();
    iIncludedTypes.Close();
    delete iCharConvUcs2;
    delete iCharConvSms7Bit;
    }

// --------------------------------------------------------------------------
// CPsu2SimContactProcessor::NewL
// --------------------------------------------------------------------------
//
CPsu2SimContactProcessor* CPsu2SimContactProcessor::NewL( 
        MVPbkContactStore& aTargetStore,
        CPsu2CopyToSimFieldInfoArray& aCopyToSimFieldInfoArray,
        MPbk2ContactNameFormatter& aNameFormatter,
        const MVPbkFieldTypeList& aMasterFieldTypeList,
        RFs& aFs )
    {
    CPsu2SimContactProcessor* self = new( ELeave ) CPsu2SimContactProcessor
        ( aTargetStore, aCopyToSimFieldInfoArray, aNameFormatter,
          aMasterFieldTypeList );
    CleanupStack::PushL( self );
    self->ConstructL( aFs );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPsu2SimContactProcessor::ConstructL
// --------------------------------------------------------------------------
//
void CPsu2SimContactProcessor::ConstructL( RFs& aFs )
    {
    const MVPbkFieldTypeList& supportedTypes =
            iTargetStore.StoreProperties().SupportedFields();
    // Remove the unsupported fieldInfo from array first.
    iCopyToSimFieldInfoArray.RemoveUnSupportedFieldInfo( supportedTypes );
        
    const TInt count = iCopyToSimFieldInfoArray.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        iIncludedTypes.AppendL( 
            &iCopyToSimFieldInfoArray[i].SourceType() );
        }


    iCharConvUcs2 = CPsu2CharConv::NewL( aFs, KCharacterSetIdentifierUcs2 );
    // Symbian character converter uses CR/LF by default. (U)SIM uses CR.
    // (JustLineFeed is ok here because the converter is used just to check
    // SIM conversion lengths)
    iCharConvUcs2->SetDownGradeLf( CCnvCharacterSetConverter::
            EDowngradeExoticLineTerminatingCharactersToJustLineFeed );

    iCharConvSms7Bit = CPsu2CharConv::NewL( aFs, KCharacterSetIdentifierSms7Bit );
    iCharConvSms7Bit->SetDownGradeLf( CCnvCharacterSetConverter::
            EDowngradeExoticLineTerminatingCharactersToJustLineFeed );
    }

// --------------------------------------------------------------------------
// CPsu2SimContactProcessor::HandleSimError
// --------------------------------------------------------------------------
//
TBool CPsu2SimContactProcessor::HandleSimError( TInt aError )
    {
    TBool result = EFalse;
    switch (aError)
        {
        case KErrGsmSimServAnrFull:
            {
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
            ("CPsu2SimContactProcessor::HandleSimError KErrGsmSimServAnrFull"));

            // There might be several EF ANR files in USIM. Save the number
            // of error messages and use it later to check how many numbers
            // can be put to one contact
            ++iNumOfAdditionalNumberErrors;
            result = ETrue;
            break;
            }
        case KErrGsmSimServEmailFull:
            {
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                ("CPsu2SimContactProcessor::HandleSimError KErrGsmSimServEmailFull"));

            // SIM contact can not have emails anymore because EF(email) is full
            iSimErrors |= KPsu2EMailFullError;
            RemoveFieldTypesFromIncludedTypes( KPsu2EMailFullError );
            result = ETrue;
            break;
            }
        case KErrGsmSimServSneFull:
            {
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                ("CPsu2SimContactProcessor::HandleSimError KErrGsmSimServSneFull"));
            iSimErrors |= KPsu2SecondNameFullError;
            RemoveFieldTypesFromIncludedTypes( KPsu2SecondNameFullError );
            result = ETrue;
            break;
            }
        default:
            {
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                ("CPsu2SimContactProcessor::HandleSimError unhandled error code %d"),
                    aError );
            break;
            }
        }
    return result;
    }

// --------------------------------------------------------------------------
// CPsu2SimContactProcessor::CreateSimContactsL
//
// This called to create SIM contacts from a source contact
// The copying logic:
//  1) Create a SIM contact and add all the fields from the source
//     that possibly can be copied.
//  2) If the created SIM contact has too many fields for one SIM contact,
//     split the SIM contact. If the splitted contact has still too many
//     fields for one SIM contact split the splitted contact. This is
//     continued until the splitted contact doesn't need to be splitted
//     again.
// --------------------------------------------------------------------------
//
void CPsu2SimContactProcessor::CreateSimContactsL( 
        MVPbkStoreContact& aSourceContact,
        RPointerArray<MVPbkStoreContact>& aSimContacts )
    {
    iNewSimContacts.ResetAndDestroy();
    // Create a target contact
    MVPbkStoreContact* contact = iTargetStore.CreateNewContactLC();
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPsu2SimContactProcessor::CreateSimContactsL target contact created"));

    // Add name to the new contact
    AddNameFieldsL(aSourceContact, *contact);
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPsu2SimContactProcessor::CreateSimContactsL name fields added"));

    // Append fields that can possible be copied to the SIM
    AppendSupportedFieldsL(aSourceContact, *contact);
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPsu2SimContactProcessor::CreateSimContactsL supported fields added"));

    // Split the contact
    CleanupStack::Pop(); // contact
    SplitToSimContactsL(contact);
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPsu2SimContactProcessor::CreateSimContactsL contact splitted"));

    // Contacts that have only name are not copied to SIM
    RemoveContactsThatHaveOnlyNameL();
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPsu2SimContactProcessor::CreateSimContactsL name only contacts removed"));

    const TInt firstPos = 0;
    const TInt count = iNewSimContacts.Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        aSimContacts.InsertL( iNewSimContacts[i], firstPos );
        PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
            ("CPsu2SimContactProcessor::CreateSimContactsL contact added to new SIM contacts"));

        iNewSimContacts.Remove( i );
        }
    }

// --------------------------------------------------------------------------
// CPsu2SimContactProcessor::CreateFixedSimContactsL
//
// This is called when saving a SIM contact has failed and HandleSimError
// has handled the error. It means that the state of this processor has
// changed and the failed SIM contact can be splitted or there are fields
// that must be removed from the failed contact. After this the resulted
// SIM contacts will be copied again.
// --------------------------------------------------------------------------
//
void CPsu2SimContactProcessor::CreateFixedSimContactsL( 
        MVPbkStoreContact& aSimContact,
        RPointerArray<MVPbkStoreContact>& aSimContacts )
    {
    iNewSimContacts.ResetAndDestroy();
    // Create a target contact
    MVPbkStoreContact* contact = iTargetStore.CreateNewContactLC();
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPsu2SimContactProcessor::CreateFixedSimContactsL target contact created"));

    // Add name to the new contact
    AddNameFieldsL(aSimContact, *contact);
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPsu2SimContactProcessor::CreateFixedSimContactsL name fields added"));

    // Append fields that can possible be copied to the SIM
    AppendSupportedFieldsL(aSimContact, *contact);
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPsu2SimContactProcessor::CreateFixedSimContactsL supported fields added"));

    const TInt firstPos = 0;
    const TInt newCount = contact->Fields().FieldCount();
    if (newCount > 0 && newCount != aSimContact.Fields().FieldCount())
        {
        // After appending supported fields the amount of field is different
        // than in the original contact. This means that there has been
        // an error that has changed the included types list.
        if ( IsValidContactToSaveL( *contact ) )
            {
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                ("CPsu2SimContactProcessor::CreateFixedSimContactsL is valid contact to save"));

            aSimContacts.InsertL(contact, firstPos);
            CleanupStack::Pop(); // contact

            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                ("CPsu2SimContactProcessor::CreateFixedSimContactsL contact added to new SIM contacts"));
            }
        else
            {
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                ("CPsu2SimContactProcessor::CreateFixedSimContactsL is not valid contact to save"));

            CleanupStack::PopAndDestroy(); // contact
            }
        }
    else
        {
        // Split the contact
        CleanupStack::Pop(); // contact
        // Takes ownership of the contact
        if ( SplitToSimContactsL( contact ) )
            {
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                ("CPsu2SimContactProcessor::CreateFixedSimContactsL contact splitted"));

            // Contacts that have only name are not copied to SIM
            RemoveContactsThatHaveOnlyNameL();
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                ("CPsu2SimContactProcessor::CreateFixedSimContactsL name only contacts removed"));

            const TInt count = iNewSimContacts.Count();
            for (TInt i = count - 1; i >= 0; --i)
                {
                aSimContacts.InsertL(iNewSimContacts[i], firstPos);
                iNewSimContacts.Remove(i);

                PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                    ("CPsu2SimContactProcessor::CreateFixedSimContactsL contact added to new SIM contacts"));
                }
            }
        else
            {
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                ("CPsu2SimContactProcessor::CreateFixedSimContactsL contact does not split"));

            iNewSimContacts.ResetAndDestroy();
            }
        }
    }

// --------------------------------------------------------------------------
// CPsu2SimContactProcessor::CreateFixedSimContactsL
// --------------------------------------------------------------------------
//
TBool CPsu2SimContactProcessor::DetailsDropped()
    {
    TBool ret( EFalse );
    ret = iSimErrors & KPsu2EMailFullError;
    if ( !ret  )
        {
        ret = iSimErrors & KPsu2SecondNameFullError;
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CPsu2SimContactProcessor::RemoveFieldTypesFromIncludedTypes
//
// Removes error related field types from the included types so those
// types won't be copied to SIM contact anymore.
// --------------------------------------------------------------------------
//
void CPsu2SimContactProcessor::RemoveFieldTypesFromIncludedTypes( 
        TPsu2ErrorCode aBlockingError )
    {
    const TInt includedCount = iIncludedTypes.Count();
    for ( TInt i = includedCount - 1; i >= 0; --i )
        {
        const TPsu2CopyToSimFieldInfo* info = 
            iCopyToSimFieldInfoArray.FindInfoForSourceType( 
                *iIncludedTypes[i] );
        if ( info && info->BlockedByError( aBlockingError ) )
            {
            iIncludedTypes.Remove( i );
            }
        }
    }

// --------------------------------------------------------------------------
// CPsu2SimContactProcessor::AddNameFieldsL
//
// Adds name fields from source contact to the target.
// --------------------------------------------------------------------------
//
void CPsu2SimContactProcessor::AddNameFieldsL( 
        MVPbkStoreContact& aSource, MVPbkStoreContact& aTarget )
    {
    // Copy formatted name always to SIM's name field
    CopyTitleFieldDataL( aSource, aTarget,
        iCopyToSimFieldInfoArray.SimNameType() );
    }

// --------------------------------------------------------------------------
// CPsu2SimContactProcessor::CopyReadingFieldsL
//
// Copies reading fields in Japanese variants.
// --------------------------------------------------------------------------
//
void CPsu2SimContactProcessor::CopyReadingFieldsL( 
        MVPbkStoreContact& aSource, MVPbkStoreContact& aTarget )
    {
    // In case the name can be built without first name reading
    // and last name reading, the last name reading and first name reading
    // field data is combined according to name formatting rules and
    // the formatted name Reading is copied to Second name field
    // in Japanese variants.

    const MVPbkFieldTypeList& supportedTypes =
        iTargetStore.StoreProperties().SupportedFields();
    if ( supportedTypes.ContainsSame(
         iCopyToSimFieldInfoArray.LastNameReadingType() ) )
        {
        CVPbkFieldTypeRefsList* list = CVPbkFieldTypeRefsList::NewL();
        CleanupStack::PushL( list );
        // Get fields that actually a part of the formatted name
        CVPbkBaseContactFieldTypeListIterator* itr =
            iNameFormatter.ActualTitleFieldsLC( *list, aSource.Fields() );
        // Check if the title has reading fields
        TBool containsReading = EFalse;
        while ( itr->HasNext() )
            {
            TInt typeId =
                itr->Next()->BestMatchingFieldType()->FieldTypeResId();
            if ( typeId == R_VPBK_FIELD_TYPE_LASTNAMEREADING ||
                 typeId == R_VPBK_FIELD_TYPE_FIRSTNAMEREADING )
                {
                containsReading = ETrue;
                }
            }
        CleanupStack::PopAndDestroy(2, list);

        // If title doesn't contain reading fields then copy formatted
        // reading to SIM's reading (=second name) field
        if ( !containsReading )
            {
            // Create a temp contact from source store for getting
            // field collection of reading fields
            MVPbkStoreContact* tmpCnt =
                aSource.ParentStore().CreateNewContactLC();
            MVPbkStoreContactFieldCollection& sourceFields =
                aSource.Fields();
            const TInt fieldCount = sourceFields.FieldCount();
            for ( TInt i = 0; i < fieldCount; ++i )
                {
                const MVPbkFieldType* sourceType =
                    sourceFields.FieldAt( i ).BestMatchingFieldType();
                if ( sourceType )
                    {
                    TInt typeId = sourceType->FieldTypeResId();
                    if ( typeId == R_VPBK_FIELD_TYPE_LASTNAMEREADING ||
                         typeId == R_VPBK_FIELD_TYPE_FIRSTNAMEREADING )
                        {
                        Pbk2ContactFieldCopy::CopyFieldL
                            ( sourceFields.FieldAt( i ),
                              *sourceType, *tmpCnt );
                        }
                    }
                }
            if ( tmpCnt->Fields().FieldCount() > 0 )
                {
                // If there were reading fields in the source then copy
                // the formatted reading to SIM's last name reading
                CopyTitleFieldDataL( *tmpCnt, aTarget,
                    iCopyToSimFieldInfoArray.LastNameReadingType() );
                }
            CleanupStack::PopAndDestroy(); // tmpCnt
            }
        }
    }

// --------------------------------------------------------------------------
// CPsu2SimContactProcessor::AppendSupportedFieldsL
//
// Appends fields that are in included properties and supported
// by the SIM store.
// --------------------------------------------------------------------------
//
void CPsu2SimContactProcessor::AppendSupportedFieldsL( 
        MVPbkBaseContact& aSource, MVPbkStoreContact& aTarget)
    {
    const MVPbkBaseContactFieldCollection& fields = aSource.Fields();
    const TInt fieldCount = fields.FieldCount();
    const TInt typeCount = iIncludedTypes.Count();
    const TInt maxPriority = iMasterFieldTypeList.MaxMatchPriority();
    const MVPbkFieldTypeList& supportedTypes =
        iTargetStore.StoreProperties().SupportedFields();

    TBool contactCopyFailed = EFalse;
    for (TInt i = 0; i < fieldCount && !contactCopyFailed; ++i)
        {
        // Get the source field
        const MVPbkBaseContactField& field = fields.FieldAt(i);
        // Get the source field type
        const MVPbkFieldType* type = field.BestMatchingFieldType();

        TBool fieldCopied = EFalse;
        for (TInt j = 0; j < typeCount && type && !fieldCopied
                && !contactCopyFailed; ++j)
            {
            // Check if the field is one of the fields that can be copied
            // to the SIM
            if (type->IsSame(*iIncludedTypes[j]))
                {
                // The field possible can be copied to the SIM
                // Get the target(=SIM) field type for source type
                const MVPbkFieldType* simType =
                    iCopyToSimFieldInfoArray.ConvertToSimType(*type);
                // Check if the sim store supports the converted field
                if (simType && supportedTypes.ContainsSame(*simType))
                    {
                    if ( !CopyFieldL(field, aTarget, *simType) )
                        {
                        // If copying of one field fails then copying
                        // the contact fails
                        contactCopyFailed = ETrue;

                        PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                            ("CPsu2SimContactProcessor::AppendSupportedFieldsL contact copy failed"));
                        }
                    fieldCopied = ETrue;
                    }
                }
            }
        }

    if ( contactCopyFailed )
        {
        // Remove all fields from the target contact
        aTarget.RemoveAllFields();
        }
    }

// --------------------------------------------------------------------------
// CPsu2SimContactProcessor::SplitToSimContactsL
//
// Tries to split the source contact, return ETrue if splitted.
// --------------------------------------------------------------------------
//
TBool CPsu2SimContactProcessor::SplitToSimContactsL( 
        MVPbkStoreContact* aSourceContact )
    {
    __ASSERT_DEBUG(iNewSimContacts.Count() == 0,
        Panic(EPreCond_SplitToSimContactsL));

    MVPbkStoreContact* contact = aSourceContact;
    CleanupDeletePushL(contact);
    TBool result = EFalse;

    MVPbkStoreContact* splitted = SplitContactLC(*contact);
    if (splitted)
        {
        // Contact is splitted at least into two contacts
        iNewSimContacts.AppendL(contact);
        CleanupStack::Pop(2); // contact, splitted
        contact = splitted;
        CleanupDeletePushL(contact);
        while (contact)
            {
            // Split as long as must
            splitted = SplitContactLC(*contact);
            iNewSimContacts.AppendL(contact);
            if (splitted)
                {
                CleanupStack::Pop(2); // contact, splitted
                contact = splitted;
                CleanupDeletePushL(contact);
                }
            else
                {
                CleanupStack::Pop(); // contact
                contact = NULL;
                }
            }
        result = ETrue;
        }
    // Contact can be empty if the CopyFieldL failed
    else if ( contact->Fields().FieldCount() > 0 )
        {
        // Contact is not splitted
        iNewSimContacts.AppendL(contact);
        CleanupStack::Pop(); // contact
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPsu2SimContactProcessor::SplitContactLC
//
// Splits the master contact if there is too much fields to one
// SIM contact.
// --------------------------------------------------------------------------
//
MVPbkStoreContact* CPsu2SimContactProcessor::SplitContactLC( 
        MVPbkStoreContact& aSimContact )
    {
    MVPbkStoreContactFieldCollection& fields = aSimContact.Fields();
    TInt count =  fields.FieldCount();

    RArray<TFieldTypeCounter> fieldTypeCounterArray;
    CleanupClosePushL(fieldTypeCounterArray);

    RArray<TInt> removedIndexes;
    CleanupClosePushL(removedIndexes);

    MVPbkStoreContact* splittedContact = NULL;
    // Loop all fields of the source contact
    for (TInt i = 0; i < count; ++i)
        {
        const MVPbkFieldType* type =
            fields.FieldAt(i).BestMatchingFieldType();
        // Check all the data fields
        if (type && !iNameFormatter.IsTitleFieldType( *type ) )
            {
            TInt maxNumber = MaxNumberOfFieldL( aSimContact, *type );
            // Compare the max amount fields to the current amount of the
            // fields in the contact
            if ( CheckNumberOfFieldsL
                    ( maxNumber, *type, fieldTypeCounterArray ) )
                {
                // Create a new contact for the fields that can not fit to
                // the source contact
                if (!splittedContact)
                    {
                    splittedContact = iTargetStore.CreateNewContactLC();
                    AddNameFieldsL(aSimContact, *splittedContact);
                    }
                // Copy field to the new contact
                // and remove it from the source
                CopyFieldL( fields.FieldAt(i), *splittedContact, *type );
                removedIndexes.AppendL(i);
                }
            }
        }

    count = removedIndexes.Count();
    for (TInt k = count - 1; k >= 0; --k)
        {
        aSimContact.RemoveField(removedIndexes[k]);
        }

    if (splittedContact)
        {
        CleanupStack::Pop(); // splittedContact
        CleanupStack::PopAndDestroy(2); // removedIndexes,
                                        // fieldTypeCounterArray
        CleanupDeletePushL(splittedContact);
        }
    else
        {
        CleanupStack::PopAndDestroy(2); // removedIndexes,
                                        // fieldTypeCounterArray
        }
    return splittedContact;
    }

// --------------------------------------------------------------------------
// CPsu2SimContactProcessor::CopyFieldL
//
// Copies the source field to the target contact, EFalse if not copied.
// --------------------------------------------------------------------------
//
TBool CPsu2SimContactProcessor::CopyFieldL( 
        const MVPbkBaseContactField& aFieldToCopy,
        MVPbkStoreContact& aTarget,
        const MVPbkFieldType& aSimType )
    {
    TBool result = ETrue;
    if ( aFieldToCopy.FieldData().DataType() == EVPbkFieldStorageTypeText )
        {
        // Get source data
        const MVPbkContactFieldTextData& sourceData =
            MVPbkContactFieldTextData::Cast( aFieldToCopy.FieldData() );
        // Create target field
        MVPbkStoreContactField* field = aTarget.CreateFieldLC( aSimType );
        // Get target data
        MVPbkContactFieldTextData& targetData =
            MVPbkContactFieldTextData::Cast( field->FieldData() );

        // Invalid characters must be removed before copying if the
        // field is number
        if ( IsNumberType( aSimType ) )
            {
            HBufC* number = CreateValidNumberLC( sourceData.Text(),
                iCopyToSimFieldInfoArray.NumberKeyMap() );
            result = CopyFieldDataL( *number, targetData, aSimType );
            CleanupStack::PopAndDestroy( number );
            }
        else
            {
            result = CopyFieldDataL
                ( sourceData.Text(), targetData, aSimType );
            }

        if ( result )
            {
            aTarget.AddFieldL(field);
            CleanupStack::Pop(); // field
            }
        else
            {
            CleanupStack::PopAndDestroy(); // field
            }
        }
    return result;
    }

// --------------------------------------------------------------------------
// CPsu2SimContactProcessor::CopyTitleFieldDataL
//
// Copies title field from source contact to given field type.
// --------------------------------------------------------------------------
//
void CPsu2SimContactProcessor::CopyTitleFieldDataL( 
        MVPbkStoreContact& aSource, MVPbkStoreContact& aTarget,
        const MVPbkFieldType& aSimTitleFieldType )
    {
    HBufC* title = iNameFormatter.GetContactTitleOrNullL
        ( aSource.Fields(),
          MPbk2ContactNameFormatter::EPreserveLeadingSpaces );

    if (title)
        {
        CleanupStack::PushL( title );
        MVPbkStoreContactField* field =
            aTarget.CreateFieldLC( aSimTitleFieldType );
        MVPbkContactFieldTextData& data =
            MVPbkContactFieldTextData::Cast( field->FieldData() );
        TruncateAndCopyFieldDataL( *title, data );
        aTarget.AddFieldL(field);
        CleanupStack::Pop(field);
        CleanupStack::PopAndDestroy(title);
        }
    }


// --------------------------------------------------------------------------
// CPsu2SimContactProcessor::CopyFieldDataL
// --------------------------------------------------------------------------
//
TBool CPsu2SimContactProcessor::CopyFieldDataL( 
        const TDesC& aSource, MVPbkContactFieldTextData& aTarget,
        const MVPbkFieldType& aSimType )
    {
    TBool result = ETrue;
    // If truncation is allowed for the field type then truncate
    // and copy
    if ( iCopyToSimFieldInfoArray.TruncationAllowed( aSimType ) )
        {
        TruncateAndCopyFieldDataL( aSource, aTarget );
        }
    // otherwise check if there is enough space for data
    else if ( aSource.Length() <=
            aTarget.MaxLength() )
        {
        aTarget.SetTextL( aSource );
        }
    else
        {
        result = EFalse;
        }
    return result;
    }

// --------------------------------------------------------------------------
// CPsu2SimContactProcessor::TruncateAndCopyFieldDataL
// --------------------------------------------------------------------------
//
void CPsu2SimContactProcessor::TruncateAndCopyFieldDataL( 
        const TDesC& aSource, 
        MVPbkContactFieldTextData& aTarget )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPsu2SimContactProcessor::TruncateAndCopyFieldDataL max len: [%i]"),
        aTarget.MaxLength() );

    // Try first with SMS 7-bit encoding
    TInt unconvertedCount(0);
    TPtrC data = iCharConvSms7Bit->CheckFieldLengthL
            ( aSource, aTarget.MaxLength(), unconvertedCount );

    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPsu2SimContactProcessor:: max converted len: [%i], unconv: [%i]"),
        data.Length(), unconvertedCount );

    // If any characters could not be converted with SMS 7-bit encoding we need
    // to check length using UCS-2 encoding which requires more space per a
    // character.
    // With some character sets that contain less than 128 characters the
    // resulting data is shorter than the maximum field length allowed by the
    // (U)SIM. In other words we may truncate the text fields stored to the
    // (U)SIM more than would be required by the (U)SIM. This is because
    // there are actually three possible coding schemes for such character
    // sets available. See Annex B in document 3GPP TS 11.11.
    // The conversion is made by SIM ATK TSY so we would need to have some
    // SAT server API available to be able to determine the actual maximum
    // length for the data unambiguously.
    if( unconvertedCount > 0 )
        {
        // Leave one character extra space in case of non-7-bit conversions
        // This seems to be a feature of (U)SIM that it requires one extra byte
        // for unicode contact name (see also the 3GPP reference mentioned in
        // the comment above).
        data.Set( iCharConvUcs2->CheckFieldLengthL
                ( aSource, aTarget.MaxLength() - 1, unconvertedCount ) );
        PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
            ("CPsu2SimContactProcessor:: max converted len: [%i], unconv: [%i]"),
            data.Length(), unconvertedCount );
        }

    aTarget.SetTextL( data );
    }

// --------------------------------------------------------------------------
// CPsu2SimContactProcessor::MaxNumberOfFieldL
//
// Returns the maximum amount of field of given type that
// can be added to the contact.
// --------------------------------------------------------------------------
//
TInt CPsu2SimContactProcessor::MaxNumberOfFieldL( 
        MVPbkStoreContact& aContact, const MVPbkFieldType& aType )
    {
    TInt maxAmount( aContact.MaxNumberOfFieldL( aType ) );
    // For numbers it must be checked that is ANR file(s) of USIM full.
    if ( IsNumberType( aType ) )
        {
        // Reduce the max according to amount of ANR errors from TSY
        maxAmount -= iNumOfAdditionalNumberErrors;
        }
    return maxAmount;
    }

// --------------------------------------------------------------------------
// CPsu2SimContactProcessor::RemoveContactsThatHaveOnlyNameL
// --------------------------------------------------------------------------
//
void CPsu2SimContactProcessor::RemoveContactsThatHaveOnlyNameL()
    {
    const TInt cntCount = iNewSimContacts.Count();
    for ( TInt i = cntCount - 1; i >= 0; --i )
        {
        if ( !IsValidContactToSaveL( *iNewSimContacts[i] ) )
            {
            delete iNewSimContacts[i];
            iNewSimContacts.Remove( i );
            }
        }
    }

// --------------------------------------------------------------------------
// CPsu2SimContactProcessor::IsValidContactToSaveL
// --------------------------------------------------------------------------
//
TBool CPsu2SimContactProcessor::IsValidContactToSaveL( 
        MVPbkStoreContact& aSimContact )
    {
    // Specification says that "If the contact does not contain any number
    // it is not copied to SIM.". This is valid for 2G SIMs but it's assumed
    // that in USIM this means that contact that don't have any data fields
    // is not copied. In other words contact is valid if it has other than
    // title fields.
    MVPbkStoreContactFieldCollection& fields = aSimContact.Fields();
    const TInt fieldCount = fields.FieldCount();
    for ( TInt i = 0; i < fieldCount; ++i )
        {
        if ( !iNameFormatter.IsTitleField( fields.FieldAt( i ) ) )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

//  End of File
