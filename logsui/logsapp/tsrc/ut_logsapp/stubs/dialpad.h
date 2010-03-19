/*!
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
* Description: Dialpad popup
*
*/

#ifndef DIALPAD_H
#define DIALPAD_H

#include <hbwidget.h>

#include <QTime>
#include <QTimeLine>

class HbLineEdit;

const int DialpadButtonCount = 13;

class Dialpad : public HbWidget
{
    Q_OBJECT

public:
    explicit Dialpad();
    virtual ~Dialpad();

    
    HbLineEdit& editor() const;
    
public slots:

    void openDialpad() { setVisible(true); }

    void closeDialpad() { setVisible(false); }

    void setCallButtonEnabled(bool enabled);
    
signals:
    void aboutToClose();

public:
    HbLineEdit* mLineEdit; 
    bool mIsCallButtonEnabled;
};

#endif // DIALPAD_H
