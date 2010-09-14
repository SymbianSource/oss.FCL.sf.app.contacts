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
*           Methods for Phonebook Contact editor dialog.
*
*/


// INCLUDE FILES
#include "CPbkContactEditorDlg.h"

#include <eikspane.h>
#include <avkon.hrh>
#include <akntitle.h>
#include <aknnavi.h>
#include <eikmenub.h>
#include <featmgr.h>
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <aknnotewrappers.h>
#include <AknsUtils.h>
#include <eikcapc.h>
#include <hlplch.h>   

#include "CPbkThumbnailCmd.h"
#include "CPbkContextPaneIcon.h"
#include "CPbkContactEditorFieldArray.h"
#include "MPbkContactEditorUiBuilder.h"
#include <MPbkContactEditorField.h>
#include <MPbkContactEditorControl.h>
#include "PbkContactEditorStrategyFactory.h"
#include "MPbkContactEditorStrategy.h"
#include "TPbkAddItemManager.h"
#include "CPbkThumbnailPopup.h"
#include <PbkView.hrh>
#include "PbkUID.h"
#include "MPbkFieldEditorVisitor.h"
#include "MPbkFieldEditorControl.h"
#include "MPbkEditorOkToExitCallback.h"

#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
#include <CPbkFieldsInfo.h>
#include <CPbkConstants.h>
#include <CPbkFieldData.h>
#include <pbkview.rsg>
#include <MPbkContactEditorExtension.h>
#include <MPbkExtensionFactory.h>
#include <CPbkExtGlobals.h>

#include <PbkDebug.h>


/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS

/// Escape key press event
static const TKeyEvent KEscKeyEvent = { EKeyEscape, 0, 0, 0 };

#ifdef _DEBUG
/// Panic codes for CPbkContactEditorDlg
enum TPanicCode
    {
    EPanicPostCond_Constructor,
    EPanicPreCond_ResetWhenDestroyed,
    EPanicPreCond_CmdFetchThumbnailL,
    EPanicPreCond_CmdRemoveThumbnailL,
    EPanicPreCond_PreLayoutDynInitL,
    EPanicPostCond_PreLayoutDynInitL,
    EPanicPreCond_SetInitialCurrentLine,
    EPanicPreCond_ContactAlreadyExistsL
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkContactEditorDlg");
    User::Panic(KPanicText, aReason);
    }
#endif  // _DEBUG

void UpdateCbasL( MPbkContactEditorField* aCurrentField,
                  CEikButtonGroupContainer& aCba )
    {
    if ( aCurrentField )
        {
        TPbkFieldEditMode editMode =
            aCurrentField->ContactItemField().FieldInfo().EditMode();
        
        if ( editMode ==  EPbkFieldEditModeSelector )
            {
            if ( !aCba.IsCommandInGroup(EAknSoftkeyChange) )
                {
                aCba.SetCommandSetL( R_PBK_SOFTKEYS_OPTIONS_DONE_CHANGE );    
                aCba.DrawDeferred();
                } 
            }
        else
            {
            if ( !aCba.IsCommandInGroup(EAknSoftkeyContextOptions) )
                {
                aCba.SetCommandSetL( R_PBK_SOFTKEYS_OPTIONS_DONE_CONTEXT );    
                aCba.DrawDeferred();                
                }
            }
        }
    }

}  // namespace


// ================= MEMBER FUNCTIONS =======================

/**
 * Extension implementation.
 */
NONSHARABLE_CLASS(CPbkContactEditorDlg::CExtension) :
        public CBase,
        public MPbkContactEditorControl
    {
    public: // Constructor and destructor
        /**
         * Creates a new CExtension.
         * @param aContact currently edited contact
         */
        static CExtension* NewL
            (CPbkContactItem& aContact,
            const CPbkContactEditorDlg& aParent);
        /**
         * Destructor
         */
        ~CExtension();

    public: // From MPbkContactEditorExtension API
        void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
        TBool ProcessCommandL(TInt aCommandId);
        void PreOkToExitL(TInt aButtonId);
        void PostOkToExitL(TInt aButtonId);
        void SetStateModified();

    public: // From MPbkContactEditorControl
        MPbkContactEditorField* FocusedField();
        TInt EditorCount();
        const MPbkContactEditorField& FieldAt(TInt aFieldIndex);

    private: // Implementation
        CExtension(const CPbkContactEditorDlg& aParent);
        void ConstructL(CPbkContactItem& aContact);

    private: // Data
        /// Ref: for creating editor extension
        CPbkExtGlobals* iExtGlobal;
        /// Ref: editor extension
        MPbkContactEditorExtension* iEditorExtension;
        /// Ref: parent editor
        const CPbkContactEditorDlg& iParent;
    };

CPbkContactEditorDlg::CExtension::CExtension
        (const CPbkContactEditorDlg& aParent) :
        iParent(aParent)
    {
    }

void CPbkContactEditorDlg::CExtension::ConstructL
        (CPbkContactItem& aContact)
    {
    iExtGlobal = CPbkExtGlobals::InstanceL();
    iEditorExtension = iExtGlobal->FactoryL().
        CreatePbkContactEditorExtensionL(aContact, iParent.iEngine, *this);
    }

CPbkContactEditorDlg::CExtension* CPbkContactEditorDlg::CExtension::NewL
        (CPbkContactItem& aContact,
        const CPbkContactEditorDlg& aParent)
    {
    CExtension* self = new (ELeave) CExtension(aParent);
    CleanupStack::PushL(self);
    self->ConstructL(aContact);
    CleanupStack::Pop(self);
    return self;
    }

CPbkContactEditorDlg::CExtension::~CExtension()
    {
    Release(iEditorExtension);
    Release(iExtGlobal);
    }

void CPbkContactEditorDlg::CExtension::DynInitMenuPaneL
        (TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    iEditorExtension->DynInitMenuPaneL(aResourceId, aMenuPane);
    }

