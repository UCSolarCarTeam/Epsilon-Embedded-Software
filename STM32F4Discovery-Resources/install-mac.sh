#!/usr/bin/env bash

# install homebrew
if ! which brew > /dev/null 2>&1;
then
  echo "*** Installing homebrew"
  /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install.sh)"
else
  echo "*** Homebrew already installed"
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
	ln -s /opt/gcc4mbed/gcc-arm-none-eabi/bin/* /usr/local/bin/*
else
    echo "*** ARM compiler already installed"
fi

# Sourcing Profile
source ~/.profile
