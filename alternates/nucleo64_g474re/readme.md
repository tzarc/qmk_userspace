# ST Microelectronics Nucleo64-G474RE onekey

Supported Hardware: <https://www.st.com/en/evaluation-tools/nucleo-g474re.html>

To trigger keypress, short together pins *C2* and *C3*.

**Not working**: To enable DFU via BOOT0, connect with ST-Link and modify the option bytes:
    * turn on nSWBOOT0
    * turn off DBANK
    * turn off BFB2