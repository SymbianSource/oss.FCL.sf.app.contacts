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
*       Provides methods for Phonebook Data Save service helper.
*
*/


// INCLUDE FILES
#include "CPbkDataSaveAppUi.h"
#include <cntfldst.h>
#include <eikmenup.h>
#include <avkon.rsg>

// PbkView include files
#include "PbkDataSaveAppUi.hrh"
#include <PbkView.rsg>
#include "CPbkContactEditorDlg.h"
#include "CPbkSingleEntryFetchDlg.h"
#include "CPbkMemoryEntryAddItemDlg.h"
#include <CPbkFFSCheck.h>

// PbkExt include files
#include <CPbkExtGlobals.h>

// PbkEng include files
#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
#include <CPbkFieldInfo.h>


/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS
#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_CTextFieldData_AddFieldL = 1,
    EPanicPreCond_CDateFieldData_AddFieldL,
    EPanicPreCond_DoHandleCommandL,
    EPanicPreCond_ResetWhenDestroyed,
    EPanicPreCond_SelectFieldTypeL,
    EPanicPreCond_EditContactL,
    EPanicPreCond_GetContactItemL,
    EPanicPreCond_ExecuteLD
    };
#endif

/**
 * Flags for CPbkDataSaveAppUi.
 */
enum TFlags
    {
    EFlagsHideEditorExit = 0x00000001
    };


// ==================== LOCAL FUNCTIONS ====================

#ifdef _DEBUG
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkDataSaveAppUi");
    User::Panic(KPanicText, aReason);
    }
#endif


}  // namespace


// MODULE DATA STRUCTURES

/**
 * Field adder strategy class for CPbkDataSaveAppUi implementation.
 * Encapsulates the data to be added to the contact.
 */
class CPbkDataSaveAppUi::MFieldData
    {
    public:
        /**
         * Virtual destructor.
         */
        virtual ~MFieldData() { }

        /**
         * Returns the storage type of data to be added.
         */
        virtual TUint StorageType() const = 0;

        /**
         * Adds a field with data encapsulated in this class in to a contact.
         *
         * @param aContact  contact where the field and data is to be added.
         * @param aFieldType    type of the field to add. 
         * @param aCallback     calls back to this object if there are 
         *                      problems when adding the field.
         * @return  the added field containing data, NULL if field couldn't be
         *          added.
         * @precond aFieldType.FieldStorageType()==this->StorageType()
         */
        virtual TPbkContactItemField* AddFieldL
            (CPbkContactItem& aContact, 
            CPbkFieldInfo& aFieldType,
            MPbkDataSaveCallback* aCallback) = 0;

    };

/**
 * Text field data implementation.
 */
NONSHARABLE_CLASS(CPbkDataSaveAppUi::CTextFieldData) : 
        public CBase, 
        public CPbkDataSaveAppUi::MFieldData
    {
    public:
        static CTextFieldData* NewLC(const TDesC& aText);
        ~CTextFieldData();

    public:  // from MFieldData
        TUint StorageType() const;
        TPbkContactItemField* AddFieldL
            (CPbkContactItem& aContact, 
            CPbkFieldInfo& aFieldType,
            MPbkDataSaveCallback* aCallback);

    private:  // Implementation
        CTextFieldData();

    private:  // Data
        /// Own: text data
        HBufC* iText;
    };

/**
 * Date field data implementation.
 */
NONSHARABLE_CLASS(CPbkDataSaveAppUi::CDateFieldData) : 
        public CBase, 
        public CPbkDataSaveAppUi::MFieldData
    {
    public:
        static CDateFieldData* NewLC(const TTime& aText);

    public:  // from MFieldData
        TUint StorageType() const;
        TPbkContactItemField* AddFieldL
            (CPbkContactItem& aContact, 
            CPbkFieldInfo& aFieldType,
            MPbkDataSaveCallback* aCallback);

    private:  // Implementation
        CDateFieldData();

    private:  // Data
        /// Own: date data
        TTime iDate;
    };

/**
 * Command base class for CPbkDataSaveAppUi::HandleCommandL implementation.
 */
