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
 * t_familyeditor.h
 *
 *  Created on: Feb 8, 2010
 *      Author: juhapaal
 */

#ifndef T_FAMILYEDITOR_H_
#define T_FAMILYEDITOR_H_

#include <QObject>
#include <QContact.h>
#include <HbDataForm.h>

#include <qtcontacts.h>
#include "qtpbkglobal.h"

QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE
QTM_USE_NAMESPACE

class T_FamilyEditorTest : public QObject
    {
    Q_OBJECT
    
private slots: // framework functions
    void init();
    void cleanup();
    
private slots: // test functions
    void testFamilyModelWithDefaultData();
    void testFamilyModelWithData();
    
private: // data
   QContact* mContact;
   HbDataForm* mForm;
   };

#endif /* T_FAMILYEDITOR_H_ */
