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
*       Provides methods for Phonebook Single Item Fetch API.
*
*/


// INCLUDE FILES
#include "CPbkSingleItemFetchDlg.h"  // This class

#include <avkon.rsg>
#include <StringLoader.h>

#include <pbkview.rsg>
#include "CPbkFetchDlg.h"
#include "MPbkFetchCallbacks.h"
#include "CPbkSelectFieldDlg.h"

#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
#include <CPbkFieldInfo.h>

// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS
#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPostCond_Constructor = 1,
    EPanicPreCond_ConstructL,
    EPanicPreCond_ExecuteLD
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkSingleItemFetchDlg");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

CPbkFieldArray* CreateFieldArrayLC
        (const CPbkContactItem& aItem, const CPbkFieldIdArray* aFieldIds)
    {
    CPbkFieldArray* fieldArray = new(ELeave) CPbkFieldArray;
    CleanupStack::PushL(fieldArray);
    const TInt count = aItem.CardFields().Count();
    for (TInt i=0; i < count; ++i)
        {
        const TPbkContactItemField& field = aItem.CardFields()[i];
        // Add only nonempty fields
        if (!field.IsEmptyOrAllSpaces())
            {
            if (!aFieldIds)
                {
                // add all nonempty fields if no field type filter is given
                fieldArray->AppendL(field);
                }
            else
                {
                // Check that field's type matches with selectable types
                if (aFieldIds->Contains(field.FieldInfo().FieldId()))
                    {
                    fieldArray->AppendL(field);
                    }
                }
            }
        }
    return fieldArray;
    }

}  // namespace


// MODULE DATA STRUCTURES

/**
 * MPbkFetchDlgAccept implementation for CPbkSingleItemFetchDlg.
 */
NONSHARABLE_CLASS(CPbkSingleItemFetchDlg::TFetchCallback) : 
        public MPbkFetchDlgAccept
    {
    public: // constructor
        inline TFetchCallback(CPbkSingleItemFetchDlg& aFetchDlg) 
            : iFetchDlg(aFetchDlg)
            {
            };

    public: // from MPbkFetchEntryDlgAccept
        /**
         * called by CPbkFetchEntryDlg when selection is made. 
         * @param aId   The focused contact entry
         * @param aMarkedEntries    The marked contact entries
         * @return          ETrue to close CPbkFetchEntryDlg, 
         *                  EFalse to keep open.
         */
        TPbkFetchAccepted PbkFetchAcceptedL
            (TContactItemId aId, CContactIdArray* aMarkedEntries);

    private:
        /// Ref: to parent
        CPbkSingleItemFetchDlg& iFetchDlg;
    };


// ================= MEMBER FUNCTIONS =======================

// CPbkSingleItemFetchDlg::TFetchCallback
MPbkFetchDlgAccept::TPbkFetchAccepted CPbkSingleItemFetchDlg::TFetchCallback::PbkFetchAcceptedL
        (TContactItemId aId, CContactIdArray* /*aMarkedEntries*/)
    {
    if (iFetchDlg.EntryFetchAcceptedL(aId))
        {
        return KFetchYes;
        }
    return KFetchNo;
    }

// CPbkSingleItemFetchDlg::TParams
EXPORT_C CPbkSingleItemFetchDlg::TParams::TParams() :
    iContactView(NULL), 
    iFieldIdArray(NULL), 
    iContactItem(NULL),
    iContactItemField(NULL),
    iCbaId(0)
    {
    }

EXPORT_C CPbkSingleItemFetchDlg::TParams::operator TCleanupItem()
    {
    return TCleanupItem(Cleanup,this);
    }

void CPbkSingleItemFetchDlg::TParams::Cleanup(TAny* aSelf)
    {
    TParams* self = static_cast<TParams*>(aSelf);
    delete self->iContactItem;
    self->iContactItem = NULL;
    self->iContactItemField = NULL;
    }

// CPbkSingleItemFetchDlg
inline CPbkSingleItemFetchDlg::CPbkSingleItemFetchDlg
        (TParams& aParams, 
        CPbkContactEngine& aEngine) : 
    iParams(aParams),
    iContactView(aParams.iContactView),
    iPbkEngine(aEngine)
    {
    // PostCond
    __ASSERT_DEBUG(!iFetchDlg && !iDestroyedPtr, Panic(EPanicPostCond_Constructor));
    }

inline void CPbkSingleItemFetchDlg::ConstructL()
    {
    // PreCond
    __ASSERT_DEBUG(!iFetchDlg && !iDestroyedPtr, Panic(EPanicPreCond_ConstructL));

    // Set up contact array if needed
    if (!iContactView)
        {
        iContactView = &iPbkEngine.AllContactsView();
        }
    }

