######################################
#
# midirouter
#
######################################
MIDIROUTER_VERSION = 3419e183eb5be1cd0b7e54ee90238a97e0d8ce52
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
