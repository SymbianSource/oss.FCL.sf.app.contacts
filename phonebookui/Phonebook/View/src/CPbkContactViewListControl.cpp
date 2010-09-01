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
*       Provides methods for phonebook contact view list control.
*
*/


// INCLUDE FILES
#include <CPbkContactViewListControl.h>  // This class
#include <barsread.h>    // TResourceReader
#include <eikclbd.h>     // CColumnListBoxData
#include <aknsfld.h>     // CAknSearchField
#include <aknlists.h>    // AknListBoxUtils
#include <AknsDrawUtils.h>
#include <featmgr.h>
#include <bldvariant.hrh>
#include <PbkView.rsg>
#include <AknDef.h>      // KEikDynamicLayoutVariantSwitch
#include <AknUtils.h>


// PbkView classes
#include <PbkView.hrh>
#include "CPbkViewState.h"
#include "CPbkThumbnailPopup.h"
#include <CPbkIconArray.h>
#include <MPbkContactViewListControlObserver.h>
#include "PbkFindPrimitivesFactory.h"
#include "PbkContactViewListModelFactory.h"
#include "MPbkFetchDlgSelection.h"

// PbkEng classes
#include <CPbkContactEngine.h>
#include <CPbkConstants.h>
#include <CPbkContactFindView.h>

// PbkExt classes
#include <CPbkExtGlobals.h>
#include <MPbkViewExtension.h>
#include <MPbkContactUiControlExtension.h>
#include <MPbkExtensionFactory.h>

// Phonebook common include files
#include <PbkDebug.h>        // Phonebook debugging support


// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS

/// Listbox model's entry cache size
const TInt KCacheSize = 32;

const TUint16 KZwsChar = 0x200b;
const TUint16 KZwnjChar = 0x200c;

enum TPanicCode
    {
    EPanicPostCond_Constructor = 1,
    EPanicPreCond_ConstructFromResourceL,
    EPanicInvalidListBoxType_ConstructFromResourceL,
    EPanicPostCond_ConstructFromResource,
    EPanicPreCond_ConstructL,
    EPanicPostCond_ConstructL,
    EPanicListBoxNull,
    EPanicLogic_ComponentControl,
    EPanicPostCond_EnableFindBoxL,
    EPanicPostCond_DisableFindBoxL,
    EPanicLogic_FindTextL,
    EPanicPre_SetControlExtensionL
    };


// MODULE DATA STRUCTURES

NONSHARABLE_CLASS(TCleanupEnableListBoxViewRedraw)
    {
    public:  // Interface
        inline TCleanupEnableListBoxViewRedraw(CListBoxView& aListBoxView)
            : iCleanupItem(CleanupOp,&aListBoxView)
            {
            }

        inline operator TCleanupItem() const
            {
            return iCleanupItem;
            }

    private:  // Implementation
        static void CleanupOp(TAny* aPtr);

    private:  // Data
        TCleanupItem iCleanupItem;
    };

// ==================== LOCAL FUNCTIONS ====================

/**
 * Returns index of aValue in aArray or KErrNotFound.
 */
template<class T>
TInt Find(const CArrayFix<T>& aArray, const T& aValue)
    {
    const TInt count = aArray.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (aArray[i] == aValue)
            {
            return i;
            }
        }
    return KErrNotFound;
    }

/**
 * Returns CPbkIconArray icon array from aListBox.
 */
inline CPbkIconArray* IconArray(CEikColumnListBox& aListBox)
    {
    return static_cast<CPbkIconArray*>
        (aListBox.ItemDrawer()->ColumnData()->IconArray());
    }

/**
 * Sets aIconArray as aListBox'es icon array.
 */
inline void SetIconArray
        (CEikColumnListBox& aListBox, CPbkIconArray* aIconArray)
    {
    aListBox.ItemDrawer()->ColumnData()->SetIconArray(aIconArray);
    }

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkContactViewListControl");
    User::Panic(KPanicText, aReason);
    }

/**
 * Returns ETrue, if a character is to be included in the search string
 */
inline TBool IsSearchableChar( const TChar aChar )
    {
    switch (aChar)
        {
        case KZwsChar:  // FALLTHROUGH
        case KZwnjChar:
            {
            return EFalse;
            }
        }
    return ETrue;
    }

// MACROS

/// Define this to print find performance data to debug output
#ifdef PBK_BENCHMARK_FIND
  #pragma message("Warning: PBK_BENCHMARK_FIND is set")
#endif

// MODULE DATA STRUCTURES

#ifdef PBK_BENCHMARK_FIND

/// Benchmarking helper class.
template<class NumT>
NONSHARABLE_CLASS(TRunningTimes)
    {
    public:  // Interface
        /**
         * Constructor.
         */
        inline TRunningTimes()
            {
            Reset();
            }
        /**
         * () operator.
         */
        inline NumT operator()() const
            {
            return iTotal;
            }

        inline void Start()
            {
            iStart.UniversalTime();
            }
        inline void Stop()
            {
            iStop.UniversalTime();
            const TInt timeDiff =
                I64LOW(iStop.MicroSecondsFrom(iStart).Int64()) / 1000;
            iTotal += timeDiff;
            iCount++;
            }

        inline TInt Count() const
            {
            return iCount;
            }

        /**
         * Returns total time
         */
        inline NumT Total() const
            {
            return iTotal;
            }

        /**
         * Reset.
         */
        inline void Reset()
            {
            iTotal=0; iCount=0;
            }

    private:  // Data
        TTime iStart;
        TTime iStop;
        /// Own: count
        TInt iCount;
        /// Own: total time
        NumT iTotal;
    };

#endif  // PBK_BENCHMARK_FIND

}  // namespace


// ================= MEMBER FUNCTIONS =======================

//
// CPbkContactViewListControl
//
inline CEikColumnListBox& CPbkContactViewListControl::ListBox() const
    {
    __ASSERT_DEBUG(iListBox, Panic(EPanicListBoxNull));
    return(*iListBox);
    }

inline MPbkContactViewListModel& CPbkContactViewListControl::Model() const
    {
    return(*static_cast<MPbkContactViewListModel*>
        (iListBox->Model()->ItemTextArray()));
    }

inline TBool CPbkContactViewListControl::PostCond_Constructor()
    {
    return
        (!iListBox && !iFindBox && !iFindTextBuf && !iBaseView && !iView &&
        !iResourceData.iUnnamedText && !iResourceData.iFindEmptyText &&
        !iMarkedItemsArray && !iThumbnailPopup);
    }

inline TBool CPbkContactViewListControl::PostCond_ConstructFromResource()
    {
    return
        (iListBox && !iFindBox && !iFindTextBuf &&
        iResourceData.iUnnamedText && !iMarkedItemsArray);
    }

EXPORT_C CPbkContactViewListControl* CPbkContactViewListControl::NewL
        (CPbkContactEngine& aEngine,
        CContactViewBase& aView,
        TInt aResourceId,
        const CCoeControl* aParent)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkContactViewListControl::NewL()"));
    CPbkContactViewListControl* self = new(ELeave) CPbkContactViewListControl();
    CleanupStack::PushL(self);
    self->ConstructL(aEngine, aView, aResourceId, aParent);
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C CPbkContactViewListControl::CPbkContactViewListControl() :
    iLastFocusId(KNullContactId),
    iChangedIndexes(1 /*allocation granularity*/)
    {
    // CBase::operator new(TLeave) resets other members
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
        ("CPbkContactViewListControl::CPbkContactViewListControl(0x%x)"),
            this);
    __ASSERT_DEBUG(PostCond_Constructor(), Panic(EPanicPostCond_Constructor));
    }