TBool CPbkContactEditorDlg::CExtension::ProcessCommandL(TInt aCommandId)
    {
    return iEditorExtension->ProcessCommandL(aCommandId);
    }

void CPbkContactEditorDlg::CExtension::PreOkToExitL(TInt aButtonId)
    {
    iEditorExtension->PreOkToExitL(aButtonId);
    }

void CPbkContactEditorDlg::CExtension::PostOkToExitL(TInt aButtonId)
    {
    iEditorExtension->PostOkToExitL(aButtonId);
    }

void CPbkContactEditorDlg::CExtension::SetStateModified()
    {
    iParent.iEditorStrategy->SetStateModified();
    }

MPbkContactEditorField* CPbkContactEditorDlg::CExtension::FocusedField()
    {
    const TInt id = iParent.IdOfFocusControl();
    return iParent.iFieldManager->Find(id);
    }

TInt CPbkContactEditorDlg::CExtension::EditorCount()
    {
    return iParent.iFieldManager->EditorCount();
    }

const MPbkContactEditorField& CPbkContactEditorDlg::CExtension::
        FieldAt(TInt aFieldIndex)
    {
    return iParent.iFieldManager->FieldAt(aFieldIndex);
    }

/**
 * Editor implementation class.
 */
NONSHARABLE_CLASS(CPbkContactEditorDlgImpl) :
        public CPbkContactEditorDlg,
        private MPbkContactEditorUiBuilder
    {
    public: // constructor
        /**
         * Creates a new instance of this class.
         * @param aEngine phonebook engine
         * @param aContactItem contact item
         * @param aIsNewContact defines is this new contact or not
         * @param aFocusIndex field to focus
         * @param AEdited edited flag
         */
        static CPbkContactEditorDlgImpl* NewL(
            CPbkContactEngine& aEngine,
            CPbkContactItem& aContactItem,
            TBool aIsNewContact,
            TInt aFocusIndex = -1,
            TBool aEdited = EFalse);

        /**
         * Destructor.
         */
        ~CPbkContactEditorDlgImpl();

    private: // from MPbkContactEditorUiBuilder
        CCoeControl* CreateLineL(const TDesC& aCaption, TInt aControlId,
                TInt aControlType);
        CEikCaptionedControl* LineControl(TInt aControlId) const;
        void DeleteControl(TInt aControlId);
        void TryChangeFocusL(TInt aControlId);
        void SetCurrentLineCaptionL(const TDesC& aText);
        CCoeControl* Control(TInt aControlId) const;
        void SetEditableL(TBool aState);

    private: // from CAknForm
        void HandleResourceChange(TInt aType);
        
    private: // from CCoeControl
        void FocusChanged(TDrawNow aDrawNow);

    private: // implementation
        CPbkContactEditorDlgImpl
            (CPbkContactEngine& aEngine,
            CPbkContactItem& aContactItem,
            TInt aFocusIndex = -1);
    private:
        class CPbkIconReloaderVisitor;
        CPbkIconReloaderVisitor* iIconReloaderVisitor;
    };

/**
 * Editor icon reloader visitor.
 */
NONSHARABLE_CLASS(CPbkContactEditorDlgImpl::CPbkIconReloaderVisitor) :
        public CBase,
        public MPbkFieldEditorVisitor
    {
    public:
        CPbkIconReloaderVisitor(MPbkContactEditorUiBuilder& aUiBuilder);
        ~CPbkIconReloaderVisitor();

    private: // From MPbkFieldEditorVisitor
        void VisitL(MPbkFieldEditorControl& aThis);

    private: // Data
        MPbkContactEditorUiBuilder& iUiBuilder;
    };

CPbkContactEditorDlgImpl::CPbkIconReloaderVisitor::CPbkIconReloaderVisitor
        (MPbkContactEditorUiBuilder& aUiBuilder) :
    iUiBuilder(aUiBuilder)
    {
    }

CPbkContactEditorDlgImpl::CPbkIconReloaderVisitor::~CPbkIconReloaderVisitor()
    {
    }

void CPbkContactEditorDlgImpl::CPbkIconReloaderVisitor::VisitL
        (MPbkFieldEditorControl& aThis)
    {
    aThis.LoadBitmapToFieldL(iUiBuilder);
    }

inline CPbkContactEditorDlgImpl::CPbkContactEditorDlgImpl(
        CPbkContactEngine& aEngine,
        CPbkContactItem& aContactItem,
        TInt aFocusIndex) :
    CPbkContactEditorDlg(aEngine, aContactItem, aFocusIndex)
    {
    }

CPbkContactEditorDlgImpl* CPbkContactEditorDlgImpl::NewL(
        CPbkContactEngine& aEngine,
        CPbkContactItem& aContactItem,
        TBool aIsNewContact,
        TInt aFocusIndex,
        TBool aEdited)
    {
    // Create the dialog
    CPbkContactEditorDlgImpl* self = new(ELeave) CPbkContactEditorDlgImpl(
            aEngine, aContactItem, aFocusIndex);
    CleanupStack::PushL(self);
    self->BaseConstructL(aIsNewContact, aEdited);

    // create field managing object
    self->iFieldManager = CPbkContactEditorFieldArray::NewL
        (aContactItem, *self);

    self->iIconReloaderVisitor = new(ELeave) CPbkIconReloaderVisitor(*self);
    CleanupStack::Pop(self);
    return self;
    }

CPbkContactEditorDlgImpl::~CPbkContactEditorDlgImpl()
    {
    delete iIconReloaderVisitor;
    }

CCoeControl* CPbkContactEditorDlgImpl::CreateLineL
        (const TDesC& aCaption,
        TInt aControlId,
        TInt aControlType)
    {
    return CreateLineByTypeL(
            aCaption, aControlId, aControlType, NULL);
    }

