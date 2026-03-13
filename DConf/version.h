#ifndef __VERSION_H__
#define __VERSION_H__

#define VER_MAJOR		7
#define VER_MINOR		6
#define VER_REVISION	0
#define VER_BUILD		0

#define STRINGIZER(version) #version
#define VERSION_STRING(major,minor,revision) STRINGIZER(major) "." STRINGIZER(minor) "." STRINGIZER(revision)
#define FILE_VERSION_STRING(major,minor,revision, build) STRINGIZER(major) "." STRINGIZER(minor) "." STRINGIZER(revision) "." STRINGIZER(build)

#define DCONF_VERSION_STRING VERSION_STRING(VER_MAJOR, VER_MINOR, VER_REVISION)
#define DCONF_FILE_VERSION_STRING FILE_VERSION_STRING(VER_MAJOR, VER_MINOR, VER_REVISION, VER_BUILD)


#endif	//	__VERSION_H__
