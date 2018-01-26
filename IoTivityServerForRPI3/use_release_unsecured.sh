sudo rm /etc/ld.so.conf.d/iotivity*.conf
sudo cp ./iotivity_release_unsecured.conf /etc/ld.so.conf.d
sudo ldconfig
cp ./SConstruct_release ./SConstruct
cp ./common_unsecured.h ./common.h
./build_release_unsecured.sh
echo '***********************************************************'
echo '  UNSECURED RELEASE MODE BUILD END'
echo '***********************************************************'