CEikCaptionedControl* CPbkContactEditorDlgImpl::LineControl
        (TInt aControlId) const
    {
    return Line(aControlId);
    }

void CPbkContactEditorDlgImpl::DeleteControl
        (TInt aControlId)
    {
    DeleteLine(aControlId);
    }

void CPbkContactEditorDlgImpl::TryChangeFocusL
        (TInt aControlId)
    {
    if (ControlOrNull(aControlId))
        {
        TryChangeFocusToL(aControlId);
        }
    }

void CPbkContactEditorDlgImpl::SetCurrentLineCaptionL
        (const TDesC& aText)
    {
    SetControlCaptionL(IdOfFocusControl(), aText);
    }

CCoeControl* CPbkContactEditorDlgImpl::Control
        (TInt aControlId) const
    {
    return ControlOrNull(aControlId);
    }

void CPbkContactEditorDlgImpl::SetEditableL(TBool aState)
    {
    CPbkContactEditorDlg::SetEditableL(aState);
    }

void CPbkContactEditorDlgImpl::HandleResourceChange(TInt aType)
    {
    CPbkContactEditorDlg::HandleResourceChange(aType);

    if (aType == KAknsMessageSkinChange)
        {
        TRAP_IGNORE(iFieldManager->AcceptL(*iIconReloaderVisitor));
        }
    else if (aType == KEikDynamicLayoutVariantSwitch)
        {        
        // It is not bad if thumbnail showing fails
        TRAP_IGNORE( ReloadThumbnailL() );        
        }
    }
    
void CPbkContactEditorDlgImpl::FocusChanged(TDrawNow /*aDrawNow*/)
    {
    // Thumbnail will be set as hidden if the control is not focused.
    // all is done in ReloadThumbnailL -method.
    // No big deal if thumbnail is not shown    
    TRAP_IGNORE(ReloadThumbnailL());
    }


///////////////////////////////////////////////////////////////
/// CPbkContactEditorDlg

inline CPbkContactEditorDlg::CPbkContactEditorDlg
        (CPbkContactEngine& aEngine,
        CPbkContactItem& aContactItem,
        TInt aFocusIndex) :
    iContactItem(aContactItem),
    iEngine(aEngine),
    iFocusIndex(aFocusIndex)
    {
    __ASSERT_DEBUG(!iTitlePane && !iThumbnailHandler && !iStoredTitlePaneText,
        Panic(EPanicPostCond_Constructor));
    }

inline void CPbkContactEditorDlg::StoreTitlePaneTextL()
    {
    CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
    if (statusPane && statusPane->PaneCapabilities(TUid::Uid(EEikStatusPaneUidTitle)).IsPresent())
        {
        iTitlePane = static_cast<CAknTitlePane*>
            (statusPane->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
        if (iTitlePane->Text())
            {
            iStoredTitlePaneText = iTitlePane->Text()->AllocL();
            }
        }
    }

inline void CPbkContactEditorDlg::ConstructContextMenuL()
    {
    CEikMenuBar* newMenuBar = new(ELeave) CEikMenuBar();
    CleanupStack::PushL(newMenuBar);
    newMenuBar->ConstructL(this, NULL, R_PBK_CONTACTEDITOR_CONTEXT_MENUBAR);
    iEikonEnv->EikAppUi()->AddToStackL(newMenuBar, ECoeStackPriorityMenu, ECoeStackFlagRefusesFocus);
    iContextMenuBar = newMenuBar;
    CleanupStack::Pop(newMenuBar);
    }

inline void CPbkContactEditorDlg::ConstructNaviPaneL()
    {
    CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
    if (statusPane && statusPane->PaneCapabilities(TUid::Uid(EEikStatusPaneUidNavi)).IsPresent())
        {
        // create a default empty status pane, otherwise sync field will show
        iNaviContainer = static_cast<CAknNavigationControlContainer *>(statusPane->ControlL(
                TUid::Uid(EEikStatusPaneUidNavi)));
        iNaviContainer->PushDefaultL();
        }
    }

inline void CPbkContactEditorDlg::BaseConstructL
        (TBool aIsNewContact, TBool aEdited)
    {
    // set feature manager in TLS
    FeatureManager::InitializeLibL();

    CAknDialog::ConstructL(R_PBK_CONTACTEDITOR_MENUBAR);

    iEditorStrategy = PbkContactEditorStrategyFactory::CreateL
        (iEngine, iContactItem, aIsNewContact);
    if (aEdited)
        {
        iEditorStrategy->SetStateModified();
        }

    // check existance of syncronization field
    if(!iContactItem.FindField(EPbkFieldIdSyncronization))
        {
        CPbkFieldInfo* syncFieldInfo = iEngine.FieldsInfo().Find(
            EPbkFieldIdSyncronization);
        if (syncFieldInfo)
            {
            // add syncronization field to contact - compulsory
            iContactItem.AddFieldL(*syncFieldInfo);
            iEditorStrategy->SetStateModified();
            }
        }
        
    StoreTitlePaneTextL();
    ConstructContextMenuL();
    ConstructNaviPaneL();

    iThumbnailHandler = CPbkThumbnailPopup::NewL(iEngine);
    iThumbnailCmd = CPbkThumbnailCmd::NewL(iEngine, *iThumbnailHandler);
    iExtension = CExtension::NewL(iContactItem, *this);
    }

EXPORT_C CPbkContactEditorDlg* CPbkContactEditorDlg::NewL
        (CPbkContactEngine& aEngine,
        CPbkContactItem& aContactItem,
        TBool aIsNewContact,
        TInt aFocusIndex/*=-1*/,
        TBool aEdited/*=EFalse*/)
    {
    return CPbkContactEditorDlgImpl::NewL(
            aEngine, aContactItem, aIsNewContact, aFocusIndex, aEdited);
    }

EXPORT_C void CPbkContactEditorDlg::ResetWhenDestroyed
        (CPbkContactEditorDlg** aSelfPtr)
    {
    __ASSERT_DEBUG(!aSelfPtr || *aSelfPtr == this,
            Panic(EPanicPreCond_ResetWhenDestroyed));
    iSelfPtr = aSelfPtr;
    }

EXPORT_C void CPbkContactEditorDlg::HideExitCommand()
    {
    iStateFlags.Set(EHideExit);
    }

EXPORT_C void CPbkContactEditorDlg::SetExitCommandId
        (TInt aCommandId)
    {
    iExitCommandId = aCommandId;
    }

CPbkContactEditorDlg::~CPbkContactEditorDlg()
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkContactEditorDlg::~CPbkContactEditorDlg"));

    iStateFlags.Set(EUnderDestruction);
    CloseAllPopups();

    // Save unless saved yet. No other way to handle errors than ignore
    // because exiting the dialog must always be possible.
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkContactEditorDlg::~CPbkContactEditorDlg(0x%x) saving"), this);
    TRAP_IGNORE(CmdSaveL());
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("    saved"));

    // delete member data
    delete iThumbnailHandler;
    delete iThumbnailCmd;

    if (iContextMenuBar)
        {
        iEikonEnv->EikAppUi()->RemoveFromStack(iContextMenuBar);
        delete iContextMenuBar;
        }

    delete iFieldManager; // context menu bar's stack removal (above) needs field manager to be alive
    delete iEditorStrategy;
    delete iContextPaneIcon;

    // Restore the titlepane text
    if (iTitlePane && iStoredTitlePaneText)
        {
        // iTitlePane takes ownership of iStoredTitlePaneText
        iTitlePane->SetText(iStoredTitlePaneText);
        }

    // Remove empty status pane
    if (iNaviContainer)
        {
        iNaviContainer->Pop();
        }
    // Reset self pointer
    if (iSelfPtr)
        {
        *iSelfPtr = NULL;
        }
    delete iExtension;

    FeatureManager::UnInitializeLib();
    }

