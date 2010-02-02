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
* Description:  Phonebook 2 memory entry defaults dialog.
*
*/


// INCLUDE FILES
#include "cpbk2memoryentrydefaultsdlg.h"

// Phonebook 2
#include "cpbk2defaultattributeprocess.h"
#include "cpbk2fieldlistboxmodel.h"
#include "cpbk2fieldselector.h"
#include <cpbk2memoryentrydefaultsassigndlg.h>
#include <cpbk2iconarray.h>
#include <pbk2uicontrols.rsg>
#include <cpbk2presentationcontact.h>
#include <cpbk2presentationcontactfield.h>
#include <cpbk2presentationcontactfieldcollection.h>
#include <cpbk2fieldpropertyarray.h>
#include <cpbk2storepropertyarray.h>
#include <phonebook2privatecrkeys.h>
#include <tpbk2destructionindicator.h>

// Virtual Phonebook
#include <cvpbkcontactmanager.h>
#include <cvpbkdefaultattribute.h>
#include <cvpbkfieldfilter.h>
#include <cvpbkfieldtypeselector.h>
#include <mvpbkcontactfielddata.h>
#include <mvpbkcontactoperationbase.h>
#include <mvpbkfieldtype.h>
#include <vpbkfieldtype.hrh>
#include <vpbkvariant.hrh>

// System includes
#include <aknlists.h>   // EikControlFactory
#include <barsread.h>   // TResourceReader
#include <stringloader.h>
#include <featmgr.h>
#include <centralrepository.h>

// Debugging headers
#include <pbk2debug.h>


// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS

/**
 * Enum for default field types. Defines also order of items in list.
 *
 * Because the list returns the index of the currently displayed item,
 * it's been decided to treat the two versions of SMS (regular SMS versus
 * the enhanced email-over-SMS version) the same when populating the list,
 * and differentiate between the two when an SMS operation is selected.
 */
enum TDefaults
    {
    EPbk2CallDefault = 0,
    EPbk2VideoDefault,
    EPbk2VoIPDefault,
    EPbk2POCDefault,
    EPbk2SmsDefault,
    EPbk2MmsDefault,
    EPbk2EmailDefault,
    KPbk2NumberOfDefaults
    };

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPostCond_Constructor = 1,
    EPanicPostCond_ConstructL,
    EPanicPreCond_FixIndex,
    EPanicPostCond_FixIndex,
    EPanicLogic_DefaultFieldL
    };

static void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2MemoryEntryDefaultsDlg");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG


// LOCAL FUNCTIONS

/**
 * Returns the default property at index aDefaultIndex.
 *
 * @param aDefaultIndex         The selected default index.
 * @return Corresponding field property.
 */
TVPbkDefaultType PropertyAtIndex
        (const TInt aDefaultIndex)
    {
    TVPbkDefaultType property = EVPbkDefaultTypeUndefined;
    switch (aDefaultIndex)
        {
        case EPbk2CallDefault:
            {
            property = EVPbkDefaultTypePhoneNumber;
            break;
            }
        case EPbk2VideoDefault:
            {
            property = EVPbkDefaultTypeVideoNumber;
            break;
            }
        case EPbk2SmsDefault:
            {
            if (FeatureManager::FeatureSupported(KFeatureIdEmailOverSms))
                {
                property = EVPbkDefaultTypeEmailOverSms;
                }
            else
                {
                property = EVPbkDefaultTypeSms;
                }
            break;
            }
        case EPbk2MmsDefault:
            {
            property = EVPbkDefaultTypeMms;
            break;
            }
        case EPbk2EmailDefault:
            {
            property = EVPbkDefaultTypeEmail;
            break;
            }
        case EPbk2VoIPDefault:
            {
            property = EVPbkDefaultTypeVoIP;
            break;
            }
        case EPbk2POCDefault:
            {
            property = EVPbkDefaultTypePOC;
            break;
            }
        default:
            {
            User::Leave(KErrNotFound);
            break;
            }
        }
    return property;
    }

/**
 * Returns correct excludable selector resource id for given default index.
 *
 * @param aDefaultIndex     The index of the default to use.
 * @return  Appropriate selector resource id.
 */    
