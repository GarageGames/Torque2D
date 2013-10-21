
ANDROID_NDK_ROOT?=~/android-ndk
TARGET_ARCH_ABI?=armeabi-v7a
LOCAL_ARM_NEON?=true

# Set to yes to include support for output to OpenSL
POST_FROYO?=yes

default: lib

lib:
	(cd jni; \
		TARGET_ARCH_ABI=$(TARGET_ARCH_ABI) \
		LOCAL_ARM_NEON=$(LOCAL_ARM_NEON) \
		ANDROID_NDK_ROOT=$(ANDROID_NDK_ROOT) \
		POST_FROYO=$(POST_FROYO) \
		$(ANDROID_NDK_ROOT)/ndk-build;)
