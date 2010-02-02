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


#ifndef ut_CCCAppViewPluginAknContainer_h
#define ut_CCCAppViewPluginAknContainer_h

#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>

class CTestedClass;
class CCCAAppAppUi;

_LIT( Kt_testiLogPath, "\\logs\\testframework\\ccapp\\" ); 
_LIT( Kt_testiLogFile, "ccapputil_unittests.txt" ); 

/**
*  ut_CCCAppViewPluginAknContainer test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(ut_CCCAppViewPluginAknContainer) : 
    public CScriptBase
    {
    public:  // Constructors and destructor
        static ut_CCCAppViewPluginAknContainer* NewL( CTestModuleIf& aTestModuleIf );
        virtual ~ut_CCCAppViewPluginAknContainer();

    public: // Functions from base classes
        virtual TInt RunMethodL( CStifItemParser& aItem );
        
        void DeleteCallback();

    private:
        ut_CCCAppViewPluginAknContainer( CTestModuleIf& aTestModuleIf );
        void ConstructL();
        
        // tests
        TInt SetupL( CStifItemParser& aItem );
        TInt Teardown( CStifItemParser& aItem );
        
        TInt Test_BaseConstructL( CStifItemParser& aItem );
        TInt Test_OfferKeyEventL( CStifItemParser& aItem );
        TInt Test_HandleResourceChange( CStifItemParser& aItem );
        
    private:
        CTestedClass* iTestedClass;
        CCCAAppAppUi* iCCaAppUi;
    };

#include "ccapputilheaders.h"

class CTestedClass : public CCCAppViewPluginAknContainer
    {
public:
    void ConstructL()
        { iConstructL_called = ETrue; };
    
    CCoeAppUi* AppUi()
        { return iAppUi; }

    virtual TKeyResponse OfferKeyEventL( 
        const TKeyEvent& aKeyEvent,
        TEventCode aType )
        { return CCCAppViewPluginAknContainer::OfferKeyEventL( aKeyEvent, aType ); };
    
    virtual void HandleResourceChange( TInt aType )
        { CCCAppViewPluginAknContainer::HandleResourceChange( aType ); };
        
    TBool iConstructL_called;
    };

#endif// ut_CCCAppViewPluginAknContainer_h

// End of File
