sudo cp -p -r $MTCP_SDK/util/. $MTCP_REDIS/util/
sudo cp -p -r $MTCP_SDK/io_engine/. $MTCP_REDIS/io_engine/
cd $MTCP_REDIS/mtcp/src
make
cd ../../src
make

