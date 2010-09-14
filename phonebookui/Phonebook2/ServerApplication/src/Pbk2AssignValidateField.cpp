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
* Description:  Phonebook 2 assign service field validator.
*
*/


#include "Pbk2AssignValidateField.h"

// Phonebook 2
#include <CPbk2PresentationContact.h>
#include <CPbk2PresentationContactFieldCollection.h>

// Virtual Phonebook
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkFieldType.h>
#include <vpbkeng.rsg>


/// Unnamed namespace for local definitions
namespace {

const TInt KOne = 1;

/**
 * Checks does a given file exist.
 *
 * @param aFs           File server session.
 * @param aFileName     Name of the file to check.
 * @return  ETrue if file exists.
 */
inline TBool FileExists( RFs& aFs, const TDesC& aFileName )
    {
    TEntry ignore;
    return ( aFs.Entry( aFileName, ignore ) == KErrNone );
    }

/**
 * Returns the amount of fields of given type in given contact.
 *
 * @param aContact      The contact to inspect.
 * @param aFieldType    The field type to inspect.
 * @return  Amount of fields of given type in the given contact.
 */
TInt CurrentAmountOfFieldTypeInContact
        ( CPbk2PresentationContact& aContact,
          const MVPbkFieldType& aFieldType )
    {
    TInt res = 0;

    // Count of contact fields
    const TInt count = aContact.Fields().FieldCount();

    const TInt maxMatchPriority = aContact.ContactStore().
        StoreProperties().SupportedFields().MaxMatchPriority();

    for ( TInt i = 0; i < count; ++i )
        {
        const MVPbkFieldType* type = NULL;
        const MVPbkStoreContactField& field = aContact.Fields().FieldAt( i );

        for ( TInt j = 0; j < maxMatchPriority && !type; ++j )
            {
            type = field.MatchFieldType( j );
            }

        if ( (aFieldType.IsSame( *type ) ) )
            {
            ++res;
            }
        }

    return res;
    }

} /// namespace

// --------------------------------------------------------------------------
// Pbk2AssignValidateField::ValidateFieldTypeUsageInContactL
// --------------------------------------------------------------------------
//
TInt Pbk2AssignValidateField::ValidateFieldTypeUsageInContactL
        ( CPbk2PresentationContact& aContact,
          const MVPbkFieldType& aFieldType, RFs& aFsSession,
          TInt& aFieldIndex )
    {
    TInt ret = KErrNotFound;

    const MVPbkStoreContactFieldCollection& fields = aContact.Fields();
    const MVPbkStoreContactField* field = NULL;
    const TInt fieldCount = fields.FieldCount();

    for ( TInt i( 0 ); i < fieldCount; ++i)
        {
        field = &fields.FieldAt(i);
        const MVPbkFieldType* fieldType = field->BestMatchingFieldType();

        if ( fieldType && fieldType->IsSame( aFieldType ) )
            {
            // Field type was found from contact
            ret = KErrNone;
            aFieldIndex = aContact.PresentationFields().StoreIndexOfField( i );
            break;
            }
        }

    if ( ret == KErrNone )
        {
        // Field was found from contact but if it was a ringing tone or an
        // image field do a file exist check. If the file those fields are
        // referencing is not found from the file system, the field is
        // hidden in Phonebook UI. It is not removed, but hidden because
        // the file might reside on a removable media and come back later on.
        //
        // The logic here is to not to ask overwrite contact field
        // confirmation from the user if the file does not currently exist.

        TInt fieldTypeResId = aFieldType.FieldTypeResId();
        if ( fieldTypeResId == R_VPBK_FIELD_TYPE_RINGTONE ||
             fieldTypeResId == R_VPBK_FIELD_TYPE_CALLEROBJIMG )
            {
            const MVPbkContactFieldTextData& textData =
                MVPbkContactFieldTextData::Cast( field->FieldData() );

            if ( !FileExists( aFsSession, textData.Text() ) )
                {
                // The file the contact field indicates is not
                // on the file system, return KErrPathNotFound to indicate
                // that the contact haves this field but the file refenced
                // is not found
                ret = KErrPathNotFound;
                // Do not change the aFieldIndex value here though.
                }
            }
        }

    if ( ret == KErrNone )
        {
        // Get maximum number of fields of given type in the contact
        const TInt max = aContact.MaxNumberOfFieldL( aFieldType );

        if ( max != KVPbkStoreContactUnlimitedNumber )
            {
            // Get current amount of fields of given type in the contact
            TInt current = CurrentAmountOfFieldTypeInContact
                ( aContact, aFieldType );

            if ( current >= max )
                {
                // Return code depends on multiplicity
                if ( max == KOne )
                    {
                    ret = KErrAlreadyExists;
                    }
                else
                    {
                    ret = KErrNotSupported;
                    }
                }
            }
        }

    return ret;
    }

// End of File
