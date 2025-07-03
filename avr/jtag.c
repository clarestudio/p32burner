/*
 * JTAG state control over ICSP functions
 */

/*
 * Move TAP state to idle
 */
static void
jtag_idle(void)
{
    uint8_t cmd = 0b011111;
    uint8_t dat = 0;
    uint8_t bc = 6;

    icsp_io(cmd, dat, bc);
}

/*
 * Send to 5-bit MTAP/ETAP command register
 */
static void
jtag_command(uint8_t bits)
{
    uint8_t cmd;
    uint8_t dat;
    uint8_t bc;

    cmd = 0b0011;
    dat = 0;
    bc = 4;
    icsp_io(cmd, dat, bc);

    cmd = 0b10000;
    dat = bits;
    bc = 5;
    icsp_io(cmd, dat, bc);

    cmd = 0b01;
    dat = 0;
    bc = 2;
    icsp_io(cmd, dat, bc);
}

/*
 * transfer 8bit data register
 */
static uint8_t
jtag_data_byte(uint8_t data)
{
    uint8_t rv;

    icsp_io(0b001, 0, 3); /* start */
    rv = icsp_io(0b10000000, data, 8); /* xfer */
    icsp_io(0b01, 0, 2); /* finish */
    return rv;
}

/*
 * transfer 32bit data register
 */
static uint32_t
jtag_data_dword(uint32_t data)
{
    uint8_t d0, d1, d2, d3;
    uint8_t r0, r1, r2, r3;
    uint32_t rv;

    d0 = data >>  0;
    d1 = data >>  8;
    d2 = data >> 16;
    d3 = data >> 24;
    icsp_io(0b001, 0, 3); /* start */
    r0 = icsp_io(0b00000000, d0, 8);
    r1 = icsp_io(0b00000000, d1, 8);
    r2 = icsp_io(0b00000000, d2, 8);
    r3 = icsp_io(0b10000000, d3, 8);
    icsp_io(0b01, 0, 2); /* finish */
    rv = ((uint32_t)r0 <<  0)
       | ((uint32_t)r1 <<  8)
       | ((uint32_t)r2 << 16)
       | ((uint32_t)r3 << 24)
       ;
    return rv;
}

/*
 * EJTAG FASTDATA transfer (with 32bit data chain)
 */
static uint32_t
ejtag_fastdata(uint32_t data)
{
    uint8_t d0, d1, d2, d3;
    uint8_t r0, r1, r2, r3;
    uint32_t rv;
    uint8_t flag;
    uint8_t limit;

    d0 = data >>  0;
    d1 = data >>  8;
    d2 = data >> 16;
    d3 = data >> 24;
    limit = 255;
    do {
        icsp_io(0b001, 0, 3); /* start */
        flag = icsp_io(0, 0, 1);
        r0 = icsp_io(0b00000000, d0, 8);
        r1 = icsp_io(0b00000000, d1, 8);
        r2 = icsp_io(0b00000000, d2, 8);
        r3 = icsp_io(0b10000000, d3, 8);
        icsp_io(0b01, 0, 2); /* finish */
	if (--limit == 0)
	    break;
    } while ((flag & 1) == 0);
    rv = ((uint32_t)r0 <<  0)
       | ((uint32_t)r1 <<  8)
       | ((uint32_t)r2 << 16)
       | ((uint32_t)r3 << 24)
       ;
    return rv;
}

/*
 * EJTAG execute an instruction
 */
static void
ejtag_exec_insn(uint32_t insn)
{
    uint32_t cv;
    uint8_t limit;

    jtag_command(ETAP_CONTROL);
    limit = 255;
    do {
        cv = jtag_data_dword(0x0004C000L);
    } while ((cv & (1L<<18)) == 0 || --limit == 0); /* wait for PrAcc == 1 */
    jtag_command(ETAP_DATA);
    jtag_data_dword(insn);
    jtag_command(ETAP_CONTROL);
    jtag_data_dword(0x0000C000L); /* exec CPU */
}
