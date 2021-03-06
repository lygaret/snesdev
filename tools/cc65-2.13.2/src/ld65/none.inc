static const char CfgNone [] = 
"MEMORY {\n"
"ZP:  start = $0000, size = $0001F, type = rw, define = yes;\n"
"RAM: start = %S,    size = $10000, file = %O;\n"
"}\n"
"SEGMENTS {\n"
"LOWCODE:  load = RAM, type = ro,               optional = yes;\n"
"INIT:     load = RAM, type = ro, define = yes, optional = yes;\n"
"CODE:     load = RAM, type = rw;\n"
"RODATA:   load = RAM, type = rw;\n"
"DATA:     load = RAM, type = rw;\n"
"BSS:      load = RAM, type = bss, define = yes;\n"
"HEAP:     load = RAM, type = bss, optional = yes;\n" /* must sit just below stack */
"ZEROPAGE: load = ZP,  type = zp;\n"
"}\n"
"FEATURES {\n"
"CONDES: segment = INIT,\n"
"type = constructor,\n"
"label = __CONSTRUCTOR_TABLE__,\n"
"count = __CONSTRUCTOR_COUNT__;\n"
"CONDES: segment = RODATA,\n"
"type = destructor,\n"
"label = __DESTRUCTOR_TABLE__,\n"
"count = __DESTRUCTOR_COUNT__;\n"
"CONDES: type = interruptor,\n"
"segment = RODATA,\n"
"label = __INTERRUPTOR_TABLE__,\n"
"count = __INTERRUPTOR_COUNT__;\n"
"}\n"
"SYMBOLS {\n"
"__STACKSIZE__ = $800;\n"	/* 2K stack */
"}\n"
;
