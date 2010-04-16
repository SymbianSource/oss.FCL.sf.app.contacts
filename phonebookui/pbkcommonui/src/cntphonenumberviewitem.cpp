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
#include "cntdetailconst.h"
#include "cntphonenumberviewitem.h"
#include "cntdetailmodelitem.h"
#include "cntdetaileditormodel.h"

#include <qdebug.h>
#include <qgraphicslinearlayout.h>
#include <qcontactphonenumber.h>
#include <qcontactonlineaccount.h>
#include <qcontactdetail.h>
#include <qstandarditemmodel.h>

#include <hbwidget.h>
#include <hbmainwindow.h>
#include <hbabstractviewitem.h>
#include <hbabstractitemview.h>
#include <hbdataformmodelitem.h>
#include <hbdataformmodel.h>
#include <hbcombobox.h>
#include <hblineedit.h>
#include <hbinputstandardfilters.h>

CntPhoneNumberViewItem::CntPhoneNumberViewItem( QGraphicsItem* aParent ) : 
CntDetailViewItem( aParent ),
mBox(0),
mEdit(0),
mFilter( 0 )
    {
    }

CntPhoneNumberViewItem::~CntPhoneNumberViewItem()
    {
    delete mFilter;
    }

HbAbstractViewItem* CntPhoneNumberViewItem::createItem()
    {
    return new CntPhoneNumberViewItem( *this );
    }

void CntPhoneNumberViewItem::indexChanged( int aIndex )
    {
    QString subType = mBox->itemData( aIndex, DetailSubType ).toString();
    QString context = mBox->itemData( aIndex, DetailContext ).toString();
    
    // check that if current QContactDetail contains the changed subtype
    HbDataFormModel* model = static_cast<HbDataFormModel*>(itemView()->model());
    CntDetailModelItem* item = static_cast<CntDetailModelItem*>( model->itemFromIndex(modelIndex()) );
    
    QStringList contextList;
    if ( !context.isEmpty() )
        contextList << context;
    
    // check if we switched to QContactOnlineAccount subtype
    if ( subType == QContactOnlineAccount::SubTypeSip || 
         subType == QContactOnlineAccount::SubTypeSipVoip ||
         subType == QContactOnlineAccount::SubTypeVideoShare )
        {
        qDebug() << "QContactOnlineAccount(" << subType << ")";
        constructOnlineAccount( item, subType, contextList );
        }
    else
        {
        qDebug() << "QContactPhoneNumber(" << subType << ")";
        constructPhoneNumber( item, subType, contextList );
        }
    }

void CntPhoneNumberViewItem::textChanged( QString aText )
    {
    qDebug() << "textChanged( " << aText << " )";
    HbDataFormModel* model = static_cast<HbDataFormModel*>(itemView()->model());
    CntDetailModelItem* item = static_cast<CntDetailModelItem*>( model->itemFromIndex(modelIndex()) );
    QContactDetail detail = item->detail();
    if ( detail.definitionName() == QContactPhoneNumber::DefinitionName )
        {
        QContactPhoneNumber number = detail;
        number.setNumber( aText );
        item->setDetail( number );
        }
    
    if ( detail.definitionName() == QContactOnlineAccount::DefinitionName )
        {
        QContactOnlineAccount account = detail;
        account.setAccountUri( aText );
        item->setDetail( account );
        }
    }

HbWidget* CntPhoneNumberViewItem::createCustomWidget()
    {
    QGraphicsLinearLayout* layout = new QGraphicsLinearLayout( itemView()->mainWindow()->orientation() );
    HbWidget* widget = new HbWidget();
    mBox = new HbComboBox();
    mEdit = new HbLineEdit();
    mFilter = new HbEditorInterface( mEdit );
        
    widget->setLayout( layout );
    layout->addItem( mBox );
    layout->addItem( mEdit );
    
    connect( mBox, SIGNAL(currentIndexChanged(int)), this, SLOT(indexChanged(int)) );
    connect( mEdit, SIGNAL(textChanged(QString)),this, SLOT(textChanged(QString)) );
    
    HbDataFormModel* model = static_cast<HbDataFormModel*>(itemView()->model());
    CntDetailModelItem* item = static_cast<CntDetailModelItem*>( model->itemFromIndex(modelIndex()) );
    
    // select correct index
    QContactDetail detail = item->detail();
    QStringList context;
    QString subType;
    QString value;
    if ( detail.definitionName() == QContactPhoneNumber::DefinitionName )
        {
        mEdit->setMaxLength( CNT_PHONENUMBER_EDITOR_MAXLENGTH );
        QContactPhoneNumber phone = detail;
        subType = phone.subTypes().isEmpty() ? "" : phone.subTypes().first();
        if ( !phone.contexts().isEmpty() )
            context << phone.contexts().first();
        value = phone.number();
        }
    
    if ( detail.definitionName() == QContactOnlineAccount::DefinitionName )
        {
        mEdit->setMaxLength( CNT_ONLINEACCOUNT_EDITOR_MAXLENGTH );
        QContactOnlineAccount account = detail;
        subType = account.subTypes().isEmpty() ? "" : account.subTypes().first();
        if (!account.contexts().isEmpty())
            context << account.contexts().first();
        value = account.accountUri();
        }
    
    mEdit->setText( value );
    constructSubtypeModel( subType, context );
    
    return widget;        
    }

