#
# Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
#             Phonebook 2 USIM UI Extension icons makefile.
# ===========================================================================
#


# ---------------------------------------------------------------------------
# Inspect platform variables to find out root of the Z drive.
# ---------------------------------------------------------------------------
ifeq (WINS,$(findstring WINS, $(PLATFORM)))
  ZDIR=\epoc32\release\$(PLATFORM)\$(CFG)\Z
else
  ZDIR=\epoc32\data\z
endif

# ---------------------------------------------------------------------------
# Set target directories for the icons.
#----------------------------------------------------------------------------

MIF_TARGETDIR=$(ZDIR)\resource\apps

# ---------------------------------------------------------------------------
# Define the icon and header files using directories defined above.
# ---------------------------------------------------------------------------
HEADERDIR=\epoc32\include
MIF_ICONFILE=$(MIF_TARGETDIR)\psu2.mif
MIF_HEADERFILE=$(HEADERDIR)\psu2.mbg

# ---------------------------------------------------------------------------
# Inspect the SCALABLE_FLAG to decide which bitmaps will be used.
# ---------------------------------------------------------------------------
ifeq ($(SCALABLE_FLAG), off)
  SIMIN_CONTEXT_ICON=/c8,8 qgn_menu_simin_cxt
  SIMFDN_CONTEXT_ICON=/c8,8 qgn_menu_simfdn_cxt
else
  SIMIN_CONTEXT_ICON=/c8,8 qgn_menu_simin
  SIMFDN_CONTEXT_ICON=/c8,8 qgn_menu_simfdn
endif

# ---------------------------------------------------------------------------
# Empty rules.
# ---------------------------------------------------------------------------
do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

# ---------------------------------------------------------------------------
# NOTE 1: DO NOT DEFINE MASK FILE NAMES! They are included automatically by
# MifConv if the mask detph is defined.
#
# NOTE 2: Usually, source paths should not be included in the bitmap
# definitions. MifConv searches for the icons in all icon directories in a
# predefined order, which is currently \s60\icons, \s60\bitmaps2,
# \s60\bitmaps.
# The directory \s60\icons is included in the search only if the feature
# flag __SCALABLE_ICONS is defined.
# ---------------------------------------------------------------------------

RESOURCE : MIFICONS

MIFICONS :
	mifconv $(MIF_ICONFILE) /h$(MIF_HEADERFILE) \
		/c8,1 qgn_prop_nrtyp_sdn \
		/c8,8 qgn_prop_nrtyp_sim_contact \
		$(SIMIN_CONTEXT_ICON) \
		$(SIMFDN_CONTEXT_ICON)
FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(MIF_HEADERFILE) && \
	@echo $(MIF_ICONFILE)

FINAL : do_nothing

# End of File
