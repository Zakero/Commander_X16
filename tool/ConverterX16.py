#!/usr/bin/env python3
################################################################################
#
# Convert data so that it can be used with the Commander X16
#
################################################################################
#
# Copyright 2020 Andrew Moore
#
#    ConverterX16.py is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    ConverterX16.py is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with ConverterX16.py.  If not, see <https://www.gnu.org/licenses/>.
#
################################################################################
#
# Bugs
# - None created yet
#
################################################################################
#
# Todo
# - Add Tilemap Support [In-Progress]
# - Add Sprite Support
# - Make the "--makefile" command-line option SSD friendly by calculating a
#   hash (MD5?) of the new contents and comparing that with the has in the
#   previously generated makefile.  If the same, don't write a new file.
#   - Or just compare the contents of the file...
# - Be able to specify the output file name
#
################################################################################

import argparse
import configparser
import os
import sys
import textwrap

from PIL import Image


################################################################################
# Global Variables
version_info = (0, 4, 0)


################################################################################
# Check Python Version
if sys.version_info[0] < 3:
	print(
		"Python " + str(sys.version_info[0]) + " is not supported."
		"  Use Python 3 or a more recent version."
		)
	sys.exit(1)


################################################################################
# Configure the Application
def configure():
	########################################
	# Build configuration from the args

	version_string='.'.join(str(v) for v in version_info)

	parser = argparse.ArgumentParser(add_help = True
		, description =
			f'Convert resource data into a format that the'
			f' Commander X16 can use natively.  The files that are'
			f' to be converted and to what format is determined by'
			f' a resource file. To learn more about the resource'
			f' file use the "--help-resource" option.'
			f'\n'
			f'\nversion:'
			f'\n  {version_string}'
		, formatter_class = argparse.RawTextHelpFormatter
		)

	parser.add_argument('--case'
		, help    = 'Convert the output filename to all upper (or lower) case.'
		, choices = [ 'lower', 'upper' ]
		, default = None
		, type    = str
		)

	parser.add_argument('--convert-bitmap'
		, help    = 'Only convert the bitmap resource'
		, action  = 'store'
		, dest    = 'convert_bitmap'
		, metavar = 'BITMAP'
		)

	parser.add_argument('--convert-file'
		, help    = 'Only convert the resources related to the file'
		, action  = 'store'
		, dest    = 'convert_file'
		, metavar = 'FILE'
		)

	parser.add_argument('--convert-palette'
		, help    = 'Only convert the palette resource'
		, action  = 'store'
		, dest    = 'convert_palette'
		, metavar = 'PALETTE'
		)

	parser.add_argument('--convert-tilemap'
		, help    = 'Only convert a tilemap resource'
		, action  = 'store'
		, dest    = 'convert_tilemap'
		, metavar = 'TILE_SET'
		)

	parser.add_argument('--convert-tileset'
		, help    = 'Only convert a tileset resource'
		, action  = 'store'
		, dest    = 'convert_tileset'
		, metavar = 'TILE_SET'
		)

	parser.add_argument('--help-resource'
		, help   = 'Print information about the resource file'
		, action = 'store_true'
		, dest   = 'help_resource'
		)

	parser.add_argument('--list-input-files'
		, help   = 'Print a list of input files and exit'
		, action = 'store_true'
		, dest   = 'list_input_files'
		)

	parser.add_argument('--list-output-files'
		, help   = 'Print a list of output files and exit'
		, action = 'store_true'
		, dest   = 'list_output_files'
		)

	parser.add_argument('--makefile'
		, help   = 'Generate a GNU Makefile that can be included by other Makefiles'
		, action = 'store_true'
		, dest   = 'generate_makefile'
		)

	parser.add_argument('--output-dir'
		, help   = 'Specify where to write the data files. Default: "."'
		, action = 'store'
		, dest   = 'output_dir'
		)

	parser.add_argument('-v', '--verbose'
		, help    = 'Print information during execution: -v = Print output files, -vv = Print status info, -vvv = Extra debugging output'
		, action  = 'count'
		, default = 0
		)

	parser.add_argument('resource'
		, action  = 'store'
		, default = None
		, metavar = 'RESOURCE_FILE'
		, nargs   = '?'
		, type    = str
		)

	arg = parser.parse_args()

	arg.version = version_info

	arg.output_ext = \
	{	'bitmap'  : 'x16b'
	,	'palette' : 'x16p'
	,	'sprite'  : 'x16s'
	,	'tilemap' : 'x16m'
	,	'tileset' : 'x16t'
	}

	return arg


