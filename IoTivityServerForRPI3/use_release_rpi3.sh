sudo rm /etc/ld.so.conf.d/iotivity*.conf
sudo cp ./iotivity_release_rpi3.conf /etc/ld.so.conf.d
sudo ldconfig
cp ./SConstruct_release_rpi3 ./SConstruct
cp ./common_rpi3_secured.h ./common.h
./build_release_rpi3.sh
echo '***********************************************************'
echo '  RPi3 SECURED RELEASE MODE BUILD END'
echo '***********************************************************'