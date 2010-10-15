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
* Description:
*
*/


#include <QObject>
#include <qmobilityglobal.h>
#include <qtcontacts.h>
#include <cntabstractserviceprovider.h>
#include <cntviewparams.h>

class CntServices;
class TestViewManager;
class HbMainWindow;
class TestEngine;

/**
 * A class for testing CntServices.
 */
class UT_CntServices : public QObject, public CntAbstractServiceProvider
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    
    void testCreating();

    void test_setEngine();

    
    void test_singleFetch();
    
    void test_multiFetch();
    
    void test_editCreateNew();
    
    void test_editCreateNew2();
    
    void test_editCreateNewFromVCard();
    
    void test_editUpdateExisting();
    
    void test_editUpdateExisting2();
    
    void test_editExisting();
    
    void test_launchContactCard();
    
    void test_launchGroupMemberView();
    
    void test_launchTemporaryContactCard();

    void test_terminateService();
    
    void test_removeNotSupportedFields();

    void removeNotSupportedDetails();

    void test_fillOnlineAccount();

    void test_updateLocalId();    

    void test_CompleteServiceAndCloseApp();

    void test_overrideReturnValue();

    void test_allowSubViewsExit();

    //Pointless now: void test_quitApp();
    //Pointless now: void test_contactManager();    

public slots:
    void onViewChanged( const CntViewParameters aArgs );
        
private:
    virtual void CompleteServiceAndCloseApp(const QVariant& retValue);
    //virtual void overrideReturnValue(const QVariant& retValue) { Q_UNUSED(retValue) };
    //virtual bool allowSubViewsExit() { return true; };

    
private:
    CntServices*   mCntServices;
    TestViewManager* mTestViewManager;
    TestEngine* mEngine;
    CntViewParameters mViewParams;
};

// EOF
