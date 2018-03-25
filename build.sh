#! /bin/bash

# �������� ����� �����
BUILD=`git rev-list --count HEAD`


# ������� ������ �����
if [ -d builds/$BUILD ]; then
    rm -rf builds/$BUILD
fi


# �������� ���� ����
cd soft

cd boot-2apps
make || exit
cd ..

cd EmuAPP
make || exit
cd ..

cd WiFiAPP
make || exit
cd ..

cd mkfw
rm -f 0x00000.bin fota.bin
./mkfw || exit
cd ..

cd ..


# ������� ������� ��� �����
mkdir -p builds/$BUILD || exit

# �������� �����
cp soft/mkfw/0x00000.bin builds/$BUILD || exit
cp soft/mkfw/fota.bin builds/$BUILD || exit

echo "Build $BUILD done"