EXPORT_C TContactItemId CPbkContactEditorDlg::ExecuteLD()
    {
    TContactItemId ret = KNullContactId;
    iSavedContactPtr = &ret;

    // Set Phonebook icon on context pane
    if (!iContextPaneIcon)
        {
        iContextPaneIcon = CPbkContextPaneIcon::NewL(*iEikonEnv);
        }

    // Set status pane layout to the Phonebook one
    CEikStatusPane* statusPane = iAvkonAppUi->StatusPane();
    TInt statusPaneLayout = KErrNotFound;
    if (statusPane)
        {
        statusPaneLayout = statusPane->CurrentLayoutResId();
        statusPane->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL);
        }

    CAknForm::ExecuteLD(R_PBK_EDIT_MEMORY_ENTRY_DIALOG);

    if (statusPane && (statusPaneLayout != KErrNotFound))
        {
        statusPane->SwitchLayoutL(statusPaneLayout);
        }

    return ret;
    }


EXPORT_C void CPbkContactEditorDlg::SetHelpContext
        (TCoeHelpContext aContext)
    {
    iHelpContext = aContext;
    }


/**
 * Saves dialog's fields to contact fields and saves the contact.
 * Command sent from class destructor.
 */
void CPbkContactEditorDlg::CmdSaveL()
    {
    if (iFieldManager)
        {
        // Save editor fields
        iFieldManager->SaveFieldsL();
        if (iFieldManager->FieldsChanged())
            {
            iEditorStrategy->SetStateModified();
            }

        if (iSavedContactPtr && !iFieldManager->AreAllFieldsEmpty())
            {
            *iSavedContactPtr = iEditorStrategy->SaveContactL();
            }
        }
    }

/**
 * Command sent from OkToExitL.
 */
void CPbkContactEditorDlg::CmdDoneL()
    {
    if (iFieldManager)
        {
        // Save editor fields to contact item
        iFieldManager->SaveFieldsL();
        if (iFieldManager->FieldsChanged())
            {
            iEditorStrategy->SetStateModified();
            }

        if (iFieldManager->AreAllFieldsEmpty())
            {
            iEditorStrategy->DeleteContactL();
            }

        *iSavedContactPtr = iEditorStrategy->SaveContactL();
        }
    }

/**
 * Edits the focused memory items caption.
 */
void CPbkContactEditorDlg::CmdEditItemLabelL()
    {
    MPbkContactEditorField* field = iFieldManager->Find(IdOfFocusControl());
    if (field)
        {
        HBufC* textBuf = HBufC::NewLC(CPbkConstants::FormEditorLength());
        TPtr text = textBuf->Des();
        text.Copy(field->FieldLabel().Left(text.MaxLength()));
        HBufC* prompt = StringLoader::LoadLC(R_QTN_FORM_PRMPT_FIELD_LABEL);

        CAknTextQueryDialog* dlg = CAknTextQueryDialog::NewL(text);
        dlg->SetMaxLength(CPbkConstants::FormEditorLength());
        if (dlg->ExecuteLD(R_PBK_EDIT_FIELD_LABEL, *prompt))
            {
            if (text != field->FieldLabel())
                {
                // Assign new label leave-safely to this form and to the contact field
                HBufC* newLabel = text.AllocLC();
                field->SetFieldLabelL(newLabel); // takes ownership of newLabel
                CleanupStack::Pop(newLabel);
                iEditorStrategy->SetStateModified();
                }
            }
        CleanupStack::PopAndDestroy(2);  // prompt, textBuf
        }
    }

/**
 * Add new memory item menu command.
 */
