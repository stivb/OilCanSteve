######################################
#
# midirouter
#
######################################
MIDIROUTER_VERSION = 9b1d3f7a6c5e2a8f0b4d6c9e3f1a2b7c8d0e5f4a
MIDIROUTER_SITE = $(call github,stivb,OilCanSteve,$(MIDIROUTER_VERSION))
MIDIROUTER_BUNDLES = midirouter.lv2

MIDIROUTER_TARGET_MAKE = $(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_OPTS) $(MAKE) MOD=1 OPTIMIZATIONS="" PREFIX=/usr -C $(@D)

define MIDIROUTER_BUILD_CMDS
	$(MIDIROUTER_TARGET_MAKE)
endef

define MIDIROUTER_INSTALL_TARGET_CMDS
	$(MIDIROUTER_TARGET_MAKE) install DESTDIR=$(TARGET_DIR)
	cp -rL $($(PKG)_PKGDIR)/midirouter/* $(TARGET_DIR)/usr/lib/lv2/midirouter.lv2/
endef

$(eval $(generic-package))
