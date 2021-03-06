#
# Copyright (C) 2017 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_PACKAGE_NAME := CtsIsolatedSplitApp
LOCAL_USE_AAPT2 := true
LOCAL_MODULE_TAGS := tests
LOCAL_COMPATIBILITY_SUITE := cts vts general-tests
LOCAL_SDK_VERSION := current

# Feature splits are dependent on this base, so it must be exported.
LOCAL_EXPORT_PACKAGE_RESOURCES := true

# Make sure our test locale polish is not stripped.
LOCAL_AAPT_INCLUDE_ALL_RESOURCES := true

LOCAL_STATIC_JAVA_LIBRARIES := ctstestrunner android-support-test
LOCAL_SRC_FILES := $(call all-java-files-under, src)

# Generate a locale split.
LOCAL_PACKAGE_SPLITS := pl

include $(BUILD_CTS_SUPPORT_PACKAGE)

# Build the other splits.
include $(call first-makefiles-under,$(LOCAL_PATH))
