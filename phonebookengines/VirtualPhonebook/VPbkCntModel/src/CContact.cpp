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
* Description:  The virtual phonebook contact
*
*/



#include "CContact.h"
#include <cntitem.h>
#include <babitflags.h>

#include <VPbkError.h>
#include <MVPbkContactObserver.h>
#include <TVPbkFieldTypeMapping.h>
#include <MVPbkFieldType.h>
#include <MVPbkContactStoreProperties.h>
#include <CVPbkContactLinkArray.h>
#include <CVPbkContactFieldCollection.h>
#include <MVPbkStoreContactProperties.h>

#include "CContactStore.h"
#include "CFieldTypeMap.h"
#include "TNewContactField.h"
#include "CViewContact.h"
#include "CContactLink.h"

namespace VPbkCntModel {

// ======== LOCAL CLASSES ========

/**
 * Internal store contact properties class,
 * use CContactItem to implement MVPbkStoreContactProperties methods
 *
 */
class CVPbkStoreContactProperties :
    public CBase,
    public MVPbkStoreContactProperties
    {
    public:
        static CVPbkStoreContactProperties* NewL(
        		CContactItem& aContactItem, 
        		CContactDatabase& aContactDb );
        
        ~CVPbkStoreContactProperties();
    protected: // MVPbkStoreContactProperties
        TTime LastModifiedL() const;
        TPtrC GuidL() const;
    private:
        CVPbkStoreContactProperties(
        		CContactItem& aContactItem,
        		CContactDatabase& aContactDb );

    private:
        CContactItem& iContactItem;  
        CContactDatabase& iContactDb;
    };
    
CVPbkStoreContactProperties* CVPbkStoreContactProperties::NewL(
    CContactItem& aContactItem,
    CContactDatabase& aContactDb )
    {
    CVPbkStoreContactProperties* self = new(ELeave) CVPbkStoreContactProperties(
    		aContactItem,
    		aContactDb );
    return self;
    }
    
CVPbkStoreContactProperties::CVPbkStoreContactProperties(
    CContactItem& aContactItem, 
	CContactDatabase& aContactDb ) :
    iContactItem( aContactItem ),
    iContactDb( aContactDb )
    {
    }
    
CVPbkStoreContactProperties::~CVPbkStoreContactProperties()
    {
    }
    
TTime CVPbkStoreContactProperties::LastModifiedL() const
    {
    return iContactItem.LastModified();
    }

TPtrC CVPbkStoreContactProperties::GuidL() const
    {
    return iContactItem.UidStringL(iContactDb.MachineId());
    }
// ======== LOCAL FUNCTIONS ========

TInt FindMatchingField( const MVPbkBaseContactFieldCollection& aFields, 
                       const MVPbkFieldType* aFieldType,
                       const MVPbkFieldTypeList& aMasterFieldTypeList )
    {
    TInt result = KErrNotFound;
    
    const TInt fieldCount = aFields.FieldCount();
    const TInt maxMatchPriority = aMasterFieldTypeList.MaxMatchPriority();
    for ( TInt matchPriority = 0; matchPriority <= maxMatchPriority; 
                    ++matchPriority )
        {
        for ( TInt i = 0; i < fieldCount; ++i )
            {
            const MVPbkFieldType* fieldType =
                aFields.FieldAt( i ).MatchFieldType( matchPriority );
            if ( fieldType && fieldType->IsSame(*aFieldType) )
                { 
                result = i;
                break;
                }
            }
        }
        
    return result;
    }

// ======== MEMBER FUNCTIONS ========

inline CContact::CContact(
        CContactStore& aParentStore, 
        CContactItem& aContactItem,
        TBool aIsNewContact ) :
    iIsNewContact( aIsNewContact ),
    iFields( *this, aContactItem.CardFields() ),
    iParentStore( aParentStore ),
    iLastUpdatedGroupContactId( KNullContactId )
    {
    }

inline void CContact::ConstructL()
    {
    }

CContact* CContact::NewL(
        CContactStore& aParentStore, 
        CContactItem* aContactItem,
        TBool aIsNewContact /* = EFalse */)
    {
    CContact* self = new(ELeave) CContact( aParentStore, *aContactItem, 
                                            aIsNewContact );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    // Potentially leaving initialisation is done; take parameter ownership now
    self->iContactItem = aContactItem;
    return self;
    }

CContact::~CContact()
    { 
    // Incase there was a problem when updating the time stamps of contacts
    // belonging to a group we need to make sure the contact is unlocked by
    // closing it. Symbian documentation says that despite the trailing L 
    // in the function's name, CloseContactL cannot leave. Also specifying 
    // a contact item that is not open, or cannot be found, is harmless.  
   if ( iLastUpdatedGroupContactId != KNullContactId )
       {
       iParentStore.NativeDatabase().CloseContactL( iLastUpdatedGroupContactId );  
       }
          
    iParentStore.ContactDestroyed( iContactItem, iModified );
    delete iContactItem;
    delete iAddedContacts;    
    }

void CContact::SetContact( CContactItem* aContactItem )
    {
    if ( aContactItem && aContactItem != iContactItem )
        {
        delete iContactItem;
        iContactItem = aContactItem;
        iFields.SetContact( *this, iContactItem->CardFields() );
        }
    }

const CFieldTypeMap& CContact::FieldTypeMap() const
    {
    return iParentStore.FieldTypeMap();
    }

MVPbkObjectHierarchy& CContact::ParentObject() const
    {
    return iParentStore;
    }

const MVPbkStoreContactFieldCollection& CContact::Fields() const
    {
    return iFields;
    }

TBool CContact::IsSame( const MVPbkStoreContact& aOtherContact ) const
    {
    if ( &iParentStore == &aOtherContact.ContactStore() )
        {
        return ( iContactItem->Id() == 
            static_cast<const CContact&>(aOtherContact).iContactItem->Id() );
        }
    return EFalse;
    }

TBool CContact::IsSame( const MVPbkViewContact& aOtherContact ) const
    {
    return aOtherContact.IsSame( *this, &ContactStore() );
    }

MVPbkContactLink* CContact::CreateLinkLC() const
    {
    return iParentStore.CreateLinkLC( iContactItem->Id() );
    }

void CContact::LockL(MVPbkContactObserver& aObserver) const
    {
    iParentStore.LockContactL( *this, aObserver );
    }

void CContact::DeleteL(MVPbkContactObserver& aObserver) const
    {
    iParentStore.NativeDatabase().CloseContactL( iContactItem->Id() );
    iParentStore.DeleteContactL( iContactItem->Id(), aObserver );
    }

TBool CContact::MatchContactStore(const TDesC& aContactStoreUri) const
    {
    return iParentStore.MatchContactStore( aContactStoreUri );
    }
    
TBool CContact::MatchContactStoreDomain( const TDesC& aContactStoreDomain ) const
    {
    return iParentStore.MatchContactStoreDomain( aContactStoreDomain );
    }

MVPbkContactBookmark* CContact::CreateBookmarkLC() const
    {
    return iParentStore.CreateBookmarkLC( iContactItem->Id() );
    }
    
MVPbkContactStore& CContact::ParentStore() const
    {
    return iParentStore;
    }

MVPbkStoreContactFieldCollection& CContact::Fields()
    {
    return iFields;
    }

MVPbkStoreContactField* CContact::CreateFieldLC( const MVPbkFieldType& 
                                                    aFieldType ) const
    {
    // Match the field type to the Contact Db's system template
    CContactItemField* newField = iParentStore.CreateFieldLC( aFieldType );
    if ( !newField )
        {
        User::Leave( KErrNotSupported );
        }

    // Create a wrapper for the newly created field
    TNewContactField* fieldWrapper = 
        new(ELeave) TNewContactField( const_cast<CContact&>(*this), newField );
    CleanupStack::Pop( newField );
    CleanupDeletePushL( fieldWrapper );

    // Return the wrapper
    return fieldWrapper;
    }

TInt CContact::AddFieldL( MVPbkStoreContactField* aField )
    {
    __ASSERT_ALWAYS( aField, VPbkError::Panic( VPbkError::ENullContactField ) );
    __ASSERT_ALWAYS( &aField->ParentContact() == this, 
        VPbkError::Panic(VPbkError::EInvalidContactField) );
    // Test that the client doesn't pass an existing field of this contact as 
    // a new one
    __ASSERT_ALWAYS( aField != iFields.FieldPointer(), 
        VPbkError::Panic(VPbkError::EInvalidContactField) );

    // After all the checks the field can be cast back to the wrapper that was
    // created in CreateFieldLC
    TNewContactField* fieldWrapper = static_cast<TNewContactField*>( aField );
    
    // Add the Contact Model field to the contact item
    iContactItem->AddFieldL( *fieldWrapper->NativeField() );
    // Field added succesfully, release wrapper's ownership
    fieldWrapper->ReleaseNativeField();

    // Delete fieldWrapper. This function must not leave after deletion
    // of fieldWrapper because CreateFieldLC has put fieldWrapper 
    // into the cleanup stack and client pops it after this function
    delete fieldWrapper;
    // The field is appended to the contact -> return the last field index
    return iContactItem->CardFields().Count() - 1;
    }

void CContact::RemoveField(TInt aIndex)
    {
    __ASSERT_ALWAYS( aIndex >= 0 && aIndex < iFields.FieldCount(), 
        VPbkError::Panic(VPbkError::EInvalidFieldIndex) );
    __ASSERT_ALWAYS( !iParentStore.StoreProperties().ReadOnly(),
        VPbkError::Panic(VPbkError::EInvalidAccessToReadOnlyContact ) );

    iContactItem->RemoveField( aIndex );
    } 

void CContact::RemoveAllFields()
    {
    __ASSERT_ALWAYS( !iParentStore.StoreProperties().ReadOnly(),
        VPbkError::Panic(VPbkError::EInvalidAccessToReadOnlyContact ) );

    iContactItem->CardFields().Reset();
    }

void CContact::CommitL( MVPbkContactObserver& aObserver ) const
    {
    iParentStore.CommitContactL( *this, aObserver );
    }
    
MVPbkContactLinkArray* CContact::GroupsJoinedLC() const
    {
    CVPbkContactLinkArray* result = CVPbkContactLinkArray::NewLC();
    
    if ( iContactItem->Type() == KUidContactCard )
        {
        CContactCard* contactCard = static_cast<CContactCard*>( iContactItem );
        CContactIdArray* groups = contactCard->GroupsJoinedLC();
        const TInt count = groups->Count();
        for ( TInt i = 0; i < count; ++i )
            {
            MVPbkContactLink* link = iParentStore.CreateLinkLC( (*groups)[i] );
            result->AppendL( link );
            CleanupStack::Pop(); // link
            }
        CleanupStack::PopAndDestroy( groups );
        }
    
    return result;
    }

TInt CContact::MaxNumberOfFieldL( const MVPbkFieldType& aType ) const
    {
    if ( iParentStore.StoreProperties().SupportedFields().ContainsSame(aType) )
        {
        return KVPbkStoreContactUnlimitedNumber;
        }
    return 0;
    }
        
MVPbkContactGroup* CContact::Group()
    {
    MVPbkContactGroup* result = NULL;
    
    if ( iContactItem->Type() == KUidContactGroup )
        {
        result = this;
        }
        
    return result;
    }

void CContact::SetGroupLabelL( const TDesC& aLabel )
    {
    TVPbkFieldTypeMapping typeMapping;
    typeMapping.SetNonVersitType( EVPbkNonVersitTypeGenericLabel );
    const MVPbkFieldType* labelType = 
            typeMapping.FindMatch( iParentStore.MasterFieldTypeList() );
    
    TInt labelFieldIndex = FindMatchingField( Fields(), 
                                             labelType, 
                                             iParentStore.MasterFieldTypeList() );

    // Update the timestamp of the all contacts that belong to the group. 
    // This is needed for synch service as it checks the timestamps of contacts 
    // and get's the group information from the vCard of a contact. 
    UpdateTimeStampOfAllContactsInGroupL();
       
    if ( labelFieldIndex != KErrNotFound )
        {
        // field already exists
        MVPbkStoreContactField& labelField = Fields().FieldAt( labelFieldIndex );
        MVPbkContactFieldTextData::Cast( labelField.FieldData()).SetTextL(aLabel );
        }
    else
        {
        // field does not exist => add it
        MVPbkStoreContactField* labelField = CreateFieldLC( *labelType );
        MVPbkContactFieldTextData::Cast( labelField->FieldData() ).SetTextL( aLabel );
        AddFieldL( labelField );
        CleanupStack::Pop(); // labelField
        }
    }
    
TPtrC CContact::GroupLabel() const
    {
    TVPbkFieldTypeMapping typeMapping;
    typeMapping.SetNonVersitType( EVPbkNonVersitTypeGenericLabel );
    const MVPbkFieldType* labelType = 
            typeMapping.FindMatch( iParentStore.MasterFieldTypeList() );
    
    TInt labelFieldIndex = FindMatchingField( Fields(), 
                                             labelType, 
                                             iParentStore.MasterFieldTypeList() );
    
    if ( labelFieldIndex != KErrNotFound )
        {
        const MVPbkBaseContactField& labelField = Fields().FieldAt( labelFieldIndex );
        return MVPbkContactFieldTextData::Cast(labelField.FieldData()).Text();
        }
    else
        {
        return KNullDesC();        
        }
    }

// --------------------------------------------------------------------------
// CContact::UpdateTimeStampOfAllContactsInGroupL
// --------------------------------------------------------------------------
//
void CContact::UpdateTimeStampOfAllContactsInGroupL( )
    {
    MVPbkContactLinkArray* contactsInGroup = ItemsContainedLC();
    
    // Loop through all contacts in the group and update time stamps
    for (TInt i=0; i < contactsInGroup->Count(); i++ )
        {
        UpdateTimeStampOfContactInGroupL( contactsInGroup->At(i) );
        }
    
    CleanupStack::PopAndDestroy(); // contactsInGroup
    }

// --------------------------------------------------------------------------
// CContact::UpdateTimeStampOfContactInGroupL
// --------------------------------------------------------------------------
//
void CContact::UpdateTimeStampOfContactInGroupL(const MVPbkContactLink& aContactLink )
    {
    const CContactLink& link = static_cast<const CContactLink&>( aContactLink );
    
    // Store the id of currently processed contact in the group
    iLastUpdatedGroupContactId = link.ContactId();
    
    // Try to open the contact for editing and then commit.
    // This will update the timestamp of the contact.
    // In case of a leave, we will make sure in the destructor that the
    // contact is unlocked (see CContact::~CContact).
    CContactItem* contact = 
          iParentStore.NativeDatabase().OpenContactL( link.ContactId() );  
    CleanupStack::PushL(contact);     
    
    iParentStore.NativeDatabase().CommitContactL(*contact);
    CleanupStack::PopAndDestroy(contact);
    
    // No need to store the id anymore.
    iLastUpdatedGroupContactId = KNullContactId;
    }

void CContact::AddContactL( const MVPbkContactLink& aContactLink )
    {        
    // We have to maintain iAddedContacts ID array here because
    // the AddContactToGroup(id, id) method does not update
    // the native contact group we have in hand. It updates the
    // database only. So, in the ItemsContainedLC function
    // we have to know both the group members in the database
    // and the group members that have been added after this group
    // has been read from database
    const CContactLink& link = static_cast<const CContactLink&>( aContactLink );
    
    // Read the contact so that 
    // AddContactToGroupL(CContactItem &aItem, CContactItem &aGroup)
    // can be used.
    CContactItem* contact = 
        iParentStore.NativeDatabase().ReadContactLC( link.ContactId() );
        
    if ( !iAddedContacts )
        {
        iAddedContacts = CContactIdArray::NewL();
        }        
    if ( iAddedContacts->Find( link.ContactId() ) == KErrNotFound )
        {
        iAddedContacts->AddL( link.ContactId() );
        }
    
    // Use AddContactToGroupL(CContactItem &aItem, CContactItem &aGroup)
    // instead of 
    // AddContactToGroupL(TContactItemId aItemId, TContactItemId aGroupId)
    // because otherwise the member iContactItem won't be updated and commiting
    // it would loose the information about added contact
    TRAPD( err1, iParentStore.NativeDatabase().AddContactToGroupL(
               *contact, *iContactItem ) );
    if ( err1 != KErrNone )
        {
        iAddedContacts->Remove( iAddedContacts->Count() - 1 );
        User::Leave( err1 );
        } 
    
    // Update the timestamp of the added contact. This is needed for 
    // synch service as it checks the timestamps of contacts and get's the group
    // information from the vCard of a contact.    
    TRAPD( err2, UpdateTimeStampOfContactInGroupL( aContactLink ) );
    if ( err2 != KErrNone )
        {
        iAddedContacts->Remove( iAddedContacts->Count() - 1 );
        iParentStore.NativeDatabase().RemoveContactFromGroupL(
                *contact, *iContactItem );
        User::Leave( err2 );
        } 
    CleanupStack::PopAndDestroy( contact );
    }
    
void CContact::RemoveContactL( const MVPbkContactLink& aContactLink )
    {    
    const CContactLink& link = static_cast<const CContactLink&>( aContactLink );
    
    // Read the contact so that 
    // RemoveContactFromGroupL(CContactItem &aItem, CContactItem &aGroup)
    // can be used.
    CContactItem* contact = 
        iParentStore.NativeDatabase().ReadContactLC( link.ContactId() );
    
    // First update the timestamp of the removed contact. This is needed for 
    // synch service as it checks the timestamps of contacts and get's the group
    // information from the vCard of a contact. If updating fails this function 
    // will leave and contact won't be removed from the group.
    UpdateTimeStampOfContactInGroupL(aContactLink);
    
    TInt index = KErrNotFound;
    if ( iAddedContacts )
    	{
    	index = iAddedContacts->Find( link.ContactId() );
    	}
    
    // Use RemoveContactFromGroupL(CContactItem &aItem, CContactItem &aGroup)
    // instead of 
    // RemoveContactFromGroupL(TContactItemId aItemId, TContactItemId aGroupId)
    // because otherwise the member iContactItem won't be updated and commiting
    // it would loose the information about removed contact
    iParentStore.NativeDatabase().RemoveContactFromGroupL(
            *contact, *iContactItem );
    if ( index != KErrNotFound )
        {
        iAddedContacts->Remove( index );
        }
    CleanupStack::PopAndDestroy( contact );
    }

MVPbkContactLinkArray* CContact::ItemsContainedLC() const
    {
    CVPbkContactLinkArray* result = CVPbkContactLinkArray::NewLC();
    TInt i;

    const CContactGroup* thisGroup = static_cast<const CContactGroup*>( iContactItem );
    // 1. append the IDs found in the group
    const CContactIdArray* contacts = thisGroup->ItemsContained();
    const TInt count = ( contacts ? contacts->Count() : 0 );
    for ( i = 0; i < count; ++i )
        {
        MVPbkContactLink* link = iParentStore.CreateLinkLC( (*contacts)[i] );
        result->AppendL( link );
        CleanupStack::Pop(); // link
        }
    // 2. append the IDs in iAddedContacts
    const TInt addedCount = ( iAddedContacts ? iAddedContacts->Count() : 0 );
    for ( i = 0; i < addedCount; ++i )
        {
        // add the contact if the iAddedContact[i] was not in contacts already
        if ( !contacts ||
            contacts->Find( (*iAddedContacts)[i]) == KErrNotFound )
            {
            MVPbkContactLink* link = iParentStore.CreateLinkLC( (*iAddedContacts)[i] );
            result->AppendL( link );
            CleanupStack::Pop(); // link
            }
        }
    return result;
    }

TAny* CContact::StoreContactExtension(TUid aExtensionUid) 
{
    if( aExtensionUid == KMVPbkStoreContactExtension2Uid )
		return static_cast<MVPbkStoreContact2*>( this );
    return NULL;
}
    
MVPbkStoreContactProperties* CContact::PropertiesL() const
    {
    return CVPbkStoreContactProperties::NewL( *iContactItem, iParentStore.NativeDatabase() );
    }

void CContact::SetAsOwnL(MVPbkContactObserver& aObserver) const
	{
	iParentStore.SetAsOwnL( *this, aObserver );
	}

TAny* CContact::BaseContactExtension( TUid aExtensionUid )
    {
    if( aExtensionUid == KVPbkBaseContactExtension2Uid )
        return static_cast<MVPbkBaseContact2*>( this );
    return NULL;
    }

TBool CContact::IsOwnContact( TInt& aError ) const
    {
    aError = KErrNone;
    return ( iContactItem->Type() == KUidContactOwnCard );
    }


} // namespace VPbkCntModel

// end of file