################################################################################
# {{{ Help Functions
################################################################################

################################################################################
def help_resource(converter):
	app_name = __file__
	print(f'''\
The Resource File

The purpose of the resource file is to instruct ConverterX16.py which files 
are available for conversion to the Commander X16 data format as well as the 
type of conversion to be performed.  To accomplish this, a resource file that 
uses the INI format will be processed by ConverterX16.py.

The Section titles in the resource "ini" file are parsed to get the name of 
the resource and the type of resource:
	[NAME TYPE]
The name and type are separated by white-space.  For this reason, the NAME of 
the resource can not contain any white-space.  The converted data will be 
written to a file using the resource name.  So a resource name "Foo" will 
result in a file called "Foo".

The file will also have an extension based on the resource type:
	Bitmap  ==> .{converter.output_ext["bitmap"]}
	Palette ==> .{converter.output_ext["palette"]}

Continuing with the "Foo" example, if the resource type is "Bitmap" then the 
converted file will be "Foo.{converter.output_ext["bitmap"]}".

Below is an example of a series of Section titles for a fictional game:
	[TitleScreen Bitmap]

	[TitleScreen Palette]

	[UI Palette]

	[UI_Alien Tileset]

	[UI_Human Tileset]
         ''')

	return


################################################################################
# }}}
################################################################################

################################################################################
# {{{ Arg Functions
################################################################################

################################################################################
# Print all input files
def list_input_files(converter):
	resource = configparser.ConfigParser()
	resource.read(converter.resource)

	file_list = set()

	for section_id in resource.sections():
		filename = _filename_get(resource[section_id])
		if filename is None:
			continue

		file_list.add(filename)
	
	filename_list = ' '.join(file_list)
	print(f'{filename_list}')

	return


################################################################################
# Print all output files
def list_output_files(converter):
	resource = configparser.ConfigParser()
	resource.read(converter.resource)

	file_list = set()

	for section_id in resource.sections():
		resource_type = _resource_type(section_id)
		if resource_type is None:
			_log_warning(converter, f'Not a resource section: "{section_id}"')
			continue
		if resource_type not in converter.output_ext:
			_log_warning(converter, f'Unknown resource type: {resource_type}')
			continue

		filename = _output_filename(converter, section_id)
		file_list.add(filename)
	
	filename_list = ' '.join(file_list)
	print(f'{filename_list}')

	return


################################################################################
# Generate a Makefile
def generate_makefile(converter):
	resource = configparser.ConfigParser()
	resource.read(converter.resource)

	case = converter.case
	if case is None:
		case = ''
	else:
		case = f'--case {case}'

	output_dir = converter.output_dir
	if output_dir is None:
		output_dir = ''
	else:
		output_dir = f'--output-dir {output_dir}'


	makefile = ''

	for section_id in resource.sections():
		src = _filename_get(resource[section_id])
		if src is None:
			src = ''

		resource_name = _resource_name(section_id)
		resource_type = _resource_type(section_id)
		if resource_type is None:
			_log_warning(converter, f'Not a resource section: "{section_id}"')
			continue
		if resource_type not in converter.output_ext:
			_log_warning(converter, f'Unknown resource type: {resource_type}')
			continue

		dst = _output_filename(converter, section_id)

		makefile += textwrap.dedent(f'''\
		{dst}: {converter.resource} {src}
			$(ConverterX16) --convert-{resource_type} {resource_name}
		''')
	
	filename = ".ConverterX16.Makefile."
	output = open(filename, "w")
	output.write(makefile)
	output.close()

	print(f'{filename}')

	return


