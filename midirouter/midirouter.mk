######################################
#
# midirouter
#
######################################
MIDIROUTER_VERSION = 5b8fb01bf4ea5c6d94896b022b2f32fbdc8d2b63
MIDIROUTER_SITE = https://github.com/stivb/OilCanSteve.git
MIDIROUTER_SITE_METHOD = git
MIDIROUTER_BUNDLES = midirouter.lv2

MIDIROUTER_TARGET_MAKE = $(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_OPTS) $(MAKE) MOD=1 OPTIMIZATIONS="" PREFIX=/usr -C $(@D)/midirouter/source

define MIDIROUTER_BUILD_CMDS
	$(MIDIROUTER_TARGET_MAKE)
endef

define MIDIROUTER_INSTALL_TARGET_CMDS
	$(MIDIROUTER_TARGET_MAKE) install DESTDIR=$(TARGET_DIR)
	cp -rv $(@D)/midirouter.lv2 $(TARGET_DIR)/usr/lib/lv2/
endef

$(eval $(generic-package))
