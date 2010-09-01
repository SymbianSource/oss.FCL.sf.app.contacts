/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the ccacontactor
*
*/


// INCLUDE FILES
#include "ccacontactorheaders.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCAContactor::ExecuteServiceL()
// -----------------------------------------------------------------------------
//
void CCAContactor::ExecuteServiceL(VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aCommMethod, 
		const TDesC& aParam, const TDesC& aName, const TDesC8& aContactLinkArray, TUint32 aServiceId)
    {
    CCCAContactorOperation* operation = 0;
    
    switch (aCommMethod)
        {
        case VPbkFieldTypeSelectorFactory::EVoiceCallSelector:              
            operation = CCCAContactorCallOperation::NewL(aParam, aContactLinkArray);
            break;
            
        case VPbkFieldTypeSelectorFactory::EVideoCallSelector:
            operation = CCCAContactorVideocallOperation::NewL(aParam, aContactLinkArray);
            break;
        case VPbkFieldTypeSelectorFactory::EUniEditorSelector:
            operation = CCCAContactorUniEditorOperation::NewL(aParam, aName);
            break;
        case VPbkFieldTypeSelectorFactory::EEmailEditorSelector:
            operation = CCCAContactorEmailOperation::NewL(aParam, aName);
            break;
        case VPbkFieldTypeSelectorFactory::EURLSelector:
            operation = CCCAContactorURLOperation::NewL(aParam);
            break;
        case VPbkFieldTypeSelectorFactory::EVOIPCallSelector:
            operation = CCCAContactorVOIPOperation::NewL(aParam, aServiceId, aContactLinkArray);
            break;
        case VPbkFieldTypeSelectorFactory::EInstantMessagingSelector:
            MCmsContactorImPluginParameter* cmsParameter;
            cmsParameter = CCmsContactorImPluginParameter::NewL(
                    aName, aContactLinkArray);	   
            operation = CCCAContactorIMOperation::NewL(aParam,cmsParameter);
            break;
        
        }
    if (operation)
        {
        operation->ExecuteLD();
        operation = NULL;
        }
    }
// End of File