NONSHARABLE_CLASS(CPbkDataSaveAppUi::CCommandBase) : 
        public CBase
    {
    public:  // Interface
        /**
         * Command intialisation data.
         */
        struct TData
            {
            /// Own: the command id to execute
            TInt iCommandId;
            /// Ref: Phonebook engine
            CPbkContactEngine* iEngine;
            /// Ref: field types to apply
            CArrayPtr<CPbkFieldInfo>* iFieldTypes;
            /// Ref: field data to save
            MFieldData* iFieldData;
            /// Ref: callback interface
            MPbkDataSaveCallback* iCallback;
            /// Own: copy of CPbkDataSaveAppUi's flags
            TUint iFlags;
            };

        /**
         * Resets aSelfPtr to NULL when this object is destroyed.
         * @precond !aSelfPtr || *aSelfPtr==this
         */
        void ResetWhenDestroyed(CCommandBase** aSelfPtr);

        /**
         * Executes this command
         */
        TBool ExecuteLD();

        /**
         * Destructor.
         */
        ~CCommandBase();

    protected:  // Interface
        /**
         * Constructor.
         */
        CCommandBase();

        /**
         * Creates/Reads the contact item where to add data.
         */
        virtual CPbkContactItem* GetContactItemL() = 0;

        /**
         * Creates the field type dialog to use in SelectFieldTypeL.
         */
        virtual CPbkMemoryEntryAddItemDlg* CreateFieldTypeDialogL() = 0;

        /**
         * Selects field type.
         * @param aFieldTypes   types where to select from.
         * @return selected field type or NULL if canceled or nothing to select.
         */
        CPbkFieldInfo* SelectFieldTypeL(CArrayPtr<CPbkFieldInfo>& aFieldTypes);

        /**
         * Opens contact editor for a contact.
         *
         * @param aContact  contact to be edited.
         * @param aField    field to focus intially in the editor.
         * @return ETrue if contact was saved, EFalse otherwise.
         */
        TBool EditContactL
            (CPbkContactItem& aContact, const TPbkContactItemField& aField);

        /**
         * Returns the index of the field to be focused in the contact editor
         * dialog. Default implementation returns aField's index in aContact's
         * field set.
         */
        virtual TInt FocusedFieldIndex
            (CPbkContactItem& aContact,
            const TPbkContactItemField& aField) const;

        /**
         * Returns ETrue if this command modfies a new contact, EFalse 
         * otherwise.
         */
        virtual TBool NewContact() = 0;

        /**
         * Filters applicable field types according to what new fields the
         * contact can accept and compabilty between field type and storage
         * type.
         *
         * @param aContact  contact where the field is to be added.
         * @param aFieldTypes   list of field types to be filtered.
         * @param aStorageType  type of data to be added to the field.
         * @return new filtered array of field types.
         */
        CArrayPtr<CPbkFieldInfo>* GetFilterFieldTypesLC
            (const CPbkContactItem& aContact, 
            CArrayPtr<CPbkFieldInfo>& aFieldTypes,
            TUint aStorageType);

        /**
         * Returns true field type aFieldType should be included in selectable 
         * types. Checks if the new field can be added to aContact and that the
         * field type is compatible with aStorageType.
         */
        TBool FieldTypeIncluded
            (CPbkFieldInfo& aFieldType,
            const CPbkContactItem& aContact, 
            TUint aStorageType);

    protected:  // Data
        /// Own: data
        TData iData;

    private: // Implementation
        NONSHARABLE_CLASS(TSelfCleanup)
            {
            public:
                inline TSelfCleanup(CCommandBase* aSelf) : iSelf(aSelf) 
                    { 
                    }
                inline operator TCleanupItem()
                    {
                    return TCleanupItem(Cleanup,this);
                    }
                inline void Reset() { iSelf=NULL; }
            private:
                static void Cleanup(TAny* aObject);
                CCommandBase* iSelf;
            };

    private:  // Data
        /// Ref: referred pointer is set to NULL in destructor
        CCommandBase** iSelfPtr;
        /// Own: for cleanup purposes
        TSelfCleanup* iSelfCleanup;
        /// Own: iContactItem
        CPbkContactItem* iContactItem;
        /// Own: add item dialog
        CPbkMemoryEntryAddItemDlg* iItemTypeDlg;
        /// Own: editor dialog
        CPbkContactEditorDlg* iEditDlg;
    };

