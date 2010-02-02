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
* Description:        ?Description
*
*/









#ifndef BCTESTPBKVCARDIMPORTEXPORT_H
#define BCTESTPBKVCARDIMPORTEXPORT_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>

#include <cpbkcontactengine.h>
#include <f32file.h>
#include <s32file.h>


// CONSTANTS
//const ?type ?constant_var = ?constant;
_LIT(KCardBasic02, "c:\\bcardtest2\\Basic02.src");
_LIT(KCardBasic03, "c:\\bcardtest2\\basic03.vcf");
_LIT(KCardDummy, "c:\\bcardtest2\\dummy.vcf");
_LIT(KCardPicture2, "c:\\bcardtest2\\picture2.vcf");
_LIT(KCardBasicCC01, "c:\\bcardtest2\\BasicCC01.ccf");
_LIT(KCardBasicCC02, "c:\\bcardtest2\\BasicCC02.ccf");
_LIT(KCardBasicMaxSize, "c:\\bcardtest2\\BasicCompactMaxSize.ccf");
_LIT(KCardBasicNoBusiness, "c:\\bcardtest2\\BasicCompactNoBusiness.ccf");
_LIT(KCardOut01, "c:\\bcardtest2\\OutCard01.vcf");
_LIT(KCardOut02, "c:\\bcardtest2\\OutCard02.vcf");

_LIT( KTestDBName, "c:MT_CBCardEngine.cdb" );

////////////////////////////////////////

_LIT(KFName, "AnkkapaaHanhinenAnkkapaaHanhinenAnkkapaaHanhinenAn");
_LIT(KLName, "JulleJullenpoikaJulleJullenpoikaJulleJullenpoikaJu");
_LIT(KDefaultPhone,"<DEFAULTPHONE>");
_LIT(KPhoneHome, "876876876876876876876876876876876876876876876876");
_LIT(KPhoneStd, "666999666999666999666999666999666999666999666999");
_LIT(KPhoneWork, "129879129879129879129879129879129879129879129879");
_LIT(KPhoneMobile, "040-7276733040-7276733040-7276733040-7276733040-");
_LIT(KFax, "090909090909090909090909090909090909090909090909");
_LIT(KPager, "112233112233112233112233112233112233112233112233");
_LIT(KEmail, "abcdefghijklmnopqrstuvwxyzo.abcdefghijklmnopqrstuvwxyzo.ABCDEFGHIJKLMNOPQRSTUVWXYZO.ABCDEFGHIJKLMNOPQRSTUVWXYZO@abcdefghijklmnopqrstuvwxyzop.nokia.com");
_LIT(KPostalAdd, "Untamontie 3 c 16 70211 KuopioUntamontie 3 c 16 70211KuopioUntamontie 3 c 16 70211 KuopioUntamontie 3 c 16 70211 KuopioUntamontie 3 c 16 70211 KuopioUntamontie 3 c 16 70211 Kuopio Untamontie 3 c 16 70211 KuopioUntamontie 3 c 16 70211 Kuopio 012345678");
_LIT(KUrl, "http://www.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokiahttp://www.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.comhttp://www.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.forum.nokia.f.com");
_LIT(KJob, "Super SW Engineer Super SW Engineer Super SW Engin");
_LIT(KCompany, "MajorWood ltd MajorWood ltd MajorWood ltd MajorWoo");
_LIT(KCompanyAdd, "Hameentie 34 Jappila Hameentie 34 Jappila Hameentie 34 Jappila Hameentie 34 Jappila Hameentie 34 Jappila Hameentie 34 Jappila Hameentie 34 Jappila Hameentie 34 Jappila Hameentie 34 Jappila Hameentie 34 Jappila Hameentie 34 Jappila Hameentie 34 012345");
_LIT(KDate, "19440122:");
_LIT(KNote, "this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this is not lovemetal this 01234");
//picture to BCard
_LIT(KThumb, "c:\\bcardtest2\\JohnPic.GIF");

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 30
#define TEST_CLASS_VERSION_MINOR 9
#define TEST_CLASS_VERSION_BUILD 6

// Logging path
_LIT( KBCTestPbkVcardImportExportLogPath, "\\logs\\testframework\\BCTestPbkVcardImportExport\\" ); 
// Log file
_LIT( KBCTestPbkVcardImportExportLogFile, "BCTestPbkVcardImportExport.txt" ); 
_LIT( KBCTestPbkVcardImportExportLogFileWithTitle, "BCTestPbkVcardImportExport_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CBCTestPbkVcardImportExport;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CBCTestPbkVcardImportExport test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CBCTestPbkVcardImportExport) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CBCTestPbkVcardImportExport* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CBCTestPbkVcardImportExport();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );


    protected:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CBCTestPbkVcardImportExport( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        /**
        * Test methods are listed below. 
        */

        /**
        * Example test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt ExampleL( CStifItemParser& aItem );
        virtual TInt TestNewL( CStifItemParser& aItem );
        virtual TInt TestImportVCardWithMemoryL( CStifItemParser& aItem );
        virtual TInt TestEmptyStreamL( CStifItemParser& aItem );
        virtual TInt TestImportPicVCardL( CStifItemParser& aItem );
        virtual TInt TestImportCCardWithMemoryL( CStifItemParser& aItem );
        virtual TInt TestImportCCardPhoneNbrsL( CStifItemParser& aItem );
        virtual TInt TestMaxSizeCompactCardL( CStifItemParser& aItem );
        virtual TInt TestNoBusinessCompactCardL( CStifItemParser& aItem );
        virtual TInt TestSupportsFieldTypeL( CStifItemParser& aItem );
        virtual TInt TestExportCardL( CStifItemParser& aItem );
        virtual TInt TestExportCard2L( CStifItemParser& aItem );
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
    		CPbkContactEngine* iContactEngine;
    		CPbkContactItem* iContactItem;
    		RFileReadStream iReadStream;
        RFileWriteStream iWriteStream;
        
        // ?one_line_short_description_of_data
        //?data_declaration;

        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // BCTESTPBKVCARDIMPORTEXPORT_H

// End of File