void CPbkContactViewListControl::ConstructFromResourceL
        (TResourceReader& aReader)
    {
    __ASSERT_DEBUG(PostCond_Constructor(),
        Panic(EPanicPreCond_ConstructFromResourceL));
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
        ("CPbkContactViewListControl::ConstructFromResourceL(0x%x)"),this);

    // emptyText
    TPtrC emptyText = aReader.ReadTPtrC();
    // unnamedText
    iResourceData.iUnnamedText = aReader.ReadHBufCL();
    // flags
    iResourceData.iFlags = aReader.ReadUint32();

    // Empty text in find state
    TResourceReader rr;
    iEikonEnv->CreateResourceReaderLC(rr, R_AVKON_FIND_NO_MATCHES);
    iResourceData.iFindEmptyText = rr.ReadHBufCL();
    CleanupStack::PopAndDestroy(); // rr

    // listboxType
    TInt listBoxType = aReader.ReadInt16();
    iListBox = static_cast<CEikColumnListBox*>
        (EikControlFactory::CreateByTypeL(listBoxType).iControl);
    __ASSERT_ALWAYS(iListBox, Panic
        (EPanicInvalidListBoxType_ConstructFromResourceL));
    iListBox->SetContainerWindowL(*this);
    // listbox
    iListBox->ConstructFromResourceL(aReader);
    iListBox->View()->SetListEmptyTextL(emptyText);
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL
        (CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
    iListBox->SetObserver(this);
    iListBox->MakeVisible(EFalse);

    // iconArray, use granularity of 4

    CPbkIconArray* iconArray = new(ELeave) CPbkIconArray(4);
    CleanupStack::PushL(iconArray);
    const TInt iconArrayRes = aReader.ReadInt32();
    iconArray->ConstructFromResourceL(iconArrayRes);

    // get extension factory for setting extension icons
    CPbkExtGlobals* extGlobal = CPbkExtGlobals::InstanceL();
    extGlobal->PushL();
    MPbkExtensionFactory& factory = extGlobal->FactoryL();
    factory.AddPbkFieldIconsL(NULL, iconArray);
    CleanupStack::PopAndDestroy(extGlobal);

    CleanupStack::Pop(iconArray);

    SetIconArray(*iListBox,iconArray);

    // emptyIconId
    iResourceData.iEmptyIconId =
        static_cast<TPbkIconId>(aReader.ReadInt16());
    // defaultIconId
    iResourceData.iDefaultIconId =
        static_cast<TPbkIconId>(aReader.ReadInt16());

    // Preallocate space for the one required integer
    iChangedIndexes.AppendL(-1);

    __ASSERT_DEBUG(PostCond_ConstructFromResource(),
        Panic(EPanicPostCond_ConstructFromResource));
    }

EXPORT_C void CPbkContactViewListControl::ConstructL
        (CPbkContactEngine& aEngine,
        CContactViewBase& aView)
    {
    // Check that ConstructFromResourceL is executed
    __ASSERT_DEBUG(PostCond_ConstructFromResource() && !iBaseView && !iView,
        Panic(EPanicPreCond_ConstructL));
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
        ("CPbkContactViewListControl::ConstructL(0x%x)"),this);

    iFindPrimitives = PbkFindPrimitivesFactory::CreateL(
        aEngine.ContactNameFormat());
    iBaseView = &aView;
    iView = CPbkContactFindView::NewL(
        aEngine, *iBaseView, *this, *iFindPrimitives);

    // Set aParams for MPbkContactViewListModel
    PbkContactViewListModelFactory::TParams params;
    params.iEngine = &aEngine;
    params.iView = iView;
    params.iCacheSize = KCacheSize;
    params.iIconArray = IconArray(*iListBox);
    params.iEmptyId = iResourceData.iEmptyIconId;
    params.iDefaultId = iResourceData.iDefaultIconId;

    // Listbox model
    MPbkContactViewListModel* model = PbkContactViewListModelFactory::
                                            CreateL(params);
    model->SetUnnamedText(iResourceData.iUnnamedText);
    iListBox->Model()->SetItemTextArray(model);
    iListBox->Model()->SetOwnershipType(ELbmOwnsItemArray);

    // thumbnail handler
    if (iResourceData.iFlags & KPbkContactViewListControlUpdateContextPane)
        {
        iThumbnailPopup = CPbkThumbnailPopup::NewL(aEngine);
        }

    // Sets up TLS, must be done before FeatureManager is used.
    FeatureManager::InitializeLibL();
    // iEngine != NULL means that Featuremanager has been initialized
    iEngine = &aEngine;

    __ASSERT_DEBUG(PostCond_ConstructFromResource() && iBaseView && iView &&
        iThumbnailPopup || !(iResourceData.iFlags &
        KPbkContactViewListControlUpdateContextPane),
        Panic(EPanicPostCond_ConstructL));
    }

CPbkContactViewListControl::~CPbkContactViewListControl()
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
        ("CPbkContactViewListControl::~CPbkContactViewListControl(0x%x)"),
        this);

    // This should be done before listbox delete
    // (which deletes dummy control extension)
    Release(iControlExtension);

    // Fetch dialog pages might delay calling ConstructL and thus Feature
    // manager might not be initialized if destructing early.
    if (iEngine)
        {
        // iEngine != NULL means that Featuremanager has been initialized
        FeatureManager::UnInitializeLib();
        }

    delete iOriginalEmptyText;
    iObservers.Reset();
    delete iFindTextBuf;
    if (iView)
        {
        iView->Close(*this);
        }
    delete iFindPrimitives;
    delete iThumbnailPopup;
    delete iMarkedItemsArray;
    delete iFindBox;
    delete iListBox;
    delete iResourceData.iFindEmptyText;
    delete iResourceData.iUnnamedText;
    }

EXPORT_C TContactItemId CPbkContactViewListControl::ContactIdAtL
        (TInt aIndex) const
    {
    return iView->AtL(aIndex);
    }

EXPORT_C TInt CPbkContactViewListControl::FindContactIdL
        (TContactItemId aId) const
    {
    return iView->FindL(aId);
    }

EXPORT_C TInt CPbkContactViewListControl::CurrentItemIndex() const
    {
    return ListBox().CurrentItemIndex();
    }

EXPORT_C void CPbkContactViewListControl::SetCurrentItemIndex(TInt aIndex)
    {
    if (ListBox().CurrentItemIndex() != aIndex)
        {
        ListBox().SetCurrentItemIndex(aIndex);
        HandleFocusChange();
        }
    }

EXPORT_C void CPbkContactViewListControl::SetCurrentItemIndexAndDraw
        (TInt aIndex)
    {
    if (ListBox().CurrentItemIndex() != aIndex)
        {
        ListBox().SetCurrentItemIndexAndDraw(aIndex);
        HandleFocusChange();
        }
    }

