/*
 * high level programmer functions
 */

static uint32_t addrreg;

static void
prog_set_addr(uint32_t addr)
{
    addrreg = addr;
}

static uint32_t
prog_read_dword(void)
{
    uint32_t insn;
    uint32_t data;

    /*
     * lui s3, DMSEG
     */
    insn = 0x3C130000;
    insn |= 0xFF20; /* DMSEG */
    ejtag_exec_insn(insn);

    /*
     * lui t0, hi(addr)
     * ori t0, lo(addr)
     */
    insn = 0x3C080000;
    insn |= (addrreg >> 16);
    ejtag_exec_insn(insn);
    insn = 0x35080000;
    insn |= (addrreg & 0xFFFF);
    ejtag_exec_insn(insn);
    addrreg += sizeof(uint32_t);

    /*
     * lw t1,(t0)
     * sw t1,(s3)
     * nop
     */
    insn = 0x8D090000;
    ejtag_exec_insn(insn);
    insn = 0xAE690000;
    ejtag_exec_insn(insn);
    insn = 0x00000000;
    ejtag_exec_insn(insn);

    jtag_command(ETAP_FASTDATA);
    data = ejtag_fastdata(0);

    return data;
}

static void
prog_write_dword(uint32_t data)
{
    uint32_t insn;

    /*
     * lui s0,hi(addr)
     */
    insn = 0x3C100000;
    insn |= addrreg >> 16;
    ejtag_exec_insn(insn);

    /*
     * lui t0,hi(data)
     * ori t0,lo(data)
     * sw  t0,(s0 + offset)
     */
    insn = 0x3C080000;
    insn |= (data >> 16);
    ejtag_exec_insn(insn);
    insn = 0x35080000;
    insn |= (data & 0xFFFF);
    ejtag_exec_insn(insn);
    insn = 0xAE080000;
    insn |= (addrreg & 0xFFFF);
    ejtag_exec_insn(insn);
    addrreg += sizeof(uint32_t);
}

static void
prog_rewind(void)
{
    uint32_t insn;

    /*
     * lui t0, DMSEG
     * ori t0, 0x0200
     */
    insn = 0x3C080000;
    insn |= 0xFF20; /* DMSEG */
    ejtag_exec_insn(insn);
    insn = 0x35080000;
    insn |= 0x0200;
    ejtag_exec_insn(insn);

    /*
     * jr t0
     * nop
     */
    insn = 0x01000008;
    ejtag_exec_insn(insn);
    insn = 0x00000000;
    ejtag_exec_insn(insn);

    return;
}