/**
 * Command implementation for Create New service.
 */
NONSHARABLE_CLASS(CPbkDataSaveAppUi::CCreateNewCommand) : 
        public CPbkDataSaveAppUi::CCommandBase
    {
    public:  // Interface
        static CCreateNewCommand* NewL(const TData& aData);

    public:  // from CCommandBase
        CPbkContactItem* GetContactItemL();
        CPbkMemoryEntryAddItemDlg* CreateFieldTypeDialogL();
        TInt FocusedFieldIndex
            (CPbkContactItem& aContact,
            const TPbkContactItemField& aField) const;
        TBool NewContact();

    private:  // Implementation
        CCreateNewCommand();
    };

/**
 * Command implementation for Add to Existing service.
 */
NONSHARABLE_CLASS(CPbkDataSaveAppUi::CAddToExistingCommand) : 
        public CPbkDataSaveAppUi::CCommandBase
    {
    public:  // Interface
        static CAddToExistingCommand* NewL(const TData& aData);
        ~CAddToExistingCommand();

    public:  // from CCommandBase
        CPbkContactItem* GetContactItemL();
        CPbkMemoryEntryAddItemDlg* CreateFieldTypeDialogL();
        TBool NewContact();

    private:  // Implementation
        CAddToExistingCommand();

    private:  // Data
        /// Own: Active fetch dialog
        CPbkSingleEntryFetchDlg* iFetchDlg;
    };


// ==================== MEMBER FUNCTIONS ====================

////////////////////////////////////////////////////////////
// CPbkDataSaveAppUi
inline CPbkDataSaveAppUi::CPbkDataSaveAppUi(CPbkContactEngine& aEngine) :
    iEngine(&aEngine),
    iBaseCommandId(EPbkCmdDataSaveBase)
    {
    // CBase::operator new() will reset other members
    }

inline void CPbkDataSaveAppUi::ConstructL()
    {
    iFFSCheck = CPbkFFSCheck::NewL();

    // Keep a handle to the UI extension factory in this dialog to prevent 
    // multiple inits of the factory by the subdialogs that are launched by
    // this service.
    iExtGlobals = CPbkExtGlobals::InstanceL();
    }

EXPORT_C CPbkDataSaveAppUi* CPbkDataSaveAppUi::NewL
        (CPbkContactEngine& aEngine)
    {
    CPbkDataSaveAppUi* self = new(ELeave) CPbkDataSaveAppUi(aEngine);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CPbkDataSaveAppUi::~CPbkDataSaveAppUi()
    {
    delete iExecutingCommand;
    Release(iExtGlobals);
    delete iFFSCheck;
    }

EXPORT_C void CPbkDataSaveAppUi::AddMenuItemsL
        (CEikMenuPane* aMenuPane, TInt aCommandId)
    {
    // Store base command id
    iBaseCommandId = aCommandId;
    // Store replaced menu item's position
    TInt pos;
    aMenuPane->ItemAndPos(aCommandId, pos);
    // Add data save submenu after marker item
    aMenuPane->AddMenuItemsL(R_PBK_DATA_SAVE_MENU, aCommandId);
    // Delete marker item from the menu
    aMenuPane->DeleteBetweenMenuItems(pos,pos);
    }

EXPORT_C void CPbkDataSaveAppUi::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    if (aResourceId == R_PBK_DATA_SAVE_SUBMENU)
        {
        // Patch command ids of the submenu to be relative to iBaseCommandId
        CEikMenuPaneItem::SData& createNewItemData = 
            aMenuPane->ItemData(EPbkCmdDataSaveCreateNew);
        CEikMenuPaneItem::SData& addToExistingItemData =
            aMenuPane->ItemData(EPbkCmdDataSaveAddToExisting); 
        createNewItemData.iCommandId += iBaseCommandId;
        addToExistingItemData.iCommandId += iBaseCommandId;
        }
    }

