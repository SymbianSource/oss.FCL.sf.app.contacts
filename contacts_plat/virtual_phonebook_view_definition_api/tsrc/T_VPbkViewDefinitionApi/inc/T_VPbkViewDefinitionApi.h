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
* Description:  STIF test module for testing VirtualPhonebook View Definition API
*
*/



#ifndef T_VPBKVIEWDEFINITIONAPI_H
#define T_VPBKVIEWDEFINITIONAPI_H

#include <e32def.h>
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>

// DEFINES
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// FORWARD DECLARATIONS
class CVPbkContactViewDefinition;

NONSHARABLE_CLASS(CT_VPbkViewDefinitionApi) :
		public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CT_VPbkViewDefinitionApi* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CT_VPbkViewDefinitionApi();

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

    private:

        /**
        * C++ default constructor.
        */
    	CT_VPbkViewDefinitionApi( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Test method for creating contact view instance
        * @return Symbian OS error code.
        */
        virtual TInt CreateContactViewDefinition();
        
        /**
        * Test method for deleting contact view
        * @return Symbian OS error code.
        */
        virtual TInt DeleteContactViewDefinition();
        
        /**
        * Test method for counting sub views of a contact view
        * @return Symbian OS error code.
        */
        virtual TInt CountSubViews();

        /**
        * Tests retrieving of a sub view with index number
        * @return Symbian OS error code.
        */
        virtual TInt ReturnSubViewAt();

        /**
        * Tests retrieving of view type
        * @return Symbian OS error code.
        */
        virtual TInt ViewType();

        /**
        * Tests retrieving the state of a flag
        * @return Symbian OS error code.
        */
        virtual TInt FlagIsOn();

        /**
        * Test method for retrieving URI of a view
        * @return Symbian OS error code.
        */
        virtual TInt Uri();

        /**
        * Test method for retrieving sort policy of a view
        * @return Symbian OS error code.
        */
        virtual TInt SortPolicy();

        /**
        * Test method for retrieving name of a view
        * @return Symbian OS error code.
        */
        virtual TInt Name();

        /**
        * Test method for retrieving sharing type of a view
        * @return Symbian OS error code.
        */
        virtual TInt Sharing();

        /**
        * Test method for retrieving field type filter of a view
        * @return Symbian OS error code.
        */
        virtual TInt FieldTypeFilter();
        
        /**
        * Tests adding of a sub view for a view
        * @return Symbian OS error code.
        */
        virtual TInt AddSubView();

        /**
        * Tests setting of a view type
        * @return Symbian OS error code.
        */
        virtual TInt SetType();

        /**
        * Tests setting of flag
        * @return Symbian OS error code.
        */
        virtual TInt SetFlag();

        /**
        * Tests setting of a URI
        * @return Symbian OS error code.
        */
        virtual TInt SetUri();

        /**
        * Tests setting of a sort policy
        * @return Symbian OS error code.
        */
        virtual TInt SetSortPolicy();

        /**
        * Tests setting of a name
        * @return Symbian OS error code.
        */
        virtual TInt SetName();

        /**
        * Tests setting of sharing type
        * @return Symbian OS error code.
        */
        virtual TInt SetSharing();
        
        /**
        * Tests setting of field type filter
        * @return Symbian OS error code.
        */
        virtual TInt SetFieldTypeFilter();

        void Delete();
        void CreateContactViewDefinitionL();
        void AddSubViewL();
        void SetUriL();
        void SetNameL();
        void SetFieldTypeFilterL();
        
        CVPbkContactViewDefinition* iViewDefinition;
    };

#endif      // T_VPBKVIEWDEFINITIONAPI_H

// End of File
