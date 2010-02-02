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
* Description:  Phonebook 2 profiling support.
*
*/


#ifndef PBK2PROFILE_H
#define PBK2PROFILE_H

#include <e32def.h>
#include <e32std.h>
#include "Pbk2Config.hrh"

/**
 * Phonebook 2 profiling implementation.
 *
 */
#ifdef PBK2_ENABLE_PROFILE

    /* profiling implementation */
    #define PBK2_PROFILE_START(aBin) \
        Pbk2Profile::TPbk2Profiling::GlobalL()->StartProfile(aBin);

    #define PBK2_PROFILE_END(aBin) \
        Pbk2Profile::TPbk2Profiling::GlobalL()->EndProfile(aBin);

    /// Needs PBK2_ENABLE_DEBUG_LOGGER
    #define PBK2_PROFILE_DISPLAY(aBin)
    #define PBK2_PROFILE_RESET(aBin)
    #define PBK2_PROFILE_DELETE() \
        delete Pbk2Profile::TPbk2Profiling::GlobalL()

#else // PBK2_ENABLE_PROFILE

    /* Profiling not enabled - empty implementation */
    #define PBK2_PROFILE_START(aBin)
    #define PBK2_PROFILE_END(aBin)
    #define PBK2_PROFILE_DISPLAY(aBin)
    #define PBK2_PROFILE_RESET(aBin)
    #define PBK2_PROFILE_DELETE()

#endif // PBK2_ENABLE_PROFILE


namespace Pbk2Profile {

    // CONSTANTS
    enum TPbk2ProfileBin
        {
        /////////////////////////////////////////////////////////////////////
        // Measures full startup.
        /////////////////////////////////////////////////////////////////////
        /**
         * NewApplication() (CPbk2Application.cpp)
         *    -->
         * CPbk2NamesListAppView::HandleNamesListControlEventL
         *   (TPbk2NamesListControlEvent::EReady)
         */
        EFullStartup                        =  1,

        /////////////////////////////////////////////////////////////////////
        // Following bins are in sequential order and there is no overlapping
        /////////////////////////////////////////////////////////////////////
        EAppUiConstruction = 2, // CPbk2AppUi::ConstructL
        EAppUiBaseConstruct = 3, // call to AppUI::BaseContructL
        EAppUiResouceFileLoading = 4, // call to load needed resource files
        EAppUiCreateGlobals = 5, // Call to create application globals
        EAppUiCreateViews = 6, // call to view explorer to create view

        /////////////////////////////////////////////////////////////////////
        // EAppUiConstruction split in several parts
        /////////////////////////////////////////////////////////////////////
        EAppUiCreateViewExplorer = 7,   // 45000
        EAppUiCreateStoreManager = 8,
        EAppUiCreateAppUiExtension = 9,
        EAppUiCreateStartupMonitor = 10,
        EAppUiOpenStores = 11,
        EViewExplorerCreateAppViewFactory = 12,
        EViewExplorerCreateAppView = 13,
        EViewExplorerAddView = 14,

        /////////////////////////////////////////////////////////////////////
        // Names list view split in several parts
        /////////////////////////////////////////////////////////////////////
        /**
         * CPbk2NamesListAppView::ConstructL's BaseConstructL call.
         */
        ENamesListViewBaseConstruct         = 15,

        /**
         * CPbk2NamesListAppView::CreateControlsL.
         */
        ENamesListViewCreateControls        = 16,

        /**
         * CPbk2NamesListAppView::DoActivateL.
         */
        ENamesListViewDoActivateL           = 17,

        /////////////////////////////////////////////////////////////////////
        // Contact info view split in several parts
        /////////////////////////////////////////////////////////////////////
        /**
         * CPbk2ContactInfoAppView::ConstructL's BaseConstructL call.
         */
        EContactInfoViewBaseConstruct       = 18,

        /////////////////////////////////////////////////////////////////////
        // AIW
        /////////////////////////////////////////////////////////////////////
        /**
         * CPbk2AiwInterestItemBase::AttachL's menu attach.
         */
        EAiwMenuAttach                      = 19,

        /**
         * CPbk2AiwInterestItemBase::AttachL's base service attach.
         */
        EAiwBaseServiceAttach               = 20,

        /////////////////////////////////////////////////////////////////////
        // App view factory create view split in several parts
        /////////////////////////////////////////////////////////////////////

        EAppViewFactoryCreateAppViewL       = 21,
        EAppViewFactoryNamesListAppViewNewL = 23,   // costly
        EAppViewFactoryContactInfoAppViewNewL = 24,
        EAppViewFactorySettingsAppViewNewL  = 25,

        /////////////////////////////////////////////////////////////////////
        // Names list app view construction split in several parts
        /////////////////////////////////////////////////////////////////////
        ENamesListAppViewConstruction       = 26,
        ENamesListGetCommandHandler         = 27, // 5000
        ENamesListAIWRegisterInterestCall   = 28, // 53125
        ENamesListAIWRegisterInterestPOC    = 29, // 100000
        ENamesListAIWRegisterInterestSyncML = 30, // 55000
        ENamesListRegisterStoreConfiguration = 31,
        ENamesListPenSupportConstruction    = 32,

