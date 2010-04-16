#                                                                    
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).   
# All rights reserved.                                                
# This component and the accompanying materials are made available    
# under the terms of "Eclipse Public License v1.0"    
# which accompanies this distribution, and is available               
# at the URL "http://www.eclipse.org/legal/epl-v10.html".   
#                                                                     
# Initial Contributors:                                               
# Nokia Corporation - initial contribution.                           
#                                                                     
# Contributors:                                                       
#                                                                     
# Description:                                                        
#                                                                     
#

TEMPLATE = subdirs
CONFIG += ordered

symbian: {
    :BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>"
    :BLD_INF_RULES.prj_exports += "rom/logs.iby CORE_APP_LAYER_IBY_EXPORT_PATH(logs.iby)"
    :BLD_INF_RULES.prj_exports += "rom/logsresources.iby LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(logsresources.iby)"
    :BLD_INF_RULES.prj_exports += "rom/logs_stub.sis /epoc32/data/z/system/install/logs_stub.sis"
    :BLD_INF_RULES.prj_exports += "cenrep/backup_registration.xml /epoc32/data/z/private/101F4CD5/backup_registration.xml"
    :BLD_INF_RULES.prj_exports += "conf/logs.confml APP_LAYER_CONFML(logs.confml)"
    :BLD_INF_RULES.prj_exports += "conf/logs_101F874E.crml APP_LAYER_CRML(logs_101F874E.crml)"
    :BLD_INF_RULES.prj_exports += "conf/logs_102750C6.crml APP_LAYER_CRML(logs_102750C6.crml)"
    :BLD_INF_RULES.prj_exports += "inc/logsexport.h /epoc32/include/logsexport.h"
    :BLD_INF_RULES.prj_exports += "../contacts_plat/logs_services_api/inc/logsservices.h APP_LAYER_PLATFORM_EXPORT_PATH(logsservices.h)"
    :BLD_INF_RULES.prj_exports += "../contacts_plat/logs_engine_api_extensions/inc/LogsApiConsts.h APP_LAYER_PLATFORM_EXPORT_PATH(LogsApiConsts.h)"
    :BLD_INF_RULES.prj_exports += "../contacts_plat/logs_timers_api/inc/LogsDomainCRKeys.h APP_LAYER_PLATFORM_EXPORT_PATH(LogsDomainCRKeys.h)" 
    :BLD_INF_RULES.prj_exports += "../contacts_plat/logs_engine_api/inc/logsmodel.h APP_LAYER_PLATFORM_EXPORT_PATH(logsmodel.h)"
    :BLD_INF_RULES.prj_exports += "../contacts_plat/logs_engine_api/inc/logsabstractmodel.h APP_LAYER_PLATFORM_EXPORT_PATH(logsabstractmodel.h)"
    :BLD_INF_RULES.prj_exports += "../contacts_plat/logs_engine_api/inc/logsfilter.h APP_LAYER_PLATFORM_EXPORT_PATH(logsfilter.h)"
    :BLD_INF_RULES.prj_exports += "../contacts_plat/logs_engine_api/inc/logscustomfilter.h APP_LAYER_PLATFORM_EXPORT_PATH(logscustomfilter.h)"
    :BLD_INF_RULES.prj_exports += "../contacts_plat/logs_engine_api/inc/logsevent.h APP_LAYER_PLATFORM_EXPORT_PATH(logsevent.h)"
	exists(confml/logsuda.confml) :BLD_INF_RULES.prj_exports += "confml/logsuda.confml CONFML_EXPORT_PATH(logsuda.confml,uda_content)"
	exists(implml/logsuda.implml) :BLD_INF_RULES.prj_exports += "implml/logsuda.implml CRML_EXPORT_PATH(logsuda.implml,uda_content)"
	exists(content/LOGDBU.zip) :BLD_INF_RULES.prj_exports += "content/LOGDBU.zip CRML_EXPORT_PATH(../content/zip/,uda_content)"

}

SUBDIRS += logsservices
SUBDIRS += logscntfinder
SUBDIRS += logsengine
SUBDIRS += logsapp
