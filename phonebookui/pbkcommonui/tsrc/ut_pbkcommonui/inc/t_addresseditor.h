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
 * t_addresseditor.h
 *
 *  Created on: Feb 3, 2010
 *      Author: juhapaal
 */

#ifndef T_ADDRESSEDITOR_H_
#define T_ADDRESSEDITOR_H_

#include <QObject>

class CntAddressModel;
class HbDataForm;

#include <qtcontacts.h>
#include "qtpbkglobal.h"

QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE
QTM_USE_NAMESPACE


class T_AddressEditorTest : public QObject
    {
    Q_OBJECT
       
private slots:
    void init();
    void cleanup();
       
private slots:
    // tests
    void testAddressModelWithDefaultData();
    void testAddressModelWithData();
    void testAddressCustomViewItem();
    
private:
    void testItemLayout( CntAddressModel* aModel );
    
private:
    QContact* mContact;
    HbDataForm* mForm;
    };
#endif /* T_URLEDITOR_H_ */
