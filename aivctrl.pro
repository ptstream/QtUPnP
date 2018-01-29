TEMPLATE = subdirs

SUBDIRS +=  upnp \
            aivctrl \
            googledrive

CONFIG += ordered
TRANSLATIONS = $$PWD/aivctrl/languages/aivctrl_fr.ts

