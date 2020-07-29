rm -f ./bin/out.png
rm -f ./bin/out.tiff

nvidia-smi stats -i 0 -d pwrDraw >& power.csv &
make test

../parse_power.py
rm -f power.csv
