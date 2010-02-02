/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements MPbk2ExtensionIconSupport API
*
*/



// INCLUDE FILES
#include "CPbk2UIExtensionIconSupport.h"

#include "CPbk2UIExtensionLoader.h"
#include "CPbk2UIExtensionInformation.h"
#include "CPbk2UIExtensionIconInformation.h"

#include <CPbk2IconArray.h>
#include <CPbk2IconInfoContainer.h>

#include <coemain.h>
#include <barsread.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPbk2UIExtensionIconSupport::CPbk2UIExtensionIconSupport
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPbk2UIExtensionIconSupport::CPbk2UIExtensionIconSupport(
        CPbk2UIExtensionLoader& aExtensionLoader) :   
    iExtensionLoader(aExtensionLoader)
    {
    }

// Destructor
CPbk2UIExtensionIconSupport::~CPbk2UIExtensionIconSupport()
    {    
    }

// -----------------------------------------------------------------------------
// CPbk2UIExtensionIconSupport::AppendExtensionIconsL
// -----------------------------------------------------------------------------
//       
void CPbk2UIExtensionIconSupport::AppendExtensionIconsL(
        CPbk2IconArray& aIconArray)
    {
    // append icons from the extension resources to aIconArray
    TArray<CPbk2UIExtensionInformation*> extInfoArray = 
        iExtensionLoader.PluginInformation();
    const TInt count = extInfoArray.Count();
    for (TInt i = 0; i < count; ++i)
        {
        CPbk2UIExtensionInformation* extInfo = extInfoArray[i];
        if (extInfo->HasIconExtension())
            {
            TInt iconArrayResId = KErrNotFound;
            if (extInfo->IconInformation().FindIconArrayExtension(
                aIconArray.Id(), iconArrayResId))
                {
                TResourceReader reader;
                CCoeEnv::Static()->CreateResourceReaderLC( reader, 
                        iconArrayResId );

                // create the icon info container from the extension info
                CPbk2IconInfoContainer* container = 
                    CPbk2IconInfoContainer::NewL( 
                        extInfo->IconInformation().IconInfoArrayResourceId() );
                CleanupStack::PushL( container );

                // append the icons from the extension
                aIconArray.AppendIconsFromResourceL( reader, *container );
                CleanupStack::PopAndDestroy(2);
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CPbk2UIExtensionIconSupport::RefreshL
// -----------------------------------------------------------------------------
//       
void CPbk2UIExtensionIconSupport::RefreshL(
        CPbk2IconArray& aIconArray)
    {
    TArray<CPbk2UIExtensionInformation*> extInfoArray = 
        iExtensionLoader.PluginInformation();
    const TInt count = extInfoArray.Count();
    for (TInt i = 0; i < count; ++i)
        {
        CPbk2UIExtensionInformation* extInfo = extInfoArray[i];
        if (extInfo->HasIconExtension())
            {
            TInt iconArrayResId = KErrNotFound;
            if (extInfo->IconInformation().FindIconArrayExtension(
                aIconArray.Id(), iconArrayResId))
                {
                CPbk2IconInfoContainer* container = 
                    CPbk2IconInfoContainer::NewL( 
                        extInfo->IconInformation().IconInfoArrayResourceId() );
                CleanupStack::PushL( container );
                aIconArray.RefreshL( *container );
                CleanupStack::PopAndDestroy();
                }
            }
        }
    }

//  End of File  
