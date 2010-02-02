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
* Description:  Ps Utils test suite header file
*
*/



#ifndef PS_UTILS_TESTSUITE_H
#define PS_UTILS_TESTSUITE_H

//  SYSTEM INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <CPcsDefs.h>
#include <CPsQueryItem.h>
#include <CPsQuery.h>
#include <CPsData.h>
#include <CPsClientData.h>
#include <CPsPattern.h>
#include <CPsSettings.h>

// USER INCLUDES
#include "psutilstestsuitedefs.h"

// CLASS DECLARATION

/**
*  CPsUtilsTestSuite test class for STIF Test Framework TestScripter.
*
*  @since S60 v3.2
*/

NONSHARABLE_CLASS(CPsUtilsTestSuite) : public CScriptBase
    {
public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPsUtilsTestSuite* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CPsUtilsTestSuite();

public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * 
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

private:

        /**
        * C++ default constructor.
        */
        CPsUtilsTestSuite( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Frees all resources allocated from test methods.
        * 
        */
        void Delete();
    
    
      
private:  
	
		/**
		* This test case tests CPsQueryItem
		*
		* @param aItem Script line containing parameters.
		*/
        TInt CPsQueryItem_SetAndGetL( CStifItemParser& aItem );
                	
   
		/**
		* This test case tests CPsQuery
		*
		* @param aItem Script line containing parameters.
		*/
        TInt CPsQuery_SetAndGetL( CStifItemParser& aItem );
 
 		/**
		* This test case tests CPsClientData
		*
		* @param aItem Script line containing parameters.
		*/
	 	TInt PsClientData_SetAndGetL( CStifItemParser& aItem );
	 	
	 	/**
		* This test case tests CPsData
		*
		* @param aItem Script line containing parameters.
		*/
	 	TInt PsData_SetAndGetL( CStifItemParser& aItem );
	 	
	 	/**
		* This test case tests CPsSettings
		*
		* @param aItem Script line containing parameters.
		*/
	 	TInt PsSettings_SetAndGetL(CStifItemParser& aItem );
	 	
	 	/**
		* This test case tests CPsPattern
		*
		* @param aItem Script line containing parameters.
		*/
	 	TInt PsPattern_SetAndGetL(CStifItemParser& aItem );
 	
 		/**
		* Parses the stif input parameters
		*
		* @param aItem Script line containing parameters.
		* @param aInputParameter - Array of descriptors containing input parameters
		*/
        void Parse_StifItem(CStifItemParser& aItem,RPointerArray<TDesC>& aInputParameter);
        
		/**
		* Parses the stif input parameters for CPsQueryItem 
		*
		* @param aInputParameter - Array of descriptors containing input parameters
		* @param aCurrentMode - The input keyboard mode 
		* @param aCurrentChar - The input character
		*/
		void ParseForCPsQueryItemL(RPointerArray<TDesC>& aInputParameterArray,
									TKeyboardModes& aCurrentMode,
									TChar& aCurrentChar);
									
		/**
		* Parses the stif input parameters for CPsQuery
		*
		* @param aInputParameter - Array of descriptors containing input parameters
		* @param aCurrentMode - The input keyboard mode 
		* @param aSearchQuery - The input search query
		* @param aRemoveIndex - The item to be removed
		*/							
		void ParseForCPsQueryL(RPointerArray<TDesC>& aInputParameterArray,
														TKeyboardModes& aCurrentMode,
														CPsQuery& aSearchQuery, 
														TInt& aRemoveIndex);
	
		/**
		* Compares two CPsQuery items
		*
		* @param aInputSearchQuery - CPsQuery object1
		* @param aDestSearchQuery - CPsQuery object2
		* @return - ETrue if both objects are similar
		*           else EFalse
		*/	
		TBool ComparePsQueryL(CPsQuery& aInputSearchQuery, CPsQuery& aDestSearchQuery);
		
		/**
		* Compares two CPsSettings items
		*
		* @param aSrcPsSettings - CPsSettings object1
		* @param aDestPsSettings - CPsSettings object2
		* @return - ETrue if both objects are similar
		*           else EFalse
		*/
		TBool ComparePsSettingsL(CPsSettings& aSrcPsSettings, CPsSettings& aDestPsSettings);
		
		/**
		* Compares two CPsClientData items
		*
		* @param aObject1 - CPsClientData object1
		* @param aObject2 - CPsClientData object2
		* @return - ETrue if both objects are similar
		*           else EFalse
		*/
		TBool ComparePsClientDataL( CPsClientData& aObject1, CPsClientData& aObject2 );
		
		/**
		* Compares two CPsData items
		*
		* @param aObject1 - CPsData object1
		* @param aObject2 - CPsData object2
		* @return - ETrue if both objects are similar
		*           else EFalse
		*/
		TBool ComparePsDataL( CPsData& aObject1, CPsData& aObject2 );

    private:    // Data
    	
};

#endif      // PS_UTILS_TESTSUITE_H

// End of File
