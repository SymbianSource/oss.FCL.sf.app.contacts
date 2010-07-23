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

#ifndef CNTDETAILEDITOR_H_
#define CNTDETAILEDITOR_H_

#include <hbdataform.h>
#include <hbabstractviewitem.h>
#include <hbgroupbox.h>
#include "cntdetaileditormodel.h"
#include "cntabstractview.h"

#include <qtcontacts.h>

class HbDocumentLoader;
class HbMenu;
class HbView;
class HbAction;
class CntEditorFactory;
class CntSaveManager;

class CntDetailEditor : public QObject,
                        public CntAbstractView
    {
    Q_OBJECT
    
public:
    CntDetailEditor( int aId );
    ~CntDetailEditor();
    
private slots:
    void saveChanges();
    void discardChanges();
    void insertField();
    void handleItemShown(const QModelIndex& aIndex );
    
    void saveContact();
    void contactDeletedFromOtherSource(const QList<QContactLocalId>& contactIds);
    void showRootView();
    
public:
    void setViewId( int aId );
    void setHeader( QString aHeader );
    void setDetails( CntDetailEditorModel* aModel, HbAbstractViewItem* aPrototype );
    
    void setInsertAction( const QString aInsert );
    
public:
    // Documentation inherited from CntAbstractView.
    void activate( CntAbstractViewManager* aMgr, const CntViewParameters aArgs );
    void deactivate();
        
    bool isDefault() const;
    HbView* view() const;
    
    int viewId() const;
    
private:    
    HbDocumentLoader* document();
    
private:
    QPointer<HbDataForm>           mDataForm;
    CntDetailEditorModel           *mDataFormModel;
    QPointer<HbGroupBox>           mHeader;
    int                            mId;
    HbView                         *mView;
    HbDocumentLoader               *mLoader;   
    CntAbstractViewManager         *mViewManager; // Do not own
    CntEditorFactory               *mEditorFactory;
    HbAction                       *mSoftkey;
    HbAction                       *mCancel;
    CntViewParameters               mArgs;
    CntSaveManager                 *mSaveManager; // own
    
    friend class TestCntDetailEditor;
    };
#endif /* CNTDETAILEDITOR_H_ */
