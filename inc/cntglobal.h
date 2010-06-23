/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CNTGLOBAL_H
#define CNTGLOBAL_H

#include <QtGlobal>

#ifdef PBK_UNIT_TEST
#define QTPBK_EXPORT
#else
#ifdef BUILD_QTPBK
#define QTPBK_EXPORT Q_DECL_EXPORT
#else
#define QTPBK_EXPORT Q_DECL_IMPORT
#endif
#endif


// Format: qtcontacts:<managerid>:<key>=<value>&<key>=<value>
const QString SYMBIAN_BACKEND     = "qtcontacts:symbian:";
const QString SIM_BACKEND         = "qtcontacts:symbiansim:";
const QString SIM_BACKEND_ADN     = "qtcontacts:symbiansim:store=ADN";
const QString SIM_BACKEND_SDN     = "qtcontacts:symbiansim:store=SDN";


#endif // CNTGLOBAL_H