EXPORT_C TBool CPbkDataSaveAppUi::HandleCommandL
        (TInt aCommandId, 
        CArrayPtr<CPbkFieldInfo>& aFieldTypes,
        const TDesC& aText,
        MPbkDataSaveCallback* aCallback/*=NULL*/)
    {
    TBool result = EFalse;
    if (iFFSCheck->FFSClCheckL())
        {
        // Pack data into a MFieldData wrapper
        CTextFieldData* fieldData = CTextFieldData::NewLC(aText);
        result = DoHandleCommandL
            (aCommandId, aFieldTypes, *fieldData, aCallback);
        CleanupStack::PopAndDestroy();  // fieldData
        }
    return result;
    }

EXPORT_C void CPbkDataSaveAppUi::HideEditorExitCommand()
    {
    iFlags |= EFlagsHideEditorExit;
    }

EXPORT_C TBool CPbkDataSaveAppUi::HandleCommandL
        (TInt aCommandId, 
        CArrayPtr<CPbkFieldInfo>& aFieldTypes,
        const TTime& aDate,
        MPbkDataSaveCallback* aCallback/*=NULL*/)
    {
    TBool result = EFalse;
    if (iFFSCheck->FFSClCheckL())
        {
        // Pack data into a MFieldData wrapper
        CDateFieldData* fieldData = CDateFieldData::NewLC(aDate);
        result = DoHandleCommandL
            (aCommandId, aFieldTypes, *fieldData, aCallback);
        CleanupStack::PopAndDestroy();  // fieldData
        }
    return result;
    }

TBool CPbkDataSaveAppUi::DoHandleCommandL
        (TInt aCommandId, 
        CArrayPtr<CPbkFieldInfo>& aFieldTypes,
        MFieldData& aFieldData,
        MPbkDataSaveCallback* aCallback)
    {
    // Create command object parameter data
    __ASSERT_DEBUG(!iExecutingCommand, Panic(EPanicPreCond_DoHandleCommandL));

    CCommandBase::TData cmdData;
    cmdData.iCommandId = aCommandId-iBaseCommandId;
    cmdData.iEngine = iEngine;
    cmdData.iFieldTypes = &aFieldTypes;
    cmdData.iFieldData = &aFieldData;
    cmdData.iFlags = iFlags;

    // Default callback object
    MPbkDataSaveCallback defaultCallback;
    if (aCallback)
        {
        cmdData.iCallback = aCallback;
        }
    else
        {
        cmdData.iCallback = &defaultCallback;
        }

    // Create a command object
    TBool result = EFalse;
    switch (cmdData.iCommandId)
        {
        case EPbkCmdDataSaveCreateNew:
            {
            iExecutingCommand = CCreateNewCommand::NewL(cmdData);
            break;
            }
        case EPbkCmdDataSaveAddToExisting:
            {
            iExecutingCommand = CAddToExistingCommand::NewL(cmdData);
            break;
            }
        }

    if (iExecutingCommand)
        {
        iExecutingCommand->ResetWhenDestroyed(&iExecutingCommand);
        result = iExecutingCommand->ExecuteLD();
        }

    return result;
    }


////////////////////////////////////////////////////////////
// CPbkDataSaveAppUi::CTextFieldData

inline CPbkDataSaveAppUi::CTextFieldData::CTextFieldData()
    {
    // operator new(TLeave) resets member data
    }

CPbkDataSaveAppUi::CTextFieldData* CPbkDataSaveAppUi::CTextFieldData::NewLC
        (const TDesC& aText)
    {
    CTextFieldData* self = new(ELeave) CTextFieldData;
    CleanupStack::PushL(self);
    self->iText = aText.AllocL();
    return self;
    }

CPbkDataSaveAppUi::CTextFieldData::~CTextFieldData()
    {
    delete iText;
    }

TUint CPbkDataSaveAppUi::CTextFieldData::StorageType() const
    {
    return KStorageTypeText;
    }