EXPORT_C void CPbkContactViewListControl::HandleMarkableListProcessCommandL
        (TInt aCommandId)
    {
    AknSelectionService::HandleMarkableListProcessCommandL
        (aCommandId, &ListBox());
    }

EXPORT_C void CPbkContactViewListControl::HandleMarkableListDynInitMenuPane
        (TInt aResourceId,
        CEikMenuPane *aMenu)
    {
    AknSelectionService::HandleMarkableListDynInitMenuPane
        (aResourceId, aMenu, &ListBox());
    }

EXPORT_C void
    CPbkContactViewListControl::HandleMarkableListUpdateAfterCommandExecution()
    {
    AknSelectionService::HandleMarkableListUpdateAfterCommandExecution
        (&ListBox());
    }

EXPORT_C const TDesC& CPbkContactViewListControl::FindTextL() const
    {
    // Initial minimum size for the find text buffer
    const TInt KInitialBufSize = 8;

    if (iFindBox)
        {
        const TInt findBoxTextLength = iFindBox->TextLength();
        if (findBoxTextLength > 0)
            {
            TInt bufCapacity = 0;
            if (iFindTextBuf)
                {
                bufCapacity = iFindTextBuf->Des().MaxLength();
                }
            if (bufCapacity < findBoxTextLength)
                {
                // Allocate a new buffer of at least KInitialBufSize
                // characters or twice as large as the previous one
                const TInt newBufSize = Max(Max(KInitialBufSize,2*bufCapacity),
                    findBoxTextLength);
                HBufC* newBuf = HBufC::NewL(newBufSize);
                delete iFindTextBuf;
                iFindTextBuf = newBuf;
                }
            TPtr bufPtr = iFindTextBuf->Des();
            __ASSERT_DEBUG
                (bufPtr.MaxLength()>=KInitialBufSize && bufPtr.MaxLength()>=findBoxTextLength,
                Panic(EPanicLogic_FindTextL));
            iFindBox->GetSearchText(bufPtr);
            // Strip from search string all unwanted characters that e.g. Hindi input places there
            for( TInt i = bufPtr.Length()-1; i >= 0; --i )
                {
                if (!IsSearchableChar(bufPtr[i]))
                    {
                    bufPtr.Delete( i, 1 );
                    }
                }
            return(*iFindTextBuf);
            }
        }
    return KNullDesC;
    }

EXPORT_C void CPbkContactViewListControl::ResetFindL()
    {
    if (iFindBox && iFindBox->TextLength() > 0)
        {
        iFindBox->ResetL();
        UpdateFindResultL();
        iFindBox->DrawDeferred();
        }
    }

EXPORT_C TBool CPbkContactViewListControl::ItemMarked(TInt aIndex) const
    {
    const CListBoxView::CSelectionIndexArray* selections =
        ListBox().View()->SelectionIndexes();
    return (selections ? Find(*selections,aIndex)!=KErrNotFound : EFalse);
    }

EXPORT_C TInt CPbkContactViewListControl::NextUnmarkedIndexFromFocus() const
    {
    const TInt focus = ListBox().CurrentItemIndex();
    TInt index = focus;
    const CListBoxView::CSelectionIndexArray* selections =
        ListBox().SelectionIndexes();
    if (selections && selections->Count() > 0)
        {
        const TInt count = iListBox->Model()->NumberOfItems();
        for (index = focus; index < count; ++index)
            {
            if (Find(*selections,index) == KErrNotFound)
                {
                return index;
                }
            }
        for (index = focus; index >= 0; --index)
            {
            if (Find(*selections,index) == KErrNotFound)
                {
                return index;
                }
            }
        }
    return index;
    }

EXPORT_C void CPbkContactViewListControl::MarkItemL
        (TContactItemId aContactId,
        TBool aMark)
    {
    const TInt index = FindContactIdL(aContactId);
    if (index >= 0)
        {
        if (aMark)
            {
            ListBox().View()->SelectItemL(index);
            }
        else
            {
            ListBox().View()->DeselectItem(index);
            }
        }
    }

EXPORT_C void CPbkContactViewListControl::ClearMarks()
    {
    if (ItemsMarked())
        {
        CListBoxView& listBoxView = *ListBox().View();
        listBoxView.SetDisableRedraw(ETrue);
        ListBox().ClearSelection();
        listBoxView.SetDisableRedraw(EFalse);
        Redraw();
        }
    }

EXPORT_C CPbkViewState* CPbkContactViewListControl::GetStateL
        (TBool aSaveMarks/*=ETrue*/) const
    {
    CPbkViewState* state = GetStateLC(aSaveMarks);
    CleanupStack::Pop();  // state
    return state;
    }

EXPORT_C CPbkViewState* CPbkContactViewListControl::GetStateLC
        (TBool aSaveMarks/*=ETrue*/) const
    {
    // Create a state object
    CPbkViewState* state = CPbkViewState::NewLC();

    // Init the state object
    if (NumberOfItems() > 0)
        {
        TInt index;
        if ((index = ListBox().TopItemIndex()) >= 0)
            {
            state->SetTopContactId(ContactIdAtL(index));
            }
        if ((index = ListBox().CurrentItemIndex()) >= 0)
            {
            state->SetFocusedContactId(ContactIdAtL(index));
            }
        if (aSaveMarks && ItemsMarked())
            {
            state->SetMarkedContactIds(CContactIdArray::NewL(&MarkedItemsL()));
            }
        }

    // Return the state object
    return state;
    }

EXPORT_C void CPbkContactViewListControl::RestoreStateL(const CPbkViewState* aState)
    {
    PBK_DEBUG_PRINT(
        PBK_DEBUG_STRING("CPbkContactViewListControl::RestoreStateL(0x%x,0x%x)"),
        this, aState);

    if (!aState || !iStateFlags.IsSet(EReady))
        {
        return;
        }

    CEikColumnListBox& listBox = ListBox();
    TBool redraw = EFalse;
    if (aState->Flags() & CPbkViewState::EInitialized)
        {
        // Find box is recreated because default input language needs to
        // be reset here if Always on feature is enabled for Phonebook
        delete iFindBox;
        iFindBox=NULL;

        // Do not enable the find box if there does not exist any contacts
        if (iBaseView->CountL() > 0)
            {
            EnableFindBoxL();
            }

        listBox.Reset();
        redraw = ETrue;
        }
    else
        {
        if (iBaseView->CountL() > 0)
            {
            if (aState->Flags() & CPbkViewState::EFocusFirst)
                {
                listBox.SetTopItemIndex(0);
                }
            else if (aState->Flags() & CPbkViewState::EFocusLast)
                {
                listBox.SetCurrentItemIndex(listBox.Model()->NumberOfItems());
                }
            else
                {
                // Restore top item
                if (aState->TopContactId() != KNullContactId)
                    {
                    const TInt index = FindContactIdL(aState->TopContactId());
                    if (index >= 0)
                        {
                        const TInt prevIndex = listBox.TopItemIndex();
                        listBox.SetTopItemIndex(index);
                        FixTopItemIndex();
                        if (index != prevIndex)
                            {
                            redraw = ETrue;
                            }
                        }
                    }

                // Restore focus
                if (aState->FocusedContactId() != KNullContactId)
                    {
                    const TInt index = FindContactIdL(aState->FocusedContactId());
                    if (index >= 0)
                        {
                        const TInt prevIndex = listBox.CurrentItemIndex();
                        listBox.SetCurrentItemIndex(index);
                        if (index != prevIndex)
                            {
                            redraw = ETrue;
                            }
                        }
                    }

                }
            // Restore selections
            const CContactIdArray* markedContactIds = aState->MarkedContactIds();
            if (RestoreMarkedItemsL(markedContactIds))
                {
                redraw = ETrue;
                }
            }
        }

    if (redraw)
        {
        Redraw();
        iListBox->UpdateScrollBarsL();
        HandleFocusChange();
        }
    }

