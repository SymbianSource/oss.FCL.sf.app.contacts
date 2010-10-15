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

#ifndef CNTACTIVITIES_H_
#define CNTACTIVITIES_H_

#include "cntviewparams.h"

class AfActivation;
class AfActivityStorage;

/*
* Class for load and save activities of contacts application.
*/
class CntActivities : public QObject 
{ 
    Q_OBJECT
    
public:
    CntActivities();
    ~CntActivities();
    bool loadActivity(QByteArray &serializedModel);
    void saveActivity(const QString &name, const QByteArray &serializedActivity, const QPixmap &screenshot);
    void removeActivities();
      
private:
    AfActivation*           mActivation; 
    AfActivityStorage*      mActivityStorage;

friend class TestCntActivities;
    
};

#endif /* CNTACTIVITIES_H_ */
