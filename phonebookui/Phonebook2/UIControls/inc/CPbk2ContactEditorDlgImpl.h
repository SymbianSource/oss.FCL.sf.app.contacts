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
* Description:  Phonebook 2 contact editor dialog implementation.
*
*/


#ifndef CPBK2CONTACTEDITORDLGIMPL_H
#define CPBK2CONTACTEDITORDLGIMPL_H

//  INCLUDES
#include <AknForm.h>
#include <babitflags.h>
#include <coeutils.h>
#include <MPbk2ContactEditorControl.h>
#include "MPbk2ContactEditorUiBuilder.h"
#include "MPbk2ContactEditorUiBuilderExtension.h"
#include "MPbk2ContactEditorControlExtension.h"
#include "MPbk2ContactEditorEventObserver.h"
#include "MPbk2DialogEliminator.h"
#include "TPbk2ContactEditorParams.h"
#include <MVPbkContactStoreObserver.h>
#include "MPbk2BaseCommand.h"
#include "MPbk2ImageFieldObserver.h"

// FORWARD DECLARATIONS
class CAknTitlePane;
class MPbk2ContactEditorStrategy;
class CPbk2FieldPropertyArray;
class CPbk2PresentationContact;
class CPbk2ContactEditorFieldArray;
class MPbk2ContactNameFormatter;
class CPbk2UIExtensionManager;
class MPbk2ContactEditorExtension;
class MPbk2EditedContactObserver;
class MPbk2ContactEditorContactRelocator;
class CAknNavigationControlContainer;
class MPbk2ContactEditorUIField;
class CPbk2ContactEditorFieldFactory;
class CAknNavigationDecorator;
class CPbk2ContactEditorArrayItem;
class CAknInputBlock;
class MPbk2ApplicationServices;

// CLASS DECLARATION

/**
 * Phonebook 2 contact editor dialog implementation.
 */
