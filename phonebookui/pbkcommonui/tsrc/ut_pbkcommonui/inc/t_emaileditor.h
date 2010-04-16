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
 * t_emaileditor.h
 *
 *  Created on: Feb 2, 2010
 *      Author: juhapaal
 */

#ifndef T_EMAILEDITOR_H_
#define T_EMAILEDITOR_H_

#include <QObject>
#include <qtcontacts.h>
#include "qtpbkglobal.h"

class HbDataForm;
QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE
QTM_USE_NAMESPACE

class T_EmailEditorTest : public QObject
    {
    Q_OBJECT
    
private slots:
    void init();
    void cleanup();

private slots:
    // tests
    void testEmailModelWithDefaultData();
    void testEmailModelWithData();
    void testInsertModelItem();
    void testEmailViewItem();
    void testEmailViewItemComboChange();
    void testEmailViewItemLineEdit();
    
private:
    QContact* mContact;
    HbDataForm* mForm;
    };
#endif /* T_EMAILEDITOR_H_ */