################################################################################
# Convert all resources
def convert(converter, section_list):
	_log_info(converter, f'Building Resources: {section_list}')

	resource = configparser.ConfigParser()
	resource.read(converter.resource)

	for section_id in section_list:
		_log_info(converter, f'Resource Name: "{_resource_name(section_id)}"')
		_log_info(converter, f'Resource Type: "{_resource_type(section_id)}"')

		section_ini = resource[section_id]

		resource_type = _resource_type(section_id)
		if resource_type == 'bitmap':
			_convert_bitmap(converter, section_id, section_ini)
		elif resource_type == 'palette':
			_convert_palette(converter, section_id, section_ini)
		elif resource_type == 'tilemap':
			_convert_tilemap(converter, section_id, section_ini)
		elif resource_type == 'tileset':
			_convert_tileset(converter, section_id, section_ini)


################################################################################
# }}}
################################################################################

################################################################################
# {{{ Queries
################################################################################

################################################################################
# Get a Section ID list
def get_section(converter, name, type):
	resource = configparser.ConfigParser()
	resource.read(converter.resource)

	section_list = []

	for section_id in resource.sections():
		resource_name = _resource_name(section_id)
		resource_type = _resource_type(section_id)

		if resource_name == name and resource_type == type:
			_log_debug(converter, f'Section ID: {section_id}')
			section_list.append(section_id)
			break

	_log_debug(converter, f'Section List: {section_list}')

	if len(section_list) == 0:
		_error(f'Resource "[{name} {type}]" not found')

	return section_list


################################################################################
# Get Section ID list
def get_section_all(converter_):
	resource = configparser.ConfigParser()
	resource.read(converter_.resource)

	section_list = []

	for section_id in resource.sections():
		resource_type = _resource_type(section_id)
		if resource_type is None:
			_log_warning(converter, f'Not a resource section: "{section_id}"')
			continue
		if resource_type not in converter.output_ext:
			_log_warning(converter, f'Unknown resource type: {resource_type}')
			continue
		section_list.append(section_id)

	if len(section_list) == 0:
		_error(f'No resources were found')

	return section_list


################################################################################
# Get a Section ID list
def get_sections_for_file(converter_, filename_):
	resource = configparser.ConfigParser()
	resource.read(converter_.resource)

	section_list = []

	for section_id in resource.sections():
		filename = _filename_get(resource[section_id])
		if filename == filename_:
			section_list.append(section_id)

	if len(section_list) == 0:
		_error(f'No resources use the file "{filename_}"')

	return section_list

################################################################################
# }}}
################################################################################

################################################################################
# {{{ Convert : Bitmap
################################################################################

################################################################################
# Determine the output target based on the input file extension.
def _convert_bitmap(converter, section_id, section_ini):
	filename = _filename_get(section_ini)
	if filename is None:
		_error(f'Ini Key "File" not found')

	_log_info(converter, f'File: {filename}')

	filename = _filename_normalize(filename)
	_filename_validate(filename)

	_filename_set(section_ini, filename)

	image = Image.open(filename)

	_log_info(converter
		, f"Format: {image.format}, " \
		  f"Size: {image.size[0]}x{image.size[1]}, " \
		  f"Color: {image.mode}"
		)

	if image.size[0] > 640 or image.size[1] > 480:
		log_warning(converter, "Image maybe too large.")

	data = []
	if image.format == 'GIF':
		bits_per_pixel = _bits_per_pixel_get(section_ini)
		data = _load_gif(image, bits_per_pixel)
	else:
		_error(f'Unsupported image type: {image.format}')
	
	address = _address_get(section_ini)
	filename = _output_filename(converter, section_id)
	_write_data(converter, address, data, filename)

	return


################################################################################
# }}}
################################################################################

################################################################################
# {{{ Convert : Palette
################################################################################

################################################################################
# Determine the source of the palette data
def _convert_palette(converter, section_id, section_ini):
	data = []
	filename = _filename_get(section_ini)
	if filename is None:
		data = _convert_palette_ini_section(converter, section_ini)
	else:
		_log_debug(converter, f'File: {filename}')

		filename = _filename_normalize(filename)
		_filename_validate(filename)
		_filename_set(section_ini, filename)

		ext = filename.split('.')[-1]
		if ext == 'gif':
			data = _convert_palette_gif(converter, section_ini)
		elif ext == 'ini':
			data = _convert_palette_ini(converter, section_ini)
		else:
			_error(f'Palette resources do not support file extension ".{ext}"')

	address = _address_get(section_ini)
	filename = _output_filename(converter, section_id)
	_write_data(converter, address, data, filename)

	return


