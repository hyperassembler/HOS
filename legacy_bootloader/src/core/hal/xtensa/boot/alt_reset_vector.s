/*
 *  Alternate reset vector
 */
    .begin    literal_prefix    .AltResetVector
    .section        .AltResetVector.text, "ax"

/* Minimal vector, just jump to the handler dfefined below */
    .align    4
    .global    _AltResetVector
_AltResetVector:
    j    _AltResetHandler

    .size    _AltResetVector, . - _AltResetVector


/* Alternate reset vector handler, just jump to the normal boot vector */
    .align    4
    .literal_position    // tells the assembler/linker to place literals here

    .align    4
    .global    _AltResetHandler
_AltResetHandler:
    j.l    0x7100000, a0
    .end    literal_prefix

