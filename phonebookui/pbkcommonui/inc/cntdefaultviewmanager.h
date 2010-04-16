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
#ifndef CNTDEFAULTVIEWMANAGER_H_
#define CNTDEFAULTVIEWMANAGER_H_

#include "cntviewmanager.h"
#include "cntabstractviewmanager.h"

//class CntModelProvider;
class MobCntModel;
class CntDefaultViewFactory;
class CntAbstractView;
class CntViewNavigator;

QTM_BEGIN_NAMESPACE
class QContactManager;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class QTPBK_EXPORT CntDefaultViewManager : public CntViewManager, public CntAbstractViewManager
    {
public:
    CntDefaultViewManager( CntMainWindow* aWindow, CntViewParameters::ViewId defaultView );
    ~CntDefaultViewManager();
    
public: // From CntAbstractViewManager
    void changeView( const CntViewParameters& aArgs );
    void back( const CntViewParameters& aArgs );
    QContactManager* contactManager( const QString& aType );
    
public: // From CntViewManager
    bool isDepracatedView( CntViewParameters::ViewId aViewId );
    
private:
    void removeCurrentView();
    void switchView( const CntViewParameters& aArgs );
    
private:
    CntDefaultViewFactory* mFactory;
    CntAbstractView* mCurrent;
    QMap<CntViewParameters::ViewId, CntAbstractView*> mDefaults;
    CntViewNavigator* mNavigator;
    CntViewParameters* mArgs;
    
    //CntModelProvider* mModelProvider;
    QList<QContactManager*> mBackends;
    };
#endif /* CNTDEFAULTVIEWMANAGER_H_ */
