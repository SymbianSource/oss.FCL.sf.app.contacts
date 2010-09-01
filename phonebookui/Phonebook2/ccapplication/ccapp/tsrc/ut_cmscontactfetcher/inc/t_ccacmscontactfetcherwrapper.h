/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/



#ifndef T_CCCACMSCONTACTFETCHERWRAPPER_H
#define T_CCCACMSCONTACTFETCHERWRAPPER_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>

#include "ccapputilheaders.h"
// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KT_CCCACmsContactFetcherWrapperLogPath, "\\logs\\testframework\\T_CCCACmsContactFetcherWrapper\\" ); 
// Log file
_LIT( KT_CCCACmsContactFetcherWrapperLogFile, "T_CCCACmsContactFetcherWrapper.txt" ); 
_LIT( KT_CCCACmsContactFetcherWrapperLogFileWithTitle, "T_CCCACmsContactFetcherWrapper_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class T_CCCACmsContactFetcherWrapper;
class CStopSchedulerAfterDelay;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  T_CCCACmsContactFetcherWrapper test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(T_CCCACmsContactFetcherWrapper) : public CScriptBase, 
public MCCAppContactFieldDataObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static T_CCCACmsContactFetcherWrapper* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~T_CCCACmsContactFetcherWrapper();

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
        T_CCCACmsContactFetcherWrapper( CTestModuleIf& aTestModuleIf );

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
        virtual TInt CreateAndDeleteL( CStifItemParser& aItem );
        
        TInt CreateWithContentL ( CStifItemParser& aItem );
        
        TInt CreateWithManyItemsL ( CStifItemParser& aItem );
        
        TInt CreateWithContactLinkL( CStifItemParser& aItem );
        
        TInt CreateWithMSISDNL( CStifItemParser& aItem );
        
        TInt CreateWithEmailL( CStifItemParser& aItem );
        
        TInt NoDataFieldsL( CStifItemParser& aItem );
        
        TInt ErrorInContactOpenL( CStifItemParser& aItem );
        
        TInt ErrorInContactFetchL( CStifItemParser& aItem );
        
        TInt HandlePhonebookNotificationL( CStifItemParser& aItem );
        
        TInt TestContactStoreApiL( CStifItemParser& aItem );
        
        TInt FindFromOtherStoresL( CStifItemParser& aItem );
                
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();
        
        void ContactFieldDataObserverNotifyL( 
        MCCAppContactFieldDataObserver::TParameter& aParameter );
        
        void ContactFieldDataObserverHandleErrorL( 
        TInt aState, TInt aError );   
        
        TInt SetupL( TInt aTestCase, TInt aParameterType );
        
        void CompleteRequestL( TRequestStatus* aStatus, TInt aReason );

        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        
        TInt iObserverCalledCount;
        TBool iObserverCalled;
        TBool iErrorObserverCalled;
        TInt iOngoingTestCase;
        TInt iSchedulerStarted;
        TBool iObserverCalledWithContactsChanged;
        
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

class CStopSchedulerAfterDelay : public CTimer
     {
     public:
         CStopSchedulerAfterDelay(): CTimer( EPriorityStandard )
             {
             CActiveScheduler::Add( this );
             };
         virtual ~CStopSchedulerAfterDelay(){};
         void ConstructL()
             {
             CTimer::ConstructL();
             }
         void RunL()
             {
             CActiveScheduler::Stop();
             }
     };

#endif      // T_CCCACMSCONTACTFETCHERWRAPPER_H

// End of File
