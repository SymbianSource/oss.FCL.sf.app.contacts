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

#ifndef CNTIMPORTSVIEW_H
#define CNTIMPORTSVIEW_H

#include <QObject>

#include "cntsimutility.h"
#include "cntimportviewcallback.h"

#include <hbdocumentloader.h>
#include <cntabstractview.h>

class HbProgressDialog;
class HbListView;
class HbView;
class HbAction;
class QStandardItemModel;
class CntSimEngine;
class QModelIndex;

QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntImportsView : public QObject, public CntAbstractView, public CntImportViewCallback
{
    Q_OBJECT
    friend class TestCntImportsView;
     
public:
    CntImportsView();
    ~CntImportsView();

public: // From CntAbstractView
    void activate( const CntViewParameters aArgs );
    void deactivate();
    bool isDefault() const { return false; }
    HbView* view() const { return mView; }
    int viewId() const { return importsView; }
    inline void setEngine( CntAbstractEngine& aEngine ){ mEngine = &aEngine; }
    QContactManager *contactSymbianManager();
    
private slots:
    void showPreviousView();
    void closeImportPopup();
    void onListViewActivated(const QModelIndex &index);
    void userCancelsImport();
    
private:
    void showWaitNote();
    void setPreferredDetails( QContact *aContact );
    
private: // from CntImportViewCallback
    void setListBoxItemText(QString& aPrimary, QString& aSecondary);
    void setListBoxItemEnabled(bool aEnabled);
        
private:
    CntAbstractViewManager  *mViewManager;
    CntSimEngine            *mSimEngine;
    HbListView              *mListView;
    HbDocumentLoader         mDocumentLoader;
    HbView                  *mView;
    HbAction                *mSoftkey;
	QStandardItemModel      *mModel;
	HbProgressDialog        *mImportSimPopup;
    CntAbstractEngine       *mEngine;
};

#endif /* CNTIMPORTSVIEW_H */