################################################################################
# Extract the palette information from the GIF and convert it to a format the
# can be directly loaded into the CommanderX16's Vera chip
def _convert_palette_gif(converter, section_ini):
	data = []
	filename = _filename_get(section_ini)
	image = Image.open(filename)
	palette = image.getpalette()
	_log_debug(converter, f"Palette R,G.B,...: {palette}")

	rgb_index = 0
	r = 0
	g = 0
	b = 0
	flip_flop = True

	for c in palette:
		if rgb_index == 0:
			rgb_index += 1
			r = c
		elif rgb_index == 1:
			rgb_index += 1
			g = c
		else:
			rgb_index = 0
			b = c
			
			byte = (g & 0xf0) | (b >> 4)
			data.append(byte)
			byte = (r & 0xf0) >> 4
			data.append(byte)

	_log_debug(converter, f'Data BG,0R,...: {data}')

	return data


################################################################################
# Convert the INI palette data to a format the can be directly loaded into the
# CommanderX16's Vera chip
def _convert_palette_ini(converter, section_ini):
	filename = _filename_get(section_ini)

	file_ini = configparser.ConfigParser()
	file_ini.read(filename)

	if 'Palette' not in file_ini:
		_error(f'{filename} does not contain a "Palette" section')

	section_ini = file_ini['Palette']

	data = _convert_palette_ini_section(converter, section_ini)

	return data


################################################################################
# Convert the inlined palette data to a format the can be directly loaded into
# the CommanderX16's Vera chip
def _convert_palette_ini_section(converter, section_ini):
	_log_debug(converter, f'Palette: {[option for option in section_ini]}')

	index = '00'
	if index not in section_ini:
		_error(f'Palette: Unable to locate the first index "{index}"')
	
	rgb_max_count = 0xff

	data = []
	done = False
	while not done:
		row = section_ini[index]
		for rgb in row.split():
			byte = (int(rgb[0], 16) << 4) | int(rgb[1], 16)
			data.append(byte)
			byte = int(rgb[2], 16)
			data.append(byte)

		next_index = int(index, 16) + len(row.split())
		index = '{0:0{1}x}'.format(next_index, 2)
		_log_debug(converter, f'index: {index}')

		if index not in section_ini:
			_log_info(converter, f'Palette: Unable to locate index {index}, assuming done')
			break
		if next_index > rgb_max_count:
			_log_warning(converter, f'Number of RGB values exceeds the maximum of {rgb_max_count}')
			break
	
	_log_debug(converter, f'data: {data}')

	return data


################################################################################
# }}}
################################################################################

################################################################################
# {{{ Convert : Tilemap
################################################################################

################################################################################
# Determine the source of the tilemap data
def _convert_tilemap(converter, section_id, section_ini):
	data = []
	filename = _filename_get(section_ini)
	if filename is None:
		data = _convert_tilemap_ini_section(converter, section_ini)
	else:
		_log_debug(converter, f'File: {filename}')

		filename = _filename_normalize(filename)
		_filename_validate(filename)
		_filename_set(section_ini, filename)

		ext = filename.split('.')[-1]
		if ext == 'ini':
			data = _convert_tilemap_ini(converter, section_ini)
		else:
			_error(f'Tilemap resources do not support file extension ".{ext}"')

	address = _address_get(section_ini)
	filename = _output_filename(converter, section_id)
	_write_data(converter, address, data, filename)

	return


################################################################################
# Convert the tilemap data
def _convert_tilemap_ini(converter, section_ini):
	_log_debug(converter, f'Tilemap: {[option for option in section_ini]}')
	filename = _filename_get(section_ini)

	file_ini = configparser.ConfigParser()
	file_ini.read(filename)

	if 'Tilemap' not in file_ini:
		_error(f'{filename} does not contain a "Tilemap" section')

	section_ini = file_ini['Tilemap']

	data = _convert_tilemap_ini_section(converter, section_ini)

	return data



