#!/usr/bin/python3

import argparse
import os
import yaml

class FS:
	def __init__(self, num, yml, cfg):
		self._yaml = yml
		self.num = num
		self.cfg = cfg
		self.partfile = "{}.{}".format(self.cfg.imgfile, self.num)
		self.label = self._yaml['label']
		self.fs = self._yaml['fs']

	@property
	def bootable(self):
		if('bootable' in self._yaml and self._yaml['bootable'] == True):
			return True
		else:
			return False

	@property
	def content(self):
		if('content' in self._yaml and self._yaml['content']):
			if(os.path.isdir(self._yaml['content'])):
				return self._yaml['content']
			else:
				raise RuntimeError('specified content directory {} doesn\'t exist'.format(self._yaml['content']))
		else:
			return None

	def step_bootable(self):
		if(self.bootable):
			print("parted {} -s -a minimal toggle {} boot".format(self.cfg.imgfile, self.num + 1))

class FAT32(FS):
	def step_image(self):
		print("parted {} -s -a minimal mkpart {} fat32 {}s {}s".format(self.cfg.imgfile, self.label, self.start, self.start + self.size - 1))

	def step_create(self):
		print('dd if=/dev/zero of={} bs=512 count={} status=none'.format(self.partfile, self.size))
		print('mkfs.fat -F32 -s 1 {}'.format(self.partfile))

		if(self.content != None):
			for path, subdirs, files in os.walk(self.content):
				for name in subdirs:
					print('mmd -i {} ::/{}'.format(self.partfile, os.path.join(path, name)[len(self.content):]))

				for name in files:
					print('mcopy -i {} {} ::/{}'.format(self.partfile, os.path.join(path, name), os.path.join(path, name)[len(self.content):]))

	def step_modify(self):
		print("dd if={} of={} bs=512 seek={} count={} conv=notrunc status=none".format(self.partfile, self.cfg.imgfile, self.start, self.size))

class Btrfs(FS):
	def step_image(self):
		print("parted {} -s -a minimal mkpart {} btrfs {}s {}s".format(self.cfg.imgfile, self.label, self.start, self.start + self.size - 1))

	def step_create(self):
		print('dd if=/dev/zero of={} bs=512 count={} status=none'.format(self.partfile, self.size))
		content_string = ''
		if(self.content):
			content_string = '-r {} '.format(self.content)
		print('mkfs.btrfs -q -L {} {}{}'.format(self.label, content_string, self.partfile))

	def step_modify(self):
		print("dd if={} of={} bs=512 seek={} count={} conv=notrunc status=none".format(self.partfile, self.cfg.imgfile, self.start, self.size))

class Image:
	def __init__(self, yml, cfg):
		self._yaml = yml
		self.cfg = cfg

	def build(self):
		print('#!/bin/bash')
		print('dd if=/dev/zero of={} bs=1{} count={} status=none'.format(self.cfg.imgfile, self._yaml['size'][-1], self._yaml['size'][:-1]))
		if(self._yaml['type'] == 'gpt'):
			print('parted {} -s -a minimal mktable gpt'.format(self.cfg.imgfile))
		else:
			raise RuntimeError('unknown partition type {}'.format(self._yaml['type']))

class Config:
	def __init__(self, config, args):
		self._yaml = config
		self.args = args
		self.disk_sectors = size_to_sectors(config['size'])
		self.image = Image(config, self)
		self.partitions = dict()
		self.partition_start = 2048

		for num, part in enumerate(self._yaml['partitions']):
			if(part['fs'] == 'fat32'):
				self.partitions[num] = FAT32(num, part, self)
			elif(part['fs'] == 'btrfs'):
				self.partitions[num] = Btrfs(num, part, self)
			else:
				raise RuntimeError('unexpected fs {} in partition {}'.format(part['fs'], num))

			self.partitions[num].start = self.partition_start
			if(part['size'] == 'fit'):
				assert((self.partition_start % 2048) == 0)
				self.partitions[num].size = self.disk_sectors - self.partition_start - 2047
			else:
				self.partitions[num].size = size_to_sectors(part['size'])
			self.partition_start += self.partitions[num].size

	@property
	def imgfile(self):
		if(self.args.output):
			return self.args.output
		elif('file' in self._yaml):
			return self._yaml['file']
		else:
			raise RuntimeError('no output file specified')

	def build(self):
		if(args.modify == None):
			self.image.build()
			for num, part in self.partitions.items():
				print("# create partition {}".format(num))
				part.step_image()
				part.step_bootable()

			for num, part in self.partitions.items():
				print("# build partition {} ({})".format(num, part.fs))
				part.step_create()
				part.step_modify()
		else:
			self.partitions[args.modify].step_modify()

def size_to_sectors(size):
	if(size[-1] == 'M'):
		return int(size[:-1]) << 11;
	elif(size[-1] == 'G'):
		return int(size[:-1]) << 21;
	else:
		raise RuntimeError('invalid size given: {}'.format(size))

parser = argparse.ArgumentParser()
parser.add_argument('file', help='input YAML file')
parser.add_argument('-m', '--modify', type=int)
parser.add_argument('-o', '--output', help='override output file')
args = parser.parse_args()

config = Config(yaml.load(open(args.file, 'r'), Loader=yaml.SafeLoader), args)
config.build()