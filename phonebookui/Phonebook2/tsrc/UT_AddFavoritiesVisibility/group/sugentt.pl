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
#!perl
use strict;
use File::Find;
use File::Copy;
use File::Spec::Functions;
use File::Basename;
use File::Path;

# Verify command line parameters
if   ($#ARGV != 2 )
{
	PrintHelp();
}

# Take target path or use default
my $srcPath=$ARGV[0];
my $targetPath=$ARGV[1];
my $moduleClassName=$ARGV[2];

opendir( srcDir, $srcPath) or die "couldn't open $srcPath: $!\n";
my @src_files = readdir(srcDir);
close(srcDir);
print "Files there $#src_files\n";

my @tc_table; #table with test cases
my $f;
foreach $f (@src_files)
{
    unless ( ($f eq ".") || ($f eq "..") )
    {
        my $fromPath = catfile($srcPath, $f);
        
        if($f  =~ /.*.cpp/)
        {
            ProcessCpp($fromPath);
        }     
    }
}

print "generate testcase table\n";
open (OUTFILE, ">".$targetPath ) || die ("Can not open $targetPath");
foreach (@tc_table)
{
    print OUTFILE "\nENTRY(@$_[0],@$_[1],\n\t${moduleClassName}::@$_[2],\n\t${moduleClassName}::@$_[3],\n\t${moduleClassName}::@$_[4]),\n";
}
close (OUTFILE);

print "DONE\n";
        

sub ProcessCpp()
{
    my $fromPath = $_[0];
    
	open (INFILE, $fromPath ) || die ("Can not find $fromPath");

	# Replace text in files
	my $in_table = 0;
	my $in_case = 0;
	my $case_args;
	while (<INFILE>)
	{
	  ## search for "//TESTCASE("Name", "TestClass", Setup1L, TestDeletedL, TeardownL)"
	  if (/^\s*\/\/\s*TESTCASE\(\s*(.*)\s*,\s*(.*)\s*,\s*(.*)\s*,\s*(.*)\s*,\s*(.*)\s*\)/s )
	  {
	    print "TC: $1, $2, $3, $4, $5\n";
	    push(@tc_table, [$1, $2, $3, $4, $5]);
	  }
	}

	close (INFILE);
}

sub PrintHelp()
{
	print "sugentt srcDir targetFile\n";
	print "\n";
	exit;
}