TInt ExcludedSelectorResourceByDefaultIndex( const TInt aDefaultIndex )
    {
    TInt resourceId = KErrNotFound;

    switch ( aDefaultIndex )
        {
        case EPbk2VideoDefault:
            {
            // pager and fax numbers are excluded from video selector
            resourceId = R_PHONEBOOK2_PAGERFAX_SELECTOR;
            break;
            }
        default:
            {
            // Do nothing
            break;
            }
        }

    return resourceId;    
    }
    
/**
 * Returns fields applicable to be set as the selected default.
 *
 * @param aContact          The contact whose fields to show.
 * @param aDefaultIndex     The index of the default to use.
 * @param aManager          Reference to contact manager.
 * @return Collection of fields applicable to use as default.
 */
CPbk2PresentationContactFieldCollection* ApplicableFieldsLC
        (CPbk2PresentationContact& aContact, const TInt aDefaultIndex,
        const CVPbkContactManager& aManager)
    {
    TInt resourceId = KErrNotFound;

    // Construct a correct field type selector
    switch (aDefaultIndex)
        {
        case EPbk2CallDefault:   // FALLTHROUGH
        case EPbk2VideoDefault:
            {
            resourceId = R_PHONEBOOK2_PHONENUMBER_SELECTOR;
            break;
            }
        case EPbk2SmsDefault:
            {
            if (FeatureManager::FeatureSupported(KFeatureIdEmailOverSms))
                {
                resourceId = R_PHONEBOOK2_EMAIL_OVER_SMS_SELECTOR;
                }
            else
                {
                resourceId = R_PHONEBOOK2_PHONENUMBER_SELECTOR;
                }
            break;
            }
        case EPbk2MmsDefault:
            {
            resourceId = R_PHONEBOOK2_MMS_SELECTOR;
            break;
            }
        case EPbk2EmailDefault:
            {
            resourceId = R_PHONEBOOK2_EMAIL_SELECTOR;
            break;
            }
        case EPbk2VoIPDefault: // FALLTHROUGH
        case EPbk2POCDefault:
            {
            resourceId = R_PHONEBOOK2_SIP_MSISDN_SELECTOR;
            break;
            }
        default:
            {
            User::Leave(KErrNotFound);
            break;
            }
        }

    // Create a resource reader
    User::LeaveIfError(resourceId);
    TResourceReader resReader;
    CCoeEnv::Static()->CreateResourceReaderLC
        (resReader, resourceId);

    // Construct a filtered store contact field collection
    CVPbkFieldTypeSelector* fieldTypeSelector =
        CVPbkFieldTypeSelector::NewL(resReader, aManager.FieldTypes());

    CleanupStack::PopAndDestroy(); // resReader
    // Create composite field selector
    CPbk2FieldSelector* selector = CPbk2FieldSelector::NewLC();
    // takes ownership, no need to push to cleanupstack
    selector->AddIncludedSelector( fieldTypeSelector );
    
    // Optional, if some fields should be exclude from original selector
    TInt excludedResId = 
        ExcludedSelectorResourceByDefaultIndex( aDefaultIndex );
     
    // If excluding is needed, create excluded field selector  
    CVPbkFieldTypeSelector* excludedFieldTypeSelector = NULL;
    if ( excludedResId != KErrNotFound )
        { 
        CCoeEnv::Static()->CreateResourceReaderLC( resReader, excludedResId ); 
        excludedFieldTypeSelector =
            CVPbkFieldTypeSelector::NewL( resReader, aManager.FieldTypes() );
            
        CleanupStack::PopAndDestroy(); // resReader
        // takes ownership, no need to push to cleanupstack
        selector->AddExcludedSelector( excludedFieldTypeSelector ); 
        }    

    const CVPbkFieldFilter::TConfig config(
            const_cast<MVPbkStoreContactFieldCollection&>
                (aContact.Fields()), selector, NULL);
    CVPbkFieldFilter* fieldFilter = CVPbkFieldFilter::NewL(config);
    CleanupStack::PushL(fieldFilter);

    CPbk2PresentationContactFieldCollection* fields =
        CPbk2PresentationContactFieldCollection::NewL
            (aContact.PresentationFields().FieldProperties(), *fieldFilter,
            aContact);

    CleanupStack::PopAndDestroy( 2 ); // fieldFilter,
                                     // selector

    CleanupStack::PushL(fields);
    return fields;
    }