TPbkContactItemField* CPbkDataSaveAppUi::CTextFieldData::AddFieldL
        (CPbkContactItem& aContact, 
        CPbkFieldInfo& aFieldInfo,
        MPbkDataSaveCallback* aCallback)
    {
    __ASSERT_DEBUG(aFieldInfo.FieldStorageType() == StorageType(), 
        Panic(EPanicPreCond_CTextFieldData_AddFieldL));

    TPtrC text = *iText;
    if (text.Length() > aFieldInfo.MaxLength())
        {
        if (aCallback)
            {
            text.Set(aCallback->PbkDataSaveClipTextL(*iText, aFieldInfo));
            // Give up if text is still too long
            if (text.Length() > aFieldInfo.MaxLength())
                {
                User::Leave(KErrOverflow);
                }
            }
        else
            {
            text.Set(iText->Left(aFieldInfo.MaxLength()));
            }
        }

    // Don't add an empty field
    if (text.Length() == 0)
        {
        return NULL;
        }

    // Add field and text into it leave-safely
    HBufC* textBuf = text.AllocLC();
    TPbkContactItemField* field = aContact.AddOrReturnUnusedFieldL(aFieldInfo);
    CleanupStack::Pop();  // textBuf
    field->TextStorage()->SetText(textBuf);
    return field;
    }


////////////////////////////////////////////////////////////
// CPbkDataSaveAppUi::CDateFieldData

inline CPbkDataSaveAppUi::CDateFieldData::CDateFieldData()
    {
    // operator new(TLeave) resets member data
    }

CPbkDataSaveAppUi::CDateFieldData* CPbkDataSaveAppUi::CDateFieldData::NewLC
        (const TTime& aDate)
    {
    CDateFieldData* self = new(ELeave) CDateFieldData;
    CleanupStack::PushL(self);
    self->iDate = aDate;
    return self;
    }

TUint CPbkDataSaveAppUi::CDateFieldData::StorageType() const
    {
    return KStorageTypeDateTime;
    }

TPbkContactItemField* CPbkDataSaveAppUi::CDateFieldData::AddFieldL
        (CPbkContactItem& aContact, 
        CPbkFieldInfo& aFieldInfo,
        MPbkDataSaveCallback* /*aCallback*/)
    {
    __ASSERT_DEBUG(aFieldInfo.FieldStorageType() == StorageType(), 
        Panic(EPanicPreCond_CDateFieldData_AddFieldL));
    TPbkContactItemField* field = aContact.AddOrReturnUnusedFieldL(aFieldInfo);
    field->DateTimeStorage()->SetTime(iDate);
    return field;
    }


////////////////////////////////////////////////////////////
// CPbkDataSaveAppUi::CCommandBase

inline CPbkDataSaveAppUi::CCommandBase::CCommandBase()
    {
    // operator new(TLeave) resets member data
    }

void CPbkDataSaveAppUi::CCommandBase::ResetWhenDestroyed(CCommandBase** aSelfPtr)
    {
    __ASSERT_DEBUG(!aSelfPtr || *aSelfPtr==this, Panic(EPanicPreCond_ResetWhenDestroyed));

    iSelfPtr = aSelfPtr;
    }

TBool CPbkDataSaveAppUi::CCommandBase::ExecuteLD()
    {
    __ASSERT_DEBUG(!iContactItem, Panic(EPanicPreCond_ExecuteLD));

    TSelfCleanup selfCleanup(this);
    iSelfCleanup = &selfCleanup;
    CleanupStack::PushL(selfCleanup);

    TBool result = EFalse;
    iContactItem = GetContactItemL();
    if (iContactItem)
        {
        CArrayPtr<CPbkFieldInfo>* filteredFieldTypes = GetFilterFieldTypesLC
            (*iContactItem, *iData.iFieldTypes, iData.iFieldData->StorageType());

        CPbkFieldInfo* fieldType = NULL;
        if (filteredFieldTypes->Count() > 1)
            {
            fieldType = SelectFieldTypeL(*filteredFieldTypes);
            }
        else if (filteredFieldTypes->Count() == 1)
            {
            fieldType = (*filteredFieldTypes)[0];
            }
        else
            {
            if (iData.iCallback) 
                {
                iData.iCallback->PbkDataSaveNoFieldL(*iContactItem);
                }
            }

        TPbkContactItemField* field = NULL;
        if (fieldType)
            {
            field = iData.iFieldData->AddFieldL(*iContactItem, *fieldType, iData.iCallback);
            }

        if (field)
            {
            result = EditContactL(*iContactItem, *field);
            }

        CleanupStack::PopAndDestroy();  // filteredFieldTypes
        }

    CleanupStack::PopAndDestroy();  // selfCleanup
    return result;
    }