void CntPhoneNumberViewItem::constructPhoneNumber( CntDetailModelItem* aItem, QString aSubType, QStringList aContext )
    {
    mFilter->setFilter( HbPhoneNumberFilter::instance() );
    mFilter->setUpAsPhoneNumberEditor();
    mEdit->setMaxLength( CNT_PHONENUMBER_EDITOR_MAXLENGTH );
            
    QContactDetail detail = aItem->detail();
    
    // check if the detail type needs to changed
    if ( detail.definitionName() == QContactOnlineAccount::DefinitionName )
        {
        // also we need to remove the old online account and replace it
        // with phonenumber
        CntDetailEditorModel* model = static_cast<CntDetailEditorModel*>( aItem->model() );
        QContact* contact = model->contact();
        contact->removeDetail( &detail );
                
        QContactPhoneNumber number;
        number.setSubTypes( aSubType );
        number.setContexts( aContext );
        aItem->setDetail( number );
        }
    else
        {
        QContactPhoneNumber number = detail;
        number.setSubTypes( aSubType );
        number.setContexts( aContext );
        aItem->setDetail( number );
        }
    qDebug() << mEdit->text();
    }

void CntPhoneNumberViewItem::constructOnlineAccount( CntDetailModelItem* aItem, QString aSubType, QStringList aContext )
    {
    mFilter->setFilter( HbEmailAddressFilter::instance() );
    mFilter->setUpAsCompletingEmailField();
    mEdit->setMaxLength( CNT_ONLINEACCOUNT_EDITOR_MAXLENGTH );
    
    QContactDetail detail = aItem->detail();
    // check if the detail type needs to changed
    if ( detail.definitionName() == QContactPhoneNumber::DefinitionName )
        {
        // also we need to remove the old phonenumber and replace it
        // with online account
        CntDetailEditorModel* model = static_cast<CntDetailEditorModel*>( aItem->model() );
        QContact* contact = model->contact();
        contact->removeDetail( &detail );
        
        QContactOnlineAccount account;
        account.setSubTypes( aSubType );
        account.setContexts( aContext );
        account.setAccountUri( mEdit->text() );
        aItem->setDetail( account );
        }
    else
        {
        QContactOnlineAccount account = detail;
        account.setSubTypes( aSubType );
        account.setContexts( aContext );
        account.setAccountUri( mEdit->text() );
        aItem->setDetail( account );
        }
    qDebug() << mEdit->text();
    }

