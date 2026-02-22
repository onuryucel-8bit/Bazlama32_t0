### Yazim kurallari

    bellekler(0, 1, 2, 3, 4, 5, 6, 7[yigin/sp])
    x = [0,7] y = [0,7]

    bellek uzunlugu
    (
        x86 karsiligi 
        s => a, 
        a => ax, 
        o => eax
    )

    u = (s,a,o)

    s = Sekiz
    a = on Alti
    o = Otuz iki
      *-----------------------*
    32|        $rox           |
      *-----------------------*
    16|         |   $rax      |
      *-----------------------*
    8 |         |      | $rsx |
      *-----------------------*
    $ra2 => on altilik bellek 2
    $ro4 => otuz ikilik bellek 4

    load $ra2, @0xf2040021
    ;@0xf2040021 adresine gidip on altilik veriyi bellek 2 ye yerlestirir

    ;       bayt1           
    ;r2 = ram[@0xf2040021] [0xf2040022] [0xf2040023] [0xf2040024]

### Komutlar
    load $rux, sayi
    load $rux, @ry
    load $rux, @adr
    load $rux, @adr+ry

    str @adr, sayi

### hatalar

    kaynak:
        Lexer::lexSingleChar()
        case '@'

    load $rs1, @0f'00'2f'00
    load $rs1, @0f00'2f00 calismakta