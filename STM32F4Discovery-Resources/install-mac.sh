#!/usr/bin/env bash

 if [[ $UID != 0 ]]; then
     echo "Please run this script with sudo:"
     echo "sudo $0 $*"
     exit 1
 fi

# brew install git autoconf pkg-config cmake
brew install git autoconf libusb pkg-config cmake 

# Install arm compiler
if [ ! -d "/opt/gcc4mbed" ]; then
    echo "*** Installing arm compiler"
    sudo git clone https://github.com/UCSolarCarTeam/gcc4mbed.git /opt/gcc4mbed --depth 1
    (cd /opt/gcc4mbed && \
        sudo chmod +x mac_install && \
        sudo sed -i '' -e '108d;109d;110d;147d' mac_install && \
        sudo ./mac_install)
    if ! grep "export PATH=\$PATH:/opt/gcc4mbed/gcc-arm-none-eabi/bin/" ~/.profile; then
        echo "export PATH=\$PATH:/opt/gcc4mbed/gcc-arm-none-eabi/bin/" >> ~/.profile
    fi
    export PATH=$PATH:/opt/gcc4mbed/gcc-arm-none-eabi/bin/
	ln -s /opt/gcc4mbed/gcc-arm-none-eabi/bin/arm-none-eabi-gdb /usr/local/bin/arm-none-eabi-gdb
else
    echo "*** ARM compiler already installed"
fi

# Sourcing Profile
source ~/.profile