void CPbkContactViewListControl::ShowThumbnail(TContactItemId aContactId)
    {
    if (!iThumbnailPopup && iEngine)
        {
        TRAP_IGNORE(iThumbnailPopup = CPbkThumbnailPopup::NewL( *iEngine ));
        }

    if (iThumbnailPopup && aContactId!=KNullContactId  && IsReadyToDraw() &&
        !iStateFlags.IsSet(EBlank))
        {
        iThumbnailPopup->Load(aContactId, iListBox);
        }
    else if (iThumbnailPopup)
        {
        iThumbnailPopup->CancelLoading();
        }
    }

EXPORT_C void CPbkContactViewListControl::ShowThumbnailL()
    {
    if (!iThumbnailPopup && iEngine)
        {
        iThumbnailPopup = CPbkThumbnailPopup::NewL( *iEngine );
        }

    if (iLastFocusId!=KNullContactId  && IsReadyToDraw() &&
        !iStateFlags.IsSet(EBlank))
        {
        iThumbnailPopup->Load(iLastFocusId, iListBox);
        }
    else
        {
        iThumbnailPopup->CancelLoading();
        }
    }

EXPORT_C void CPbkContactViewListControl::HideThumbnail()
    {
    if (iThumbnailPopup)
        {
        iThumbnailPopup->CancelLoading();
        }
    }

EXPORT_C void CPbkContactViewListControl::AddObserverL
        (MPbkContactViewListControlObserver& aObserver)
    {
    User::LeaveIfError(iObservers.Append(&aObserver));
    }

EXPORT_C void CPbkContactViewListControl::RemoveObserver
        (MPbkContactViewListControlObserver& aObserver)
    {
    const TInt index = iObservers.Find(&aObserver);
    if (index >= 0)
        {
        iObservers.Remove(index);
        }
    }

EXPORT_C TBool CPbkContactViewListControl::IsReady() const
    {
    return (iStateFlags.IsSet(EReady));
    }

void CPbkContactViewListControl::SetDisableRedraw(TBool aDisableRedraw)
    {
    ListBox().View()->SetDisableRedraw(aDisableRedraw);
    }

void CPbkContactViewListControl::DisableRedrawEnablePushL()
    {
    CListBoxView& listBoxView = *ListBox().View();
    listBoxView.SetDisableRedraw(ETrue);
    CleanupStack::PushL(TCleanupEnableListBoxViewRedraw(listBoxView));
    }

EXPORT_C void CPbkContactViewListControl::SetBlank(TBool aBlank)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
        ("CPbkContactViewListControl::SetBlank(0x%x,%d)"),this,aBlank);

    if ((iStateFlags.IsSet(EBlank) && aBlank) ||
        (!iStateFlags.IsSet(EBlank) && !aBlank))
        {
        // No change in state
        return;
        }

    // Set the EBlank flag
    iStateFlags.Assign(EBlank,aBlank);

    // Hide/unhide component controls
    MakeComponentsVisible(!aBlank);
    Redraw();
    }

EXPORT_C void CPbkContactViewListControl::SetEntryLoader
        (MPbkContactEntryLoader& aContactEntryLoader)
    {
    Model().SetEntryLoader(aContactEntryLoader);
    }


void CPbkContactViewListControl::SetSelectionAccepter(MPbkFetchDlgSelection* aAccepter)
    {
    iSelectionAccepter = aAccepter;
    }

TInt CPbkContactViewListControl::NumberOfItems() const
    {
    return ListBox().Model()->NumberOfItems();
    }

TBool CPbkContactViewListControl::ItemsMarked() const
    {
    const CListBoxView::CSelectionIndexArray* selections =
        ListBox().View()->SelectionIndexes();
    return (selections && selections->Count()>0);
    }

const CContactIdArray& CPbkContactViewListControl::MarkedItemsL() const
    {
    if (!iMarkedItemsArray)
        {
        iMarkedItemsArray = CContactIdArray::NewL();
        }

    // Remove old contents of array (use Remove instead of Reset to keep the
    // array buffer)
    iMarkedItemsArray->Remove(0,iMarkedItemsArray->Count());

    // Get the list boxes marked items index array
    const CListBoxView::CSelectionIndexArray* selArray =
        ListBox().SelectionIndexes();

    if (selArray && selArray->Count() > 0)
        {
        // Initialize the marked contact id array using selArray
        CContactViewBase::TVirtualFunction1Params params
            (selArray, iMarkedItemsArray);

        iView->CContactViewBase_Reserved_1
            (CContactViewBase::ECContactViewBaseVirtualFunction1, &params);
        }
    else
        {
        // No items marked, add the focused contact to the array
        iMarkedItemsArray->AddL(FocusedContactIdL());
        }

    return *iMarkedItemsArray;
    }

TContactItemId CPbkContactViewListControl::FocusedContactIdL() const
    {
    TContactItemId retId(KNullContactId);

    const TInt focusIndex = ListBox().CurrentItemIndex();
    if (focusIndex >= 0)
        {
        retId = iView->AtL(focusIndex);
        }
    return retId;
    }

const TPbkContactItemField* CPbkContactViewListControl::FocusedField() const
    {
    // This control does not support field level focus
    return NULL;
    }

MObjectProvider* CPbkContactViewListControl::ObjectProvider()
    {
    return this;
    }

