./json2cbor_x86 ./server.json ./server.dat
sudo killall -9 server
sleep 0.3
sudo ./server T &
sleep 0.3
sudo ./server P &
sleep 0.3
sudo ./server G &
sleep 0.3
sudo ./server S &
sleep 0.3
ps -ef | grep server
