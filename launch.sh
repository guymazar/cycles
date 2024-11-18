export CYCLES_PORT=50019

cat<<EOF> config.yaml
gameHeight: 600
gameWidth: 600
gameBannerHeight: 100
gridHeight: 100
gridWidth: 100
maxClients: 60
enablePostProcessing: false
EOF

./build/bin/server &
sleep 1

for i in {1..10}
do
./build/bin/client randomio$i &
done
./build/bin/client_guy guymazar &
