TEMPLATE = subdirs

SUBDIRS +=  upnp \
            aivctrl

CONFIG += ordered
TRANSLATIONS = $$PWD/aivctrl/languages/aivctrl_fr.ts
