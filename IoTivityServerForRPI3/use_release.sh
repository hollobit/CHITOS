sudo rm /etc/ld.so.conf.d/iotivity*.conf
sudo cp ./iotivity_release.conf /etc/ld.so.conf.d
sudo ldconfig
cp ./SConstruct_release ./SConstruct
cp ./common_secured.h ./common.h
./build_release.sh
echo '***********************************************************'
echo '  SECURED RELEASE MODE BUILD END'
echo '***********************************************************'