TKeyResponse CPbkContactViewListControl::OfferKeyEventL
        (const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    if (!iStateFlags.IsSet(EReady) || iStateFlags.IsSet(EBlank))
        {
        // Don't handle any keys when not ready or blanked
        return EKeyWasNotConsumed;
        }

    TKeyResponse result = EKeyWasNotConsumed;

    if (iFindBox)
        {
        // Find box is active, offer key first to it
        result = iFindBox->OfferKeyEventL(aKeyEvent, aType);
        }

    if (result == EKeyWasNotConsumed && iListBox)
        {
        // Find box didn't consume the event -> offer it to the list box
        const TInt focusIndex = iListBox->CurrentItemIndex();
        const TBool markedBefore = ItemMarked(focusIndex);

        // If selection key was pressed and the focused contact was earlier
        // unselected, then first check from selection accepter if this item
        // can be selected
        // Also do the acception-check if there are no selected items and
        // OK-softkey was pressed (causing focused item to be selected and
        // closing the dialog
        TBool checkSelection = EFalse;
        const CListBoxView::CSelectionIndexArray* selArray =
                ListBox().SelectionIndexes();
        
        
        const TInt selectionKeyPressed(aKeyEvent.iCode == EKeyDevice3);
        
        // AVKON may create EKeyApplicationF key event 
        // when list item is selected.
        const TInt avkonSelectionKeyNotification(
                        aKeyEvent.iCode == EKeyApplicationF);
                        
        const TBool noSelectionMade(!selArray||selArray->Count() == 0 );
        
        const TBool validNewFocusedItem(
                        !markedBefore && focusIndex >= KErrNone);

        if ((selectionKeyPressed || (avkonSelectionKeyNotification && noSelectionMade))
            && validNewFocusedItem)
            {
            checkSelection = ETrue;
            }
        
        const CListBoxView::CSelectionIndexArray* selections =
            ListBox().SelectionIndexes();
        TInt selCount = selections ? selections->Count() : 0;
        
        if (!checkSelection || !iSelectionAccepter ||
            focusIndex != KErrNotFound &&
            iSelectionAccepter->ContactSelectionAcceptedL(
                ContactIdAtL(focusIndex), selCount))
            {
            result = iListBox->OfferKeyEventL(aKeyEvent, aType);

            if (result == EKeyWasConsumed)
                {
                const TBool markedAfter = ItemMarked(focusIndex);
                if (markedAfter != markedBefore)
                    {
                    TPbkContactViewListControlEvent
                        event(TPbkContactViewListControlEvent::EContactSelected);
                    event.iInt = focusIndex;
                    event.iContactId = ContactIdAtL(focusIndex);
                    if (!markedAfter)
                        {
                        event.iEventType =
                            TPbkContactViewListControlEvent::EContactUnselected;
                        }
                    SendEventToObserversL(event);
                    }                
                }
            }
        else
            {
            result = EKeyWasConsumed;
            }
        }
    return result;
    }

void CPbkContactViewListControl::HandlePointerEventL(
        const TPointerEvent& aPointerEvent )
    {
    if ( AknLayoutUtils::PenEnabled() )
        {
        if ( iFindBox )
            {
            iFindBox->HandlePointerEventL( aPointerEvent );
            }

        if ( iListBox )
            {
            switch ( aPointerEvent.iType )
                {
                case TPointerEvent::EButton1Down:
                    {
                    iPrevIndex = iListBox->CurrentItemIndex();
                    iListBox->HandlePointerEventL( aPointerEvent );
                    break;
                    }
                case TPointerEvent::EButton1Up:
                    {
                    TInt focusIndex;
                    TBool focusableContactPointed =
                        iListBox->View()->XYPosToItemIndex(
                            aPointerEvent.iPosition, focusIndex );
                    if (!focusableContactPointed || focusIndex < 0)
                        {
                        // Nothing special to do when tapping empty space
                        iListBox->HandlePointerEventL( aPointerEvent );
                        }
                    else
                        {
                        // Send contact tap events                        
                        TPbkContactViewListControlEvent event(
                            TPbkContactViewListControlEvent::EContactTapped );
                        event.iInt = focusIndex;
                        event.iContactId = ContactIdAtL( focusIndex );
                        if ( iPrevIndex == iListBox->CurrentItemIndex() )
                            {
                            event.iEventType =
                                TPbkContactViewListControlEvent::EContactDoubleTapped;
                            }
                        SendEventToObserversL( event );

                        // Do markings
                        const TBool markedBefore = ItemMarked(focusIndex);
                        const CListBoxView::CSelectionIndexArray* selections =
                            ListBox().SelectionIndexes();
                        TInt selCount = selections ? selections->Count() : 0;

                        if ( markedBefore ||         // can unmark always
                             !iSelectionAccepter ||  // no select restrictions
                             iSelectionAccepter->ContactSelectionAcceptedL(
                                ContactIdAtL(focusIndex), selCount) )
                            {
                            // Handle the pointer event to list box to fix the problem that items in multiple
                            // fetch dialog can't be select with touch screen.
                            iListBox->HandlePointerEventL( aPointerEvent );
                            
                            const TBool markedAfter = ItemMarked( focusIndex );
                            if ( markedAfter != markedBefore )
                                {
                                event.iEventType =
                                    TPbkContactViewListControlEvent::EContactSelected;
                                event.iInt = focusIndex;
                                event.iContactId = ContactIdAtL(focusIndex);
                                if (!markedAfter)
                                    {
                                    event.iEventType =
                                        TPbkContactViewListControlEvent::EContactUnselected;
                                    }
                                SendEventToObserversL(event);
                                }
                            }
                        }
                    break;
                    }
                default:
                    {
                    iListBox->HandlePointerEventL( aPointerEvent );
                    }
                }
            }
        }
    }

void CPbkContactViewListControl::MakeVisible(TBool aVisible)
    {
    PBK_DEBUG_PRINT(
        PBK_DEBUG_STRING("CPbkContactViewListControl::MakeVisible(0x%x,%d)"),
        this, aVisible);

    CCoeControl::MakeVisible(aVisible);
    MakeComponentsVisible(aVisible);
    }

void CPbkContactViewListControl::UpdateContact(TContactItemId aContactId)
    {
    TInt listBoxRow = -1;
    TRAPD(err, listBoxRow = FindContactIdL(aContactId) );
    if (err)
        {
        iEikonEnv->NotifyIdleErrorWhileRedrawing(err);
        }
    if (listBoxRow >= 0)
        {
        // if visible
        if (listBoxRow >= ListBox().TopItemIndex() &&
            listBoxRow <= ListBox().BottomItemIndex() )
            {
            ListBox().DrawItem(listBoxRow);
            }
        }
    }

TInt CPbkContactViewListControl::CountComponentControls() const
    {
    TInt controls = 0;
    if (iListBox)
        {
        ++controls;
        }
    if (iFindBox)
        {
        ++controls;
        }
    return controls;
    }

CCoeControl* CPbkContactViewListControl::ComponentControl(TInt aIndex) const
    {
    switch (aIndex)
        {
        case 0:
            {
            __ASSERT_DEBUG(iListBox, Panic(EPanicLogic_ComponentControl));
            return iListBox;
            }
        case 1:
            {
            __ASSERT_DEBUG(iFindBox, Panic(EPanicLogic_ComponentControl));
            return iFindBox;
            }
        default:
            {
            // Illegal state
            __ASSERT_DEBUG(EFalse, Panic(EPanicLogic_ComponentControl));
            return NULL;
            }
        }
    }

void CPbkContactViewListControl::FocusChanged(TDrawNow aDrawNow)
    {
    const TBool focused = IsFocused();
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
        ("CPbkContactViewListControl::FocusChanged(0x%x,%d),focused=%d"),
        this, aDrawNow, focused ? ETrue : EFalse);

    if (focused)
        {
        ShowThumbnail(iLastFocusId);
        }
    else
        {
        HideThumbnail();
        }

    const TInt count = CountComponentControls();
    for (TInt i=0; i < count; ++i)
        {
        CCoeControl* componentControl = ComponentControl(i);
        // Don't try to focus non-focusing controls
        if (!componentControl->IsNonFocusing())
            {
            componentControl->SetFocus(focused,aDrawNow);
            }
        }
    }

