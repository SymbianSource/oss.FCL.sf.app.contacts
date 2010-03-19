#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).

#
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
SUBDIRS = pbkcommonui phonebookapp phonebookservices
CONFIG += ordered

deploy.path = /epoc32/rom/include/core/app/

# Build.inf rules
BLD_INF_RULES.prj_exports += \
     "$${LITERAL_HASH}include <platform_paths.hrh>" \
     "./rom/phonebookservices.iby                CORE_APP_LAYER_IBY_EXPORT_PATH(phonebookservices.iby)" \
     "./rom/pbk.iby                              CORE_APP_LAYER_IBY_EXPORT_PATH(pbk.iby)" \
     "./rom/pbkresources.iby                     LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(pbkresources.iby)"


DEPLOYMENT += romfiles
