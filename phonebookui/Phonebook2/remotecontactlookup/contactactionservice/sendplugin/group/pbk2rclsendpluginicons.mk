#
# Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
# Description:  Make file for sendplugin icons.
#

ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=\epoc32\release\$(PLATFORM)\$(CFG)\Z
else
ZDIR=\epoc32\data\z
endif

TARGETDIR=$(ZDIR)\RESOURCE\APPS
HEADERDIR=\epoc32\include
ICONTARGETFILENAME=$(TARGETDIR)\pbk2rclsendplugin.mif

HEADERFILENAME=$(HEADERDIR)\pbk2rclsendplugin.MBG

SOURCEDIR=$(EPOCROOT)\epoc32\s60\icons

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE :
	mifconv $(ICONTARGETFILENAME) /H$(HEADERFILENAME) \
		/c16,8 $(SOURCEDIR)\qgn_prop_cmail_action_msg.svg \
		/c16,8 $(SOURCEDIR)\qgn_prop_cmail_action_audio.svg \
		/c16,8 $(SOURCEDIR)\qgn_prop_cmail_action_postcard.svg \
		/c16,8 $(SOURCEDIR)\qgn_prop_cmail_action_email.svg \
		
FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(HEADERFILENAME)&& \
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing