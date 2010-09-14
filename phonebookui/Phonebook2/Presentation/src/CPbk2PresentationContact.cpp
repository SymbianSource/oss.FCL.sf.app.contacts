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
* Description:  A phonebook2 presentation level contact
*
*/


#include "CPbk2PresentationContact.h"

// Phonebook 2
#include <CPbk2PresentationContactField.h>
#include <CPbk2PresentationContactFieldCollection.h>
#include <MPbk2FieldProperty.h>
#include <MPbk2FieldProperty2.h>
#include <CPbk2FieldPropertyArray.h>
#include <Phonebook2PrivateCRKeys.h>

// Virtual Phonebook
#include <MVPbkFieldType.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactFieldUriData.h>
#include <MVPbkStoreContact2.h>
#include <TVPbkFieldVersitProperty.h>
#include <VPbkVariant.hrh>
#include <vpbkeng.rsg>

// System includes
#include <centralrepository.h>

/// Unnamed namespace for local definitions
namespace {
    // This is done locally here because we are not dependent from CommonUI
    TBool LocalVariationFeatureEnabled( TInt aFeatureFlag )
        {
        TInt lvFlags( KErrNotFound );
        TBool ret( EFalse );

        CRepository* key = NULL;
        // Read local variation flags
        TRAPD( err, key =
                CRepository::NewL(TUid::Uid(KCRUidPhonebook)));
        // if NewL fails do not set iLVFlags, new query is made next time
        if ( err == KErrNone )
            {
            err = key->Get(KPhonebookLocalVariationFlags, lvFlags);
            if (err != KErrNone)
                {
                // If there were problems reading the flags,
                // assume everything is off
                lvFlags = 0;
                }
            delete key;
            }

        if ( lvFlags != KErrNotFound )
            {
            ret = lvFlags & aFeatureFlag;
            }

        return ret;
        }

    TBool IsVariatedLocallyL( const MPbk2FieldProperty& aProperty )
        {
        // Check if "pager" field should be part of the template,
        // USA flagged feature
        // Read local variation flag for pager field
        TBool useInTemplate ( ETrue );
        if ( aProperty.FieldType().FieldTypeResId() ==
                    R_VPBK_FIELD_TYPE_PAGERNUMBER )
            {
            useInTemplate =
                    LocalVariationFeatureEnabled( EVPbkLVPagerInTemplate );
            }

        // Returns ETrue if field with aProperty should be added to contact.
        return useInTemplate;
        }

} /// namespace


