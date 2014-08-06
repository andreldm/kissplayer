SUBDIRS = 48x48 64x64 scalable

UPDATE_ICON_CACHE = gtk-update-icon-cache -f -t $(datadir)/icons/hicolor || :

install-data-hook:
	$(UPDATE_ICON_CACHE)
uninstall-hook:
	$(UPDATE_ICON_CACHE)