def make_targets(self, object_list, source_list=[], debug=0):

	import glob
	import string
	import re

	if source_list == []:
		dir = self.Dir('.').abspath
		if self.build_dir:

			## strip build_dir from cwd for glob
			edir = '^' + re.escape(self.Dir('#').abspath) + r'[/\\]' + re.escape(self.build_dir)

			dir = re.sub(edir, '', dir)
			dir = self.Dir('#').abspath + dir
			
			stmp = glob.glob(dir + "/*.cpp")

			## add build_dir to cwd for Object
			sources = []
			for src in stmp:
				obj = re.search(r'[/\\]([^/\\]*)$', src)
				file = self.Dir('.').abspath + obj.group(0)
				sources.append(file)
		else:
			sources = glob.glob(dir + "/*.cpp")
		
	else:
		sources = []
		for src in source_list:
			sources.append(self.Dir('.').abspath + '/' + src)

	for file in sources:

		sobject = file[:-4] + self['OBJSUFFIX']
		if sobject in object_list:
			continue

		if (self.make_so):
			self.SharedObject(file[:-4], file)
		else:
			self.Object(file[:-4], file)

		object_list.append(sobject)


