######################################
#
# midirouter
#
######################################
MIDIROUTER_VERSION = 49b67201a39e864ed35eb7cbf6619ee6d8788fcf
MIDIROUTER_SITE = https://github.com/stivb/OilCanSteve.git
MIDIROUTER_SITE_METHOD = git
MIDIROUTER_BUNDLES = midirouter.lv2

MIDIROUTER_TARGET_MAKE = $(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_OPTS) $(MAKE) MOD=1 OPTIMIZATIONS="" PREFIX=/usr -C $(@D)/midirouter/source

define MIDIROUTER_BUILD_CMDS
	$(MIDIROUTER_TARGET_MAKE)
endef

define MIDIROUTER_INSTALL_TARGET_CMDS
	$(MIDIROUTER_TARGET_MAKE) install DESTDIR=$(TARGET_DIR)
endef

$(eval $(generic-package))
