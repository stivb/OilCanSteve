######################################
#
# midirouter
#
######################################
MIDIROUTER_VERSION = 0bde3774bca687317008ac7d64ba9682dbf8293c
MIDIROUTER_SITE = https://github.com/stivb/OilCanSteve.git
MIDIROUTER_SITE_METHOD = git
MIDIROUTER_BUNDLES = midirouter.lv2

MIDIROUTER_TARGET_MAKE = $(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_OPTS) $(MAKE) MOD=1 OPTIMIZATIONS="" PREFIX=/usr -C $(@D)/midirouter/source

define MIDIROUTER_BUILD_CMDS
	$(MIDIROUTER_TARGET_MAKE)
endef

define MIDIROUTER_INSTALL_TARGET_CMDS
	$(MIDIROUTER_TARGET_MAKE) install DESTDIR=$(TARGET_DIR)
	cp -rv $(@D)/midirouter/midirouter.lv2 $(TARGET_DIR)/usr/lib/lv2/
endef

$(eval $(generic-package))
