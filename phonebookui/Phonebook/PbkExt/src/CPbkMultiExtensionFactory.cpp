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
*       Composite extension factory.
*
*/


// INCLUDE FILES
#include "CPbkMultiExtensionFactory.h"
#include "CPbkMultiAppUiExtension.h"
#include "CPbkMultiViewExtension.h"
#include "CPbkMultiContactEditorExtension.h"

// Debugging headers
#include <PbkDebug.h>
#include "PbkProfiling.h"

// CONSTANTS
const TInt KFactoryGranularity = 2;

// ==================== MEMBER FUNCTIONS ====================
CPbkMultiExtensionFactory::CPbkMultiExtensionFactory() :
    iFactories( KFactoryGranularity )
    {
    }

CPbkMultiExtensionFactory::~CPbkMultiExtensionFactory()
    {
    iFactories.Reset();
    }

CPbkMultiExtensionFactory* CPbkMultiExtensionFactory::NewL()
    {
    CPbkMultiExtensionFactory* self = new (ELeave) CPbkMultiExtensionFactory;
    return self;
    }

void CPbkMultiExtensionFactory::AppendL( MPbkExtensionFactory* aFactory )
    {
    iFactories.AppendL(aFactory);
    }

// This method is getting deprecated, use the overload instead
MPbkViewExtension* CPbkMultiExtensionFactory::CreatePbkViewExtensionL
    (TUid aId, CPbkContactEngine& aEngine)
    {
    CPbkMultiViewExtension* multiViewExt = CPbkMultiViewExtension::NewL();
    CleanupStack::PushL(multiViewExt);

	const TInt count = iFactories.Count();
    for (TInt i=0; i<count; ++i)
        {
        MPbkViewExtension* viewExt = 
            iFactories[i]->CreatePbkViewExtensionL(aId, aEngine);
        if (viewExt)
            {
            CleanupStack::PushL(viewExt);
            multiViewExt->AppendL(viewExt);
            CleanupStack::Pop(viewExt);
            }
        }
    CleanupStack::Pop(multiViewExt);
    return multiViewExt;
    }

MPbkViewExtension* CPbkMultiExtensionFactory::CreatePbkViewExtensionL
    (TUid aId, CPbkContactEngine& aEngine, CPbkAppViewBase& aAppView)
    {
    CPbkMultiViewExtension* multiViewExt = CPbkMultiViewExtension::NewL();
    CleanupStack::PushL(multiViewExt);

	const TInt count = iFactories.Count();
    for (TInt i=0; i<count; ++i)
        {
        // Try with the new CreatePbkViewExtensionL first
        MPbkViewExtension* viewExt = 
            iFactories[i]->CreatePbkViewExtensionL(aId, aEngine, aAppView);
        if (!viewExt)
            {
            // If the new overload was not implemented by extension,
            // try with the traditional one
            viewExt = 
                iFactories[i]->CreatePbkViewExtensionL(aId, aEngine);
            }
        
        if (viewExt)
            {
            CleanupStack::PushL(viewExt);
            multiViewExt->AppendL(viewExt);
            CleanupStack::Pop(viewExt);
            }
        }
    CleanupStack::Pop(multiViewExt);
    return multiViewExt;
    }

MPbkContactEditorExtension* 
	CPbkMultiExtensionFactory::CreatePbkContactEditorExtensionL
    (CPbkContactItem& aContact, 
     CPbkContactEngine& aEngine, 
     MPbkContactEditorControl& aEditorControl)
    {
    CPbkMultiContactEditorExtension* multiContactEditorExt =
        CPbkMultiContactEditorExtension::NewL();
    CleanupStack::PushL(multiContactEditorExt);

    const TInt count = iFactories.Count();
    for (TInt i=0; i<count; ++i)
        {
        MPbkContactEditorExtension* contactEditorExt =
            iFactories[i]->CreatePbkContactEditorExtensionL
                (aContact, aEngine, aEditorControl);
        if (contactEditorExt)
            {
            CleanupStack::PushL(contactEditorExt);
            multiContactEditorExt->AppendL(contactEditorExt);
            CleanupStack::Pop(contactEditorExt);
            }
        }

    CleanupStack::Pop(multiContactEditorExt);
    return multiContactEditorExt;
    }

MPbkContactUiControlExtension* 
    CPbkMultiExtensionFactory::CreatePbkUiControlExtensionL
	(CPbkContactEngine& aEngine)
    {
    const TInt count = iFactories.Count();
    for (TInt i=0; i<count; ++i)
        {
        MPbkContactUiControlExtension* contactUiControlExt =
            iFactories[i]->CreatePbkUiControlExtensionL(aEngine);
        if (contactUiControlExt)
            {
            return contactUiControlExt;
            }
        }
    return &iDummyControlExtension;
    }

MPbkContactUiControlExtension* 
    CPbkMultiExtensionFactory::CreateDummyPbkUiControlExtensionL()
    {
	return &iDummyControlExtension;
    }

CAknView* CPbkMultiExtensionFactory::CreateViewL(TUid aId)
    {
    const TInt count = iFactories.Count();
	for (TInt i=0; i<count; ++i)
        {
        CAknView* view = iFactories[i]->CreateViewL(aId);
        if (view)
            {
            return view;
            }
        }
    return NULL;
    }

MPbkAppUiExtension* CPbkMultiExtensionFactory::CreatePbkAppUiExtensionL
    (CPbkContactEngine& aEngine)
    {
    __PBK_PROFILE_START(PbkProfiling::EPbkExtMultiAppUiConstruct);    
    CPbkMultiAppUiExtension* multiAppUiExt = CPbkMultiAppUiExtension::NewL();
    CleanupStack::PushL(multiAppUiExt);
    __PBK_PROFILE_END(PbkProfiling::EPbkExtMultiAppUiConstruct);

    const TInt count = iFactories.Count();
    for (TInt i=0; i<count; ++i)
        {
        __PBK_PROFILE_START(PbkProfiling::EPbkExtCreatePbkAppUiExtensionL);
        MPbkAppUiExtension* appUiExt = 
            iFactories[i]->CreatePbkAppUiExtensionL(aEngine);
        __PBK_PROFILE_END(PbkProfiling::EPbkExtCreatePbkAppUiExtensionL);
        __PBK_PROFILE_START(PbkProfiling::EPbkExtAppendAppUiExtension);
        if (appUiExt)
            {
            CleanupStack::PushL(appUiExt);
            multiAppUiExt->AppendL(appUiExt);
            CleanupStack::Pop(appUiExt);
            }
        __PBK_PROFILE_END(PbkProfiling::EPbkExtAppendAppUiExtension);
        }
    CleanupStack::Pop(multiAppUiExt);
	return multiAppUiExt;
    }

void CPbkMultiExtensionFactory::AddPbkFieldIconsL
        (CPbkIconInfoContainer* aIconInfoContainer, 
        CPbkIconArray* aIconArray)
    {
    const TInt count = iFactories.Count();
    for (TInt i=0; i<count; ++i)
        {
        iFactories[i]->AddPbkFieldIconsL(aIconInfoContainer, aIconArray);
        }
    }

void CPbkMultiExtensionFactory::AddPbkTabIconsL
        (CPbkIconInfoContainer* aIconInfoContainer, 
        CPbkIconArray* aIconArray)
    {
    const TInt count = iFactories.Count();
    for (TInt i=0; i<count; ++i)
        {
        iFactories[i]->AddPbkTabIconsL(aIconInfoContainer, aIconArray);
        }
    }

// End of File