void CPbkContactViewListControl::SizeChanged()
    {
    const TRect rect(Rect());
    PBK_DEBUG_PRINT(
        PBK_DEBUG_STRING("CPbkContactViewListControl::SizeChanged(0x%x), rect=(%d,%d,%d,%d)"),
        this, rect.iTl.iX, rect.iTl.iY, rect.iBr.iX, rect.iBr.iY);

    if (iListBox && iFindBox)
        {
        AknLayoutUtils::LayoutControl(iListBox, rect,
            AKN_LAYOUT_WINDOW_list_gen_pane(1));
        AknLayoutUtils::LayoutControl(iFindBox, rect,
            AKN_LAYOUT_WINDOW_find_pane);
        if (iFindBox->IsVisible() && iListBox->IsVisible())
            {
            // The correct line position to use is 2, which corresponds
            // EABColumn in Avkon (not a public enumeration,
            // hence hard-coding used here)
            const TInt KSeparatorLinePos = 2;
            iFindBox->SetLinePos(KSeparatorLinePos);
            }
        }
    else if (iListBox)
        {
        AknLayoutUtils::LayoutControl(iListBox, rect,
            AKN_LAYOUT_WINDOW_list_gen_pane(0));
        }
    }

void CPbkContactViewListControl::Draw(const TRect& aRect) const
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING(
        "CPbkContactViewListControl::Draw(0x%x, TRect(%d,%d,%d,%d)), iStateFlags=0x%x"),
        this, aRect.iTl.iX, aRect.iTl.iY, aRect.iBr.iX, aRect.iBr.iY, iStateFlags.iFlags);

    if (!iStateFlags.IsSet(EReady) || iStateFlags.IsSet(EBlank))
        {
        // If control is not ready or blanked draw a blank background
        CWindowGc& gc = SystemGc();
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        MAknsControlContext* cc = AknsDrawUtils::ControlContext(iListBox);
        if ( !AknsDrawUtils::Background(skin, cc, iListBox, gc, aRect) )
            {
            // blank background if no skin present
            gc.SetPenStyle(CGraphicsContext::ENullPen);
            gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
            gc.DrawRect(aRect);
            }
        }
    }

void CPbkContactViewListControl::HandleControlEventL
        (CCoeControl* aControl,
        TCoeEvent aEventType)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
        ("CPbkContactViewListControl::HandleControlEventL(0x%x,0x%x,%d)"),
        this, aControl, aEventType);

    if (aEventType == EEventStateChanged)
        {
        if (aControl == iListBox)
            {
            HandleFocusChange();
            // Forward listbox state change events to this control's
            // observers
            ReportEventL(MCoeControlObserver::EEventStateChanged);
            }
        else if (aControl == iFindBox)
            {
            UpdateFindResultL();
            }
        }
    }

void CPbkContactViewListControl::HandleContactViewEvent
        (const CContactViewBase& aView,const TContactViewEvent& aEvent)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
        ("CPbkContactViewListControl::HandleContactViewEvent(0x%x,0x%x,%d)"),
        this, &aView, aEvent.iEventType);

    if (&aView == iView)
        {
        TRAPD(err, DoHandleContactViewEventL(aView,aEvent));
        if (err != KErrNone)
            {
            iEikonEnv->HandleError(err);
            }
        }
    }

/**
 * Called from HandleContactViewEvent().
 */
void CPbkContactViewListControl::DoHandleContactViewEventL
        (const CContactViewBase& /*aView*/,const TContactViewEvent& aEvent)
    {
    switch (aEvent.iEventType)
        {
        case TContactViewEvent::EReady:
            {
            iStateFlags.Set(EReady);
            MakeComponentsVisible(IsVisible());
            iListBox->Reset();
            UpdateFindBoxL();
            iListBox->UpdateScrollBarsL();
            Redraw();
            HandleFocusChange();
            SendEventToObserversL(TPbkContactViewListControlEvent::EReady);
            break;
            }

        case TContactViewEvent::ESortOrderChanged:
            {
            Model().FlushCache();
            Model().RefreshSortOrderL();
            iStateFlags.Set(EReady);
            MakeComponentsVisible(IsVisible());
            iListBox->Reset();
            UpdateFindBoxL();
            iListBox->UpdateScrollBarsL();
            Redraw();
            HandleFocusChange();
            // Send EReady event
            SendEventToObserversL(TPbkContactViewListControlEvent::EReady);
            break;
            }

        case TContactViewEvent::EItemAdded:
            {
            HandleItemAdditionL(aEvent.iInt);
            TPbkContactViewListControlEvent event
                (TPbkContactViewListControlEvent::EItemAdded);
            event.iInt = aEvent.iInt;
            event.iContactId = aEvent.iContactId;
            SendEventToObserversL(event);
            break;
            }

        case TContactViewEvent::EItemRemoved:
            {
            Model().PurgeEntry(aEvent.iContactId);
            HandleItemRemovalL(aEvent.iInt);
            TPbkContactViewListControlEvent event
                (TPbkContactViewListControlEvent::EItemRemoved);
            event.iInt = aEvent.iInt;
            event.iContactId = aEvent.iContactId;
            SendEventToObserversL(event);
            break;
            }

        case TContactViewEvent::EUnavailable:
            {
            iStateFlags.Clear(EReady);
            MakeComponentsVisible(EFalse);
            SendEventToObserversL
                (TPbkContactViewListControlEvent::EUnavailable);
            break;
            }

        case TContactViewEvent::ESortError:  //FALLTHROUGH
        case TContactViewEvent::EServerError:  //FALLTHROUGH
        case TContactViewEvent::EIndexingError:
            {
            iStateFlags.Clear(EReady);
            MakeComponentsVisible(EFalse);
            iEikonEnv->HandleError(aEvent.iInt);
            iListBox->Reset();
            DisableFindBoxL();
            TPbkContactViewListControlEvent event
                (TPbkContactViewListControlEvent::EUnavailable);
            event.iInt = aEvent.iInt;
            SendEventToObserversL(event);
            break;
            }

        default:
            {
            break;
            }
        }
    }

void CPbkContactViewListControl::SendEventToObserversL
        (const TPbkContactViewListControlEvent& aEvent)
    {
    // Loop backwards in case some observer destroys itself in the
    // event handler
    for (TInt i=iObservers.Count()-1; i>=0; --i)
        {
        iObservers[i]->HandleContactViewListControlEventL(*this,aEvent);
        }
    }

void CPbkContactViewListControl::ConstructL
        (CPbkContactEngine& aEngine,
        CContactViewBase& aView,
        TInt aResourceId,
        const CCoeControl* aParent)
    {
    if (aParent)
        {
        SetContainerWindowL(*aParent);
        }
    else
        {
        CreateWindowL();
        }
    ConstructFromResourceL(aResourceId);
    ConstructL(aEngine, aView);
    CreateControlExtensionL(aEngine);
    }

void CPbkContactViewListControl::ConstructFromResourceL
        (TInt aResourceId)
    {
    TResourceReader resReader;
    iCoeEnv->CreateResourceReaderLC(resReader,aResourceId);
    ConstructFromResourceL(resReader);
    CleanupStack::PopAndDestroy();  // resReader
    }

