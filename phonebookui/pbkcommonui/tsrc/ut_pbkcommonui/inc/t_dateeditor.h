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
/*
 * t_dateeditor.h
 *
 *  Created on: Feb 8, 2010
 *      Author: juhapaal
 */

#ifndef T_DATEEDITOR_H_
#define T_DATEEDITOR_H_

#include <QObject>
#include <QLocale>
#include <qtcontacts.h>
#include "qtpbkglobal.h"

class HbDataForm;

QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE
QTM_USE_NAMESPACE

class T_DateEditorTest : public QObject
    {
    Q_OBJECT
    
private slots:
    void init();
    void cleanup();
    
private slots:// tests
    void testDateModelWithDefaultData();
    void testDateModelWithData();
    void testDatelViewItem_Swahili();
    void testDatelViewItem_English();
    
private:
    QContact* mContact;
    HbDataForm* mForm;
    QLocale mLocale;
    };

#endif /* T_DATEEDITOR_H_ */
