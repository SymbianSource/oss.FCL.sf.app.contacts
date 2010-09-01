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
*       Provides methods for the phonebook engine compression UI.
*
*/


// INCLUDE FILES
#include "CPbkCompressUiImpl.h"
#include <CPbkContactEngine.h>
#include <PbkDebug.h>


// ==================== MEMBER FUNCTIONS ====================

inline CPbkCompressUiImpl::CPbkCompressUiImpl(CPbkContactEngine& aEngine) :
    iEngine(aEngine)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkCompressUiImpl::CPbkCompressUiImpl(0x%x)"), this);
    iEngine.SetCompressUi(this);
    }

EXPORT_C CPbkCompressUiImpl* CPbkCompressUiImpl::NewL(CPbkContactEngine& aEngine)
    {
    return new(ELeave) CPbkCompressUiImpl(aEngine);
    }

EXPORT_C CPbkCompressUiImpl::~CPbkCompressUiImpl()
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkCompressUiImpl::~CPbkCompressUiImpl(0x%x)"), this);
    iEngine.SetCompressUi(NULL);
    iEngine.CancelCompress();
    }

EXPORT_C void CPbkCompressUiImpl::EnableCompression(TBool aEnable)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkCompressUiImpl::EnableCompression(0x%x,%d)"), this, aEnable);
    if (!aEnable)
        {
        iEngine.CancelCompress();
        }
    iCompressionEnabled = aEnable;
    }

TBool CPbkCompressUiImpl::PbkCompressCanStartNow()
    {
    return iCompressionEnabled;
    }

void CPbkCompressUiImpl::PbkCompressStartL(TInt /*aStepsTogo*/)
    {
    }

void CPbkCompressUiImpl::PbkCompressStep(TInt /*aStep*/)
    {
    }

void CPbkCompressUiImpl::PbkCompressComplete()
    {
    }

void CPbkCompressUiImpl::PbkCompressCanceled()
    {
    }

void CPbkCompressUiImpl::PbkCompressError(TInt /*aError*/)
    {
    }


// End of File
