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
* Description:
*
*/


#ifndef ut_CCCAppViewPluginAknView_h
#define ut_CCCAppViewPluginAknView_h

#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>

class CTestedClass;
class CCCAAppAppUi;

_LIT( Kt_testiLogPath, "\\logs\\testframework\\ccapp\\" ); 
_LIT( Kt_testiLogFile, "ccapputil_unittests.txt" ); 

/**
*  ut_CCCAppViewPluginAknView test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(ut_CCCAppViewPluginAknView) : 
    public CScriptBase
    {
    public:  // Constructors and destructor
        static ut_CCCAppViewPluginAknView* NewL( CTestModuleIf& aTestModuleIf );
        virtual ~ut_CCCAppViewPluginAknView();

    public: // Functions from base classes
        virtual TInt RunMethodL( CStifItemParser& aItem );
        
    private:
        ut_CCCAppViewPluginAknView( CTestModuleIf& aTestModuleIf );
        void ConstructL();
        
        // tests
        TInt SetupL( CStifItemParser& aItem );
        TInt Teardown( CStifItemParser& aItem );
        
        TInt Test_DoActivateL( CStifItemParser& aItem );
        TInt Test_DoDeactivate( CStifItemParser& aItem );
        TInt Test_HandleCommandL( CStifItemParser& aItem );
        
    private:
        CTestedClass* iTestedClass;
        CCCAAppAppUi* iCCaAppUi;
    };

#include "ccapputilheaders.h"

class CTestedClass : public CCCAppViewPluginAknView
    {
public:
    
    // from CCCAppViewPluginAknView
    void NewContainerL()
        { iNewContainerL_called = ETrue;
          User::LeaveIfError( iLeaveCode );
          iContainer = new (ELeave) CCCAppViewPluginAknContainer(); };

    // rest
    void DoActivateL()
        { CCCAppViewPluginAknView::DoActivateL( TVwsViewId(), TUid(), KNullDesC8() ); };

    void DoDeactivate()
        { CCCAppViewPluginAknView::DoDeactivate(); };
    
    void HandleCommandL( TInt aCmd )
        { CCCAppViewPluginAknView::HandleCommandL( aCmd ); };    
    
    CCCAppViewPluginAknContainer* Container()
        { return iContainer; };
   
    TInt  iLeaveCode;
    TBool iNewContainerL_called;
    };

#endif// ut_CCCAppViewPluginAknView_h

// End of File