EXPORT_C CPbkSingleItemFetchDlg* CPbkSingleItemFetchDlg::NewL
        (TParams& aParams, CPbkContactEngine& aEngine)
    {
    CPbkSingleItemFetchDlg* self = new(ELeave) CPbkSingleItemFetchDlg(aParams, aEngine);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();  // self
    return self;
    }

EXPORT_C void CPbkSingleItemFetchDlg::SetMopParent(MObjectProvider* aParent)
    {
    iObjectProvider = aParent;
    }

EXPORT_C TInt CPbkSingleItemFetchDlg::ExecuteLD()
    {
    // PreCond
    __ASSERT_DEBUG(iContactView, Panic(EPanicPreCond_ExecuteLD));

    // "D" function semantics
    CleanupStack::PushL(this);
    TBool thisDestroyed = EFalse;
    // Destructor sets thisDestroyed to ETrue
    iDestroyedPtr = &thisDestroyed;

    // Convert aParams for CPbkFetchDlg
    CPbkFetchDlg::TParams params;
    params.iResId = R_PBK_SINGLE_ENTRY_FETCH_DLG;
    params.iFlags = CPbkFetchDlg::FETCH_FOCUSED;
    params.iContactView = iContactView;
    params.iMarkedEntries = NULL;
    TFetchCallback callback(*this);
    params.iAcceptCallback = &callback;
    params.iCbaId = iParams.iCbaId;

    iFetchDlg = CPbkFetchDlg::NewL(params, iPbkEngine);
    iFetchDlg->ResetWhenDestroyed(&iFetchDlg);
    iFetchDlg->SetMopParent(iObjectProvider);
    TInt result = iFetchDlg->ExecuteLD();
    
    if (thisDestroyed)
        {
        // this object was destroyed
        result = 0;
        CleanupStack::Pop(this);
        }
    else
        {
        if (result)
            {
            // Switch ownership of result objects to iParams
            iParams.iContactItem = iContactItem;
            iContactItem = NULL;
            iParams.iContactItemField = iContactItemField;
            iContactItemField = NULL;
            }
        CleanupStack::PopAndDestroy(this);
        }

    return result;
    }

TBool CPbkSingleItemFetchDlg::EntryFetchAcceptedL
        (TContactItemId aCid)
    {
    TBool thisDestroyed = EFalse;
    TBool* prevDestroyedPtr = iDestroyedPtr;
    iDestroyedPtr = &thisDestroyed;

    CPbkContactItem* item = iPbkEngine.ReadContactLC(aCid);

    // create array of fields    
    CPbkFieldArray* fieldArray = 
        CreateFieldArrayLC(*item, iParams.iFieldIdArray);

    TPbkContactItemField* field = NULL;

    if ( fieldArray->Count() != 1 )
        {
        HBufC* heading = StringLoader::LoadLC(R_QTN_PHOB_QTL_SELECT_ONE_ITEM);

        iFieldDlg = new(ELeave) CPbkSelectFieldDlg;
        iFieldDlg->ResetWhenDestroyed(&iFieldDlg);
        TInt cbaResourceId( R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT );
        if ( fieldArray->Count() == 0 )
            {
            // If field selection is empty, only back soft button
            // is available
            cbaResourceId = R_AVKON_SOFTKEYS_BACK;
            }
        field = iFieldDlg->ExecuteLD(*fieldArray, 
                cbaResourceId, *heading);
        CleanupStack::PopAndDestroy(heading);
        }
    else
        {
        // Only one field in the array, grab it!
        field = &fieldArray->At(0);
        }
    
    if (thisDestroyed)
        {
        // This must be enough to ensure that rest of the code in this 
        // function doesn't access member variables, this object has been 
        // destroyed, remember!
        if (prevDestroyedPtr) *prevDestroyedPtr = ETrue;
        field = NULL;
        }
    else
        {
        iDestroyedPtr = prevDestroyedPtr;
        }

    if (field)
        {
        const TInt index = item->FindFieldIndex(*field);
        TPbkContactItemField& itemField = item->CardFields()[index];
        CleanupStack::PopAndDestroy(fieldArray);
        CleanupStack::Pop(item);
        // Take ownership of item
        iContactItem = item;
        iContactItemField = &itemField;
        }
    else
        {
        CleanupStack::PopAndDestroy(2,item);
        }

    return (field!=NULL); 
    }

CPbkSingleItemFetchDlg::~CPbkSingleItemFetchDlg()
    {
    if (iDestroyedPtr) 
        {
        *iDestroyedPtr = ETrue;
        }

    // Close the dialogs
    delete iFieldDlg;
    delete iFetchDlg;

    // delete return values in case of cancel
    delete iContactItem;
    }


//  End of File  
