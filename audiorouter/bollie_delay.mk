
######################################
#
# bolliedelay
#
######################################

# Set the Git commit hash for the version to download
BOLLIEDELAY_VERSION = c42ae339855b1725d9e0c2c81900151b751af502

# Define the GitHub repository location (user: MrBollie, repo: bolliedelay.lv2)
BOLLIEDELAY_SITE = $(call github,MrBollie,bolliedelay.lv2,$(BOLLIEDELAY_VERSION))

# Specify the LV2 bundle name that will be installed
BOLLIEDELAY_BUNDLES = bolliedelay.lv2

# Configure the make command with target environment, compiler options, and build flags
# MOD=1: Enable MOD-specific build
# OPTIMIZATIONS="": Disable compiler optimizations
# PREFIX=/usr: Set installation prefix
# -C $(@D): Change to the package build directory
BOLLIEDELAY_TARGET_MAKE = $(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_OPTS) $(MAKE) MOD=1 OPTIMIZATIONS="" PREFIX=/usr -C $(@D)

# Define the build command - executes make in the source directory
define BOLLIEDELAY_BUILD_CMDS
	$(BOLLIEDELAY_TARGET_MAKE)
endef

# Define the installation commands
define BOLLIEDELAY_INSTALL_TARGET_CMDS
	# Run 'make install' with DESTDIR pointing to target filesystem
	$(BOLLIEDELAY_TARGET_MAKE) install DESTDIR=$(TARGET_DIR)
	# Copy additional files from package directory to the installed LV2 bundle
	cp -rL $($(PKG)_PKGDIR)/bolliedelay.lv2/* $(TARGET_DIR)/usr/lib/lv2/bolliedelay.lv2/
endef

# Evaluate this package using Buildroot's generic-package infrastructure
$(eval $(generic-package))