################################################################################
# Convert the tilemap data
def _convert_tilemap_ini_section(converter, section_ini):
	_log_debug(converter, f'Tilemap: {[option for option in section_ini]}')

	index = '00'
	if index not in section_ini:
		_error(f'Palette: Unable to locate the first index "{index}"')
	
	data = []
	while True:
		next_index = int(index, 16) + 1
		if next_index > 0x3ff:
			break

		field = 0
		for tile_info in section_ini[index].split():
			if field == 0:
				field += 1
				tile_index = int(tile_info, 16)
			elif field == 1:
				field += 1
				palette_offset = int(tile_info, 16)
			elif field == 2:
				field += 1
				v_flip = int(tile_info, 16)
			else:
				field = 0
				h_flip = int(tile_info, 16)
				byte = (tile_index & 0x00ff)
				data.append(byte)
				byte = (palette_offset << 4)
				byte |= (v_flip << 3)
				byte |= (h_flip << 3)
				byte |= ((tile_index >> 8) & 0x0003)
				data.append(byte)

		index = '{0:0{1}x}'.format(next_index, 2)
		_log_debug(converter, f'index: {index}')
		if index not in section_ini:
			_log_info(converter, f'Tilemap: Unable to locate index {index}, assuming done')
			break

	_log_debug(converter, f'data: {data}')

	return data


################################################################################
# }}}
################################################################################

################################################################################
# {{{ Convert : Tileset
################################################################################

################################################################################
# Determine the source of the tileset data
def _convert_tileset(converter, section_id, section_ini):
	data = []
	filename = _filename_get(section_ini)
	if filename is None:
		data = _convert_tileset_ini_section(converter, section_ini)
	else:
		_log_debug(converter, f'File: {filename}')

		filename = _filename_normalize(filename)
		_filename_validate(filename)
		_filename_set(section_ini, filename)

		ext = filename.split('.')[-1]
		if ext == 'ini':
			data = _convert_tileset_ini(converter, section_ini)
		else:
			_error(f'Tileset resources do not support file extension ".{ext}"')

	address = _address_get(section_ini)
	filename = _output_filename(converter, section_id)
	_write_data(converter, address, data, filename)

	return


################################################################################
# Convert the tileset data
def _convert_tileset_ini(converter, section_ini):
	_log_debug(converter, f'Tileset: {[option for option in section_ini]}')
	filename       = _filename_get(section_ini)

	file_ini = configparser.ConfigParser()
	file_ini.read(filename)

	if 'Tileset' not in file_ini:
		_error(f'{filename} does not contain a "Tileset" section')

	# Copy the Bits-Per-Pixel to the other INI file
	bits_per_pixel = _bits_per_pixel_get(section_ini)

	section_ini = file_ini['Tileset']

	_bits_per_pixel_set(section_ini, bits_per_pixel)

	data = _convert_tileset_ini_section(converter, section_ini)

	return data


################################################################################
# Convert the tileset data
def _convert_tileset_ini_section(converter, section_ini):
	_log_debug(converter, f'Tileset: {[option for option in section_ini]}')

	index = '000'
	if index not in section_ini:
		_error(f'Palette: Unable to locate the first index "{index}"')
	
	bits_per_pixel = _bits_per_pixel_get(section_ini)
	tile_max_count = 0x3ff

	data = []
	done = False
	while not done:
		filename = section_ini[index]
		filename = _filename_normalize(filename)
		_filename_validate(filename)
		_log_info(converter, f'Tile: {filename}')

		image = Image.open(filename)

		if image.format == 'GIF':
			image_data = _load_gif(image, bits_per_pixel)
		else:
			_error(f'Unsupported image type: {image.format}')

		_log_debug(converter, f'image_data: {image_data}')
		data.extend(image_data)

		next_index = int(index, 16) + 1
		index = '{0:0{1}x}'.format(next_index, 3)
		_log_debug(converter, f'index: {index}')

		if index not in section_ini:
			_log_info(converter, f'Tileset: Unable to locate index {index}, assuming done')
			break
		if next_index > tile_max_count:
			_log_warning(converter, f'Number of tiles exceeds the maximum of {tile_max_count}')
			break

	_log_debug(converter, f'data: {data}')

	return data


################################################################################
# }}}
################################################################################

################################################################################
# {{{ Image : GIF
################################################################################