        /////////////////////////////////////////////////////////////////////
        // Names list create control split in several parts
        /////////////////////////////////////////////////////////////////////
        ENamesListViewCreateContainer = 33,     // 5000
        ENamesListViewSetHelpContext = 34,      // 0
        ENamesListViewGetContactManager = 35,   // 0
        ENamesListViewCreateNamesListControl = 36, // 87500
        ENamesListViewSetControl = 37,              // 0
        ENamesListViewAddToStack = 38,              // 0

        /////////////////////////////////////////////////////////////////////
        // Names list DoActivateL split in several parts
        /////////////////////////////////////////////////////////////////////
        ENamesListViewDoActivateAddMenuCommandObserver = 39,    // 0
        ENamesListViewDoActivateLoadTitle = 40,                 // 0
        ENamesListViewDoActivateHandleViewActivation = 41,      // 5000
        ENamesListViewDoActivateParamHandling = 42,             // 0
        ENamesListViewDoActivateActivateControl = 43,           // 15000
        ENamesListViewDoActivateNotifyViewActivatation = 44,    // 44000
        ENamesListViewDoActivateToolbar = 45,                   // 0

        /////////////////////////////////////////////////////////////////////
        // Document Create globals
        /////////////////////////////////////////////////////////////////////
        EDocumentCreateExtensionManager = 47,

        /////////////////////////////////////////////////////////////////////
        // Shared objects construction split in several parts
        /////////////////////////////////////////////////////////////////////
        EAppServicesCreateStoreManager = 48,     // 15000
        EAppServicesCreateStorePropertyArray = 49,   // 5000
        EAppServicesCreateFieldPropertyArray = 50,   // 5000
        EAppServicesCreateStoreConfiguration = 51,   // 0
        EAppServicesCreateSortOrderManager = 52,     // 5000

        /////////////////////////////////////////////////////////////////////
        // UI Extension manager creation split in several parts
        /////////////////////////////////////////////////////////////////////
        EUiExtensionManagerCreateExtensionLoader = 53,  // 55000
        EUiExtensionManagerCreateConfigurationObserver = 55, // 0
        EUiExtensionManagerCreateThinExtensionLoader = 56,  // 45000
        EUiExtensionManagerCreateMenuManager = 57,          // 0

        /////////////////////////////////////////////////////////////////////
        // UI Extension loader creation split in several parts
        /////////////////////////////////////////////////////////////////////
        EUiExtensionloaderLoadResourceFile = 58,    // 5000
        EUiExtensionloaderListImplementations = 59, // 0
        EUiExtensionloaderLoopExtensionPlugins = 60,    // 10000
        EUiExtensionloaderLoadStartupExtensions = 61,   // 45000

        /////////////////////////////////////////////////////////////////////
        // Thin UI Extension loader creation split in several parts
        /////////////////////////////////////////////////////////////////////
        EThinUiExtensionLoaderListImplementations = 62, // 0
        EThinUiExtensionLoaderLoopImplementations = 63, // 45000

        /////////////////////////////////////////////////////////////////////
        // View explorer construction split in several parts
        /////////////////////////////////////////////////////////////////////
        EViewExplorerConstructionReadViewGraph = 64, // 0
        EViewExplorerConstructionGetNaviPane = 65,  // 0
        EViewExplorerConstructionCreateTabGroup = 66, // 45000

        /////////////////////////////////////////////////////////////////////
        // Tab group construction split in several parts
        /////////////////////////////////////////////////////////////////////
        ETabGroupCreateTabGroups = 67, // 26000
        ETabGroupSkinTabGroups = 68,    // 15000
        
        /////////////////////////////////////////////////////////////////////
        // Sort policy's compare
        /////////////////////////////////////////////////////////////////////
        ESortPolicyCompareContacts = 69,
        ESortPolicySortUtil = 70,

        /////////////////////////////////////////////////////////////////////
        // Contact info start up analysis split to several parts
        /////////////////////////////////////////////////////////////////////
        EContactInfoConstructL = 71,        // 2000
        EContactInfoStartTime = 72,         // 106520, 95875 (view loading, doactivate to view ready)
        EContactInfoDoActivateContactChanged = 73,  // 112500, 108875 (from doactivate to contact changed call, spanning start time)
        EContactInfoDoActivateL = 74,           // 71000, 61750 (doactivate)
        EContactInfoContactChangedL = 75,       // 685500, 4784000
        EContactInfoCreateControlsL = 76,       // 52000, 43750 
        EContactInfoAttachAIWProvidersL = 77,   // 0, 0
        EContactInfoContactChangedLUpdateControls = 78,     // 4732375
        EContactInfoContactChangedLSetFocusedField = 79,    // 0
        EContactInfoContactChangedLRestoreControlState = 80,    // 2000
        EContactInfoContactChangedLCommitViewTransaction = 81, // 0
        EContactInfoContactChangedLNotifyStartupMonitor = 82, // 0

