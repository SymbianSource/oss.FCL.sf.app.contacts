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
#include "cntemaileditorviewitem.h"
#include "cntdetailmodelitem.h"
#include "cntdetailconst.h"

#include <qcontactdetail.h>
#include <qgraphicslinearlayout.h>
#include <qcontactemailaddress.h>
#include <qgraphicslinearlayout.h>
#include <hbwidget.h>
#include <hbabstractitemview.h>
#include <hbdataformmodelitem.h>
#include <hbdataformmodel.h>
#include <hbcombobox.h>
#include <hblineedit.h>
#include <hbmainwindow.h>
#include <hbinputeditorinterface.h>
#include <hbinputstandardfilters.h>
#include <QStandardItemModel>

CntEmailEditorViewItem::CntEmailEditorViewItem( QGraphicsItem* aParent ) :
CntDetailViewItem( aParent ),
mBox(NULL),
mEdit(NULL),
mLayout(NULL)
    {
    }

CntEmailEditorViewItem::~CntEmailEditorViewItem()
    {
    
    }
    
HbAbstractViewItem* CntEmailEditorViewItem::createItem()
    {
    return new CntEmailEditorViewItem(*this);
    }

HbWidget* CntEmailEditorViewItem::createCustomWidget()
    {
    connect(itemView()->mainWindow(), SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(changeOrientation(Qt::Orientation)));
    mLayout = new QGraphicsLinearLayout( itemView()->mainWindow()->orientation() );
    HbWidget* widget = new HbWidget();
    mBox = new HbComboBox();
    mEdit = new HbLineEdit();
    mEdit->setMaxLength( CNT_EMAIL_EDITOR_MAXLENGTH );
    mEdit->setInputMethodHints(Qt::ImhPreferLowercase);
    
    widget->setLayout( mLayout );
    mLayout->addItem( mBox );
    mLayout->addItem( mEdit );
        
    mLayout->setStretchFactor(mBox, 2);
    mLayout->setStretchFactor(mEdit, 2);
    
    connect( mBox, SIGNAL(currentIndexChanged(int)), this, SLOT(indexChanged(int)) );
    connect( mEdit, SIGNAL(textChanged(QString)),this, SLOT(textChanged(QString)) );
        
        
    HbDataFormModel* model = static_cast<HbDataFormModel*>(itemView()->model());
    CntDetailModelItem* item = static_cast<CntDetailModelItem*>( model->itemFromIndex(modelIndex()) );
    QContactDetail detail = item->detail();
    
    HbEditorInterface editorInterface( mEdit );
    editorInterface.setFilter( HbEmailAddressFilter::instance() );
    
    constructSubTypeModel( detail.contexts() );
    
    QContactEmailAddress address = detail;
    QString d = address.emailAddress();
    mEdit->setText( address.emailAddress() );
    
    return widget;
    }
    
void CntEmailEditorViewItem::indexChanged( int aIndex )
    {
    QString context = mBox->itemData( aIndex, DetailContext ).toString();
        
    // check that if current QContactDetail contains the changed subtype
    HbDataFormModel* model = static_cast<HbDataFormModel*>(itemView()->model());
    CntDetailModelItem* item = static_cast<CntDetailModelItem*>( model->itemFromIndex(modelIndex()) );
    
    QStringList contextList;
    if ( !context.isEmpty() )
        contextList << context;
    
    QContactEmailAddress address = item->detail();
    address.setContexts( contextList );
    item->setDetail( address );
    }

void CntEmailEditorViewItem::textChanged( QString aText )
    {
    HbDataFormModel* model = static_cast<HbDataFormModel*>(itemView()->model());
    CntDetailModelItem* item = static_cast<CntDetailModelItem*>( model->itemFromIndex(modelIndex()) );
    
    QContactEmailAddress address = item->detail();
    address.setEmailAddress( aText );
    item->setDetail( address );
    }

void CntEmailEditorViewItem::changeOrientation(Qt::Orientation aOrient)
    {
        if (mLayout) {
            mLayout->setOrientation(aOrient);
        }
    }

void CntEmailEditorViewItem::constructSubTypeModel( QStringList aContext )
    {
    QStandardItemModel* model = new QStandardItemModel();
            
    QString contextHome = QContactDetail::ContextHome;
    QString contextWork = QContactDetail::ContextWork;
    QString fieldAddress = QContactEmailAddress::FieldEmailAddress;
        
    QStandardItem *noContext = new QStandardItem;
    noContext->setText(qtTrId("Email"));
    noContext->setData(fieldAddress, DetailSubType);
    noContext->setData(CNT_EMAIL_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(noContext);
           
    QStandardItem *home = new QStandardItem;
    home->setText(qtTrId("Email (home)"));
    home->setData(fieldAddress, DetailSubType);
    home->setData(contextHome, DetailContext);
    home->setData(CNT_EMAIL_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(home);
           
    QStandardItem *work = new QStandardItem;
    work->setText(qtTrId("Email (work)"));
    work->setData(fieldAddress, DetailSubType);
    work->setData(contextWork, DetailContext);
    work->setData(CNT_EMAIL_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(work);
    
    mBox->setModel( model );
    
    QString context = aContext.isEmpty() ? "" : aContext.first();
    // search the selected index to be set
    for ( int i(0); i < model->rowCount(); i++ )
        {
        if ( model->item(i)->data( DetailContext ).toString() == context )
            {
            mBox->setCurrentIndex( i );
            break;
            }
        }
    }

// End of File