class CPbk2ContactEditorDlgImpl : public CAknForm,
                                  public MPbk2ContactEditorUiBuilder,
                                  public MPbk2ContactEditorUiBuilderExtension,
                                  public MPbk2ContactEditorEventObserver,
                                  public MPbk2DialogEliminator,
                                  public MPbk2ContactEditorControl,
                                  public MPbk2ContactEditorControlExtension,
                                  public MVPbkContactStoreObserver,
                                  public MPbk2ImageFieldObserver
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aParams           Contact editor parameters.
         * @param aContact          The contact to edit.
         * @param aFieldProperties  Field properties.
         * @param aContactObserver  Observer for the contact being edited.
         * @param aEditorStrategy   Contact editing strategy.
         * @param aRelocator        Contact relocator.
         * @return  A new instance of this class.
         */
        static CPbk2ContactEditorDlgImpl* NewL(
                TPbk2ContactEditorParams& aParams,
                CPbk2PresentationContact& aContact,
                CPbk2FieldPropertyArray& aFieldProperties,
                MPbk2EditedContactObserver& aContactObserver,
                MPbk2ContactEditorStrategy& aEditorStrategy,
                MPbk2ContactEditorContactRelocator& aRelocator );
        
        /**
		* Creates a new instance of this class.
		*
		* @param aParams           Contact editor parameters.
		* @param aContact          The contact to edit.
		* @param aFieldProperties  Field properties.
		* @param aContactObserver  Observer for the contact being edited.
		* @param aEditorStrategy   Contact editing strategy.
		* @param aRelocator        Contact relocator.
		* @param aAppServices	   Pointer to Pbk2 application services
		* @param aTitleText		   ownership is transferred
		* @return  A new instance of this class.
		*/
        static CPbk2ContactEditorDlgImpl* NewL(
				TPbk2ContactEditorParams& aParams,
				CPbk2PresentationContact& aContact,
				CPbk2FieldPropertyArray& aFieldProperties,
				MPbk2EditedContactObserver& aContactObserver,
				MPbk2ContactEditorStrategy& aEditorStrategy,
				MPbk2ContactEditorContactRelocator& aRelocator,
				MPbk2ApplicationServices* aAppServices,
				HBufC* aTitleText = NULL );

        /**
         * Destructor.
         */
        virtual ~CPbk2ContactEditorDlgImpl();

    public: // Interface

        /**
         * Executes the dialog.
         */
        void ExecuteLD();

        /**
         * Adds item to the contact. If the given parameter is KErrNotFound
         * the add item dialog is run and the user selects the field type.
         *
         * @param aFieldTypeResourceId      Specifies the field type to add.
         * @param aFieldTypeXspName         Field type xSP name.
         */
        void AddItemToContactL(
                TInt aFieldTypeResourceId, 
                const TDesC& aFieldTypeXspName, 
                TBool aSetFocus = ETrue );

        /**
         * Closes an executing dialog nicely with TryExitL and
         * without saving any contact data.
         *
         * @param aInformObserver       Indicates whether the observer
         *                              should be informed about dialog
         *                              closure.
         */
        void CloseWithoutSaving( TBool aInformObserver );

        /**
         * Tries to set the focus to the first field which is of the
         * type of the given type.
         *
         * @aFieldTypeResId     Field type resource id.
         * @param aFieldTypeXspName         Field type xSP name.
         */
        void TryChangeFocusWithTypeIdL(
                TInt aFieldTypeResId, const TDesC& aFieldTypeXSpName );

    public: // From CCoeControl
        TKeyResponse OfferKeyEventL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );
        void GetHelpContext(
                TCoeHelpContext& aContext ) const;

    public: // From CAknForm
        void PreLayoutDynInitL();
        void PostLayoutDynInitL();
        void SetInitialCurrentLine();
        void ProcessCommandL(
                TInt aCommandId );
        void DynInitMenuPaneL(
                TInt aResourceId,
                CEikMenuPane* aMenuPane );
        TBool OkToExitL(
                TInt aKeycode );
        void HandleControlStateChangeL(
                TInt aControlId );
        //Let Avkon know from which base class control has been 
        //derived from in order to use the correct layout method.
        MEikDialogPageObserver::TFormControlTypes 
            ConvertCustomControlTypeToBaseControlType(TInt aControlType) const;        
        //Provide custom control
        SEikControlInfo CreateCustomControlL(TInt aControlType);
        void HandlePointerEventL( const TPointerEvent& aPointerEvent );
        // Handles a change to the control's resources.
        void HandleResourceChange( TInt aType );
        
    public: // From MPbk2ContactEditorUiBuilder
        CCoeControl* CreateLineL(
                const TDesC& aCaption,
                TInt aControlId,
                TInt aControlType );
        CEikCaptionedControl* LineControl(
                TInt aControlId ) const;
        void DeleteControl(
                TInt aControlId );
        void TryChangeFocusL(
                TInt aControlId );
        void SetCurrentLineCaptionL(
                const TDesC& aText );
        CCoeControl* Control(
                TInt aControlId ) const;
        void SetEditableL(
                TBool aState );
        void LoadBitmapToFieldL(
                const MPbk2FieldProperty& aFieldProperty,
                const CPbk2IconInfoContainer& aIconInfoContainer,
                TInt aControlId );
        TAny* ContactEditorUiBuilderExtension( TUid aExtensionUid );
        
    public: // From MPbk2ContactEditorUiBuilderExtension        
        CCoeControl* AddCustomFieldToFormL(TInt aIndex, TInt aResourceId); 
        TBool HandleCustomFieldCommandL(TInt aCommand );        

    public: // From MPbk2ContactEditorEventObserver
        void ContactEditorOperationCompleted(
                MVPbkContactObserver::TContactOpResult aResult,
                MPbk2ContactEditorEventObserver::TParams aParams );
        void ContactEditorOperationFailed(
                MVPbkContactObserver::TContactOp aOpCode,
                TInt aErrorCode,
                MPbk2ContactEditorEventObserver::TParams aParams,
                MPbk2ContactEditorEventObserver::TFailParams& aFailParams );

    public: // From MPbk2DialogEliminator
        void RequestExitL(
                TInt aCommandId );
        void ForceExit();
        void ResetWhenDestroyed(
                MPbk2DialogEliminator** aSelfPtr );

    public: // From MPbk2ContactEditorControl
        void SetFocus(
                TInt aIndex );
        TInt NumberOfControls() const;
        TInt IndexOfCtrlType(
                TPbk2FieldCtrlType aType,
                TInt& aIndex ) const;
        TBool AreAllControlsEmpty() const;
        TBool IsControlEmptyL(
                TInt aIndex ) const;
        TBool IsCustomControlEmptyL(TInt aIndex, TBool& aRet) const;        
        MPbk2ContactEditorField& EditorField(
                TInt aIndex ) const;
        virtual TAny* ContactEditorControlExtension(
                        TUid /*aExtensionUid*/ );
        
    public: // From MPbk2ContactEditorControlExtension
        void UpdateControlsL();
        TInt FocusedControlType();

    public: // From MVPbkContactStoreObserver
        void StoreReady(
                MVPbkContactStore& aContactStore );
        void StoreUnavailable(
                MVPbkContactStore& aContactStore,
                TInt aReason );
        void HandleStoreEventL(
                MVPbkContactStore& aContactStore,
                TVPbkContactStoreEvent aStoreEvent );

    public: // From MPbk2ImageFieldObserver
        /**
         * Called when image has bee loaded.
         *
         */
        TInt ImageLoadingComplete();


        /**
         * Called when image loading failed.
         *
         */
        TInt ImageLoadingFailed();


        /**
         * Called when image loading was cancelled.
         *
         */
        TInt ImageLoadingCancelled();
        
    private: // From CEikDialog
        void LineChangedL(
            TInt aControlId );

    private: // Implementation
        
        void DoImageLoadingCompleteL();
        
        CPbk2ContactEditorDlgImpl(
                TPbk2ContactEditorParams& aParams,
                CPbk2PresentationContact& aContact,
                CPbk2FieldPropertyArray& aFieldProperties,
                MPbk2EditedContactObserver& aContactObserver,
                MPbk2ContactEditorStrategy& aEditorStrategy,
                MPbk2ContactEditorContactRelocator& aRelocator,
                MPbk2ApplicationServices* aAppServices = NULL,
                HBufC* aTitleText = NULL );
        void ConstructL();
        void StoreTitlePaneTextL();
        void ConstructContextMenuL();
        void ConstructNaviPaneL();
        void UpdateTitleL();
        void UpdateTitlePictureL();
        void CloseDialog(
                MVPbkContactObserver::TContactOp aOpCode,
                TBool aInformObserver );
        void ExitApplication(
                MVPbkContactObserver::TContactOp aOpCode,
                TInt aCommandId );
        void CmdAddItemL();
        void CmdDeleteItemL();
        void CmdEditItemLabelL();
        void CmdDoneL(
                MPbk2ContactEditorEventObserver::TParams& aParams );
        TInt FocusedFieldIndexL();
        void SetCbaButtonsL();
        TBool EmptyContextMenuL();
        TBool OkToExitApplicationL(
                TInt aCommandId );
        void ForwardOkToExitL( TInt aKeycode );
        void SetCbaCommandSetL( TInt aResourceId );
        void UpdateCbasL(CPbk2ContactEditorArrayItem* aItemToUpdate = NULL);
        void DimItem( CEikMenuPane* aMenuPane, TInt aCmd );
        void UnDimItem( CEikMenuPane* aMenuPane, TInt aCmd );
        TBool CheckBaseCommandL(TInt aCommandId);
        TBool ExecuteBaseCommandL(TInt aCommandId);   
        TBool CheckFieldSpecificCommandL(TInt aCommandId);
        void RemoveAndReAddCurrentFieldL();        
        void DynInitRingtoneL(
            TInt aResourceId, 
            CEikMenuPane* aMenuPane, 
            MPbk2ContactEditorField& aCurrentField );
        void DynInitImageL(
            TInt aResourceId, 
            CEikMenuPane* aMenuPane, 
            MPbk2ContactEditorField& aCurrentField );
        void DynInitSyncL(
            TInt aResourceId, 
            CEikMenuPane* aMenuPane, 
            MPbk2ContactEditorField& aCurrentField );
        void DynInitAddressL(
            TInt aResourceId, 
            CEikMenuPane* aMenuPane );
        void ExecuteAddressDlgL(TPbk2ContactEditorParams::TActiveView aActiveView);
        void ClearControls();
        TBool ContactHasAddress(TPbk2FieldGroupId aAddressGroup);
        void AddGroupFieldsL(TPbk2FieldGroupId aGroupId);
        TBool AreAllFieldsEmpty();
        TBool IsUIDataChanged();
        TBool IsAddressValidated(TPbk2FieldGroupId aGroupId);
        void DeleteActiveAddressGeoCoords();
        void SetAddressValidationIconsL();

        void WaitFinishL();
        void CloseDialog();
        
    private: // Data structures

        /**
         * Exit states.
         */
        enum TExitState
            {
            EExitOrdered,
            EExitApproved,
            EOkToExitWithoutHandlingIt
            };
        /**
         * Using states.
         */
        enum TUseState
            {
            EUseReady,
            EUseOperatingContact,
            EUseFinished
            };

    private: // Data
        /// Ref: Editor parameters
        TPbk2ContactEditorParams& iParams;
        /// Own: Context menu bar
        CEikMenuBar* iContextMenuBar;
        /// Ref: Application's title pane
        CAknTitlePane* iTitlePane;
        /// Own: Title pane text to restore on exit
        HBufC* iStoredTitlePaneText;
        /// Ref: A strategy for the editor
        MPbk2ContactEditorStrategy& iEditorStrategy;
        /// Ref: Phonebook 2 field properties
        CPbk2FieldPropertyArray& iFieldProperties;
        /// Ref: Presentation level contact
        CPbk2PresentationContact& iContact;
        /// Ref: For setting the title of the contact
        MPbk2ContactNameFormatter* iNameFormatter;
        /// Own: Manages dialog fields
        CPbk2ContactEditorFieldArray* iUiFieldArray;
        /// Own: Exit information for TExitState
        TBitFlags8 iExitRecord;
        /// Own: Contact editor extension
        MPbk2ContactEditorExtension* iEditorExtension;
        /// Ref: Observer for the contact being edited
        MPbk2EditedContactObserver& iContactObserver;
        /// Own: Add item selection index
        TInt iAddItemSelectionIndex;
        /// Ref: Contact relocator
        MPbk2ContactEditorContactRelocator& iRelocator;
        /// Ref: Extension manager
        CPbk2UIExtensionManager* iExtensionManager;
        /// Ref: Status pane handle
        CAknNavigationControlContainer* iNaviContainer;
        /// Ref: Pointer to dialog eliminator
        MPbk2DialogEliminator** iSelfPtr;
        /// Own: Current CBA command set
        TInt iCbaCommandSet;
        ///Own: Handle to basecommand object
        MPbk2BaseCommand* iPbk2BaseCommand; 
        //Ref: Temp holder for last created custom control
        CCoeControl* iLastCreatedCustomControl;
        // Own: In order to prevent execution of multiple commands in case
        //      user taps fast enough touch ui.
        TBool iCommandPending;
        /// Own: 'end-key' was pressed
        TBool iEndKeyWasPressed;
        /// Field factory
        CPbk2ContactEditorFieldFactory* iFieldFactory;
        /// Ref: Instance of Address View
        CPbk2ContactEditorDlgImpl* iAddressView;
        /// Own: If we are in stand alone Address View
        TBool iAddressViewStandalone;
        /// Dialog using state
        TUseState iUseState;
        /// Own: Wait until finish contact store operation
        CActiveSchedulerWait* iWaitFinish;
        /// Own: User input blocker
        CAknInputBlock* iInputBlock;
        // Own: Whether the service has been canceled
        TBool iServiceCanceled;
        // Own: A store contact
        MVPbkStoreContact* iStoreContact;
        // Ref: Pbk2 application services
        MPbk2ApplicationServices* iAppServices;
        // Own: Custom title text
        HBufC* iTitleText;
    };

#endif // CPBK2CONTACTEDITORDLGIMPL_H

// End of File
