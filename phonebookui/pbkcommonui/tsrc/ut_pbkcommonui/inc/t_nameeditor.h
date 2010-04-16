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
 * t_nameditor.h
 *
 *  Created on: Feb 5, 2010
 *      Author: juhapaal
 */

#ifndef T_NAMEDITOR_H_
#define T_NAMEDITOR_H_

#include <qobject.h>
#include <qtcontacts.h>
#include "qtpbkglobal.h"

class HbDataForm;
QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE
QTM_USE_NAMESPACE

class T_NameEditorTest : public QObject
    {
    Q_OBJECT
        
private slots:
    void init();
    void cleanup();
    
private slots:
    void testNameModelDefaultData();
    void testNameModelData();
    
private:
    QContact* mContact;
    HbDataForm* mForm;
    };
#endif /* T_NAMEDITOR_H_ */
