if ! type wget > /dev/null ; then
    echo "Install wget, then run this script again."
    exit 1
fi

wget http://www.fmod.org/files/fmodapi373linux.tar.gz
mkdir /tmp/fmod
mv fmodapi373linux.tar.gz /tmp/fmod
cd /tmp/fmod/
tar xvfz /tmp/fmod/fmodapi373linux.tar.gz 
cp fmodapi373linux/api/inc/*.h /usr/local/include/
cp fmodapi373linux/api/libfmod-3.73.so /usr/local/lib/
rm /usr/local/lib/libfmod.so
ln -s /usr/local/lib/libfmod-3.73.so /usr/local/lib/libfmod.so
rm -rf /tmp/fmod