CPbkDataSaveAppUi::CCommandBase::~CCommandBase()
    {
    if (iSelfCleanup)
        {
        // Prevent double deletion through cleanup stack
        iSelfCleanup->Reset();
        }
    if (iSelfPtr)
        {
        *iSelfPtr = NULL;
        }
    delete iItemTypeDlg;
    delete iEditDlg;
    if (iContactItem)
        {
        // CpbkContactEngine::CloseContactL() never leaves despite its name
        iData.iEngine->CloseContactL(iContactItem->Id());
        delete iContactItem;
        }
    }

CPbkFieldInfo* CPbkDataSaveAppUi::CCommandBase::SelectFieldTypeL
        (CArrayPtr<CPbkFieldInfo>& aFieldTypes)
    {
    __ASSERT_DEBUG(!iItemTypeDlg, Panic(EPanicPreCond_SelectFieldTypeL));
    iItemTypeDlg = CreateFieldTypeDialogL();
    iItemTypeDlg->ResetWhenDestroyed(&iItemTypeDlg);
    CPbkFieldInfo* result = NULL;
    result = iItemTypeDlg->ExecuteLD(aFieldTypes);
    return result;
    }

TBool CPbkDataSaveAppUi::CCommandBase::EditContactL
        (CPbkContactItem& aContact, const TPbkContactItemField& aField)
    {
    __ASSERT_DEBUG(!iEditDlg, Panic(EPanicPreCond_EditContactL));

    const TInt fieldIndex = FocusedFieldIndex(aContact,aField);
	iEditDlg = CPbkContactEditorDlg::NewL
        (*iData.iEngine, aContact, NewContact(), fieldIndex, ETrue);
    iEditDlg->ResetWhenDestroyed(&iEditDlg);
    if (iData.iFlags & EFlagsHideEditorExit)
        {
        iEditDlg->HideExitCommand();
        }
	TBool result = EFalse;
    result = iEditDlg->ExecuteLD() != KNullContactId; 
    return result;
    }

TInt CPbkDataSaveAppUi::CCommandBase::FocusedFieldIndex
        (CPbkContactItem& aContact,
        const TPbkContactItemField& aField) const
    {
    return aContact.FindFieldIndex(aField);
    }

inline TBool CPbkDataSaveAppUi::CCommandBase::FieldTypeIncluded
        (CPbkFieldInfo& aFieldType,
        const CPbkContactItem& aContact, 
        TUint aStorageType)
    {
    return 
        (aFieldType.FieldStorageType()==aStorageType &&
        aFieldType.IsEditable() &&
        aContact.CanAcceptDataOfType(aFieldType) &&
        aFieldType.UserCanAddField() );
    }

CArrayPtr<CPbkFieldInfo>* CPbkDataSaveAppUi::CCommandBase::GetFilterFieldTypesLC
        (const CPbkContactItem& aContact, 
        CArrayPtr<CPbkFieldInfo>& aFieldTypes,
        TUint aStorageType)
    {
    CArrayPtrFlat<CPbkFieldInfo>* filteredFieldTypes = 
        new(ELeave) CArrayPtrFlat<CPbkFieldInfo>(1 /*realloc granularity*/);
    CleanupStack::PushL(filteredFieldTypes);
    const TInt count = aFieldTypes.Count();
    filteredFieldTypes->SetReserveL(count);
    for (TInt i = 0; i < count; ++i)
        {
        CPbkFieldInfo* fieldType = aFieldTypes[i];
        if (FieldTypeIncluded(*fieldType,aContact,aStorageType))
            {
            filteredFieldTypes->AppendL(fieldType);
            }
        }
    return filteredFieldTypes;
    }

