/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


ENTRY("OK test","CNone",
	CMT_VPbkEng_SelectorFactory::Setup,
	CMT_VPbkEng_SelectorFactory::TestPassed,
	CMT_VPbkEng_SelectorFactory::Teardown),

ENTRY("FieldSelector Valid","VPbkFieldTypeSelectorFactory",
	CMT_VPbkEng_SelectorFactory::Setup,
	CMT_VPbkEng_SelectorFactory::TestFieldSelectorFactoryValid,
	CMT_VPbkEng_SelectorFactory::Teardown),

ENTRY("FieldSelector Invalid","VPbkFieldTypeSelectorFactory",
	CMT_VPbkEng_SelectorFactory::Setup,
	CMT_VPbkEng_SelectorFactory::TestFieldSelectorFactoryInvalid,
	CMT_VPbkEng_SelectorFactory::Teardown),

ENTRY("CommSelector Valid","VPbkFieldTypeSelectorFactory",
	CMT_VPbkEng_SelectorFactory::Setup,
	CMT_VPbkEng_SelectorFactory::TestCommSelectorFactoryValid,
	CMT_VPbkEng_SelectorFactory::Teardown),

ENTRY("CommSelector uni editor","VPbkFieldTypeSelectorFactory",
	CMT_VPbkEng_SelectorFactory::Setup,
	CMT_VPbkEng_SelectorFactory::TestCommSelectorFactoryUniEditor,
	CMT_VPbkEng_SelectorFactory::Teardown),

ENTRY("CommSelector Invalid","VPbkFieldTypeSelectorFactory",
	CMT_VPbkEng_SelectorFactory::Setup,
	CMT_VPbkEng_SelectorFactory::TestCommSelectorFactoryInvalid,
	CMT_VPbkEng_SelectorFactory::Teardown),

ENTRY("TestCommSelectorFactoryNoList","VPbkFieldTypeSelectorFactory",
	CMT_VPbkEng_SelectorFactory::Setup,
	CMT_VPbkEng_SelectorFactory::TestCommSelectorFactoryNoList,
	CMT_VPbkEng_SelectorFactory::Teardown),

ENTRY("TestUniEditorSelectionL","VPbkFieldTypeSelectorFactory",
	CMT_VPbkEng_SelectorFactory::Setup,
	CMT_VPbkEng_SelectorFactory::TestUniEditorSelectionL,
	CMT_VPbkEng_SelectorFactory::Teardown),

ENTRY("TestVoiceCallSelectionL","VPbkFieldTypeSelectorFactory",
	CMT_VPbkEng_SelectorFactory::Setup,
	CMT_VPbkEng_SelectorFactory::TestVoiceCallSelectionL,
	CMT_VPbkEng_SelectorFactory::Teardown),

ENTRY("TestVoipNotSupportedSelectionL","VPbkFieldTypeSelectorFactory",
	CMT_VPbkEng_SelectorFactory::Setup,
	CMT_VPbkEng_SelectorFactory::TestVoipNotSupportedSelectionL,
	CMT_VPbkEng_SelectorFactory::Teardown),

ENTRY("Test No Voip available","VPbkFieldTypeSelectorFactory",
	CMT_VPbkEng_SelectorFactory::Setup,
	CMT_VPbkEng_SelectorFactory::TestVoipNotAvailableSelectionL,
	CMT_VPbkEng_SelectorFactory::Teardown),

ENTRY("Test Voip Enabled","VPbkFieldTypeSelectorFactory",
	CMT_VPbkEng_SelectorFactory::SetupVoipL,
	CMT_VPbkEng_SelectorFactory::TestVoipCallSelectionL,
	CMT_VPbkEng_SelectorFactory::Teardown),

ENTRY("Test Voip Call Out Enabled","VPbkFieldTypeSelectorFactory",
	CMT_VPbkEng_SelectorFactory::SetupVoipCallOutL,
	CMT_VPbkEng_SelectorFactory::TestVoipCallOutSelectionL,
	CMT_VPbkEng_SelectorFactory::Teardown),

ENTRY("Test xsp supports voip","VPbkFieldTypeSelectorFactory",
	CMT_VPbkEng_SelectorFactory::SetupGtalkSupportsVoipL,
	CMT_VPbkEng_SelectorFactory::TestXspSupportsVoipSelectionL,
	CMT_VPbkEng_SelectorFactory::Teardown),

ENTRY("TestVideoSelectorL","VPbkFieldTypeSelectorFactory",
	CMT_VPbkEng_SelectorFactory::Setup,
	CMT_VPbkEng_SelectorFactory::TestVideoSelectorL,
	CMT_VPbkEng_SelectorFactory::Teardown),

ENTRY("TestURLSelectorL","VPbkFieldTypeSelectorFactory",
	CMT_VPbkEng_SelectorFactory::Setup,
	CMT_VPbkEng_SelectorFactory::TestURLSelectorL,
	CMT_VPbkEng_SelectorFactory::Teardown),

ENTRY("TestEmailSelectorL","VPbkFieldTypeSelectorFactory",
	CMT_VPbkEng_SelectorFactory::Setup,
	CMT_VPbkEng_SelectorFactory::TestEmailSelectorL,
	CMT_VPbkEng_SelectorFactory::Teardown),

ENTRY("TestIMSelectorL","VPbkFieldTypeSelectorFactory",
	CMT_VPbkEng_SelectorFactory::Setup,
	CMT_VPbkEng_SelectorFactory::TestIMSelectorL,
	CMT_VPbkEng_SelectorFactory::Teardown),

ENTRY("Test gtalk contact selection","VPbkFieldTypeSelectorFactory",
	CMT_VPbkEng_SelectorFactory::SetupGtalkSupportsVoipL,
	CMT_VPbkEng_SelectorFactory::TestGtalkContactSelectionL,
	CMT_VPbkEng_SelectorFactory::Teardown),

ENTRY("Test two services contact selection","VPbkFieldTypeSelectorFactory",
	CMT_VPbkEng_SelectorFactory::Setup,
	CMT_VPbkEng_SelectorFactory::TestTwoServicesContactSelectionL,
	CMT_VPbkEng_SelectorFactory::Teardown),

ENTRY("TestVersion1SelectorL","VPbkFieldTypeSelectorFactory",
	CMT_VPbkEng_SelectorFactory::Setup,
	CMT_VPbkEng_SelectorFactory::TestVersion1SelectorL,
	CMT_VPbkEng_SelectorFactory::Teardown),

ENTRY("Test builder multiple selections","VPbkFieldTypeSelectorFactory",
	CMT_VPbkEng_SelectorFactory::Setup,
	CMT_VPbkEng_SelectorFactory::TestMultipleSelectionL,
	CMT_VPbkEng_SelectorFactory::Teardown),

ENTRY("TestNonVersitTypeSelectionL","VPbkFieldTypeSelectorFactory",
	CMT_VPbkEng_SelectorFactory::Setup,
	CMT_VPbkEng_SelectorFactory::TestNonVersitTypeSelectionL,
	CMT_VPbkEng_SelectorFactory::Teardown),
