# BPI-A311D-Android9

----------

**Prepare**

Get the docker image from [Sinovoip Docker Hub](https://hub.docker.com/r/sinovoip/bpi-build-android-7/) , Build the android source with this docker environment.

Other environment requirements, please refer to google Android's official build environment [requirements](https://source.android.com/setup/build/requirements) and [set up guide](https://source.android.com/setup/build/initializing) 

Download the [toolchains](https://download.banana-pi.dev/d/3ebbfa04265d4dddb81b/?p=/Tools/toolchains/bpi-m5&mode=list) and extract each tarball to /opt/ and set the PATH before build

    $ export PATH=$PATH:/opt/gcc-linaro-aarch64-none-elf-4.9-2014.09_linux/bin
    $ export PATH=$PATH:/opt/gcc-linaro-arm-none-eabi-4.8-2014.04_linux/bin
    $ export PATH=$PATH:/opt/gcc-linaro-6.3.1-2017.02-x86_64_aarch64-linux-gnu/bin
    $ export PATH=$PATH:/opt/gcc-linaro-6.3.1-2017.02-x86_64_arm-linux-gnueabihf/bin

----------

Get source code

    $ git clone https://github.com/Dangku/BPI-A311D-Android9

Because github limit 100MB size for single file, please download the [oversize files](https://download.banana-pi.dev/d/ca025d76afd448aabc63/files/?p=%2FSource_Code%2Fm2s%2Fgithub_oversize_files.zip) and merge them to correct directory before build.

Another way is get the source code tar archive from [BaiduPan(pincode: 8888)](https://pan.baidu.com/s/1rANGEB-1MLPCBXqOR5aYCg?pwd=8888) or [GoogleDrive](https://drive.google.com/drive/folders/1INIABp_MbB5UcwfqujTngGLOZN7YGuWp?usp=share_link)

----------

**Build**

    $ cd BPI-A311D-Android9
    $ source env.sh
    $ ./device/bananapi/common/quick_compile.sh
    
    [NUM]   [       PROJECT]     [  SOC TYPE]  [   HARDWARE TYPE]
    ---------------------------------------------------------------
    [ 1]    [           m2s]     [     A311D]  [    BANANAPI_M2S]
    ...
    ---------------------------------------------------------------
    please select platform type (default 1(m2s)):1
    ...

----------
**Flash**

The target usb download image is out/target/product/bananapi_*/aml_upgrade_package.img, flash it to your device by AML Usb_Burning_Tool or Burn_Card_Maker. Please refer to [Bananapi M2S wiki](http://wiki.banana-pi.org/Getting_Started_with_BPI-M2S) for how to flash the target image.
