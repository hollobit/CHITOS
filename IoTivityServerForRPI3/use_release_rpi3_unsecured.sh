sudo rm /etc/ld.so.conf.d/iotivity*.conf
sudo cp ./iotivity_release_rpi3_unsecured.conf /etc/ld.so.conf.d
sudo ldconfig
cp ./SConstruct_release_rpi3_unsecured ./SConstruct
cp ./common_rpi3_unsecured.h ./common.h
./build_release_rpi3_unsecured.sh
echo '***********************************************************'
echo '  RPi3 UNSECURED RELEASE MODE BUILD END'
echo '***********************************************************'