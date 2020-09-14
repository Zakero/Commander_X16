#!/bin/bash
################################################################################
# 
# Mount an SD Card image
# 
################################################################################
#
# Copyright 2020 Andrew Moore
#
#    sdcard.sh is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    sdcard.sh is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with sdcard.sh.  If not, see <https://www.gnu.org/licenses/>.
#
################################################################################
# 
# Exit Error Codes:
# 
# The exit error codes are based on the /usr/include/sysexits.h for all
# non-zero errors.  An exit code of '0' is success.
#
###############################################################################
#
# Bugs:
# - Nothing to admit
#
###############################################################################
#
# Todo:
# - Add a read-only mode
# - Add more Todo items
#
###############################################################################

###############################################################################
# Variables: Read-Only

########################################
# Error Codes
readonly Error_Data=65         # EX_DATAERR
readonly Error_NoInput=66      # EX_NOINPUT
readonly Error_NoPermission=77 # EX_NOPERM
readonly Error_OsFile=72       # EX_OSFILE
readonly Error_TempFail=75     # EX_TEMPFAIL
readonly Error_Unavailable=69  # EX_UNAVAILABLE
readonly Error_Usage=64        # EX_USAGE

########################################
# Other Values
readonly program_exec="$0"


###############################################################################
# Variables: Arg Configurable
mount="Yes"
sdcard=""
sdcard_img=""


###############################################################################
# Check Commands

if [ -z $(command -v losetup) ]
then
	echo "Error: \"losetup\" is not available. Try running as root."
	exit $Error_OsFile
fi


###############################################################################
main()
{
	if [ -n "$mount" ]
	then
		loop_device_attach
		sdcard_mount
	else
		sdcard_unmount
		loop_device_detach
	fi
}


###############################################################################
loop_device_attach()
{
	loopdev=$(losetup --find)

	losetup --partscan $loopdev "$sdcard_img"
	err=$?
	if [ "$err" != "0" ]
	then
		echo "Error: Failed to attach loop-device to \"$sdcard_img\""
		exit $err
	fi
}


###############################################################################
loop_device_detach()
{
	loopdev=$(losetup --associated "$sdcard_img" | cut -d: -f1)
	if [ -z "$loopdev" ]
	then
		return
	fi
	
	losetup --detach $loopdev
	err=$?
	if [ "$err" != "0" ]
	then
		echo "Error: Failed to detach loop-device from \"$sdcard_img\""
		exit $err
	fi
}


###############################################################################
sdcard_mount()
{
	if [ ! -d "$sdcard" ]
	then
		mkdir "$sdcard"
	fi

	loopdev=$(losetup --associated "$sdcard_img" | cut -d: -f1)

	mount --options umask=000 ${loopdev}p1 "$sdcard"
	err=$?
	if [ "$err" != "0" ]
	then
		echo "Error: Unable to mount \"$sdcard_img\""
		loop_device_detach
		exit $err
	fi
}


###############################################################################
sdcard_unmount()
{
	if [ ! -d "$sdcard" ]
	then
		echo "Error: Directory \"$sdcard\" does not exist"
		exit Error_Unavailable
	fi

	umount "$sdcard"
	rmdir "$sdcard"
}


###############################################################################
usage()
{
	cat <<- USAGE_END
	Usage: $program_exec [Options] IMAGE[.img]

	IMAGE
	  The SD Card image to mount.  If the file extension is not provided,
	  a ".img" will be appended to the file name.

	Options:
	  -h, --help
	    Show this message and exit.

	  -m, --mount
	    Mount the image.  This is the default action if no other options
	    are provided

	  -u, --unmount
	    Unmount the image.

	USAGE_END
}


###############################################################################
# Parse the args

while [ "$1" != "" ]
do
	case "$1" in
		-h|--help)
			usage
			exit 0
			;;
		-m|--mount)
			mount="Yes"
			;;
		-u|--unmount)
			mount=""
			;;
		-*)
			echo "Error: Option \"$1\" is not supported."
			echo "Use \"-h\" or \"--help\" for help:"
			echo "  $program_exec --help"
			exit $Error_Usage
			;;
		*)
			image="$1"
	esac
	shift
done

###############################################################################
# Make sure that all the required information is present

########################################
# Check Args
if [ -z "$image" ]
then
	echo "Error: No image file provided"
	exit $Error_Usage
fi

########################################
# SD Card variables
file_name=$(basename -- "$image")
dir=$(dirname "$image")
sdcard="$dir/${file_name%.*}"

case "$file_name" in
	*.*) sdcard_img="$dir/$sdcard.${file_name##*.}" ;;
	*)   sdcard_img="$dir/$sdcard.img"              ;;
esac

if [ ! -f "$sdcard_img" ]
then
	echo "Error: Can not find \"$sdcard_img\""
	exit $Error_NoInput
fi

########################################
# Chech "write" permissions
tmp_dir="$dir/.$$"
mkdir "$tmp_dir"
if [ "$?" != "0" ]
then
	echo "Error: No write permission: $dir"
	exit $Error_NoPermission
fi
rmdir "$tmp_dir"

################################################################################
main
exit