void CPbkContactViewListControl::FixTopItemIndex()
    {
    CEikListBox& listBox = ListBox();
    TInt index = listBox.TopItemIndex();
    const TInt height = listBox.View()->NumberOfItemsThatFitInRect
        (iListBox->View()->ViewRect());
    const TInt numItems = listBox.Model()->NumberOfItems();
    if (index + height > numItems)
        {
        index += numItems - (height + index);
        if (index < 0)
            {
            index = 0;
            }
        if (index != listBox.TopItemIndex())
            {
            listBox.SetTopItemIndex(index);
            }
        }
    }

/**
 * Redraws this control using DrawDeferred().
 */
void CPbkContactViewListControl::Redraw()
    {
    if (iStateFlags.IsSet(EReady) && !iStateFlags.IsSet(EBlank))
        {
        DrawDeferred();
        }
    }

void CPbkContactViewListControl::MakeComponentsVisible(TBool aVisible)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
        ("CPbkContactViewListControl::MakeComponentsVisible(0x%x,%d)"),
        this, aVisible);

    if (aVisible && (!iStateFlags.IsSet(EReady) || iStateFlags.IsSet(EBlank)))
        {
        // Never make components visible if this control is not ready or in
        // blank state
        return;
        }

    if (iListBox)
        {
        iListBox->MakeVisible(aVisible);
        }

    if (iFindBox)
        {
        iFindBox->SetFocus(aVisible);
        iFindBox->MakeVisible(aVisible);
        }

    // Hide/show thumbnail
    if (aVisible)
        {
        ShowThumbnail(iLastFocusId);
        }
    else
        {
        HideThumbnail();
        }
    }

void CPbkContactViewListControl::HandleItemAdditionL(TInt aIndex)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
        ("CPbkContactViewListControl::HandleItemAdditionL(0x%x, %d)"),
        this, aIndex);

    UpdateFindBoxL();

    // Update listbox
    iChangedIndexes[0] = aIndex;
    iListBox->HandleItemAdditionL(iChangedIndexes);

    // Maintain focus
    const TInt index = iView->FindL(iLastFocusId);
    if ( index >= 0 &&
         index < iListBox->Model()->NumberOfItems() &&
         index != iListBox->CurrentItemIndex() )
        {
        iListBox->SetCurrentItemIndex(index);
        }
    Redraw();
    HandleFocusChange();
    }

void CPbkContactViewListControl::HandleItemRemovalL(TInt aIndex)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
        ("CPbkContactViewListControl::HandleItemRemovalL(0x%x, %d)"),
        this, aIndex);

    // Deselect item
    CListBoxView& listBoxView = *ListBox().View();
    listBoxView.SetDisableRedraw(ETrue);
    listBoxView.DeselectItem(aIndex);
    listBoxView.SetDisableRedraw(EFalse);

    UpdateFindBoxL();

    // Update listbox
    TInt focusIndex = iListBox->CurrentItemIndex();
    TInt topIndex = iListBox->TopItemIndex();
    iChangedIndexes[0] = aIndex;
    iListBox->HandleItemRemovalL(iChangedIndexes);

    // Maintain focus
    const TInt lastIndex = iListBox->Model()->NumberOfItems() - 1;
    if (lastIndex == KNullContactId)
        {
        iListBox->Reset();
        }
    else
        {
        if (aIndex < focusIndex)
            {
            --focusIndex;
            }
        if (focusIndex > lastIndex || focusIndex < 0)
            {
            focusIndex = lastIndex;
            }
        const TInt numVisibleItems =
            iListBox->View()->NumberOfItemsThatFitInRect
                (iListBox->View()->ViewRect());
        if (topIndex + numVisibleItems > lastIndex)
            {
            topIndex = Max(lastIndex - numVisibleItems + 1, 0);
            }
        if (topIndex >= 0)
            {
            iListBox->SetTopItemIndex(topIndex);
            }
        if (focusIndex >= 0)
            {
            iListBox->SetCurrentItemIndex(focusIndex);
            }
        }
    Redraw();
    HandleFocusChange();
    }

/**
 * Hides or displays the find box depending on control state.
 */
void CPbkContactViewListControl::UpdateFindBoxL()
    {
    if ((iResourceData.iFlags & KPbkContactViewListControlFindBox) &&
        iBaseView->CountL() > 0)
        {
        EnableFindBoxL();
        }
    else
        {
        DisableFindBoxL();
        }
    }

void CPbkContactViewListControl::EnableFindBoxL()
    {
    if (!iFindBox)
        {
        // Create a find box
        iFindBox = CAknSearchField::NewL
            (*this, CAknSearchField::ESearch, NULL,
            CPbkConstants::SearchFieldLength());

        iFindBox->SetObserver(this);
        iFindBox->SetFocus(ETrue);
        iFindBox->ResetL();

        // Set default input mode to Katakana if current UI language is Japanese
        if (FeatureManager::FeatureSupported(KFeatureIdJapanese) &&
            (User::Language() == ELangJapanese))
            {
            CEikEdwin& findEditor = iFindBox->Editor();
            findEditor.SetAknEditorInputMode(EAknEditorKatakanaInputMode);
            }

        // Inform list box that find is visible again
        static_cast<CAknColumnListBoxView*>(iListBox->View())
            ->SetFindEmptyListState(ETrue);
        SetFindEmptyTextL();

        // Relayout view
        SizeChanged();
        UpdateFindResultL();
        }
    __ASSERT_DEBUG(iFindBox, Panic(EPanicPostCond_EnableFindBoxL));
    }

void CPbkContactViewListControl::DisableFindBoxL()
    {
    if (iFindBox)
        {
        // Important to set the find box non-focusing before
        // deleting it, otherwise the focus changes triggered
        // by the removal of the control from stack will focus
        // the find box which is under deletion
        iFindBox->SetNonFocusing();
        delete iFindBox;
        iFindBox = NULL;
        // Inform list box that find is hidden
        static_cast<CAknColumnListBoxView*>(iListBox->View())
            ->SetFindEmptyListState(EFalse);
        RemoveFindEmptyTextL();

        // Relayout view
        SizeChanged();
        UpdateFindResultL();
        }
    __ASSERT_DEBUG(!iFindBox, Panic(EPanicPostCond_DisableFindBoxL));
    }

/**
 * Updates find result.
 * @return ETrue if there was a change in find result set.
 */
