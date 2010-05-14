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
#include "cnturleditorviewitem.h"
#include "cntdetailmodelitem.h"
#include "cntdetailconst.h"
#include <qgraphicslinearlayout.h>
#include <qstandarditemmodel.h>
#include <qcontacturl.h>
#include <hbdataformmodel.h>
#include <hbmainwindow.h>
#include <hbabstractitemview.h>
#include <hbinputeditorinterface.h>
#include <hbinputstandardfilters.h>

CntUrlEditorViewItem::CntUrlEditorViewItem( QGraphicsItem* aParent ) :
CntDetailViewItem( aParent ),
mBox(NULL),
mEdit(NULL),
mLayout(NULL)
{
    
}

CntUrlEditorViewItem::~CntUrlEditorViewItem()
{
}
    
HbAbstractViewItem* CntUrlEditorViewItem::createItem()
{
    return new CntUrlEditorViewItem(*this);
}

HbWidget* CntUrlEditorViewItem::createCustomWidget()
{
    connect(itemView()->mainWindow(), SIGNAL(orientationChanged(Qt::Orientation)),
            this, SLOT(changeOrientation(Qt::Orientation)));
    
    mLayout = new QGraphicsLinearLayout(itemView()->mainWindow()->orientation());
    HbWidget* widget = new HbWidget();
    mBox = new HbComboBox();
    mEdit = new HbLineEdit();
    mEdit->setMaxLength( CNT_URL_EDITOR_MAXLENGTH );
    
    widget->setLayout( mLayout );
    mLayout->addItem( mBox );
    mLayout->addItem( mEdit );

    mLayout->setStretchFactor(mBox, 2);
    mLayout->setStretchFactor(mEdit, 2);
        
    connect( mBox, SIGNAL(currentIndexChanged(int)), this, SLOT(indexChanged(int)) );
    connect( mEdit, SIGNAL(textChanged(QString)),this, SLOT(textChanged(QString)) );
        
    HbDataFormModel* model = static_cast<HbDataFormModel*>(itemView()->model());
    CntDetailModelItem* item = static_cast<CntDetailModelItem*>( model->itemFromIndex(modelIndex()) );
    QContactUrl detail = item->detail();
    
    mEdit->setInputMethodHints( Qt::ImhUrlCharactersOnly );
    
    constructSubTypeModel( detail.contexts() );
    
    mEdit->setText( detail.url() );
    
    return widget;
}
    
void CntUrlEditorViewItem::indexChanged( int aIndex )
{
    QString context = mBox->itemData( aIndex, DetailContext ).toString();
    HbDataFormModel* model = static_cast<HbDataFormModel*>(itemView()->model());
    CntDetailModelItem* item = static_cast<CntDetailModelItem*>( model->itemFromIndex(modelIndex()) );
    
    QContactUrl url = item->detail();
    
    QStringList contextList;
    if ( !context.isEmpty() )
        contextList << context;
    
    url.setContexts( contextList );
    
    item->setDetail( url );
}

void CntUrlEditorViewItem::textChanged( QString aText )
{
    HbDataFormModel* model = static_cast<HbDataFormModel*>(itemView()->model());
    CntDetailModelItem* item = static_cast<CntDetailModelItem*>( model->itemFromIndex(modelIndex()) );
        
    QContactUrl url = item->detail();
    url.setUrl( aText );
    item->setDetail( url );
}

void CntUrlEditorViewItem::changeOrientation(Qt::Orientation aOrient)
{
    mLayout->setOrientation(aOrient);
}

void CntUrlEditorViewItem::constructSubTypeModel( QStringList aContext )
{
    QStandardItemModel* model = new QStandardItemModel();
            
    QString contextHome = QContactDetail::ContextHome;
    QString contextWork = QContactDetail::ContextWork;
    QString fieldAddress = QContactUrl::FieldUrl;
        
    QStandardItem *noContext = new QStandardItem;
    noContext->setText(qtTrId("URL"));
    noContext->setData(fieldAddress, DetailSubType);
    noContext->setData(CNT_URL_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(noContext);
           
    QStandardItem *home = new QStandardItem;
    home->setText(qtTrId("URL (home)"));
    home->setData(fieldAddress, DetailSubType);
    home->setData(contextHome, DetailContext);
    home->setData(CNT_URL_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(home);
           
    QStandardItem *work = new QStandardItem;
    work->setText(qtTrId("URL (work)"));
    work->setData(fieldAddress, DetailSubType);
    work->setData(contextWork, DetailContext);
    work->setData(CNT_URL_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(work);
    
    mBox->setModel( model );
    
    // search the selected index to be set
    QString context = aContext.isEmpty() ? "" : aContext.first();
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
