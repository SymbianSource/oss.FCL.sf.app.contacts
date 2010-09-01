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
* Description:    Test Class Object
*
*/





#ifndef BCTESTPBKDATAADDEDIT_H
#define BCTESTPBKDATAADDEDIT_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>

#include <cpbkcontactengine.h>
#include <MPbkEditorOkToExitCallback.h>
#include <PbkDataSaveAppUi.hrh>
#include <cpbkfieldsinfo.h> 

#include <eikmobs.h>

#include <RPbkViewResourceFile.h>
#include <coecntrl.h>
#include <eikbtgpc.h>
#include <eikrted.h>
#include <aknappui.h>
#include <avkon.rsg>
#include <avkon.hrh>

// CONSTANTS
//const ?type ?constant_var = ?constant;
//  LOCAL CONSTANTS AND MACROS
const TInt EPbkCmdExit = 12;//defined in Phonebook.hrh
const TInt KOKToExitCommand = 1;

_LIT(KContextName,"MyDlgContextName");
_LIT( KTestDBName, "c:MT_CPbkContactEditorDlg.cdb" );

#define R_PBK_DATA_SAVE_SUBMENU_TESTING_ID 0x116a2016

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 30
#define TEST_CLASS_VERSION_MINOR 9
#define TEST_CLASS_VERSION_BUILD 10

// Logging path
_LIT( KBCTestPbkDataAddEditLogPath, "\\logs\\testframework\\BCTestPbkDataAddEdit\\" ); 
// Log file
_LIT( KBCTestPbkDataAddEditLogFile, "BCTestPbkDataAddEdit.txt" ); 
_LIT( KBCTestPbkDataAddEditLogFileWithTitle, "BCTestPbkDataAddEdit_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CBCTestPbkDataAddEdit;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CBCTestPbkDataAddEdit test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CBCTestPbkDataAddEdit) : public CScriptBase, public MPbkEditorOkToExitCallback,
																					 public MEikMenuObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CBCTestPbkDataAddEdit* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CBCTestPbkDataAddEdit();

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
        
    public: //Methods from M-Classes
        TBool OkToExitL(TInt aCommandId);
        
    /**
        * MEikMenuObserver dummy implementation
        */
        void HandleAttemptDimmedSelectionL(TInt aCommandId);
        /**
        * MEikMenuObserver dummy implementation
        */
        TBool CheckHotKeyNotDimmedL(TInt aCommandId);
        /**
        * MEikMenuObserver dummy implementation
        */        
        void RestoreMenuL(CCoeControl *aMenuControl, 
            TInt aResourceId, TMenuType aType);
        /**
        * MEikMenuObserver dummy implementation
        */        
        void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane *aMenuPane);
        /**
        * MEikMenuObserver dummy implementation
        */        
        void DynInitMenuBarL(TInt aResourceId, CEikMenuBar *aMenuBar);
        /**
        * MEikMenuObserver dummy implementation
        */        
        void HandleSideBarMenuL(TInt aResourceId, 
            const TPoint &aPos, TInt aModifiers, const CEikHotKeyTable *aTable);
        /**
        * MEikMenuObserver dummy implementation
        */        
        void OfferKeyToAppL(const TKeyEvent &aKeyEvent, TEventCode aType);
        /**
        * MEikMenuObserver dummy implementation
        */        
        void SetEmphasis(CCoeControl *aMenuControl, TBool aEmphasis);
        /**
        * MEikMenuObserver dummy implementation
        */        
        CCoeControl *CreateCustomCommandControlL(TInt aControlType);
        /**
        * MEikMenuObserver dummy implementation
        */        
        void ProcessCommandL(TInt aCommandId);

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
        CBCTestPbkDataAddEdit( CTestModuleIf& aTestModuleIf );

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
        virtual TInt TestCmnEdtrDlgNewL( CStifItemParser& aItem );
        virtual TInt TestCmnEdtrResetWhenDestroyedL( CStifItemParser& aItem );
        virtual TInt TestCmnEdtrHideExitCommandL( CStifItemParser& aItem );
        virtual TInt TestCmnEdtrSetHelpContextL( CStifItemParser& aItem );
        virtual TInt TestCmnEdtrSetExitCommandIdL( CStifItemParser& aItem );
        virtual TInt TestCmnEdtrSetExitCallbackL( CStifItemParser& aItem );
        virtual TInt TestCmnEdtrExecuteLDL( CStifItemParser& aItem );
        virtual TInt TestDataSaveNewL( CStifItemParser& aItem );
        virtual TInt TestDataSaveHandleCommandL( CStifItemParser& aItem );
        virtual TInt TestDataSaveAddMenuItemsL( CStifItemParser& aItem );
        virtual TInt TestDataSaveDynInitMenuPaneL( CStifItemParser& aItem );
        
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
    		RPbkViewResourceFile 		iPbkViewResourceFile;
        
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
    
/**
* MPbkEditorOkToExitCallback implementation for testing SetExitCallbackL
*/ 



#endif      // BCTESTPBKDATAADDEDIT_H

// End of File
