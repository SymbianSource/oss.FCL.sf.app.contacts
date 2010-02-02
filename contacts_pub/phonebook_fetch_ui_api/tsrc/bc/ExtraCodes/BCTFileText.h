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








#ifndef __BCTFILETEXT_H__
#define __BCTFILETEXT_H__

//  INCLUDES
#include <E32BASE.H>
#include <f32file.h>    // file related stuff
#include <s32file.h>    // stream...

// CLASS DECLARATION
class CCnvCharacterSetConverter;
class RFileReadStream;
class RFs;

class CBCTFileText :public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CBCTFileText* NewL(const TFileName& aFileName);
        
        /**
        * Destructor.
        */
        virtual ~CBCTFileText();
    
    private:
        /**
        * C++ default constructor.
        */
        CBCTFileText();

    public: // New functions
        TInt ReadLineL(TDes8& buf);
        TInt ReadLineL(TDes16& buf);
        
    private:
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL(const TFileName& aFileName);

        void ConvertToUnicodeL(CCnvCharacterSetConverter& aCharacterSetConverter,
             TUint aForeignCharacterSet, const TDesC8& aForeignText, TDes16& aConvertedText);

        TInt GetLineFromStreamL(TDes8& buf);

        TInt GetByteFromStreamL(TInt8& c);

        // Prohibit copy constructor if not deriving from CBase.
        // CBCTFileText( const CBCTFileText& );
        // Prohibit assigment operator if not deriving from CBase.
        // CBCTFileText& operator= ( const CBCTFileText& );

    private:    // Data
        RFileReadStream iSourceStream;
        RFs iFileServerSession;
        CCnvCharacterSetConverter* iConverter;
    };

#endif // __BCTFILETEXT_H__
            
// End of File
