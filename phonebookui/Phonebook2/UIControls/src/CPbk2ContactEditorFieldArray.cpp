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
* Description:  Phonebook 2 contact editor field array.
*
*/

#include "CPbk2ContactEditorFieldArray.h"

// Phonebook 2
#include "CPbk2ReadingFieldEditorVisitor.h"
#include "CPbk2IconInfoContainer.h"
#include "Pbk2ContactEditorFieldFactory.h"
#include "MPbk2ContactEditorField.h"
#include "MPbk2ContactEditorUiBuilder.h"
#include <Pbk2CommonUi.rsg>
#include <CPbk2PresentationContact.h>
#include <CPbk2PresentationContactFieldCollection.h>
#include <CPbk2PresentationContactField.h>
#include <MPbk2FieldProperty.h>
#include <Pbk2UIControls.rsg>
#include <MPbk2AppUi.h>
#include <CPbk2ServiceManager.h>
#include "TPbk2ContactEditorParams.h"
#include <MPbk2ApplicationServices.h>
#include "CPbk2IconInfo.h"
#include <AknsUtils.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include "CPbk2UIFieldArray.h"
#include "CPbk2UIField.h"
#include "Pbk2UIFieldProperty.hrh"
#include "Pbk2AddressTools.h"
#include <MPbk2ApplicationServices2.h>


// Virtual Phonebook
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <CVPbkFieldTypeSelector.h>
#include <VPbkEng.rsg>
#include <MVPbkFieldType.h>
#include <VPbkFieldType.hrh>
#include <TVPbkFieldVersitProperty.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkContactFieldTextData.h>


// System includes
#include <barsread.h>
#include <coemain.h>
#include <AknUtils.h>
#include <StringLoader.h>

/// Unnamed namespace for local definitions
namespace  {

_LIT( KPbk2UIControlsResFile, "Pbk2UIControls.rsc" );

#ifdef _DEBUG

enum TPanicCode
    {
    EPanicInvariant_CountMismatch = 1,
    EPanicInvariant_FieldNotFound,
    EPanicInvariant_NotEditableFieldFound,
    EPanic_AddNewFieldL_OOB,
    EPanic_GetUiFieldIndex_OOB
    };

void Panic(TInt aReason)
    {
    _LIT(KPanicText, "CPbk2ContactEditorFieldArray");
    User::Panic(KPanicText, aReason);
    }

#endif // _DEBUG
    
/**
 * Finds property of given type from given contact.
 *
 * @param aFieldType    Field type to search for.
 * @param aContact      The contact in question.
 * @return  Field property if any. 
 */
const MPbk2FieldProperty* FindProperty
        ( const MVPbkFieldType& aType,
          const CPbk2PresentationContact& aContact )
    {
    CArrayPtr<const MPbk2FieldProperty>* props=
        aContact.AvailableFieldsToAddL();
    CleanupStack::PushL( props );
    TInt count( props->Count() );
    const MPbk2FieldProperty* property = NULL;
    for ( TInt i = 0; i < count && !property; ++i )
        {
        if ( props->At( i )->FieldType().IsSame( aType ) )
            {            
            property = props->At( i );
            }
        }    
    CleanupStack::PopAndDestroy( props );
    return property;
    }

/**
 * Returns maximum number of fields of given type in given contact.
 *
 * @param aType     Field type in question.
 * @param aContact  Contact in question.
 * @return  Maximum number of fields.
 */
TInt MaxNumberOfFieldL
        ( const MVPbkFieldType& aType,
          const CPbk2PresentationContact& aContact )
    {
    // Get the store limits
    TInt res = aContact.MaxNumberOfFieldL(aType);
    
    // If store has no limits or the number is bigger than one
    // -> check the UI limits
    const TInt oneFieldInContact = 1;
    if (res > oneFieldInContact || res == KVPbkStoreContactUnlimitedNumber)
        {
        const MPbk2FieldProperty* prop = FindProperty( aType, aContact );
        if (prop && prop->Multiplicity() == EPbk2FieldMultiplicityOne)
            {
            res = oneFieldInContact;
            }
        }
    return res;
    }

/**
 * Returns current amount of fields of given type in given contact.
 *
 * @param aType     Field type in question.
 * @param aContact  Contact in question.
 * @return  Amount of fields.
 */
TInt CurrentAmountOfFieldTypeInContact
        ( const MVPbkFieldType& aType,
          const CPbk2PresentationContact& aContact  )
    {
    const TInt maxMatchPriority = aContact.ContactStore().
        StoreProperties().SupportedFields().MaxMatchPriority();

    const TInt count = aContact.PresentationFields().FieldCount();
    TInt res = 0;
    for ( TInt i = 0; i < count; ++i )
        {
        const MVPbkStoreContactField& field =
            aContact.PresentationFields().FieldAt(i);
        const MVPbkFieldType* type = NULL;
        for ( TInt j = 0; j < maxMatchPriority && !type; ++j )
            {
            type = field.MatchFieldType(j);
            }            
            
        if ( (aType.IsSame( *type ) ) )
            {
            ++res;
            }
        }
    return res;
    }

/**
 * Checks is field addition possible.
 *
 * @param aType     Field type to add.
 * @param aContact  Contact in question.
 * @return  ETrue if field addition is possible.
 */
TBool IsFieldAdditionPossibleL
        ( const MVPbkFieldType& aType,
          const CPbk2PresentationContact& aContact )
    {
    TInt max = MaxNumberOfFieldL(aType, aContact);
    if (max == KVPbkStoreContactUnlimitedNumber)
        {
        return ETrue;
        }
    else
        {
        TInt cur = CurrentAmountOfFieldTypeInContact( aType, aContact );
        return cur < max;
        }
    }    

} /// namespace


// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldArray::CPbk2ContactEditorFieldArray
// --------------------------------------------------------------------------
//
inline CPbk2ContactEditorFieldArray::CPbk2ContactEditorFieldArray
        ( CPbk2PresentationContact& aContact,
          MPbk2ContactEditorUiBuilder& aUiBuilder,
          CPbk2ContactEditorFieldFactory& aFieldFactory,
          MPbk2ApplicationServices* aAppServices ) :
            iContact( aContact ),
            iUiBuilder( aUiBuilder ),
            iFieldFactory(aFieldFactory),
            iAppServices( aAppServices )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldArray::~CPbk2ContactEditorFieldArray
// --------------------------------------------------------------------------
//
CPbk2ContactEditorFieldArray::~CPbk2ContactEditorFieldArray()
    {
    iFieldArray.ResetAndDestroy();
    delete iIconInfoContainer;
    delete iReadingFieldBinder;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldArray::NewL
// --------------------------------------------------------------------------
//
CPbk2ContactEditorFieldArray* CPbk2ContactEditorFieldArray::NewL
        ( CVPbkContactManager& aContactManager,
          CPbk2PresentationContact& aContact,
          MPbk2ContactEditorUiBuilder& aUiBuilder,
          CPbk2ContactEditorFieldFactory& aFieldFactory )
    {
    CPbk2ContactEditorFieldArray* self = 
        new ( ELeave ) CPbk2ContactEditorFieldArray
            ( aContact, aUiBuilder, aFieldFactory );
    CleanupStack::PushL( self );
    self->ConstructL( aContactManager );
    CleanupStack::Pop( self );
    return self;
    }


// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldArray::NewL
// --------------------------------------------------------------------------
//
CPbk2ContactEditorFieldArray* CPbk2ContactEditorFieldArray::NewL
        ( CVPbkContactManager& aContactManager,
          CPbk2PresentationContact& aContact,
          MPbk2ContactEditorUiBuilder& aUiBuilder,
          CPbk2ContactEditorFieldFactory& aFieldFactory,
          MPbk2ApplicationServices* aAppServices )
    {
    CPbk2ContactEditorFieldArray* self = 
        new ( ELeave ) CPbk2ContactEditorFieldArray
            ( aContact, aUiBuilder, aFieldFactory,aAppServices );
    CleanupStack::PushL( self );
    self->ConstructL( aContactManager );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldArray::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2ContactEditorFieldArray::ConstructL
        ( CVPbkContactManager& aContactManager )
    {
    iIconInfoContainer = CPbk2IconInfoContainer::NewL
        ( R_PBK2_ICON_INFO_ARRAY );

    //Calculate preferred size for xsp service icons 
    TRect mainPane;
    AknLayoutUtils::LayoutMetricsRect(
        AknLayoutUtils::EMainPane, mainPane );
    TAknLayoutRect listLayoutRect;
    listLayoutRect.LayoutRect(
        mainPane,
        AknLayoutScalable_Avkon::list_single_graphic_pane_g2(0).LayoutLine() );
    TSize size(listLayoutRect.Rect().Size());
    
    /*
     * Use iAppServices if provided. This is to enable editor use outside from pbk2 context
     */
    MPbk2ApplicationServices2* servicesExtension = NULL;
    if( iAppServices )
    	{
    	// Add xsp service icons
		servicesExtension = 
			reinterpret_cast<MPbk2ApplicationServices2*>
				( iAppServices->MPbk2ApplicationServicesExtension(
						KMPbk2ApplicationServicesExtension2Uid ) );
    	}
    else
    	{
    	// Add xsp service icons
		servicesExtension = 
			reinterpret_cast<MPbk2ApplicationServices2*>
				( Phonebook2::Pbk2AppUi()->ApplicationServices().
					MPbk2ApplicationServicesExtension(
						KMPbk2ApplicationServicesExtension2Uid ) );
    	}
    
    
    CPbk2ServiceManager& servMan = servicesExtension->ServiceManager();
    const CPbk2ServiceManager::RServicesArray& services = servMan.Services();
    TUid uid;
    uid.iUid = KPbk2ServManId;
    for ( TInt i = 0; i < services.Count(); i++ )
        {
        const CPbk2ServiceManager::TService& service = services[i];
        if ( service.iBitmapId && service.iBitmap )
            {
            AknIconUtils::SetSize(
                services[i].iBitmap,
                size );
            AknIconUtils::SetSize(
                services[i].iMask,
                size );
            TPbk2IconId id = TPbk2IconId( uid, services[i].iBitmapId );
            CPbk2IconInfo* info = CPbk2IconInfo::NewLC(
                id, services[i].iBitmap, services[i].iMask, size );
            iIconInfoContainer->AppendIconL(info);
            CleanupStack::Pop(info);
            }
        }
    iReadingFieldBinder = CPbk2ReadingFieldEditorVisitor::NewL
        ( aContactManager );
    }
    
// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldArray::CreateFieldsFromContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorFieldArray::CreateFieldsFromContactL(TPbk2ContactEditorParams& aParams)
    {
    __TEST_INVARIANT;
    iParams = aParams;
    
    RArray<TPbk2FieldGroupId> addedGroups;
    CleanupClosePushL(addedGroups);
    
   
    CPbk2UIFieldArray* uiArray = CPbk2UIFieldArray::NewL(
   		KPbk2UIControlsResFile, CCoeEnv::Static()->FsSession(), iFieldFactory);
    CleanupStack::PushL(uiArray);
    
    TInt nextFreePos = 0;
    CreateUIFieldL(*uiArray, EPbk2FieldGroupIdNone, EPbk2FieldOrderTop, nextFreePos);
    
    const TInt count = iContact.PresentationFields().FieldCount();
    for (TInt i = 0; i < count; ++i)
        {
        CPbk2PresentationContactField& contactField = 
            iContact.PresentationFields().At(i);
            
        if ( contactField.IsEditable() )
            {
            if( IsFieldPartOfAddress(contactField) )
            	{
            	if( Pbk2AddressTools::MapViewTypeToAddress(iParams.iActiveView) == 
            		contactField.FieldProperty().GroupId() )
            		{
            		MPbk2ContactEditorField* field = 
            			iFieldFactory.CreateFieldLC( contactField, nextFreePos++, *iIconInfoContainer );
		            iFieldArray.AppendL( CPbk2ContactEditorArrayItem::NewL( field ) );
		            CleanupStack::Pop();
            		}
            	else if( iParams.iActiveView == TPbk2ContactEditorParams::EEditorView )
            		{
            		if( addedGroups.Find(contactField.FieldProperty().GroupId()) == KErrNotFound )
            			{	
		        		CreateUIFieldL(
		        				*uiArray, 
		        				contactField.FieldProperty().GroupId(),
		        				(TPbk2FieldOrder)Pbk2AddressTools::MapAddressToOrdering(contactField.FieldProperty().GroupId()), 
		        				nextFreePos );
		        		addedGroups.AppendL(contactField.FieldProperty().GroupId());
            			}
            		}
            	}
            else if( iParams.iActiveView == TPbk2ContactEditorParams::EEditorView )
            	{
            	MPbk2ContactEditorField* field = 
        			iFieldFactory.CreateFieldLC( contactField, nextFreePos, *iIconInfoContainer );
                if ( field )
                    {
                    nextFreePos++;
                    iFieldArray.AppendL( CPbk2ContactEditorArrayItem::NewL( field ) );
                    CleanupStack::Pop();
                    }
            	}
            }
        }
    
    CreateUIFieldL(*uiArray, EPbk2FieldGroupIdNone, EPbk2FieldOrderBottom, nextFreePos);
        
    AcceptL(*iReadingFieldBinder);
    CleanupStack::PopAndDestroy(uiArray);
    CleanupStack::PopAndDestroy(&addedGroups);
    __TEST_INVARIANT;
    }
  
// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldArray::CreateUIFieldL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorFieldArray::CreateUIFieldL(
		CPbk2UIFieldArray& aFieldsArr, 
		TPbk2FieldGroupId aAddressGroup,
		TPbk2FieldOrder aOrder,
		TInt& aNextFreePos )
    {
    if( aOrder == EPbk2FieldOrderUndefinied )
    	{
    	return;
    	}
    
    for(TInt idx = 0; idx < aFieldsArr.Count(); idx++)
       	{
       	MPbk2UIField& field = aFieldsArr.At(idx);
       	if( field.Order() == aOrder )
   	   		{
   	   		if( iParams.iActiveView == TPbk2ContactEditorParams::EEditorView && 
   	   			field.CtrlType() == EPbk2FieldCtrlTypeExtAssignFromMapsEditor )
   	   			{
   	   			continue;
   	   			}
   	   		RBuf text;
   	   		TBool textAlloc = EFalse;
   	   		if( aAddressGroup == EPbk2FieldGroupIdPostalAddress ||
   	   			aAddressGroup == EPbk2FieldGroupIdHomeAddress ||
   	   			aAddressGroup == EPbk2FieldGroupIdCompanyAddress )
   	   			{
   	   			Pbk2AddressTools::GetAddressPreviewLC( iContact.StoreContact(), aAddressGroup, text );
   	   			textAlloc = ETrue;
   	   			}
   	   		MPbk2ContactEditorUIField* cefield = 
   	   			iFieldFactory.CreateFieldLC(field, aNextFreePos, iUiBuilder, text, *iIconInfoContainer);
   	   	    if( cefield )
   	   	    	{
   	   	    	aFieldsArr.RemoveAt(idx);
   	   	    	iFieldArray.AppendL(CPbk2ContactEditorArrayItem::NewL(cefield));
   		   		CleanupStack::Pop(cefield);
   		   		aNextFreePos++;
   	   	    	}
   	   	    if ( textAlloc )
   	   	    	{
   	   	    	CleanupStack::PopAndDestroy(); //text
   	   	    	}
   	   	    else
   	   	    	{
   	   	    	text.Close();
   	   	    	}
   	   	    break;
   	   		}
       	}
    }


// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldArray::UpdateControlsL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorFieldArray::UpdateControlsL()
    {
    const TInt count = iFieldArray.Count();
    for(TInt i = 0; i < count; ++i)
        {
        if( iFieldArray[i]->ContactEditorUIField() && 
        	iFieldArray[i]->ContactEditorUIField()->UIField()->CtrlType() != 
        	EPbk2FieldCtrlTypeExtAssignFromMapsEditor )
        	{
        	RBuf text;
        	Pbk2AddressTools::GetAddressPreviewLC( iContact.StoreContact(),
				Pbk2AddressTools::MapCtrlTypeToAddress( 
					iFieldArray[i]->ContactEditorUIField()->UIField()->CtrlType()), text );
        	   	   			
  	    	iFieldArray[i]->ContactEditorUIField()->SetControlTextL(text);
  	    	CleanupStack::PopAndDestroy(); //text
        	}
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldArray::IsEditorFieldControlEmpty
// --------------------------------------------------------------------------
//
TBool CPbk2ContactEditorFieldArray::IsEditorFieldControlEmpty( TInt aIndex ) const
    {
    TBool result = ETrue;
    const TInt count = iFieldArray.Count();
    if ( aIndex >= count )
        {
        return result;
        }
    
    if ( iFieldArray[aIndex]->ContactEditorField() )
        {
        HBufC* text = NULL;
        TRAP_IGNORE( text = iFieldArray[aIndex]->ContactEditorField()->ControlTextL() );
        TInt fld = iFieldArray[aIndex]->ContactEditorField()->FieldProperty().FieldType().FieldTypeResId();
        
        if ( fld != R_VPBK_FIELD_TYPE_RINGTONE && 
                fld != R_VPBK_FIELD_TYPE_THUMBNAILPIC &&
                fld != R_VPBK_FIELD_TYPE_CALLEROBJIMG ) 
            {
            if ( text && text->Length() > 0 )
                {
                // If Fields contain only spaces; then treat field as empty field 
                text->Des().TrimAll();
                if ( text->Length() > 0 )
                    {
                    result = EFalse;
                    }
                }
            }

        delete text;
        text = NULL;
        }
    
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldArray::IsEditorUiFieldControlEmpty
// --------------------------------------------------------------------------
//
TBool CPbk2ContactEditorFieldArray::IsEditorUiFieldControlEmpty( TInt aIndex ) const
    {
    TBool result = ETrue;
    
    const TInt count = iFieldArray.Count();
    if ( aIndex >= count )
        {
        return result;
        }
    
    if( iFieldArray[aIndex]->ContactEditorUIField() ) 
        {
        TInt type = iFieldArray[aIndex]->ContactEditorUIField()->UIField()->CtrlType();
        if( type == EPbk2FieldCtrlTypeExtAddressEditor
                || type == EPbk2FieldCtrlTypeExtAddressHomeEditor
                || type == EPbk2FieldCtrlTypeExtAddressOfficeEditor )
            {
            TPtrC* data = NULL;
            RHashMap<TInt, TPtrC> aFieldsMap;

            Pbk2AddressTools::GetAddressFieldsLC( iContact.StoreContact(), 
                Pbk2AddressTools::MapCtrlTypeToAddress( 
                    iFieldArray[aIndex]->ContactEditorUIField()->UIField()->CtrlType() ), aFieldsMap );

            for(TInt addrType = EVPbkVersitSubFieldPostOfficeAddress; addrType <= EVPbkVersitSubFieldCountry; addrType++ )
                {
                data = aFieldsMap.Find( addrType );
                if(data && data->Length() > 0 )
                    {
                    result= EFalse;
                    }
                }

            CleanupStack::PopAndDestroy( &aFieldsMap );
            }
        }
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldArray::SaveFieldsL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorFieldArray::SaveFieldsL()
    {
    const TInt count = iFieldArray.Count();
    for(TInt i = 0; i < count; ++i)
        {
        if( iFieldArray[i]->ContactEditorField() )
            {
            iFieldArray[i]->ContactEditorField()->SaveFieldL();
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldArray::FieldsChanged
// --------------------------------------------------------------------------
//
TBool CPbk2ContactEditorFieldArray::FieldsChanged() const
    {
	TBool result = EFalse;
    for (TInt i = 0; i < iFieldArray.Count(); ++i)
        {
        if ( iFieldArray[i]->ContactEditorField() && 
        	 iFieldArray[i]->ContactEditorField()->FieldDataChanged() )
			{
			result = ETrue;
			}
        // can break because at least one field has changed
        if (result)
            {
            break;
            }
        }
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldArray::AreAllUiFieldsEmpty
// --------------------------------------------------------------------------
//
TBool CPbk2ContactEditorFieldArray::AreAllUiFieldsEmpty() const
    {
    TBool result(ETrue);
    const TInt count = iFieldArray.Count();
 
    for(TInt i = 0; i < count && result; ++i)
        {     
        if( iFieldArray[i]->ContactEditorField() )
            {
            result = IsEditorFieldControlEmpty( i );
            }
        
        // if not managed in ContactEditorField, judge whether available in ContactEditorUIField
        else if( iFieldArray[i]->ContactEditorUIField() ) 
            {
            result = IsEditorUiFieldControlEmpty( i );
            }
        }
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldArray::SetFocus
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorFieldArray::SetFocus(TInt aFieldIndex)
    {
    if (aFieldIndex >= 0 && aFieldIndex < iFieldArray.Count())
        {
        iFieldArray[aFieldIndex]->SetFocus();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldArray::Find
// --------------------------------------------------------------------------
//
CPbk2ContactEditorArrayItem* CPbk2ContactEditorFieldArray::Find
        ( TInt aControlId )
    {
    const TInt count = iFieldArray.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (iFieldArray[i]->ControlId() == aControlId)
            {
            return iFieldArray[i];
            }
        }
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldArray::FindIndex
// --------------------------------------------------------------------------
//
TInt CPbk2ContactEditorFieldArray::FindIndex
        ( TInt aControlId )
    {
    const TInt count = iFieldArray.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (iFieldArray[i]->ControlId() == aControlId)
            {
            return i;
            }
        }
    return KErrArgument;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldArray::AddNewFieldL
// --------------------------------------------------------------------------
//
TInt CPbk2ContactEditorFieldArray::AddNewFieldL
        ( const MVPbkFieldType& aFieldType)
    {
    return AddNewFieldL(aFieldType, KNullDesC);
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldArray::AddNewFieldL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorFieldArray::AddNewFieldL
        ( const TPbk2FieldGroupId aAddressGroup )
    {
    CPbk2UIFieldArray* uiArray = CPbk2UIFieldArray::NewL(
   		KPbk2UIControlsResFile, CCoeEnv::Static()->FsSession(), iFieldFactory);
    CleanupStack::PushL(uiArray);
    
    TInt uiIndex = GetUiFieldIndex(aAddressGroup);
            
    // Before adding a new field the focus must be on the previous item
    TInt focusIndex = uiIndex - 1;
    SetFocus(focusIndex);
    
    for(TInt idx = 0; idx < uiArray->Count(); idx++)
	   	{
	   	MPbk2UIField& field = uiArray->At(idx);
	   	if( aAddressGroup == Pbk2AddressTools::MapCtrlTypeToAddress( field.CtrlType() ) )
	   		{
	   		MPbk2ContactEditorUIField* cefield = 
	   			iFieldFactory.CreateFieldLC(field, uiIndex, iUiBuilder, KNullDesC(), *iIconInfoContainer);
	   	    if( cefield )
	   	    	{
	   	    	uiArray->RemoveAt(idx);
	   	    	iFieldArray.InsertL( CPbk2ContactEditorArrayItem::NewL(cefield), uiIndex );
	   	    	cefield->ActivateL();
		   		CleanupStack::Pop(cefield);
	   	    	}
	   	    break;
	   		}
	   	}
    
    CleanupStack::PopAndDestroy(uiArray);
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldArray::AddNewFieldL
// --------------------------------------------------------------------------
//
TInt CPbk2ContactEditorFieldArray::AddNewFieldL
        ( const MVPbkFieldType& aFieldType, const TDesC& aName )
    {
    __TEST_INVARIANT;
    
    // Check first is field adding possible. If there already is max amount
    // of this type fields, leave with KErrNotSupported.
    if ( !IsFieldAdditionPossibleL( aFieldType, iContact ) )
        {
        User::Leave( KErrNotSupported );
        }
    
    
    MVPbkStoreContactField* storeField = iContact.CreateFieldLC(aFieldType);
    TInt index = iContact.AddFieldL(storeField, aName);
    CleanupStack::Pop( storeField );
    
    TArray<TVPbkFieldVersitProperty> propArr = aFieldType.VersitProperties();
    TInt count = propArr.Count();
    TBool addrProp = EFalse;
    for( TInt idx = 0; idx < count; idx++)
    	{
    	if( propArr[idx].Name() == EVPbkVersitNameADR )
    		{
    		addrProp = ETrue;
    		}
    	}
    if( iParams.iActiveView == TPbk2ContactEditorParams::EEditorView && addrProp )
    	{
    	return KErrNone;
    	}
    
    TInt uiIndex = GetUiFieldIndex(index);
    
    // Before adding a new field the focus must be on the previous item
    TInt focusIndex = uiIndex - 1;
    SetFocus(focusIndex);
    
    __ASSERT_DEBUG( iContact.PresentationFields().FieldCount() > index,
        Panic( EPanic_AddNewFieldL_OOB ) );
    MPbk2ContactEditorField* uiField = iFieldFactory.CreateFieldLC(
        iContact.PresentationFields().At(index), NULL, *iIconInfoContainer);
    iFieldArray.InsertL(CPbk2ContactEditorArrayItem::NewL(uiField), uiIndex);
    CleanupStack::Pop();
    // The first SetFocus is needed.
    // Otherwise the added focused field can be outside of screen.
    SetFocus( 0 );
    SetFocus( uiIndex );
    uiField->AcceptL(*iReadingFieldBinder);
    uiField->ActivateL();
    
    __TEST_INVARIANT;
    return uiField->ControlId();
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldArray::RemoveField
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorFieldArray::RemoveField
        ( CPbk2ContactEditorArrayItem& aField )
    {
    __TEST_INVARIANT;
    const TInt count = iFieldArray.Count();
    for (TInt i = 0; i < count; ++i)
        {
        CPbk2ContactEditorArrayItem* field = iFieldArray[i];
        if (field == &aField)
            {
            iUiBuilder.DeleteControl(field->ControlId());
            if( field->ContactEditorField() )
            	{
		        TInt index = iContact.PresentationFields().FindFieldIndex(
	        		field->ContactEditorField()->ContactField());
		        iContact.RemoveField(index);
		        iFieldArray.Remove(i);
		        delete field;
            	}
            else if( field->ContactEditorUIField() )
            	{
            	TPbk2FieldGroupId groupId = 
					Pbk2AddressTools::MapCtrlTypeToAddress(
						field->ContactEditorUIField()->UIField()->CtrlType() );
            	iFieldArray.Remove(i);
				delete field;
            	
				if( groupId == EPbk2FieldGroupIdNone )
					{
					break;
					}
				
            	CPbk2PresentationContactFieldCollection& fields = iContact.PresentationFields();
            	TBool removed = EFalse;
				for(TInt idx = 0; idx < fields.FieldCount() && !removed; idx++)
					{
					MVPbkStoreContactField& storeField = fields.FieldAt(idx);
					TInt countProps =
						storeField.BestMatchingFieldType()->VersitProperties().Count();
					TArray<TVPbkFieldVersitProperty> props =
						storeField.BestMatchingFieldType()->VersitProperties();
					for( TInt idx2 = 0; idx2 < countProps && !removed; idx2++ )
						{
						if ( props[ idx2 ].Name() == EVPbkVersitNameGEO &&
							 ( ( props[ idx2 ].Parameters().Contains( EVPbkVersitParamHOME ) &&
									 groupId == EPbk2FieldGroupIdHomeAddress ) ||
							   ( props[ idx2 ].Parameters().Contains( EVPbkVersitParamWORK ) &&
									 groupId == EPbk2FieldGroupIdCompanyAddress ) ||
							   ( !props[ idx2 ].Parameters().Contains( EVPbkVersitParamHOME ) &&
								 !props[ idx2 ].Parameters().Contains( EVPbkVersitParamWORK ) &&
								 groupId == EPbk2FieldGroupIdPostalAddress ) ) )
							{
							iContact.RemoveField(idx);
							removed = ETrue;
							}
						}
					}
            	
				for( TInt idx = fields.FieldCount() - 1; idx >= 0 ; idx-- )
					{
					if( groupId == fields.At( idx ).FieldProperty().GroupId() )
						{
						iContact.RemoveField(idx);
						}
					}
            	}
            break;
            }
        }
    __TEST_INVARIANT;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldArray::RemoveFieldFromUI
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorFieldArray::RemoveFieldFromUI( TInt aIndex )
    {
    __TEST_INVARIANT;
    CPbk2ContactEditorArrayItem* field = iFieldArray[aIndex];
    iUiBuilder.DeleteControl(field->ControlId());
	iFieldArray.Remove(aIndex);
	delete field;
    __TEST_INVARIANT;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldArray::GetUiFieldIndex
// The fields are in the same order in the ui and in the contact.
// The difference is that ui has only editable fields while contact can have
// not editable fields too and for every address all address fields are 
// joined into one field.
// --------------------------------------------------------------------------
//
inline TInt CPbk2ContactEditorFieldArray::GetUiFieldIndex
        ( TInt aContactFieldIndex )
    {
    TPbk2FieldGroupId previousGroupId = EPbk2FieldGroupIdNone;
    TInt result = aContactFieldIndex;
    for (TInt i = aContactFieldIndex - 1; i >= 0; --i)
        {
        __ASSERT_DEBUG( iContact.PresentationFields().FieldCount() > i,
            Panic( EPanic_GetUiFieldIndex_OOB ) );
        CPbk2PresentationContactField& contactField = 
                    iContact.PresentationFields().At(i);
        
        if (!contactField.IsEditable())
            {
            --result;
            continue;
            }
        
		if(TPbk2ContactEditorParams::EEditorView && IsFieldPartOfAddress(contactField))
			{
			if(contactField.FieldProperty().GroupId() == previousGroupId)
				{
				//Every address has one button, if address has 'n' fields we must remove 'n-1'
				--result;
				}
			previousGroupId = contactField.FieldProperty().GroupId();
			}
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldArray::GetUiFieldIndex
// The fields are in the same order in the ui and in the contact.
// The difference is that ui has only editable fields while contact can have
// not editable fields too and for every address all address fields are 
// joined into one field.
// --------------------------------------------------------------------------
//
inline TInt CPbk2ContactEditorFieldArray::GetUiFieldIndex
        ( TPbk2FieldGroupId aContactAddressGroupID )
    {
    TPbk2FieldGroupId previousGroupId = EPbk2FieldGroupIdNone;
    TInt result = KErrNone;
    for (TInt i = iContact.PresentationFields().FieldCount() - 1; i >= 0; --i)
        {
        __ASSERT_DEBUG( iContact.PresentationFields().FieldCount() > i,
            Panic( EPanic_GetUiFieldIndex_OOB ) );
        CPbk2PresentationContactField& contactField = 
                    iContact.PresentationFields().At(i);
        
        if (!contactField.IsEditable() && result != KErrNone)
            {
            --result;
            continue;
            }
        
		if(TPbk2ContactEditorParams::EEditorView && IsFieldPartOfAddress(contactField))
			{
			if(contactField.FieldProperty().GroupId() == previousGroupId)
				{
				if(result != KErrNone)
					{
					//Every address has one button, if address has 'n' fields we must remove 'n-1'
					--result;
					}
				}
			else if(previousGroupId == aContactAddressGroupID)
				{
				//Start counting down from first address field position in CPbk2PresentationContact
				result = i + 1;
				}
			previousGroupId = contactField.FieldProperty().GroupId();
			}
		else if(previousGroupId != EPbk2FieldGroupIdNone)
			{
			if(previousGroupId == aContactAddressGroupID)
				{
				//Start counting down from first address field position in CPbk2PresentationContact
				result = i + 1;
				}
			previousGroupId = EPbk2FieldGroupIdNone;
			}
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldArray::AcceptL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorFieldArray::AcceptL
        ( MPbk2ContactEditorFieldVisitor& aVisitor )
    {
    const TInt count = iFieldArray.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if( iFieldArray[i]->ContactEditorField() )
        	{
        	iFieldArray[i]->ContactEditorField()->AcceptL(aVisitor);
        	}
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldArray::__DbgTestInvariant
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorFieldArray::__DbgTestInvariant() const
	{
	#if defined(_DEBUG)
	const TInt fieldCount = iContact.Fields().FieldCount();
	TInt editableFields = 0;
	for (TInt i = 0; i < fieldCount; ++i)
	    {
	    if (iContact.PresentationFields().At(i).IsEditable())
	        {
	        ++editableFields;
	        }
	    }
	__ASSERT_DEBUG(iFieldArray.Count() <= editableFields, 
			Panic(EPanicInvariant_CountMismatch));

	const TInt uiCount = iFieldArray.Count();
	const TInt storeFieldCount = iContact.StoreContact().Fields().FieldCount();
	for (TInt i = 0; i < uiCount; ++i)
		{
		if( !iFieldArray[i]->ContactEditorField() )
			{
			continue;
			}
		
        __ASSERT_DEBUG( !( iFieldArray[i]->ContactEditorField()->FieldProperty().Flags() &
            KPbk2FieldFlagDisableEdit ),
                Panic(EPanicInvariant_NotEditableFieldFound));

        TBool result = EFalse;
        for (TInt j = 0; j < storeFieldCount && !result; ++j)
			{
            MVPbkStoreContactField& field =
                iContact.StoreContact().Fields().FieldAt( j );
            if ( iFieldArray[i]->ContactEditorField()->ContactField().IsSame( field ) )
                {
                result = ETrue;
                }
            }
        __ASSERT_DEBUG(result, Panic(EPanicInvariant_FieldNotFound));
        }
    #endif // _DEBUG
	}

// --------------------------------------------------------------------------
// CPbk2ContactEditorFieldArray::IsFieldPartOfAddress
// --------------------------------------------------------------------------
//
TBool CPbk2ContactEditorFieldArray::IsFieldPartOfAddress
        ( CPbk2PresentationContactField& aField )
    {
    if( aField.FieldProperty().GroupId() == EPbk2FieldGroupIdPostalAddress ||
    	aField.FieldProperty().GroupId() == EPbk2FieldGroupIdHomeAddress ||
    	aField.FieldProperty().GroupId() == EPbk2FieldGroupIdCompanyAddress )
    	return ETrue;
    return EFalse;
    }
//  End of File  
