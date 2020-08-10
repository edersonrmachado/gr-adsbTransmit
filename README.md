# INSTALL  gr-adsbTrasnmit

```bash
git init
git clone https://github.com/edersonrmachado/gr-adsbTransmit  
cd gr-adsbTransmit/  
mkdir build
cd build/
cmake ../  # or cmake -DCMAKE_INSTALL_PREFIX=<path_to_install> ../
make
sudo make install
sudo ldconfig
```

# REMOVE

```bash
cd gr-adsbTransmit/build
make uninstall
cd ~
rm -r gr-adsbTransmit
```