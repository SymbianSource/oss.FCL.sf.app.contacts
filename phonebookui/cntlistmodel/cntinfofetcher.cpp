/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Fetches visual info about contacts.
*
*/

#include <QPluginLoader>
#include <QDir>
#include <qtcontacts.h>
#include <cntinfofetcher.h>
#include <cntinfoproviderfactory.h>
#include <cntinfoprovider.h>
#include <cntdefaultinfoprovider.h>
#include <cntpresenceinfoprovider.h>
#include <cntdebug.h>

/*!
    \class CntInfoCacheItem
    \brief Cache item that holds some visual info about a contact.
 */

/*!
    \class CntInfoFetcher
    \brief CntInfoFetcher asynchronously fetches visual info about contacts.

    CntInfoFetcher queues requests for contact info that is to be cached.
    It fetches the info later, when asked by the client to do so.

    There are three pieces of info that is fetched:
      - secondary text, e.g. phone number
      - primary icon, e.g. avatar for contact
      - secondary icon, e.g. presence status

    Internally CntInfoFetcher uses plugins (info providers) to fetch the data.
 */

// maximum amount of scheduled jobs; if there are more jobs, the least
// important job is cancelled
const int MaxInfoJobs = 20;

// directory of info providers
const char *CNT_INFO_PROVIDER_EXTENSION_PLUGIN_DIRECTORY = "/resource/qt/plugins/contacts/infoproviders/";

/*!
    Creates a CntIconFetcher object.
 */
CntInfoFetcher::CntInfoFetcher(QContactManager *contactManager)
    : CntAbstractFetcher(MaxInfoJobs),
      mContactManager(contactManager)
{
    CNT_ENTRY

    // create static info provider plugins
    mInfoProviders.insert(new CntDefaultInfoProvider(), ContactInfoAllFields);
    mInfoProviders.insert(new CntPresenceInfoProvider(), ContactInfoIcon2Field);

    // load dynamic info provider plugins
    QDir pluginsDir(CNT_INFO_PROVIDER_EXTENSION_PLUGIN_DIRECTORY);
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        // create plugin loader
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        if (pluginLoader.load()) {
            CntInfoProviderFactory *factory = qobject_cast<CntInfoProviderFactory*>(pluginLoader.instance());
            if (factory) {
                CntInfoProvider *provider = factory->infoProvider();
                mInfoProviders.insert(provider, provider->supportedFields());
            }
        }
    }

    // connect the providers
    QMapIterator<CntInfoProvider*, ContactInfoFields> i(mInfoProviders);
    while (i.hasNext()) {
        i.next();
        connect(static_cast<CntInfoProvider*>(i.key()),
                SIGNAL(infoFieldReady(CntInfoProvider*, int, ContactInfoField, const QString&)),
                this,
                SLOT(forwardInfoToClient(CntInfoProvider*, int, ContactInfoField, const QString&)));
    }

    CNT_EXIT
}

/*!
    Cleans up and destructs the CntIconFetcher object.
 */
CntInfoFetcher::~CntInfoFetcher()
{
    CNT_ENTRY

    qDeleteAll(mInfoProviders.keys());
    mInfoProviders.clear();

    CNT_EXIT
}

/*!
    Processes the next scheduled job. This function must not be
    called until the previous job has completed.
 */
void CntInfoFetcher::processNextJob()
{
    CNT_ENTRY

    if (hasScheduledJobs()) {
        // get the next job
        CntInfoJob *job = static_cast<CntInfoJob *>(takeNextJob());

        // fetch a QContact object
        QContactFetchHint restrictions;
        restrictions.setOptimizationHints(QContactFetchHint::NoRelationships);
        QContact contact = mContactManager->contact(job->contactId, restrictions);
        
        // request contact info from providers
        QMapIterator<CntInfoProvider*, ContactInfoFields> i(mInfoProviders);
        while (i.hasNext()) {
            i.next();
            if (i.value() != 0) {
                i.key()->requestInfo(contact, i.value());
            }
        }
        
        delete job;
    } else if (hasCancelledJobs()) {
        CntInfoJob *cancelledJob = static_cast<CntInfoJob *>(takeNextCancelledJob());
        emit infoCancelled(cancelledJob->contactId);
        delete cancelledJob;
    }

    CNT_EXIT
}

/*!
    \return true if a job is currently being processed; otherwise returns false.
 */
bool CntInfoFetcher::isProcessingJob()
{
    return false;
}

/*!
    Forward info from a provider to the client.
 */
void CntInfoFetcher::forwardInfoToClient(CntInfoProvider *sender,
                                         int contactId,
                                         ContactInfoField field,
                                         const QString &text)
{
    CNT_ENTRY

    // there can be 3rd party info providers, so we cannot blindly trust them;
    // info is emitted iff:
    if (mInfoProviders.contains(sender)
        && ((field & (field - 1)) == 0)                         // 1) exactly one field bit is set in parameter 'field'
        && ((field & mInfoProviders.value(sender)) != 0)) {     // 2) the field bit has been assigned to this provider
        emit infoUpdated(contactId, field, text);
    }

    CNT_EXIT
}