################################################################################
# Load GIF color indexes (Not the Palette)
def _load_gif(image, bits_per_pixel):
	valid_depths = (2, 4, 8)
	if bits_per_pixel not in valid_depths:
		_error(f'BitsPerPixel must be one of {valid_depths}')

	data = []
	if bits_per_pixel == 2:
		data = _load_gif_2bpp(image, bits_per_pixel)
	elif bits_per_pixel == 4:
		data = _load_gif_4bpp(image, bits_per_pixel)
	elif bits_per_pixel == 8:
		data = _load_gif_8bpp(image, bits_per_pixel)
	
	return data


################################################################################
# Load the GIF and pack 4 pixel index values into 1 byte
# 1 byte == 8 bits
# 1 pixel index == 2 bits
# 2 bits / 8 bits == 4 pixels per byte
def _load_gif_2bpp(image, bits_per_pixel):
	max_value = 0x01 << (bits_per_pixel - 1)
	byte = 0
	offset = 6
	data = []
	for y in range(image.size[1]):
		for x in range(image.size[0]):
			pixel = image.getpixel((x, y))
			if pixel > max_value:
				_error(f'{filename} has pixels that exceed BitsPerPixel({bits_per_pixel})')
			if offset > 0:
				byte |= (pixel << offset)
				offset -= 2
			else:
				byte |= pixel
				data.append(byte)
				byte = 0
				offset = 6
	return data


################################################################################
# Load the GIF and pack 2 color index values into 1 byte
# 1 byte == 8 bits
# 1 pixel index == 4 bits
# 4 bits / 8 bits == 2 pixels per byte
def _load_gif_4bpp(image, bits_per_pixel):
	max_value = 0x01 << (bits_per_pixel - 1)
	byte = 0
	offset = 4
	data = []
	for y in range(image.size[1]):
		for x in range(image.size[0]):
			pixel = image.getpixel((x, y))
			if pixel > max_value:
				_error(f'{filename} has pixels that exceed BitsPerPixel({bits_per_pixel})')
			if offset > 0:
				byte |= (pixel << offset)
				offset -= 4
			else:
				byte |= pixel
				data.append(byte)
				byte = 0
				offset = 4
	return data


################################################################################
# Load the GIF and pack 1 color index value into 1 byte
def _load_gif_8bpp(image, bits_per_pixel):
	data = []
	for y in range(image.size[1]):
		for x in range(image.size[0]):
			pixel = image.getpixel((x, y))
			data.append(pixel)
	return data


################################################################################
# }}}
################################################################################

################################################################################
# {{{ Logging
################################################################################

################################################################################
# Print error message and exit
#
# If it is important enough to be an error, then it is important enough to
# immediately exit!
def _error(message):
	print(f"Error: {message}")
	sys.exit(1)
	return


################################################################################
# Log a debugging message
#
# These messages are not very useful to an end-user, besides the curiosity
# factor.  However, these messages are useful to a developer to help find a
# bug or measure progress on a feature.
#
# Only printed when at least 3x "-v" args are present.  Or '-vvv'.
def _log_debug(converter, message):
	if converter.verbose >= 3:
		print(message)
	return


################################################################################
# Log an info message
#
# These messages should be useful to an end-user.  The primary purpose of info
# messages are to show progress and what files this script is accessing.  The
# end-user will use this information to figure out why ConverterX16.py is not
# working as expected.
#
# Only printed when at least 2x "-v" args are present.  Or '-vv'.
def _log_info(converter, message):
	if converter.verbose >= 2:
		print(message)
	return


################################################################################
# Log a status message
#
# When ConverterX16.py is invoked by another script or process, usually the
# only output that is desired is what was the final result.
#
# Only printed when at least 1x "-v" args are present.  Or '-v'.
def _log_status(converter, message):
	if converter.verbose >= 1:
		print(message)
	return


################################################################################
# Print warning message
#
# Warning messages are when things do not look right but could still be valid.
# Take one for the team and trust that the user knows what they are doing.
#
# Only printed when at least 1x "-v" args are present.  Or '-v'.
def _log_warning(converter, message):
	if converter.verbose >= 1:
		print(f"Warning: {message}")
	return


################################################################################
# }}}
################################################################################

################################################################################
# {{{ Support Functions
################################################################################

################################################################################
# Get the address
def _address_get(section_ini):
	addr_hi = section_ini.get('address_high', '0')
	addr_hi = int(addr_hi, 16)
	
	addr_lo = section_ini.get('address_low', '0')
	addr_lo = int(addr_lo, 16)

	return ( addr_lo, addr_hi )


