#

AUTOMAKE_OPTIONS = foreign 
SUBDIRS = src src_sound data doc
SpecFile = #nighthawk.spec
EXTRA_DIST = options.h maintainer-conf $(SpecFile)
MAINTAINERCLEANFILES = .nighthawk.scores

maintainer-clean-local:
	rm -rf bin share libexec

installcheck-local:
	(cd $(DESTDIR)$(bindir) && (./nighthawk -m || true))
