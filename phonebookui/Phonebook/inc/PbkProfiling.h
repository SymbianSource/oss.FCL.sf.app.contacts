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
*     Phonebook profiling bin allocation.
*
*/


#ifndef __PbkProfiling_H__
#define __PbkProfiling_H__

namespace PbkProfiling
    {

    // CONSTANTS
    enum TPbkProfilingBin
        {
        EFullStartup                        =  1,
        // Following bins are in sequential order and there is no overlapping
        EBeforeAppUiConstruction            =  2, // before CPbkAppUi::ConstructL gets called
        EAppUiConstruction                  =  3, // CPbkAppUi::ConstructL
        EStartupViewActivation              =  4, // from CPbkAppUi::ConstructL ==> CPbkStartupView::DoActivateL
        EStartupViewDoActivateL             =  5, 
        EExtensionStartup                   =  6, // from CPbkStartupView::DoActivateL ==> CPbkStartupView::HandleStartupComplete
        EHandleStartupComplete              =  7, // CPbkStartupView::HandleStartupComplete
        ENamesListViewActivation            =  8, // from CPbkStartupView::HandleStartupComplete ==> CPbkNamesListView::DoActivateL
        ENamesListViewDoActivateL           =  9,
        ENamesListContactViewOpen           = 10, // from CPbkNamesListView::DoActivateL ==> CPbkNamesListAppView::HandleContactViewListControlEventL
        // end sequential ordering here
        
        // EAppUiConstruction split in several parts
        EInitFeatureManager                 = 11,
        EAppUiBaseConstruct                 = 12,
        EEngineConstruction                 = 13,
        EAppUiExtensionConstruction         = 14,
        EViewNaviConstruct                  = 15,
        ESendUiConstruction                 = 16,
        EAppUiOtherMemberConstruction       = 17,
        EViewConstruction                   = 18,

        // ENamesListViewDoActivateL split in several parts
        ENamesListViewLoadTitle             = 19,
        ENameListViewActivationTransaction  = 20,
        ENamesListViewCheckParameters       = 21,
        ENamesListViewOpenContactsView      = 22,
        ENamesListViewContainerConstruct    = 23,
        ENamesListViewActivateControl       = 24,

        // ENamesListViewContainerConstruct split in several parts
        ENamesListViewCreateContainer       = 25,
        ENamesListViewCreateExtension       = 26,
        ENamesListViewCreateControl         = 27,

        // EAppUiExtensionConstruction split in several parts
        EAppUiExtGlobalInstance             = 28,
        EPbkExtGlobalsScannerConstruct      = 29,
        EExtensionScan                      = 30,
        EMultiFactoryConstruct              = 31,
        EPbkExtMultiAppUiConstruct          = 32,
        EPbkExtCreatePbkAppUiExtensionL     = 33,
        EPbkExtAppendAppUiExtension         = 34,

        // EExtensionScan split in several parts
        EEComUiExtensionScanPrepare         = 35,
        EEComUiExtensionScan                = 36,
        EEComUiExtensionLoadPrepare         = 37,
        EEComUiExtensionLoadAndInit         = 38,
        EPolymorphicUiExtensionLoadPrepare  = 39,
        EPolymorphicUiExtensionLoad         = 40,        
        EPolymorphicUiExtensionEntryLookup  = 41,
        EPolymorphicUiExtensionInit         = 42,
        
        // EViewConstruction split in several parts
        EAddView                            = 43,
        EViewBaseConstruct                  = 44,
        EViewAiwAttach                      = 45,

        EPolymorphicUiExtensionScan         /*= 46 */, // this is actually included in EPbkExtGlobalsScannerConstruct
        EEcomEngineExtensionScan            /*= 47 */,
        EEcomEngineExtensionLoadAndInit     /*= 48 */,
        EInitEngineResources                /*= 49 */,
        EContactDbOpen                      /*= 50 */,
        EAllContactsViewOpen                /*= 51 */,
        EContactsMatchingCriteria           /*= 52 */,
        EFilteredViewOpen                   /*= 53 */,
        KEndProfileBins
        };

    }  // namespace PbkProfiling

#endif // __PbkProfiling_H__

// End of File