void CntPhoneNumberViewItem::constructSubtypeModel( QString aSubType, QStringList aContext )
    {
    QStandardItemModel* model = new QStandardItemModel();
        
    QString contextHome = QContactDetail::ContextHome;
    QString contextWork = QContactDetail::ContextWork;
        
    QString subTypeMobile = QContactPhoneNumber::SubTypeMobile;
    QString subTypeLandline = QContactPhoneNumber::SubTypeLandline;
    QString subTypeFax = QContactPhoneNumber::SubTypeFacsimile;
    QString subTypePager = QContactPhoneNumber::SubTypePager;
    QString subTypeCarPhone = QContactPhoneNumber::SubTypeCar;
    QString subTypeDtmf = QContactPhoneNumber::SubTypeDtmfMenu;
    QString subTypeAssistant = QContactPhoneNumber::SubTypeAssistant;
        
    QString subTypeInternet = QContactOnlineAccount::SubTypeSipVoip;
    QString subTypeSIP = QContactOnlineAccount::SubTypeSip;
    QString subTypeVideoShare = QContactOnlineAccount::SubTypeVideoShare;
    
    QStandardItem *mobile = new QStandardItem;
    mobile->setText(hbTrId("txt_phob_dblist_mobile"));
    mobile->setData(subTypeMobile, DetailSubType);
    mobile->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(mobile);
       
    QStandardItem *mobilehome = new QStandardItem;
    mobilehome->setText(hbTrId("txt_phob_dblist_mobile_home"));
    mobilehome->setData(subTypeMobile, DetailSubType);
    mobilehome->setData(contextHome, DetailContext);
    mobilehome->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(mobilehome);
       
    QStandardItem *mobilework = new QStandardItem;
    mobilework->setText(hbTrId("txt_phob_dblist_mobile_work"));
    mobilework->setData(subTypeMobile, DetailSubType);
    mobilework->setData(contextWork, DetailContext);
    mobilework->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(mobilework);

    QStandardItem *land = new QStandardItem;
    land->setText(hbTrId("txt_phob_setlabel_val_phone"));
    land->setData(subTypeLandline, DetailSubType);
    land->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(land);
       
    QStandardItem *landhome = new QStandardItem;
    landhome->setText(hbTrId("txt_phob_setlabel_val_phone_home"));
    landhome->setData(subTypeLandline, DetailSubType);
    landhome->setData(contextHome, DetailContext);
    landhome->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(landhome);
       
    QStandardItem *landwork = new QStandardItem;
    landwork->setText(hbTrId("txt_phob_setlabel_val_phone_work"));
    landwork->setData(subTypeLandline, DetailSubType);
    landwork->setData(contextWork, DetailContext);
    landwork->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(landwork);
    
    QStandardItem *fax = new QStandardItem;
    fax->setText(hbTrId("txt_phob_dblist_fax"));
    fax->setData(subTypeFax, DetailSubType);
    fax->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(fax);
    
    QStandardItem *faxhome = new QStandardItem;
    faxhome->setText(hbTrId("txt_phob_dblist_fax_home"));
    faxhome->setData(subTypeFax, DetailSubType);
    faxhome->setData(contextHome, DetailContext);
    faxhome->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(faxhome);
    
    QStandardItem *faxwork = new QStandardItem;
    faxwork->setText(hbTrId("txt_phob_dblist_fax_work"));
    faxwork->setData(subTypeFax, DetailSubType);
    faxwork->setData(contextWork, DetailContext);
    faxwork->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(faxwork);
        
    QStandardItem *pager = new QStandardItem;
    pager->setText(hbTrId("txt_phob_setlabel_val_pager"));
    pager->setData(subTypePager, DetailSubType);
    pager->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(pager);
    
    QStandardItem *assistant = new QStandardItem;
    assistant->setText(hbTrId("txt_phob_setlabel_val_assistant"));
    assistant->setData(subTypeAssistant, DetailSubType);
    assistant->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(assistant);
        
    QStandardItem *carPhone = new QStandardItem;
    carPhone->setText(hbTrId("txt_phob_setlabel_val_car"));
    carPhone->setData(subTypeCarPhone, DetailSubType);
    carPhone->setData(CNT_PHONENUMBER_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(carPhone);
    
//    QStandardItem *dtmf = new QStandardItem;
//    dtmf->setText(hbTrId("DTMF"));
//    dtmf->setData(subTypeDtmf, DetailSubType);
//    dtmf->setData(CNT_DTMF_EDITOR_MAXLENGTH, DetailMaxLength);
//    model->appendRow(dtmf);
   
    QStandardItem *internet = new QStandardItem;
    internet->setText(qtTrId("Internet call"));
    internet->setData(subTypeInternet, DetailSubType);
    internet->setData(CNT_ONLINEACCOUNT_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(internet);
      
    QStandardItem *internethome = new QStandardItem;
    internethome->setText(hbTrId("Internet call (home)"));
    internethome->setData(subTypeInternet, DetailSubType);
    internethome->setData(contextHome, DetailContext);
    internethome->setData(CNT_ONLINEACCOUNT_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(internethome);

    QStandardItem *internetwork = new QStandardItem;
    internetwork->setText(hbTrId("Internet call (work)"));
    internetwork->setData(subTypeInternet, DetailSubType);
    internetwork->setData(contextWork, DetailContext);
    internetwork->setData(CNT_ONLINEACCOUNT_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(internetwork);
          
    QStandardItem *sip = new QStandardItem;
    sip->setText(hbTrId("SIP"));
    sip->setData(subTypeSIP, DetailSubType);
    sip->setData(CNT_ONLINEACCOUNT_EDITOR_MAXLENGTH, DetailMaxLength);
    model->appendRow(sip);
      
//    QStandardItem *shareVideo = new QStandardItem;
//    shareVideo->setText(qtTrId("Share Video"));
//    shareVideo->setData(subTypeVideoShare, DetailSubType);
//    shareVideo->setData(CNT_ONLINEACCOUNT_EDITOR_MAXLENGTH, DetailMaxLength);
//    model->appendRow(shareVideo);
   
    mBox->setModel( model );
    
    // search the selected index to be set
    QString context = aContext.isEmpty() ? "" : aContext.first();
    for ( int i(0); i < model->rowCount(); i++ )
        {
        if ( model->item(i)->data( DetailSubType ).toString() == aSubType &&
             model->item(i)->data( DetailContext ).toString() == context )
            {
            mBox->setCurrentIndex( i );
            break;
            }
        }
    }
// End of File
