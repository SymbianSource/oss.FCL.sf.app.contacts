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
* Description: 
*       Provides methods for CPbk2ImageManager implementation classes.
*
*/


// INCLUDE FILES
#include "CPbk2ImageReaderBase.h"

// From Phonebook2
#include "CPbk2ImageReader.h"
#include "MPbk2ImageOperationObservers.h"

// From system
#include <fbs.h>
#include <imageconversion.h>

/// Unnamed namespace for local defintions
namespace {

enum TPanicCode
    {
    EPanicPreCond_CancelRead = 1,
    EPanicPreCond_StartReadL,
    EPanic_ImageReadComplete_Called,
    EPanic_ImageReadFailed_Called
    };

#ifdef _DEBUG
void Panic(TPanicCode aPanicCode)
    {
    _LIT(KPanicText, "CPbk2ImageReaderBase");
    User::Panic(KPanicText, aPanicCode);
    }
#endif // _DEBUG

} // namespace

// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// CPbk2ImageReaderBase::CPbk2ImageReaderBase
// --------------------------------------------------------------------------
//
CPbk2ImageReaderBase::CPbk2ImageReaderBase
        (MPbk2ImageGetObserver& aObserver) :
    iObserver(aObserver)
    {
    }

// --------------------------------------------------------------------------
// CPbk2ImageReaderBase::~CPbk2ImageReaderBase
// --------------------------------------------------------------------------
//
CPbk2ImageReaderBase::~CPbk2ImageReaderBase()
    {
    delete iImageReader;
    }
    
// --------------------------------------------------------------------------
// CPbk2ImageReaderBase::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2ImageReaderBase::ConstructL()
    {
    iImageReader = CPbk2ImageReader::NewL(*this);
    }

// --------------------------------------------------------------------------
// CPbk2ImageReaderBase::CancelRead
// --------------------------------------------------------------------------
//
void CPbk2ImageReaderBase::CancelRead()
    {
    __ASSERT_DEBUG(iImageReader, Panic(EPanicPreCond_CancelRead));
    iImageReader->Cancel();
    }

// --------------------------------------------------------------------------
// CPbk2ImageReaderBase::ImageReadComplete
// --------------------------------------------------------------------------
//
void CPbk2ImageReaderBase::ImageReadComplete
        (CPbk2ImageReader& /*aReader*/, CFbsBitmap* aBitmap)
    {
    iObserver.Pbk2ImageGetComplete(*this,aBitmap);
    }

// --------------------------------------------------------------------------
// CPbk2ImageReaderBase::ImageReadFailed
// --------------------------------------------------------------------------
//
void CPbk2ImageReaderBase::ImageReadFailed
        (CPbk2ImageReader& /*aReader*/, TInt aError)
    {
    iObserver.Pbk2ImageGetFailed(*this,aError);
    }
    
// --------------------------------------------------------------------------
// CPbk2ImageReaderBase::ImageOpenComplete
// --------------------------------------------------------------------------
//
void CPbk2ImageReaderBase::ImageOpenComplete(CPbk2ImageReader& /*aReader*/)
	{
	// do nothing
	}

//  End of File
