TEMPLATE = subdirs

SUBDIRS +=  upnp \
            googledrive \
            aivctrl

CONFIG += ordered
TRANSLATIONS = $$PWD/aivctrl/languages/aivctrl_fr.ts