/**
 * Finds default field identified by aDefaultIndex.
 *
 * @param aContact      The contact where to find the default from.
 * @param aDefaultIndex The index of the default to use.
 * @param aManager      A reference to contact manager.
 * @return Store contact field which has the default assigned,
 *         or NULL if not found.
 */
MVPbkStoreContactField* FindDefaultFieldL
        (MVPbkStoreContact& aContact, const TInt aDefaultIndex,
        CVPbkContactManager& aManager)
    {
    MVPbkStoreContactField* field = NULL;

    // Loop through contact's fields and find the specified field
    const TInt fieldCount = aContact.Fields().FieldCount();
    TVPbkDefaultType defaultType = PropertyAtIndex(aDefaultIndex);
    CVPbkDefaultAttribute* attr =
        CVPbkDefaultAttribute::NewL(defaultType);
    CleanupStack::PushL(attr);

    for (TInt i=0; i < fieldCount; ++i)
        {
        MVPbkStoreContactField* candidate = aContact.Fields().FieldAtLC(i);

        // Check if field has default attribute defaultType
        if (aManager.ContactAttributeManagerL().
                HasFieldAttributeL(*attr, *candidate))
            {
            field = candidate;
            CleanupStack::Pop(); // candidate
            break;
            }
        else
            {
            CleanupStack::PopAndDestroy(); // candidate
            }
        }

    CleanupStack::PopAndDestroy(attr);
    return field;
    }

/**
 * Returns default field identified by aDefaultIndex from aContact
 * or NULL if the default is not set.
 *
 * @param aContact          Reference to a contact.
 * @param aDefaultIndex     Index of the default to use.
 * @param aManager          Reference to contact manager.
 * @return The correct default field.
 */
CPbk2PresentationContactField* DefaultFieldL
        (CPbk2PresentationContact& aContact, const TInt aDefaultIndex,
        CVPbkContactManager& aManager)
    {
    CPbk2PresentationContactField* result = NULL;

    // Find the correct default field
    MVPbkStoreContactField* defaultField = FindDefaultFieldL
        (aContact.StoreContact(), aDefaultIndex, aManager);

    if (defaultField)
        {
        // Find the corresponding presentation field
        CPbk2PresentationContactFieldCollection& presentationFields =
            aContact.PresentationFields();

        for (TInt i = 0; i < presentationFields.FieldCount(); ++i)
            {
            CPbk2PresentationContactField& victim =
                presentationFields.At(i);
            if (victim.IsSame(*defaultField))
                {
                result = &victim;    // temporary reference is ok
                break;
                }
            }
        }
    delete defaultField;

    return result;
    }


/**
 * Returns the default UI text for default field identified by aDefaultIndex.
 *
 * @param aDefaultIndex     Index of the default to use.
 * @return Defaul UI text for the default.
 */
HBufC* DefaultFieldTextLC(const TInt aDefaultIndex)
    {
    HBufC* result = NULL;
    switch (aDefaultIndex)
        {
        case EPbk2CallDefault:
            {
            result = StringLoader::LoadLC( R_QTN_PHOB_SETI_CALL_DEFAULT );
            break;
            }
        case EPbk2VideoDefault:
            {
            result = StringLoader::LoadLC( R_QTN_PHOB_SETI_VIDEO_DEFAULT );
            break;
            }
        case EPbk2MmsDefault:
            {
            result = StringLoader::LoadLC( R_QTN_PHOB_SETI_MMS_DEFAULT );
            break;
            }
        case EPbk2SmsDefault:
            {
            // This is the same text for either SMS or EmailOverSms
            result = StringLoader::LoadLC( R_QTN_PHOB_SETI_SMS_DEFAULT );
            break;
            }
        case EPbk2EmailDefault:
            {
            result = StringLoader::LoadLC( R_QTN_PHOB_SETI_EMAIL_DEFAULT );
            break;
            }
        case EPbk2VoIPDefault:
            {
            result = StringLoader::LoadLC( R_QTN_PHOB_SETI_VOIP_DEFAULT );
            break;
            }
        case EPbk2POCDefault:
            {
            result = StringLoader::LoadLC( R_QTN_PHOB_SETI_POC_DEFAULT );
            break;
            }
        default:
            {
            User::Leave(KErrNotFound);
            break;
            }
        }
    return result;
    }

} // namespace


// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsDlg::CPbk2MemoryEntryDefaultsDlg
// --------------------------------------------------------------------------
//
inline CPbk2MemoryEntryDefaultsDlg::CPbk2MemoryEntryDefaultsDlg
        ( CPbk2PresentationContact& aContact,
          CVPbkContactManager& aManager ) :
            iContact(aContact), iManager(aManager), iLVFlags(0)
    {
    __ASSERT_DEBUG(!iListBox && !iDestroyedPtr,
        Panic(EPanicPostCond_Constructor));
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsDlg::~CPbk2MemoryEntryDefaultsDlg
// --------------------------------------------------------------------------
//
CPbk2MemoryEntryDefaultsDlg::~CPbk2MemoryEntryDefaultsDlg()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2MemoryEntryDefaultsDlg destructor start"));
    if (iDestroyedPtr)
        {
        *iDestroyedPtr = ETrue;
        }
    delete iListBox;
    delete iAttributeProcess;

    FeatureManager::UnInitializeLib();

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2MemoryEntryDefaultsDlg destructor end"));
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsDlg::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2MemoryEntryDefaultsDlg::ConstructL()
    {
    // Initialize feature manager
    FeatureManager::InitializeLibL();

    // Read local variation flags
    CRepository* key = CRepository::NewL(TUid::Uid(KCRUidPhonebook));
    TInt err = key->Get(KPhonebookLocalVariationFlags, iLVFlags);
    if (err != KErrNone)
        {
        // If there were problems reading the flags, assume everything is off
        iLVFlags = 0;
        }
    delete key;

    // Create a list box
    iListBox = static_cast<CEikColumnListBox*>
        (EikControlFactory::CreateByTypeL
            (EAknCtMenuDoublePopupMenuListBox).iControl);

    CAknPopupList::ConstructL
        (iListBox, R_PBK2_SOFTKEYS_ASSIGN_BACK_ASSIGN,
        AknPopupLayouts::EMenuDoubleWindow);

    // Init list box
    iListBox->ConstructL(this, CEikListBox::ELeftDownInViewRect);
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL
        (CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);

    // Set title of popuplist
    HBufC* heading = StringLoader::LoadLC
        ( R_QTN_PHOB_TITLE_DEFAULT_SETTINGS );
    SetTitleL(*heading);
    CleanupStack::PopAndDestroy(heading);

    // Init attribute set/remove process object
    iAttributeProcess = CPbk2DefaultAttributeProcess::NewL
        (iManager, iContact.StoreContact(), *this);

    CreateLinesL();

    __ASSERT_DEBUG(iListBox && !iDestroyedPtr,
        Panic(EPanicPostCond_ConstructL));
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsDlg::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2MemoryEntryDefaultsDlg* CPbk2MemoryEntryDefaultsDlg::NewL
        ( CPbk2PresentationContact& aContact, CVPbkContactManager& aManager )
    {
    CPbk2MemoryEntryDefaultsDlg* self = new(ELeave)
        CPbk2MemoryEntryDefaultsDlg(aContact, aManager);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsDlg::ExecuteLD
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2MemoryEntryDefaultsDlg::ExecuteLD()
    {
    CAknPopupList::ExecuteLD();
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsDlg::ProcessCommandL
// --------------------------------------------------------------------------
//
void CPbk2MemoryEntryDefaultsDlg::ProcessCommandL( TInt aCommandId )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2MemoryEntryDefaultsDlg::ProcessCommandL start"));

    switch (aCommandId)
        {
        case EAknSoftkeyOk:
            {
            DefaultsAssignPopupL(iListBox->CurrentItemIndex());
            break;
            }

        default:
            {
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                ("About to call CAknPopupList::ProcessCommandL"));
            CAknPopupList::ProcessCommandL(aCommandId);
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                ("Called CAknPopupList::ProcessCommandL"));
            break;
            }
        }
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2MemoryEntryDefaultsDlg::ProcessCommandL end"));
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsDlg::HandleListBoxEventL
// --------------------------------------------------------------------------
//
void CPbk2MemoryEntryDefaultsDlg::HandleListBoxEventL
        ( CEikListBox* aListBox, TListBoxEvent aEventType )
    {
    // Respond to events from listboxitem
    if (aListBox == iListBox)
        {
        if ( aEventType == MEikListBoxObserver::EEventEnterKeyPressed ||
             aEventType == EEventItemClicked )
            {
            // Ok/Enter key pressed, command is same as EAknSoftkeyOk
            ProcessCommandL(EAknSoftkeyOk);
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsDlg::CreateLinesL
// --------------------------------------------------------------------------
//
void CPbk2MemoryEntryDefaultsDlg::CreateLinesL() const
    {
    CDesCArray* lines = static_cast<CDesCArray*>
        (iListBox->Model()->ItemTextArray());
    lines->Reset();

    _LIT(KFormat, "%S\t%S");
    HBufC* noDefault = StringLoader::LoadLC(R_QTN_PHOB_SETI_NO_DEFAULT);

    for (TInt row=0; row < KPbk2NumberOfDefaults; ++row)
        {
        // Only supported lines are created
        if (IsSupported(row))
            {
            HBufC* defaultFieldText = DefaultFieldTextLC(row);
            const CPbk2PresentationContactField* field =
                DefaultFieldL(iContact, row, iManager);
            TPtrC label;
            if(field)
                {
                label.Set(field->FieldLabel());
                }
            else
                {
                label.Set(*noDefault);
                }

            // Allocate and format the listbox line
            HBufC* lineBuf = HBufC::NewLC(KFormat().Length()
                + defaultFieldText->Length() + label.Length());
            TPtr line(lineBuf->Des());
            line.Format(KFormat, defaultFieldText, &label);

            lines->AppendL(line);

            // Cleanup
            CleanupStack::PopAndDestroy(2); // lineBuf, defaultFieldText
            }
        }

    CleanupStack::PopAndDestroy(); // noDefault
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsDlg::DefaultsAssignPopupL
// --------------------------------------------------------------------------
//
TBool CPbk2MemoryEntryDefaultsDlg::DefaultsAssignPopupL
        (TInt aDefaultIndex)
    {
    TBool thisDestroyed = EFalse;
    iDestroyedPtr = &thisDestroyed;
    TPbk2DestructionIndicator indicator
        ( &thisDestroyed, iDestroyedPtr );

    TBool result = EFalse;
    if (aDefaultIndex != KErrNotFound)
        {
        // First fix the index
        FixIndex(aDefaultIndex);

        // Construct a presentation contact field collection,
        // containing fields that apply to the field type requirements
        // of the chosen default
        CPbk2PresentationContactFieldCollection* fields =
            ApplicableFieldsLC(iContact, aDefaultIndex, iManager);

        // Construct and launch the assign dialog
        CPbk2MemoryEntryDefaultsAssignDlg* assignDlg =
            CPbk2MemoryEntryDefaultsAssignDlg::NewL();
        const TInt index = assignDlg->ExecuteLD(*fields, iManager,
            iContact.PresentationFields().FieldProperties());

        if (!thisDestroyed)
            {
            // Next find the field index is referencing
            MVPbkStoreContactField* field = NULL;
            const TInt fieldCount = fields->FieldCount();
            // Notice that the last index of the listbox
            // is bound to 'no default' selection, hence the index
            // must be smaller than field count.
            if (index > KErrNotFound && index < fieldCount)
                {
                // Set the new default
                field = &fields->At(index);
                if (field)
                    {
                    SetDefaultL(aDefaultIndex, *field);
                    }
                }

            else if (index == fieldCount)
                {
                // User selected the 'no default' option
                RemoveDefaultL(aDefaultIndex);
                }
            }

        CleanupStack::PopAndDestroy(fields);
        }
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsDlg::FixIndex
// This method corrects the given focus index to map with TDefaults enum.
// --------------------------------------------------------------------------
//
inline void CPbk2MemoryEntryDefaultsDlg::FixIndex( TInt& aIndex ) const
    {
    __ASSERT_DEBUG(aIndex < KPbk2NumberOfDefaults,
        Panic(EPanicPreCond_FixIndex));

    for (TInt i=0; i <= aIndex; ++i)
        {
        // If the default in question is not supported,
        // add one to the index, since there was a missing row
        if (!IsSupported(i))
            {
            ++aIndex;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsDlg::IsSupported
// --------------------------------------------------------------------------
//
TBool CPbk2MemoryEntryDefaultsDlg::IsSupported
        (const TInt aDefaults) const
    {
    TBool ret(ETrue);

    // Skip the line if MMS field not supported
    if (aDefaults == EPbk2MmsDefault &&
            !FeatureManager::FeatureSupported(KFeatureIdMMS))
        {
        ret = EFalse;
        }
      // Skip the line if Email field not supported
    else if (aDefaults == EPbk2EmailDefault &&
            !FeatureManager::FeatureSupported(KFeatureIdEmailUi))
        {

        ret = EFalse;
        }
    // Do not add video fields if they are not enabled
    else if ((aDefaults == EPbk2VideoDefault) &&
            !(iLVFlags & EVPbkLVAddVideoTelephonyFields))
        {
        ret = EFalse;
        }
    // Do not add Voip fields if they are not enabled
    else if (aDefaults == EPbk2VoIPDefault &&
            !FeatureManager::FeatureSupported(KFeatureIdCommonVoip))
        {
        ret = EFalse;
        }
    // Do not add POC fields if they are not enabled
    else if ((aDefaults == EPbk2POCDefault) &&
            !(iLVFlags & EVPbkLVPOC))
        {
        ret = EFalse;
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsDlg::AttributeProcessCompleted
// --------------------------------------------------------------------------
//
void CPbk2MemoryEntryDefaultsDlg::AttributeProcessCompleted()
    {
    delete iField;
    iField = NULL;

    // Redraw
    TRAP_IGNORE( CreateLinesL() );
    iListBox->DrawDeferred();
    DrawDeferred();
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsDlg::AttributeProcessFailed
// --------------------------------------------------------------------------
//
void CPbk2MemoryEntryDefaultsDlg::AttributeProcessFailed( TInt aErrorCode )
    {
    delete iField;
    iField = NULL;

    iCoeEnv->HandleError( aErrorCode );
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsDlg::SetDefaultL
// Sets aField as the default (identified by aDefaultIndex) field
// for the contact.
// --------------------------------------------------------------------------
//
void CPbk2MemoryEntryDefaultsDlg::SetDefaultL
        ( const TInt aDefaultIndex, MVPbkStoreContactField& aField )
    {
    delete iField;
    iField = NULL;

    // Find actual store contact field from presentation
    // contact's store contact
    for ( TInt i = 0;
            i < iContact.StoreContact().Fields().FieldCount(); ++i )
        {
        MVPbkStoreContactField* actualField =
            iContact.StoreContact().Fields().FieldAtLC( i );
        if ( aField.IsSame( *actualField ) )
            {
            iField = actualField;
            CleanupStack::Pop(); // actualField
            break;
            }
        else
            {
            CleanupStack::PopAndDestroy(); // actualField
            }
        }

    if ( iField )
        {
        TVPbkDefaultType property = PropertyAtIndex( aDefaultIndex );
        iAttributeProcess->SetDefaultL( property, *iField );
        }
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryDefaultsDlg::RemoveDefaultL
// Removes default (identified by aDefaultIndex) from aContact.
// --------------------------------------------------------------------------
//
void CPbk2MemoryEntryDefaultsDlg::RemoveDefaultL
        ( const TInt aDefaultIndex )
    {
    TVPbkDefaultType property = PropertyAtIndex( aDefaultIndex );
    iAttributeProcess->RemoveDefaultL( property );
    }

//  End of File