        /////////////////////////////////////////////////////////////////////
        // Contact info control update split
        /////////////////////////////////////////////////////////////////////
        EContactInfoControlUpdateL = 83,                                //
        EContactInfoControlUpdateLCreateStoreSpecificFieldProps = 84,   // 0
        EContactInfoControlUpdateLCreatePresentationContact = 85,       // 1000
        EContactInfoControlUpdateLCreateTitlePanePicture = 86,          // 10000
        EContactInfoControlUpdateLCreateSwapper = 87,                   // 0
        EContactInfoControlUpdateLStorePanes = 88,                      // 1000
        EContactInfoControlUpdateLCreateDynamicFieldProperties = 89,    // 1000
        EContactInfoControlUpdateLPrepareForUpdate = 90,                // 4651250
        EContactInfoControlUpdateLSetTitlePane = 91,                    // 4000
        EContactInfoControlUpdateLUpdateListbox = 92,                   // 2000
        EContactInfoControlUpdateLCleanupState = 93,                    // 0
        EContactInfoControlUpdateLSetIndex = 94,                        // 0
        EContactInfoControlUpdateLUpdateNavigators = 95,                // 14000
        EContactInfoControlUpdateLSendEvents = 96,                      // 11000

        /////////////////////////////////////////////////////////////////////
        // CPbk2ContactInfoListBox
        /////////////////////////////////////////////////////////////////////
        EContactInfoListBoxPrepareForUpdateLCreateListBoxModel = 97,    // 4634625
        EContactInfoListBoxPrepareForUpdateLStoreListBoxState = 98,     // 0
        EContactInfoListBoxCreateListBoxModelLTimeFormat = 99,          // 1000
        EContactInfoListBoxCreateListBoxModelLCreateListBoxModel = 100, // 1000
        EContactInfoListBoxCreateListBoxModelLFormatFields = 101,       // 4622625
        EContactInfoListBoxCreateListBoxModelLSetClipper = 102,         // 0

        ////////////////////////////////////////////////////////////////////
        // CPbk2FieldListBoxModel
        ////////////////////////////////////////////////////////////////////
        EContactInfoListBoxModelFormatFieldsLFieldAt = 103,             // 0
        EContactInfoListBoxModelFormatFieldsLFormatField = 104,         // 587000
        EContactInfoListBoxModelFormatFieldLShowField = 105,            // 0
        EContactInfoListBoxModelFormatFieldLCreateFieldListBoxRow = 106,    // 0
        EContactInfoListBoxModelFormatFieldLGetMasterFieldTypeList = 107, // 0
        EContactInfoListBoxModelFormatFieldLMatchFieldType = 108,   // 0
        EContactInfoListBoxModelFormatFieldLAppendIcons = 109,      // 0
        EContactInfoListBoxModelFormatFieldLAppendFieldLabel = 110, // 0
        EContactInfoListBoxModelFormatFieldLAppendFieldContent = 111,   // 1000
        EContactInfoListBoxModelFormatFieldLAppendEndIcons = 112,   // 587000
        EContactInfoListBoxModelFormatFieldLExpandBuffer = 113,     // 0
        EContactInfoListBoxModelFormatFieldLAppendRow = 114,        // 0

        /////////////////////////////////////////////////////////////////////
        // End icons
        /////////////////////////////////////////////////////////////////////
        EContactInfoListBoxModelAppendEndIconSpeedDial = 115,       // 13000
        EContactInfoListBoxModelAppendEndIconSpeedDialIcon = 116,   // 0
        EContactInfoListBoxModelAppendEndIconVoiceTag = 117,        // 421250
        EContactInfoListBoxModelAppendEndIconVoiceTagIcon = 118,    // 0

        KEndProfileBins = 119
        };


/**
 * Utility class for Phonebook 2 profiling.
 */
class TPbk2Profiling
    {
    public: // Interface

        /**
         * Returns the global shared object of this class.
         * Creates the object if it does not exist.
         *
         * @return  Instance of this class.
         */
        IMPORT_C static TPbk2Profiling* GlobalL();

        /**
         * Destructor.
         */
        IMPORT_C ~TPbk2Profiling();

        /**
         * Starts profiling.
         *
         * @param aBin  Integer identifying the profile.
         */
        IMPORT_C void StartProfile(
                TInt aBin );

        /**
         * Ends profiling.
         *
         * @param aBin  Integer identifying the profile.
         */
        IMPORT_C void EndProfile(
                TInt aBin ) const;

    private: // Implementation
        TPbk2Profiling();

    private:
        /// Own: Flat C array
        TTime iProfileArray[KEndProfileBins];
    };

}  // namespace Pbk2Profile

#endif // PBK2PROFILE_H

// End of File
