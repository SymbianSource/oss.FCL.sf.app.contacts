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
#ifndef _CNTSETTINGSVIEW_H__
#define _CNTSETTINGSVIEW_H__

#include <cntabstractview.h>
#include <QObject>
#include <hbdataformmodel.h>

class HbAction;
class HbView;
class HbDataForm;
class HbDataFormModelItem;
class HbDocumentLoader;

class CntSettingsModel : public HbDataFormModel
{
    Q_OBJECT
    
public:
    virtual ~CntSettingsModel(){}
    virtual void saveSettings() = 0;
    virtual void loadSettings() = 0;
};

class CntDefaultSettingsModel : public CntSettingsModel
{
    Q_OBJECT
    
public:
    CntDefaultSettingsModel();
    ~CntDefaultSettingsModel();
    
public:
    void saveSettings();
    void loadSettings();
    
private:
    HbDataFormModelItem* mOrder;
};

class CntSettingsView : public QObject, public CntAbstractView
{
    Q_OBJECT
    
public:
    CntSettingsView();
    ~CntSettingsView();
    
protected: // From CntAbstractView
    void activate( CntAbstractViewManager* aMgr, const CntViewParameters aArgs );
    void deactivate();
    bool isDefault() const;
    HbView* view() const;
    int viewId() const;
    
private slots:
    void back();
    
private:
    HbDocumentLoader* document();
    
private:
    HbView* mView;
    HbDocumentLoader* mDoc;
    HbDataForm* mForm;
    HbAction* mBack;
    
    CntAbstractViewManager* mViewMgr;
    CntViewParameters mArgs;
    
    CntSettingsModel* mModel;
    
    friend class TestCntEditView;
};
#endif

