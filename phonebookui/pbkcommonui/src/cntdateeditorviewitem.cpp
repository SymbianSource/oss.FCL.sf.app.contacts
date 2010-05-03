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
#include "cntdateeditorviewitem.h"
#include "cntdetailmodelitem.h"
#include "cntdetailconst.h"

#include <qcontactdetail.h>
#include <qcontactbirthday.h>
#include <qcontactanniversary.h>

#include <hbabstractviewitem.h>
#include <hbabstractitemview.h>
#include <hbdataformmodel.h>
#include <hbwidget.h>
#include <hbdatetimepicker.h>
#include <hbpushbutton.h>
#include <hbdialog.h>
#include <hbdatetimepicker.h>
#include <hbtextitem.h>
#include <hbaction.h>

#include <QPointer>

CntDateEditorViewItem::CntDateEditorViewItem( QGraphicsItem* aParent ) : 
CntDetailViewItem( aParent ),
mButton(NULL)
    {
    mLocale = QLocale::system();
    }

CntDateEditorViewItem::~CntDateEditorViewItem()
    {
    }

HbAbstractViewItem* CntDateEditorViewItem::createItem()
    {
    return new CntDateEditorViewItem( *this );
    }

void CntDateEditorViewItem::clicked()
    {
    HbDataFormModel* model = static_cast<HbDataFormModel*>(itemView()->model());
    CntDetailModelItem* item = static_cast<CntDetailModelItem*>( model->itemFromIndex(modelIndex()) );
    QContactDetail detail = item->detail();
    
    QString buttonText = qtTrId( "No date set" );
    if ( detail.definitionName() == QContactBirthday::DefinitionName )
        {
        QContactBirthday bd = detail;
        QDate date = editDate( bd.date(), qtTrId("Birthday") );
        if ( date != bd.date() )
            {
            bd.setDate( date );
            item->setDetail( bd );
            }
        buttonText = mLocale.toString( date );
        }
        
    if ( detail.definitionName() == QContactAnniversary::DefinitionName )
        {
        QContactAnniversary anniversary = detail;
        QDate date = editDate( anniversary.originalDate(), qtTrId("Anniversary") );
        if ( date != anniversary.originalDate() )
            {
            anniversary.setOriginalDate( date );
            item->setDetail( anniversary );
            }
        buttonText = mLocale.toString( date );
        }
    
    mButton->setText( buttonText );
    }

HbWidget* CntDateEditorViewItem::createCustomWidget()
    {
    mButton = new HbPushButton();
    connect( mButton, SIGNAL(clicked(bool)), this, SLOT(clicked()) );
    
    HbDataFormModel* model = static_cast<HbDataFormModel*>(itemView()->model());
    CntDetailModelItem* item = static_cast<CntDetailModelItem*>( model->itemFromIndex(modelIndex()) );
    
    QContactDetail detail = item->detail();
    QString text( qtTrId("No date set") );
    
    if ( detail.definitionName() == QContactBirthday::DefinitionName )
        {
        QContactBirthday birthday = detail;
        if ( !birthday.isEmpty() )
            {
            text = mLocale.toString( birthday.date() );
            }
        }
    
    if ( detail.definitionName() == QContactAnniversary::DefinitionName )
        {
        QContactAnniversary anniversary = detail;
        if ( !anniversary.isEmpty() )
            {
            text = mLocale.toString( anniversary.originalDate() );
            }
        }
    mButton->setText( text );
    return mButton;
    }

QDate CntDateEditorViewItem::editDate( QDate aCurrent, QString aTitle )
    {
    QPointer<HbDialog> popup = new HbDialog();
    popup->setDismissPolicy(HbDialog::NoDismiss);
    popup->setTimeout(HbPopup::NoTimeout);

    HbDateTimePicker *picker = new HbDateTimePicker( popup );
    picker->setDisplayFormat( mLocale.dateFormat() );
    picker->setDateRange(CNT_DATEPICKER_FROM, CNT_DATEPICKER_TO );
    picker->setDate( aCurrent );
                
    HbTextItem *headingText = new HbTextItem( popup );
    headingText->setFontSpec( HbFontSpec(HbFontSpec::Title) );
    headingText->setText( aTitle );
    
    popup->setHeadingWidget(headingText);
    popup->setContentWidget(picker);
    popup->setPrimaryAction(new HbAction(qtTrId("Ok"), popup));
    popup->setSecondaryAction(new HbAction(qtTrId("Cancel"), popup));

    HbAction *selected = popup->exec();
    QDate date = (selected == popup->primaryAction()) ? picker->date() : aCurrent;
    delete popup;
    
    return date;
    }

// End of File