TBool CPbkContactViewListControl::UpdateFindResultL()
    {
    TBool result = EFalse;
    const CContactIdArray* markedItems = NULL;
    const TInt countBefore = iView->CountL();

    if (ItemsMarked())
        {
        markedItems = &MarkedItemsL();
        }
    TRAPD( err,
        result = iView->SetFindTextL(FindTextL(), markedItems) );
    if (err != KErrNone)
        {
        iEikonEnv->HandleError(err);
        }

    const TInt countAfter = iView->CountL();

    if (err)
        {
        // Error (possibly OOM), only redraw what remains in the screen
        SizeChanged();
        Redraw();
        }
    else if (result)
        {
        iListBox->DrawDeferred();
        if (countAfter > countBefore)
            {
            iListBox->HandleItemAdditionL();
            }
        else if (countAfter < countBefore)
            {
            iListBox->HandleItemRemovalL();
            }

        if (markedItems)
            {
            // Restore item marks
            CListBoxView& listBoxView = *iListBox->View();
            listBoxView.SetDisableRedraw(ETrue);
            CleanupStack::PushL(TCleanupEnableListBoxViewRedraw(listBoxView));
            listBoxView.ClearSelection();
            const TInt count = markedItems->Count();
            #ifdef PBK_BENCHMARK_FIND
            TRunningTimes<TInt> timer;
            #endif
            for (TInt i=0; i < count; ++i)
                {
                const TContactItemId contactId = (*markedItems)[i];
                #ifdef PBK_BENCHMARK_FIND
                timer.Start();
                #endif
                const TInt index =iView->FindL(contactId);
                #ifdef PBK_BENCHMARK_FIND
                timer.Stop();
                #endif
                listBoxView.SelectItemL(index);
                }
            #ifdef PBK_BENCHMARK_FIND
            RDebug::Print(_L(
                "CPbkContactViewListControl::UpdateFindResultL FindL calls %d tot %d"),
                timer.Count(), timer.Total());
            #endif
            CleanupStack::PopAndDestroy(); //TCleanupEnableListBoxViewRedraw
            }

        // This event is send to observers,
        // if contact set of the control is changed
        TPbkContactViewListControlEvent
            event(TPbkContactViewListControlEvent::EContactSetChanged);
        SendEventToObserversL(event);
        }

    TInt focusIndex = iView->IndexOfFirstFindMatchL();
    if (focusIndex < 0)
        {
        if (countAfter > 0)
            {
            focusIndex = 0;
            }
        }
    if (focusIndex >= 0 && focusIndex != iListBox->CurrentItemIndex())
        {
        iListBox->SetCurrentItemIndexAndDraw(focusIndex);
        }
    HandleFocusChange();

    return result;
    }

void CPbkContactViewListControl::SetFindEmptyTextL()
    {
    if (!iOriginalEmptyText)
        {
        iOriginalEmptyText = iListBox->View()->EmptyListText()->AllocL();
        }
    iListBox->View()->SetListEmptyTextL(*iResourceData.iFindEmptyText);
    }

void CPbkContactViewListControl::RemoveFindEmptyTextL()
    {
    if (iOriginalEmptyText)
        {
        iListBox->View()->SetListEmptyTextL(*iOriginalEmptyText);
        }
    }

void CPbkContactViewListControl::HandleFocusChange()
    {
    TRAP_IGNORE(HandleFocusChangeL());
    }

void CPbkContactViewListControl::HandleFocusChangeL()
    {
    const TInt index = ListBox().CurrentItemIndex();
    TContactItemId newFocusId = KNullContactId;
    if (index >= 0 && index < iView->CountL())
        {
        newFocusId = iView->AtL(index);
        }
    if (newFocusId != iLastFocusId)
        {
        iLastFocusId = newFocusId;
        HideThumbnail();
        ShowThumbnail(newFocusId);
        }
    }

/**
 * Marks specified contacts in the listbox.
 *
 * @param aMarkedContactIds contacts to mark.
 * @return true if any contacts were marked in the list.
 */
TBool CPbkContactViewListControl::RestoreMarkedItemsL
        (const CContactIdArray* aMarkedContactIds)
    {
    CEikListBox& listBox = ListBox();
    TBool result = EFalse;
    DisableRedrawEnablePushL();
    listBox.ClearSelection();
    if (aMarkedContactIds)
        {
        const TInt count = aMarkedContactIds->Count();
        for (TInt i=0; i < count; ++i)
            {
            const TInt index = FindContactIdL((*aMarkedContactIds)[i]);
            if (index >= 0)
                {
                listBox.View()->SelectItemL(index);
                result = ETrue;
                }
            }
        }
    CleanupStack::PopAndDestroy();  // DisableRedrawEnablePushL
    return result;
    }

/**
 * Creates control extension for a model, appends new icons
 * and sets this object as control updator for the extension.
 *
 * @param aEngine provided for control extension
 */
void CPbkContactViewListControl::CreateControlExtensionL
        (CPbkContactEngine& aEngine)
    {
    __ASSERT_DEBUG(&Model(), Panic(EPanicPre_SetControlExtensionL));
    __ASSERT_DEBUG(!iControlExtension, Panic(EPanicPre_SetControlExtensionL));

    CPbkExtGlobals* extGlobal = CPbkExtGlobals::InstanceL();
    extGlobal->PushL();
    iControlExtension = extGlobal->FactoryL().
            CreatePbkUiControlExtensionL(aEngine);
    CleanupStack::PopAndDestroy(extGlobal);

    Model().SetContactUiControlExtension(*iControlExtension);

    TInt arrayInfoId = 0;
    TInt arrayId = 0;
    iControlExtension->IconArrayResourceId(arrayInfoId, arrayId);
    if ( arrayInfoId != 0 )
        {
        IconArray(*iListBox)->AppendIconsFromResourceL(arrayInfoId, arrayId);
        }

    iControlExtension->SetContactUiControlUpdate(this);
    }

void CPbkContactViewListControl::HandleResourceChange(TInt aType)
    {
    CPbkContactListControlBase::HandleResourceChange(aType);

    TRAP_IGNORE(DoHandleResourceChangeL(aType));
    }

void CPbkContactViewListControl::RefreshIconArrayL()
    {
    // Control extension may be NULL when skin is changed at run-time
    if (iControlExtension)
        {
        CPbkIconArray* iconArray = IconArray(*iListBox);
        if (iconArray)
            {
            // refresh the main pbk icon array
            iconArray->RefreshL(R_PBK_ICON_INFO_ARRAY);
            // also refresh the control extension icon array
            TInt arrayInfoId = 0;
            TInt arrayId = 0;
            iControlExtension->IconArrayResourceId(arrayInfoId, arrayId);
            if (arrayInfoId != 0)
                {
                iconArray->RefreshL(arrayInfoId);
                }
            }
        }
    }

void CPbkContactViewListControl::DoHandleResourceChangeL(TInt aType)
    {
    if (aType == KAknsMessageSkinChange)
        {
        RefreshIconArrayL();
        }
    else if (aType == KEikDynamicLayoutVariantSwitch)
        {
        RefreshIconArrayL();
        SizeChanged();
        const TBool focused( IsFocused() );
        const TBool nonFocusing( IsNonFocusing() );
        if ( focused && !nonFocusing )
            {
            ShowThumbnailL();
            }
        DrawNow();
        }
    }


// TCleanupEnableListBoxViewRedraw
void TCleanupEnableListBoxViewRedraw::CleanupOp(TAny* aPtr)
    {
    static_cast<CListBoxView*>(aPtr)->SetDisableRedraw(EFalse);
    }

EXPORT_C void CPbkContactViewListControl::DeleteThumbnail()
    {
    delete iThumbnailPopup;
    iThumbnailPopup = NULL;
    }
    
void CPbkContactViewListControl::EnableMSKObserver( TBool aEnable )
    {
    iListBox->EnableMSKObserver(aEnable);
    }
    
TInt CPbkContactViewListControl::ItemCount()
    {
    return Model().MdcaCount(); 
    }

//  End of File

