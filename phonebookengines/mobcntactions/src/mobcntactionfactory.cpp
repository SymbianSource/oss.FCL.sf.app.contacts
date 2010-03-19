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
#ifndef ACTIONFACTORYPLUGINTARGET
#define ACTIONFACTORYPLUGINTARGET mobcntactionplugin
#endif
#ifndef ACTIONFACTORYPLUGINNAME
#define ACTIONFACTORYPLUGINNAME SymbianActionFactory
#endif

#include "mobcntactionfactory.h"
#include "mobcntcallaction.h"
#include "mobcntvideocallaction.h"
#include "mobcntmessageaction.h"
#include "mobcntemailaction.h"

#define makestr(x) (#x)
#define makename(x) makestr(x)


//Factory class
MobCntActionFactory::MobCntActionFactory() 
{
	actionList.append(new MobCntCallAction());
	actionList.append(new MobCntVideoCallAction());
    actionList.append(new MobCntMessageAction());
    actionList.append(new MobCntEmailAction());
}

MobCntActionFactory::~MobCntActionFactory()
{
	 while (!actionList.isEmpty())
	     delete actionList.takeFirst();
}

QString MobCntActionFactory::name() const
{
    return QString(makename(ACTIONFACTORYPLUGINNAME));
}


QList<QContactActionDescriptor> MobCntActionFactory::actionDescriptors() const
{
   QList<QContactActionDescriptor> descriptorList; 
   
   //loop through all the actions and add the descriptor to the list
   for (int i = 0; i < actionList.size(); i++)
   {
	   descriptorList << actionList.at(i)->actionDescriptor();   
   }
   
   return descriptorList;
}

QContactAction* MobCntActionFactory::instance(const QContactActionDescriptor& descriptor) const
{
    QContactAction *action(0);
	
    //loop through the actions and return the one that supports the descriptor
    for (int i = 0; i < actionList.size() && action == 0; i++)
    {
    	if (actionList.at(i)->actionDescriptionSupported(descriptor)){
    		//create a new heap object of the action
    		action = actionList.at(i)->clone();
    	}    
    }
  
    return action;
}

QVariantMap MobCntActionFactory::actionMetadata(const QContactActionDescriptor& descriptor) const
{
    Q_UNUSED(descriptor);
    
    return QVariantMap();
}

Q_EXPORT_PLUGIN2(ACTIONFACTORYPLUGINTARGET, MobCntActionFactory);
