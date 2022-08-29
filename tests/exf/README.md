
## msp430 exfiltration

sample function that outputs the entire memory map of a MSP430FR5994 via bitbanged SPI-compatible signals.

```
 source:    https://github.com/rodan/atlas430
 author:    Petre Rodan <2b4eda@subdimension.ro>
 license:   BSD
```

## setup

```
make
make install
```

the signals are being sent out on P3.0 (CLK), P3.1 (MOSI) and P3.2 (CS) and can captured with a logic analyzer with the aid of pulseview or sigrok-cli

```
sigrok-cli --driver saleae-logic-pro --output-format binary:numchannels=16:samplerate=25000000 binary --output-file=/tmp/exf.cap --continuous
```
and then converted into a memory dump:

```
sigrok-cli --input-file /tmp/exf.sr -P spi:clk='0':mosi='1':cs='2' -A spi=mosi-transfer | grep '.\{20\}' | sed 's|spi-1: ||' | xxd -r -p > /tmp/exf.dump
```