void CPbkContactEditorDlg::CmdAddItemL()
    {
    TPbkAddItemManager manager(iContactItem, *iFieldManager);

    RArray<TInt> addedFieldsControlIds;
    CleanupClosePushL(addedFieldsControlIds);

    // Query and add field(s) to the contact
    manager.AddFieldsL(iEngine.FieldsInfo(), addedFieldsControlIds);

    if (addedFieldsControlIds.Count() > 0)
        {
        // Activate the first added field
        TInt focusedControlId = addedFieldsControlIds[0];
        TryChangeFocusToL(focusedControlId);
        iEditorStrategy->SetStateModified();
        // must be DrawNow and not DrawDeferred
        // otherwise field label and content of
        // following field will be incorrect
        DrawNow();
        }
    CleanupStack::PopAndDestroy(); //addedFieldsControlIds
    }

/**
 * Deletes focused memory entry item.
 */
void CPbkContactEditorDlg::CmdDeleteItemL()
    {
    // Delete item query
    MPbkContactEditorField* field = iFieldManager->Find(IdOfFocusControl());
    if (field)
        {
        TPtrC label = field->FieldLabel();
        HBufC* prompt = StringLoader::LoadLC(R_QTN_PHOB_QUERY_DELETE_ITEM, label);

        CAknQueryDialog* dlg = CAknQueryDialog::NewL();
        CleanupStack::PushL(dlg);
        dlg->SetPromptL(*prompt);
        CleanupStack::Pop(); // dlg
        if(dlg->ExecuteLD(R_PBK_GENERAL_CONFIRMATION_QUERY_PBKVIEW))
            {
            // Notify about voice tag field deletion (unless it's SIND tag)
            if (iContactItem.VoiceTagField() &&
                iContactItem.VoiceTagField()->IsSame(field->ContactItemField()) &&
                !FeatureManager::FeatureSupported(KFeatureIdSind))
                {
                HBufC* note = iCoeEnv->AllocReadResourceLC(R_QTN_PHOB_NOTE_VOICE_TAG_DEL);
                CAknConfirmationNote* dlg = new(ELeave) CAknConfirmationNote(ETrue);
                dlg->ExecuteLD(*note);
                CleanupStack::PopAndDestroy(); // note
                }

            // Notify about speed dial field deletion
            const TInt fieldIndex = iFieldManager->FindContactItemFieldIndex(
                    field->ContactItemField());
            if (iEngine.IsSpeedDialAssigned(iContactItem, fieldIndex))
                {
                HBufC* note = iCoeEnv->AllocReadResourceLC(R_QTN_PHOB_NOTE_SPEED_DIAL_DEL);
                CAknConfirmationNote* dlg = new(ELeave) CAknConfirmationNote(ETrue);
                dlg->ExecuteLD(*note);
                CleanupStack::PopAndDestroy(); // note
                }

            // Delete the field
            iFieldManager->RemoveField(*field);
            iEditorStrategy->SetStateModified();

            // Redraw using DrawNow, DrawDeferred does not get the job done
            DrawNow();
            }

        CleanupStack::PopAndDestroy(); // prompt
        }
    }

/**
 * Fetches thumbnail image from photoalbum and attaches it to this
 * contact.
 */
void CPbkContactEditorDlg::CmdFetchThumbnailL()
    {
    //PreCond:
    __ASSERT_DEBUG(iThumbnailCmd, Panic(EPanicPreCond_CmdFetchThumbnailL));

    if (iThumbnailCmd->FetchThumbnailL(iContactItem))
        {
        iEditorStrategy->SetStateModified();
        }
    }

/**
 * Removes a thumbnail image attachment from this contact.
 */
void CPbkContactEditorDlg::CmdRemoveThumbnailL()
    {
    //PreCond:
    __ASSERT_DEBUG(iThumbnailCmd, Panic(EPanicPreCond_CmdRemoveThumbnailL));

    if (iThumbnailCmd->RemoveThumbnailL(iContactItem))
        {
        iEditorStrategy->SetStateModified();
        }
    }

/**
 * Updates the application title pane.
 */
void CPbkContactEditorDlg::UpdateTitlePaneL()
    {
    if (iTitlePane)
        {
        CPbkFieldDataArray* titleFields = CPbkFieldDataArray::NewL();
        titleFields->ResetAndDestroyPushL();

        const TInt fieldCount = iFieldManager->EditorCount();
        for (TInt i = 0; i < fieldCount; ++i)
            {
            const MPbkContactEditorField& field = iFieldManager->FieldAt(i);
            const TPbkFieldId fieldId = field.FieldId();
            if (iEngine.IsTitleField(fieldId))
                {
                HBufC* textBuf = field.ControlTextL();
                TPtrC text;
                if (textBuf)
                    {
                    CleanupStack::PushL(textBuf);
                    text.Set(*textBuf);
                    }

                CPbkFieldData* fieldData = CPbkFieldData::NewL(fieldId,text);
                if (textBuf)
                    {
                    CleanupStack::PopAndDestroy(textBuf);
                    }
                CleanupStack::PushL(fieldData);
                titleFields->AppendL(fieldData);
                CleanupStack::Pop(fieldData);
                }
            }

        HBufC* newTitle = iEngine.GetContactTitleOrNullL(*titleFields);
        CleanupStack::PopAndDestroy();  // titleFields

        if (newTitle)
            {
            // Update title only if it doesn't contain ZWS or ZWNJ, which
            // are a sign of incomplete typing in e.g. Hindi
            const TChar KZeroWidthSpace(0x200b);
            const TChar KZeroWidthNonJoiner(0x200c);
            if ((newTitle->Locate(KZeroWidthSpace) == KErrNotFound) &&
                (newTitle->Locate(KZeroWidthNonJoiner) == KErrNotFound))
                {
                iTitlePane->SetText(newTitle);
                }
            }
        else
            {
            iTitlePane->SetTextL(iEditorStrategy->DefaultTitle());
            }
        }
    }