void CPbkDataSaveAppUi::CCommandBase::TSelfCleanup::Cleanup(TAny* aObject)
    {
    delete (static_cast<TSelfCleanup*>(aObject)->iSelf);
    }


////////////////////////////////////////////////////////////
// CPbkDataSaveAppUi::CCreateNewCommand

inline CPbkDataSaveAppUi::CCreateNewCommand::CCreateNewCommand()
    {
    // operator new(TLeave) resets member data
    }

CPbkDataSaveAppUi::CCreateNewCommand* CPbkDataSaveAppUi::CCreateNewCommand::NewL
        (const TData& aData)
    {
    CCreateNewCommand* self = new(ELeave) CCreateNewCommand;
    self->iData = aData;
    return self;
    }

CPbkContactItem* CPbkDataSaveAppUi::CCreateNewCommand::GetContactItemL()
    {
    CPbkContactItem* ci = iData.iEngine->CreateEmptyContactL();
    return ci;
    }

CPbkMemoryEntryAddItemDlg* CPbkDataSaveAppUi::CCreateNewCommand::CreateFieldTypeDialogL()
    {
    return new(ELeave) CPbkItemTypeSelectCreateNew;
    }

TInt CPbkDataSaveAppUi::CCreateNewCommand::FocusedFieldIndex
        (CPbkContactItem& /*aContact*/,
        const TPbkContactItemField& /*aField*/) const
    {
    // Always focus the first field
    return 0;
    }

TBool CPbkDataSaveAppUi::CCreateNewCommand::NewContact()
    {
    return ETrue;
    }


////////////////////////////////////////////////////////////
// CPbkDataSaveAppUi::CAddToExistingCommand

inline CPbkDataSaveAppUi::CAddToExistingCommand::CAddToExistingCommand()
    {
    // operator new(TLeave) resets member data
    }

CPbkDataSaveAppUi::CAddToExistingCommand* CPbkDataSaveAppUi::CAddToExistingCommand::NewL
        (const TData& aData)
    {
    CAddToExistingCommand* self = new(ELeave) CAddToExistingCommand;
    self->iData = aData;
    return self;
    }

CPbkDataSaveAppUi::CAddToExistingCommand::~CAddToExistingCommand()
    {
    delete iFetchDlg;
    }

CPbkContactItem* CPbkDataSaveAppUi::CAddToExistingCommand::GetContactItemL()
    {
    __ASSERT_DEBUG(!iFetchDlg, Panic(EPanicPreCond_GetContactItemL));

    // Run Single Entry Fetch
    CPbkSingleEntryFetchDlg::TParams params;
    params.iContactView = &iData.iEngine->AllContactsView();
	params.iCbaId = R_PBK_SOFTKEYS_SINGLE_ENTRY_FETCH;

    iFetchDlg = CPbkSingleEntryFetchDlg::NewL(params);
    iFetchDlg->ResetWhenDestroyed(&iFetchDlg);
	TInt res = 0;
    res = iFetchDlg->ExecuteLD();

    CPbkContactItem* ci = NULL;
    if (res && params.iSelectedEntry != KNullContactId)
        {
        ci = iData.iEngine->OpenContactL(params.iSelectedEntry);
        }
    return ci;
    }

CPbkMemoryEntryAddItemDlg* CPbkDataSaveAppUi::CAddToExistingCommand::CreateFieldTypeDialogL()
    {
    return new(ELeave) CPbkItemTypeSelectAddToExisting;
    }

TBool CPbkDataSaveAppUi::CAddToExistingCommand::NewContact()
    {
    return EFalse;
    }


////////////////////////////////////////////////////////////
// MPbkDataSaveCallback default implementation

EXPORT_C void MPbkDataSaveCallback::PbkDataSaveNoFieldL
        (CPbkContactItem& /*aContact*/)
    {
    }

EXPORT_C TPtrC MPbkDataSaveCallback::PbkDataSaveClipTextL
        (const TDesC& aText, CPbkFieldInfo& aFieldInfo)
    {
    return aText.Left(aFieldInfo.MaxLength());
    }

// End of File
