# htttc
HT &lt;-> TTC conversion tool written in Qt

This tool converts money amounts between HT (hors taxes in French, without taxes) and TTC (toutes taxes comprises, incl. taxes).

![image](https://github.com/DexterMagnific/htttc/assets/2777588/05560fa0-5c79-485e-83e6-34f78bb5c238)

# Usage
This a keyboard oriented tool.

* TAB: switch between HT and TTC input fields
* PgUp/PgDown: Change tax rate
* ESC: clear

# Build
```
mkdir build
cd build
cmake ..
make
```

It is recommended to install the OCR B font. In ubuntu:
```
sudo apt install fonts-ocr-b
```