TKeyResponse CPbkContactEditorDlg::OfferKeyEventL
        (const TKeyEvent& aKeyEvent,
        TEventCode aType)
    {
    // display menu bar if the focused control doesnt consume selection keys
    MPbkContactEditorField* field = iFieldManager->Find(IdOfFocusControl());
    if (field && field->ConsumesKeyEvent(aKeyEvent, aType))
        {
        return CAknForm::OfferKeyEventL(aKeyEvent,aType);
        }

    if (aType == EEventKey && aKeyEvent.iCode == EKeyOK)
        {
        iContextMenuBar->TryDisplayMenuBarL();
        return EKeyWasConsumed;
        }

    if (aType == EEventKey && aKeyEvent.iCode == EKeyEscape)
        {
        // See CloseAllMenusAndPopups()
        iStateFlags.Set(EEscKeyReceived);
        }

    if (!iStateFlags.IsSet(EUnderDestruction))
        {
        TKeyResponse ret(EKeyWasNotConsumed);        
        ret = CAknForm::OfferKeyEventL( aKeyEvent, aType );
        if ( !iStateFlags.IsSet(EEscKeyReceived) && 
            ret == EKeyWasConsumed )
            {
            UpdateCbasL( iFieldManager->Find(IdOfFocusControl()), 
                         ButtonGroupContainer() );
            }   
        return ret;
        }
    else
        {
        return EKeyWasConsumed;
        }
    }

void CPbkContactEditorDlg::GetHelpContext
        (TCoeHelpContext& aContext) const
    {
    if (iHelpContext.iMajor.iUid)
        {
        // If external help context has been set, return it..
        aContext = iHelpContext;
        }
    else
        {
        // ..otherwise go with the default context
        aContext.iMajor.iUid = KPbkUID3;
        iEditorStrategy->GetHelpContext(aContext);
        }
    }

void CPbkContactEditorDlg::PreLayoutDynInitL()
    {
    __ASSERT_DEBUG(iFieldManager, Panic(EPanicPreCond_PreLayoutDynInitL));

    SetEditableL(ETrue);

    // create controls for contact items fields
    iFieldManager->CreateFieldsFromContactL();

    __ASSERT_DEBUG(iFieldManager, Panic(EPanicPostCond_PreLayoutDynInitL));
    }

void CPbkContactEditorDlg::PostLayoutDynInitL()
    {
    UpdateTitlePaneL();
    
    if ( iAvkonAppUi->StatusPane()->
        PaneCapabilities(TUid::Uid(EEikStatusPaneUidContext)).IsInCurrentLayout() )
        {
        iThumbnailHandler->Load(iContactItem, NULL );
        }

    // Set state to initialised (enables save from destructor)
    iEditorStrategy->SetStateInitialized();
    }

void CPbkContactEditorDlg::SetInitialCurrentLine()
    {
    //PreCond:
    __ASSERT_DEBUG(iFieldManager,
            Panic(EPanicPreCond_SetInitialCurrentLine));

    CAknForm::SetInitialCurrentLine();
    const TInt fieldCount = iFieldManager->ContactItemFieldCount();
    if (fieldCount > 0)
        {
        if (iFocusIndex < 0 || iFocusIndex >= fieldCount)
            {
            // In any case focus the first line
            iFocusIndex = 0;
            }
        iFieldManager->SetFocusToIndex(iFocusIndex);
        }
    }

void CPbkContactEditorDlg::ProcessCommandL
        (TInt aCommandId)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkContactEditorDlg::ProcessCommandL start"));

    HideMenu();
    iContextMenuBar->StopDisplayingMenuBar();

    if ( iExtension->ProcessCommandL(aCommandId) )
        {
        return; // command processed
        }

    switch (aCommandId)
        {
        case EPbkCmdAddItem:
            {
            CmdAddItemL();
            break;
            }

        case EPbkCmdDeleteItem:
            {
            CmdDeleteItemL();
            break;
            }

        case EPbkCmdEditItemLabel:
            {
            CmdEditItemLabelL();
            break;
            }

        case EPbkCmdFetchThumbnail:
            {
            CmdFetchThumbnailL();
            break;
            }

        case EPbkCmdRemoveThumbnail:
            {
            CmdRemoveThumbnailL();
            break;
            }
        
        case EAknCmdHelp:
            {
            HlpLauncher::LaunchHelpApplicationL(iEikonEnv->WsSession(),
                iEikonEnv->EikAppUi()->AppHelpContextL());
            break;
            }

        case EEikCmdExit:   // FALLTHROUGH
        case EAknCmdExit:   // Exit calling application
            {
            PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkContactEditorDlg::ProcessCommandL exit"));

            // If exit callback returned EFalse, the exit is cancelled
            if (iExitCallback && !iExitCallback->OkToExitL(aCommandId))
                {
                break;
                }
            const TInt exitCommandId = iExitCommandId;
            CEikAppUi* appUi = iEikonEnv->EikAppUi();
            // Close this dialog first
            delete this;
            if (exitCommandId)
                {
                // Exit application
                static_cast<MEikCommandObserver*>(appUi)->ProcessCommandL(exitCommandId);
                }
            else
                {
                // Exit application
                static_cast<MEikCommandObserver*>(appUi)->ProcessCommandL(aCommandId);
                }
            break;
            }

        default:
            {
            // Command not handled here, forward it to the app ui.
            iEikonEnv->EikAppUi()->HandleCommandL(aCommandId);
            break;
            }
        }

    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkContactEditorDlg::ProcessCommandL end"));
    }

