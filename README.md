# gr-adsbTrasnmit 

This GNURadio block was developed  in order to test airplane ADS-B packet transmission in SDR devices, for simulation purposes. It continuously transmits two dummy airplanes packets locate in Italy and Philippines and shows, in terminal, packet build steps. 

## INSTALL 


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

## OPEN EXAMPLE

```bash
cd gr-adsbTransmit/examples
gnuradio-companion adsbExample37.grc
```

## REMOVE

```bash
cd gr-adsbTransmit/build
make uninstall
cd ~
rm -r gr-adsbTransmit
```