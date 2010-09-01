/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
*       Provides methods for the test dll.
*
*/








// INCLUDE FILES
#include <charconv.h>   // unicode conversion  
//#include <f32file.h>    // file related stuff
//#include <s32file.h>    // stream...
#include "BCTFileText.h"


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CBCTFileText::CBCTFileText()
    {
    }

// ---------------------------------------------------------
// CBCTFileText::ConstructL.
// ---------------------------------------------------------
//
void CBCTFileText::ConstructL(const TFileName& aFileName)
    {
    // Create unicode converter
    iConverter = CCnvCharacterSetConverter::NewL();
    // Connect to fileserversession
    User::LeaveIfError(iFileServerSession.Connect());
    // Create readstream
	User::LeaveIfError(iSourceStream.Open(iFileServerSession, aFileName, EFileRead));    
    }

// ---------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------
//
CBCTFileText* CBCTFileText::NewL(const TFileName& aFileName)
    {
    CBCTFileText* self = new (ELeave) CBCTFileText();
    
    CleanupStack::PushL( self );
    self->ConstructL(aFileName);
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CBCTFileText::~CBCTFileText()
    {
    delete iConverter;
    iSourceStream.Close() ;
    iFileServerSession.Close();
    }

/**
* Read line from file to 8bit desc.
*/ 
TInt CBCTFileText::ReadLineL(TDes8& buf)
    {
        return GetLineFromStreamL(buf);   
    }

/**
* Read line from file to 16bit desc.
*/ 
TInt CBCTFileText::ReadLineL(TDes16& buf)
    {
    buf.Zero();
    
    HBufC8* tmpHeap = HBufC8::NewMaxLC(buf.MaxLength()); 
    TPtr8 tpmBuf = tmpHeap->Des();

    TInt err;

    err = GetLineFromStreamL(tpmBuf);
    if (!err)
        {
        ConvertToUnicodeL(*iConverter,KCharacterSetIdentifierIso88591,tpmBuf,buf);
        }

    CleanupStack::PopAndDestroy();
    return err;
    }

/**
* Get byte from stream.
*/ 
TInt CBCTFileText::GetByteFromStreamL(TInt8& c)
    {
    TRAPD(err, c = iSourceStream.ReadInt8L());
    return err;
    }

/**
* Get line from stream.
*/ 
TInt CBCTFileText::GetLineFromStreamL(TDes8& buf)
    {    
    const TInt8 LINEFEED = '\n';
    const TInt8 CR = '\r';
    TInt8 c;
    TInt err;
    buf.Zero();
    
    while((err = GetByteFromStreamL(c)) == KErrNone)
        {
        if(c == LINEFEED)
            {
            break;
            }
        if(c != CR)
            {            
            if (buf.Length() < buf.MaxLength())
                {
                buf.Append(c);
                }
            else
                {
                return buf.Length();
                }
            }
        }
    return err;
    }

/**
* Convert text to unicode.
*/ 
void CBCTFileText::ConvertToUnicodeL(CCnvCharacterSetConverter& aCharacterSetConverter,
            TUint aForeignCharacterSet, const TDesC8& aForeignText, TDes16& aConvertedText)
    {       
    // reset the output buffer
    aConvertedText.SetLength(0);
    // check to see if the character set is supported 
    // - if not then leave
    if (aCharacterSetConverter.PrepareToConvertToOrFromL(aForeignCharacterSet, 
                    iFileServerSession) != CCnvCharacterSetConverter::EAvailable)
        User::Leave(KErrNotSupported);
    // Create a small output buffer 
    TBuf16<20> outputBuffer;
    // Create a buffer for the unconverted text - initialised with the input descriptor
    TPtrC8 remainderOfForeignText(aForeignText);
    // Create a "state" variable and initialise it with CCnvCharacterSetConverter::KStateDefault
    // After initialisation the state variable must not be tampered with.
    // Simply pass into each subsequent call of ConvertToUnicode()
    TInt state=CCnvCharacterSetConverter::KStateDefault;

    for(;;) // conversion loop
            {
            // Start conversion. When the output buffer is full, return the number
            // of characters that were not converted
            const TInt returnValue=aCharacterSetConverter.ConvertToUnicode(outputBuffer, remainderOfForeignText, state);
            // check to see that the descriptor isn’t corrupt - leave if it is
            if (returnValue==CCnvCharacterSetConverter::EErrorIllFormedInput)
                User::Leave(KErrCorrupt);
            else if (returnValue<0) // future-proof against "TError" expanding
                User::Leave(KErrGeneral);
            // Store the contents of the output buffer.
            aConvertedText.Append(outputBuffer);
            // Finish conversion if there are no unconverted 
            // characters in the remainder buffer
            if (returnValue==0)
                break;
            // Remove converted source text from the remainder buffer.
            // The remainder buffer is then fed back into loop
            remainderOfForeignText.Set(remainderOfForeignText.Right(returnValue));
            }
    }

//  End of File  