void CPbkContactEditorDlg::DynInitMenuPaneL
        (TInt aResourceId,
        CEikMenuPane* aMenuPane)
    {
    iExtension->DynInitMenuPaneL(aResourceId, aMenuPane);

    iEditorStrategy->DynInitMenuPaneL(aResourceId, aMenuPane);

    // Options menu
    if (aResourceId == R_CONTACTEDITOR_MENUPANE)
        {
        // don't show 'Exit' command if it is disabled
        if (iStateFlags.IsSet(EHideExit))
            {
            aMenuPane->SetItemDimmed(EAknCmdExit,ETrue);
            }

        iThumbnailCmd->DynInitMenuPaneL(aResourceId, *aMenuPane, iContactItem);
        }

    // Common items in Options and context menus
    if (aResourceId == R_CONTACTEDITOR_MENUPANE ||
        aResourceId == R_PBK_CONTACTEDITOR_CONTEXT_MENUPANE)
        {
        // filter delete item away from menu for first and last name fields
        // and from the syncronization field
        MPbkContactEditorField* currentField = iFieldManager->Find(IdOfFocusControl());
        if (currentField)
            {
            const TPbkFieldId fieldId = currentField->FieldId();
            if (fieldId == EPbkFieldIdLastName || fieldId == EPbkFieldIdFirstName ||
                fieldId == EPbkFieldIdSyncronization)
                {
                aMenuPane->SetItemDimmed(EPbkCmdDeleteItem, ETrue);
                }
            }
        }
    }

TBool CPbkContactEditorDlg::OkToExitL(TInt aKeyCode)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
        ("CPbkContactEditorDlg::OkToExitL(0x%x,%d)"), this, aKeyCode);

    // editor can't be closed if this leaves
    TRAPD(err, iExtension->PreOkToExitL(aKeyCode));

    TBool ret = EFalse;
    switch (aKeyCode)
        {
        case EAknSoftkeyOptions:
            {
            DisplayMenuL();
            break;
            }
        case EAknSoftkeyDone:
            {
            if (!iThumbnailCmd->OkToExit())
                {
                // ThumbnailCmd state is not ready for exit
                break;
                }
            if(iEngine.Constants()->
               LocallyVariatedFeatureEnabled(EPbkLVCheckDuplicateEntry))
                {
                ret = CheckForDuplicateEntryL();
                }
            else
                {
                if (err != KErrNone)
                    {
                    // cannot affect on editor closing, but let's show
                    // that something was wrong
                    iCoeEnv->HandleError(err);
                    }
                TRAP(err,CmdDoneL());
                ret = ETrue;
                }
            break;
            }
        case EAknSoftkeyChange: // Fallthrough
        case EAknSoftkeyContextOptions:
            {
            // Launch context menu if the control is not popup control
            CEikCaptionedControl* line=CurrentLine();
            TInt type=line->iControlType;
            TBool isPopUp = ( type == EAknCtPopupFieldText ) ||
                ( type == EAknCtPopupField );
            if (isPopUp)
                {
                CAknPopupField* ctrl =
                    static_cast<CAknPopupField*>( line->iControl );
                ctrl->ActivateSelectionListL();
                }
            else
                {
                iContextMenuBar->SetMenuTitleResourceId( R_PBK_CONTACTEDITOR_CONTEXT_MENUBAR );
                iContextMenuBar->SetMenuType( CEikMenuBar::EMenuContext );
                iContextMenuBar->StopDisplayingMenuBar();
                iContextMenuBar->TryDisplayMenuBarL();
                iContextMenuBar->SetMenuTitleResourceId( R_PBK_CONTACTEDITOR_MENUBAR );
                iContextMenuBar->SetMenuType( CEikMenuBar::EMenuOptions );
                }
            break;
            }
        default:
            break;
        }

    if (err == KErrNone)
        {
        TRAP(err, iExtension->PostOkToExitL(aKeyCode));
        }
    if (err != KErrNone)
        {
        // cannot affect on editor closing, but let's show
        // that something was wrong
        iCoeEnv->HandleError(err);
        }
    return ret;
    }

void CPbkContactEditorDlg::HandleControlStateChangeL
        (TInt aControlId)
    {
    MPbkContactEditorField* changedField = iFieldManager->Find(aControlId);
    if (changedField)
        {
        if ( iEngine.IsTitleField(changedField->FieldId()) )
            {
            UpdateTitlePaneL();
            }
        }
    }

/**
 * Closes popups opened from this dialog by sending them Escape key events.
 * @see CPbkContactEditorDlg::OfferKeyEventL
 */
void CPbkContactEditorDlg::CloseAllPopups()
    {
    const TInt KMaxAttempts = 50;
    iStateFlags.Clear(EEscKeyReceived);
    // Send Escape key until this dialog itself receives it
    for (TInt attempt=0;
        attempt < KMaxAttempts && !iStateFlags.IsSet(EEscKeyReceived);
        ++attempt)
        {
        TRAP_IGNORE(iEikonEnv->SimulateKeyEventL(KEscKeyEvent,EEventKey));
        }
    }


/*  This function will check to see if the user entered information, coming out of the
  *  TPbkContactItemFields, matches (exactly) any entries in the contact database, based
  *  on the results of the GetContactTitle function. This function will only return what
  *  is being displayed to the user. For example, a first name of "Eric", no last name, and
  *  a company name of "Nokia" will only display "Eric" and would match an entry with
  *  just the first name of "Eric".
  *
  *  The search has already been performed and ANY entry that matches any of the fields
  *  the user has entered (first name, last name, and/or company name) is passed into
  *  this function in the aIdArray parameter.
  *
  *  We are checking for entries that are separate from the current entry the user is
  *  creating / editing. Therefore, if the user is editing an existing entry, we don't want
  *  to signal a match when we find that specific entry. We can check this based on the stored
  *  variable iContactItem and it's Id() function.
  */