// --------------------------------------------------------------------------
// CPbk2PresentationContact::CPbk2PresentationContact
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CPbk2PresentationContact::CPbk2PresentationContact(
    MVPbkStoreContact& aStoreContact,
    const MPbk2FieldPropertyArray& aFieldProperties)
    :   iStoreContact(aStoreContact),
        iFieldProperties(aFieldProperties)
    {
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContact::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CPbk2PresentationContact::ConstructL()
    {
    iCollection = CPbk2PresentationContactFieldCollection::NewL(
        iFieldProperties, iStoreContact.Fields(), *this);
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContact::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2PresentationContact* CPbk2PresentationContact::NewL(
    MVPbkStoreContact& aStoreContact,
    const MPbk2FieldPropertyArray& aFieldProperties)
    {
    CPbk2PresentationContact* self =
        new( ELeave ) CPbk2PresentationContact(aStoreContact, aFieldProperties);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// Destructor
CPbk2PresentationContact::~CPbk2PresentationContact()
    {
    delete iCollection;
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContact::IsFieldAdditionPossibleL
// --------------------------------------------------------------------------
//
EXPORT_C TBool CPbk2PresentationContact::IsFieldAdditionPossibleL(
        const MVPbkFieldType& aType) const
    {
    TInt max = MaxNumberOfFieldL(aType);
    if (max == KVPbkStoreContactUnlimitedNumber)
        {
        return ETrue;
        }
    else
        {
        TInt cur = CurrentAmountOfFieldTypeInContact(aType, KNullDesC);
        return cur < max;
        }
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContact::AddSupportedTemplateFieldsL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2PresentationContact::AddSupportedTemplateFieldsL()
    {
    // Adds fields that belongs to the template according to UI specification.
    // In addition checks that the store supports the field and that
    // there isn't already a same type of field in the contact.
    // Also locally variated features are checked.
    const MVPbkFieldTypeList& supportedTypes =
        iStoreContact.ParentStore().StoreProperties().SupportedFields();
    const TInt count = iFieldProperties.Count();
    for (TInt i = 0; i < count; ++i)
        {
        const MPbk2FieldProperty& prop = iFieldProperties.At( i );
        
        MPbk2FieldProperty2* fieldPropertyExtension =
            reinterpret_cast<MPbk2FieldProperty2*>(
                const_cast<MPbk2FieldProperty&>( prop ).
                    FieldPropertyExtension(
                        KMPbk2FieldPropertyExtension2Uid ) );
        
        if ( fieldPropertyExtension != NULL ) 
            {
            if ( prop.Flags() & KPbk2FieldFlagTemplateField &&
                supportedTypes.ContainsSame( prop.FieldType() ) &&
                CurrentAmountOfFieldTypeInContact( prop.FieldType(),
                    fieldPropertyExtension->XSpName() ) == 0  &&
                IsVariatedLocallyL( prop ) )
                {
                AddFieldL( CreateFieldLC( prop.FieldType() ),
                    fieldPropertyExtension->XSpName() );
                CleanupStack::Pop(); // new field
                }
            }
        }
    }
// --------------------------------------------------------------------------
// CPbk2PresentationContact::AvailableFieldsToAddL
// --------------------------------------------------------------------------
//
EXPORT_C CArrayPtr<const MPbk2FieldProperty>*
    CPbk2PresentationContact::AvailableFieldsToAddL() const
    {
    const TInt count = iFieldProperties.Count();
    CArrayPtrFlat<const MPbk2FieldProperty>* propArray =
        new(ELeave) CArrayPtrFlat<const MPbk2FieldProperty>(count);
    CleanupStack::PushL(propArray);

    for (TInt i = 0; i < count; ++i)
        {
        const MPbk2FieldProperty& prop = iFieldProperties.At(i);
        if (IsFieldAdditionPossibleL(prop.FieldType()) &&
            prop.Flags() & KPbk2FieldFlagUserCanAddField)
            {
            propArray->AppendL(&prop);
            }
        }

    propArray->Compress();
    CleanupStack::Pop(propArray);
    return propArray;
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContact::ParentObject
// --------------------------------------------------------------------------
//
MVPbkObjectHierarchy& CPbk2PresentationContact::ParentObject() const
    {
    return iStoreContact.ParentObject();
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContact::Fields
// --------------------------------------------------------------------------
//
const MVPbkStoreContactFieldCollection& CPbk2PresentationContact::Fields() const
    {
    return *iCollection;
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContact::IsSame
// --------------------------------------------------------------------------
//
TBool CPbk2PresentationContact::IsSame(
    const MVPbkStoreContact& aOtherContact) const
    {
    const CPbk2PresentationContact* cnt =
        dynamic_cast<const CPbk2PresentationContact*>(&aOtherContact);
    if (cnt)
        {
        return iStoreContact.IsSame(cnt->iStoreContact);
        }
    return iStoreContact.IsSame(aOtherContact);
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContact::CreateLinkLC
// --------------------------------------------------------------------------
//
MVPbkContactLink* CPbk2PresentationContact::CreateLinkLC() const
    {
    return iStoreContact.CreateLinkLC();
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContact::DeleteL
// --------------------------------------------------------------------------
//
void CPbk2PresentationContact::DeleteL(MVPbkContactObserver& aObserver) const
    {
    iStoreContact.DeleteL(aObserver);
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContact::Close
// Does not destroy iStoreContact, since it is not owned
// --------------------------------------------------------------------------
//
void CPbk2PresentationContact::Close() const
    {
    delete this;
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContact::ParentStore
// --------------------------------------------------------------------------
//
MVPbkContactStore& CPbk2PresentationContact::ParentStore() const
    {
    return iStoreContact.ParentStore();
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContact::Fields
// --------------------------------------------------------------------------
//
MVPbkStoreContactFieldCollection& CPbk2PresentationContact::Fields()
    {
    return *iCollection;
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContact::CreateFieldLC
// --------------------------------------------------------------------------
//
MVPbkStoreContactField* CPbk2PresentationContact::CreateFieldLC(
    const MVPbkFieldType& aFieldType) const
    {
    return iStoreContact.CreateFieldLC(aFieldType);
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContact::AddFieldL
// --------------------------------------------------------------------------
//
TInt CPbk2PresentationContact::AddFieldL(MVPbkStoreContactField* aField)
    {
    // The aField instance can not be used after AddFieldL
    TInt index = iStoreContact.AddFieldL(aField);
    MVPbkStoreContactFieldCollection& fields = iStoreContact.Fields();
    // Create a presistent store field for the mapping field
    const MVPbkStoreContactField& field = fields.FieldAt(index);
    TRAPD(res, iCollection->AddFieldMappingL(field, KNullDesC));

    if (res != KErrNone)
        {
        // If creation of mapping field failed, remove also the field
        // from the store
        iStoreContact.RemoveField(index);
        User::Leave(res);
        }

    return iCollection->FindFieldIndex(field);
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContact::RemoveField
// --------------------------------------------------------------------------
//
void CPbk2PresentationContact::RemoveField(TInt aIndex)
    {
    TInt storeIndex = iCollection->StoreIndexOfField(aIndex);
    iStoreContact.RemoveField(storeIndex);
    iCollection->RemoveFieldMapping(aIndex);
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContact::RemoveAllFields
// --------------------------------------------------------------------------
//
void CPbk2PresentationContact::RemoveAllFields()
    {
    iCollection->ResetFieldMappings();
    iStoreContact.RemoveAllFields();
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContact::LockL
// --------------------------------------------------------------------------
//
void CPbk2PresentationContact::LockL(MVPbkContactObserver& aObserver) const
    {
    iStoreContact.LockL(aObserver);
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContact::CommitL
// --------------------------------------------------------------------------
//
void CPbk2PresentationContact::CommitL(MVPbkContactObserver& aObserver) const
    {
    iStoreContact.CommitL(aObserver);
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContact::GroupsJoinedLC
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray* CPbk2PresentationContact::GroupsJoinedLC() const
    {
    return iStoreContact.GroupsJoinedLC();
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContact::Group
// --------------------------------------------------------------------------
//
MVPbkContactGroup* CPbk2PresentationContact::Group()
    {
    return iStoreContact.Group();
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContact::MaxNumberOfFieldL
// In addition of checking the store maximum MaxNumberOfFieldL()
// checks the multiplicity defined in the field property for
// the given field type.
// --------------------------------------------------------------------------
//
TInt CPbk2PresentationContact::MaxNumberOfFieldL(
        const MVPbkFieldType& aType ) const
    {
    // Get the store limits
    TInt res = iStoreContact.MaxNumberOfFieldL(aType);

    // If store has no limits or the number is bigger than one
    // -> check the UI limits
    const TInt oneFieldInContact = 1;
    if (res > oneFieldInContact || res == KVPbkStoreContactUnlimitedNumber)
        {
        const MPbk2FieldProperty* prop = iFieldProperties.FindProperty(aType);
        if (prop && prop->Multiplicity() == EPbk2FieldMultiplicityOne)
            {
            res = oneFieldInContact;
            }
        }
    return res;
    }


// --------------------------------------------------------------------------
// CPbk2PresentationContact::StoreContactExtension
// --------------------------------------------------------------------------
//
TAny* CPbk2PresentationContact::StoreContactExtension(TUid aExtensionUid)
	{
	
	if( aExtensionUid == KMVPbkStoreContactExtension2Uid )
		return static_cast<MVPbkStoreContact2*>( this );
	
	return NULL;
	}

// CPbk2PresentationContact::PropertiesLC
// --------------------------------------------------------------------------
//
MVPbkStoreContactProperties* CPbk2PresentationContact::PropertiesL() const
	{
	MVPbkStoreContact2* tempContact = 
	    reinterpret_cast<MVPbkStoreContact2*>
	        (iStoreContact.StoreContactExtension
	            (KMVPbkStoreContactExtension2Uid));
	
	return tempContact->PropertiesL();
	}

// --------------------------------------------------------------------------
// CPbk2PresentationContact::SetAsOwnL
// --------------------------------------------------------------------------
//
void CPbk2PresentationContact::SetAsOwnL(
		MVPbkContactObserver& aObserver) const
	{
	MVPbkStoreContact2* tempContact = 
	    reinterpret_cast<MVPbkStoreContact2*>
	        (iStoreContact.StoreContactExtension
	            (KMVPbkStoreContactExtension2Uid));

	tempContact->SetAsOwnL( aObserver );
	}

// -----------------------------------------------------------------------------
// CPbk2PresentationContact::MatchContactStore
// -----------------------------------------------------------------------------
//
TBool CPbk2PresentationContact::MatchContactStore(
        const TDesC& aContactStoreUri) const
    {
    return iStoreContact.MatchContactStore(aContactStoreUri);
    }

// -----------------------------------------------------------------------------
// CPbk2PresentationContact::MatchContactStoreDomain
// -----------------------------------------------------------------------------
//
TBool CPbk2PresentationContact::MatchContactStoreDomain(
        const TDesC& aContactStoreDomain) const
    {
    return iStoreContact.MatchContactStoreDomain(aContactStoreDomain);
    }

// -----------------------------------------------------------------------------
// CPbk2PresentationContact::CreateBookmarkLC
// -----------------------------------------------------------------------------
//
MVPbkContactBookmark* CPbk2PresentationContact::CreateBookmarkLC() const
    {
    return iStoreContact.CreateBookmarkLC();
    }

// -----------------------------------------------------------------------------
// CPbk2PresentationContact::CurrentAmountOfFieldTypeInContact
// -----------------------------------------------------------------------------
//
TInt CPbk2PresentationContact::CurrentAmountOfFieldTypeInContact(
        const MVPbkFieldType& aType, const TDesC& aName ) const
    {
    const TInt maxMatchPriority = iStoreContact.ContactStore().
        StoreProperties().SupportedFields().MaxMatchPriority();
    const TInt count = iCollection->FieldCount();
    TInt res = 0;
    for ( TInt i = 0; i < count; ++i )
        {
        const MVPbkStoreContactField& field = iCollection->FieldAt(i);
        const MVPbkFieldType* type = NULL;
        for ( TInt j = 0; j < maxMatchPriority && !type; ++j )
            {
            type = field.MatchFieldType(j);
            }
        if ( (aType.IsSame( *type ) ) )
            {
            if (aName == KNullDesC)
                {
                ++res;
                }
            else
                {
                TVPbkFieldVersitProperty prop;
                prop.SetName(EVPbkVersitNameIMPP);
                if (aType.Matches(prop, 0) &&
                        (MVPbkContactFieldUriData::Cast(field.FieldData()).
                        Scheme() == aName))
                    {
                    res++;
                    }
                }
            }
        }
    return res;
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContact::AddFieldL
// --------------------------------------------------------------------------
//
EXPORT_C TInt CPbk2PresentationContact::AddFieldL(MVPbkStoreContactField* aField,
        const TDesC& aName)
    {
    // The aField instance can not be used after AddFieldL
    TInt index = iStoreContact.AddFieldL(aField);
    MVPbkStoreContactFieldCollection& fields = iStoreContact.Fields();
    // Create a presistent store field for the mapping field
    const MVPbkStoreContactField& field = fields.FieldAt(index);
    TRAPD(res, iCollection->AddFieldMappingL(field, aName));

    if (res != KErrNone)
        {
        // If creation of mapping field failed, remove also the field
        // from the store
        iStoreContact.RemoveField(index);
        User::Leave(res);
        }

    return iCollection->FindFieldIndex(field);
    }

//  End of File
