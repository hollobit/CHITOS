sudo cp ./iotivity_debug.conf /etc/ld.so.conf.d
sudo rm /etc/ld.so.conf.d/iotivity_release.conf
sudo ldconfig
cp ./SConstruct_debug ./SConstruct
./build_debug.sh
echo '***********************************************************'
echo '  DEBUG MODE BUILD END'
echo '***********************************************************'