TBool CPbkContactEditorDlg::DuplicateNameMatchL
        (CContactIdArray* aIdArray)
    {
    TBool exactMatch = EFalse;
    // just check the titles of the current item with the titles of each of the
    // potentially matched items

    HBufC* title = iContactItem.GetContactTitleL();
    CleanupStack::PushL(title);

    // iterate through each contact in aIdArray
    for (TInt count = 0; count < aIdArray->Count(); count++)
        {
        // only continue with this check if we aren't checking our own entry. check with ID
        if (iContactItem.Id() != (*aIdArray)[count])
            {
            // read the current contact
            CPbkContactItem* contact = iEngine.ReadContactLC((*aIdArray)[count]);

            // get the title of this current contact to compare
            HBufC* contactTitle = contact->GetContactTitleL();
            CleanupStack::PushL(contactTitle);

            // now just compare the titles of this loop contact and the user-entered contact
            if (!title->CompareF(*contactTitle))
                {
                exactMatch = ETrue;
                CleanupStack::PopAndDestroy(2);     // name, contact
                break;
                }
            CleanupStack::PopAndDestroy(2);         // name, contact
            }
        }

    CleanupStack::PopAndDestroy(title);
    return exactMatch;
    }

 /*
  *  This function pulls user-entered info from the dialog for firstname, lastname and companyname.
  *  We search the contact database on the first of these that is non-empty and not all spaces. If
  *  we come up with any matches, we pass this info to the function DuplicateNameMatch, which checks
  *  that all three fields match the found entries from the database search. If this is the case,
  *  we pass back an ETrue value, EFalse otherwise.
  */
TBool CPbkContactEditorDlg::ContactAlreadyExistsL()
    {
    TBool found = EFalse;
    CContactIdArray* idArray = NULL;

    CPbkFieldIdArray* findFrom = new(ELeave) CPbkFieldIdArray;
    CleanupStack::PushL(findFrom);

    // add all fields, firstname, lastname and companyname to our contact database find parameter
    findFrom->AppendL(EPbkFieldIdFirstName);
    findFrom->AppendL(EPbkFieldIdLastName);
    findFrom->AppendL(EPbkFieldIdCompanyName);

    // get local copies of user-filled-in contact fields' text values
    TPbkContactItemField* firstName = iContactItem.FindField(EPbkFieldIdFirstName);
    TPbkContactItemField* lastName = iContactItem.FindField(EPbkFieldIdLastName);
    TPbkContactItemField* companyName = iContactItem.FindField(EPbkFieldIdCompanyName);

    // this is terribly slow
    if (firstName && !firstName->IsEmptyOrAllSpaces())
        {
        idArray = iEngine.FindLC(firstName->Text(), findFrom);
        }
    else if (lastName && !lastName->IsEmptyOrAllSpaces())
        {
        idArray = iEngine.FindLC(lastName->Text(), findFrom);
        }
    else if (companyName && !companyName->IsEmptyOrAllSpaces())
        {
        idArray = iEngine.FindLC(companyName->Text(), findFrom);
        }
    else
        {
        // fname, lname and cname all empty - check for other "Unnamed" entries
        // Two unnamed contacts are considered duplicates
        idArray = iEngine.FindLC(KNullDesC, findFrom);
        }

    // if we have at least one match, check that all fields match
    if (idArray)
        {
        if (idArray->Count() > 0 && DuplicateNameMatchL(idArray))
            {
            found = ETrue;
            }
        CleanupStack::PopAndDestroy(idArray);
        }

    CleanupStack::PopAndDestroy(findFrom);
    return found;
    }


TBool CPbkContactEditorDlg::CheckForDuplicateEntryL()
    {
    TBool retVal = ETrue;
    TBool exitNormally = ETrue;

    // Check for duplicate entry and give the user the option to
    // save duplicate or return to enter more information.

    // To perform that check, we must save the info the user has
    // just entered (just locally, not in the contact database)
    if (iFieldManager)
        {
        iFieldManager->SaveFieldsL();
        if ( iFieldManager->FieldsChanged() )
            {
            iEditorStrategy->SetStateModified();
            }

        // Totally empty contacts are not checked for duplicates
        if ( !iFieldManager->AreAllFieldsEmpty() )
            {
            // If contact already exists, inform the user and given them
            // the option to add the duplicate, or go back to the screen
            // and enter information
            if ( ContactAlreadyExistsL() )
                {
                // Get contact title to display
                HBufC* name = iContactItem.GetContactTitleL();
                CleanupStack::PushL( name );

                HBufC* prompt = StringLoader::LoadL(R_QTN_PHOB_ADD_DUPLICATE,
                    *name );
                CleanupStack::PushL( prompt );

                CAknQueryDialog* dlg = CAknQueryDialog::NewL();
                CleanupStack::PushL( dlg );
                dlg->SetPromptL( *prompt );
                CleanupStack::Pop(); // dlg
                // If user hits no, we'll put them back to the edit screen
                if ( !dlg->ExecuteLD(R_PBK_GENERAL_CONFIRMATION_QUERY_PBKVIEW) )
                    {
                    exitNormally = EFalse;
                    retVal = EFalse;
                    }
                CleanupStack::PopAndDestroy( 2 ); // prompt, name
                }
            }
        }

    if (exitNormally)
        {
        CmdDoneL();                     // saving occurs here
        retVal = ETrue;
        }

    return retVal;
    }

EXPORT_C void CPbkContactEditorDlg::SetExitCallback
    (MPbkEditorOkToExitCallback* aCallback)
    {
    iExitCallback = aCallback;
    }

void CPbkContactEditorDlg::ReloadThumbnailL()
    {
    if ( iAvkonAppUi->StatusPane()->
        PaneCapabilities(TUid::Uid(EEikStatusPaneUidContext)).IsInCurrentLayout() )
        {
        if (IsFocused())
            {
            if (!iThumbnailHandler)
                {
                iThumbnailHandler = CPbkThumbnailPopup::NewL(iEngine);
                }
            iThumbnailHandler->Load(iContactItem, NULL );
            }
        else
            {
            if (iThumbnailHandler)
                {
                iThumbnailHandler->CancelLoading();
                }
            }   
        }
    }

// End of File