################################################################################
# Set the address
def _address_set(section_ini, address):
	addr_hi = '{0:0{1}x}'.format(address[1], 2)
	section_ini['address_high'] = addr_hi

	addr_lo = '{0:0{1}x}'.format(address[0], 2)
	section_ini['address_low'] = addr_hi
	
	return


################################################################################
# Set the Bits-Per-Pixel
def _bits_per_pixel_get(section_ini):
	bpp = int(section_ini.get('bits_per_pixel', '8'))
	return bpp


################################################################################
# Set the Bits-Per-Pixel
def _bits_per_pixel_set(section_ini, bpp):
	section_ini['bits_per_pixel'] = f'{bpp}'
	return


################################################################################
# Get the filename
def _filename_get(ini_section):
	filename = ini_section.get('file', '')
	if len(filename) == 0:
		return None
	
	return filename


################################################################################
# Set the filename
def _filename_set(ini_section, filename):
	if filename is None:
		filename = ''
	
	ini_section['file'] = filename
	
	return


################################################################################
# Get the actual filename
def _filename_normalize(filename):
	filename = os.path.expanduser(filename)
	filename = os.path.expandvars(filename)
	filename = os.path.normcase(filename)
	filename = os.path.normpath(filename)

	return filename


################################################################################
# Check the filename
def _filename_validate(filename):
	if os.path.exists(filename) == False:
		_error(f'File "{filename}" does not exist or is not accessible'
			)
	
	if os.path.isfile(filename) == False:
		_error(f'"{filename}" is not a file'
			)


################################################################################
# Get the Resource name
def _resource_name(section_id):
	resource_name = section_id.split()[0]
	return resource_name


################################################################################
# Get the Resource type
def _resource_type(section_id):
	if len(section_id.split()) < 2:
		return None

	resource_type = section_id.split()[1].lower()
	return resource_type


################################################################################
def _output_filename(converter, section_id):
	file_dir = '.'
	if converter.output_dir is not None:
		file_dir = converter.output_dir

	resource_name = _resource_name(section_id)
	resource_type = _resource_type(section_id)
	file_ext = converter.output_ext[resource_type]
	filename = f'{resource_name}.{file_ext}'

	if converter.case == 'lower':
		filename = filename.lower()
	elif converter.case == 'upper':
		filename = filename.upper()

	filename = f'{file_dir}/{filename}'
	filename = _filename_normalize(filename)

	return filename


################################################################################
def _write_data(converter, address, data, filename):
	_log_info(converter, f'Writing: {filename}')

	if address is not None:
		data.insert(0, address[1])
		data.insert(0, address[0])

	output = open(filename, "wb")
	output.write(bytearray(data))
	output.close()

	_log_status(converter, f'{filename}')

	return


################################################################################
# }}}
################################################################################

################################################################################
# {{{ Main
################################################################################
converter = configure()

_log_debug(converter, f"{converter}")

# Help
if converter.help_resource is True:
	help_resource(converter)
	sys.exit(0)

if converter.resource is None:
	_error('The following arguments are required: RESOURCE_FILE')

_filename_validate(converter.resource)

# Command-Line Utilities
if converter.list_input_files is True:
	list_input_files(converter)
	sys.exit(0)

if converter.list_output_files is True:
	list_output_files(converter)
	sys.exit(0)

if converter.generate_makefile is True:
	generate_makefile(converter)
	sys.exit(0)

# Do Conversion
if converter.convert_bitmap is not None:
	resource_list = get_section(converter, converter.convert_bitmap, 'bitmap')
elif converter.convert_palette is not None:
	resource_list = get_section(converter, converter.convert_palette, 'palette')
elif converter.convert_tilemap is not None:
	resource_list = get_section(converter, converter.convert_tilemap, 'tilemap')
elif converter.convert_tileset is not None:
	resource_list = get_section(converter, converter.convert_tileset, 'tileset')
elif converter.convert_file is not None:
	resource_list = get_sections_for_file(converter, converter.convert_file)
else:
	resource_list = get_section_all(converter)

convert(converter, resource_list)

################################################################################
# }}}
################################################################################
