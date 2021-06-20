; réutilisation et adaptation pour karres de platOrica.s de cc65chess par :
;)              _
;)  ___ ___ _ _|_|___ ___
;) |  _| .'|_'_| |_ -|_ -|
;) |_| |__,|_,_|_|___|___|
;)         raxiss (c) 2020

; ======================================================================
; cc65 chess for Oric
; ======================================================================

; ------------------------------------------------------------------------
.export __TAPE__:abs = 1
.import __AUTORUN__, __PROGFLAG__
.import __BASHEAD_START__, __MAIN_LAST__

.segment "TAPE"

            .byte   $16, $16, $16, $16, $24, $00, $00
            .byte   <__PROGFLAG__
            .byte   <__AUTORUN__
            .dbyt   __MAIN_LAST__ - 1
            .dbyt   __BASHEAD_START__
            .byte   $00
            .byte   "KARRES",0

; ======================================================================
.segment "CODE"


; ----------------------------------------------------------------------
scrnoff       = $a000
rst           = $fffc
irq           = $fffe

hires_atmos   = $ec33
hires_oric1   = $e9bb

getkey_atmos  = $eb78
getkey_oric1  = $e905

; ----------------------------------------------------------------------

; ======================================================================
_setup:
              lda   irq
              cmp   #$28
              beq   isOric1

isAtmos:      lda   #<hires_atmos
              sta   _hires+1
              lda   #>hires_atmos
              sta   _hires+2
              lda   #<getkey_atmos
              sta   _getkey+1
              lda   #>getkey_atmos
              sta   _getkey+2
              jmp   _hires

isOric1:      lda   #<hires_oric1
              sta   _hires+1
              lda   #>hires_oric1
              sta   _hires+2
              lda   #<getkey_oric1
              sta   _getkey+1
              lda   #>getkey_oric1
              sta   _getkey+2

; ----------------------------------------------------------------------
_hires:       jsr   $f00d
              lda   #$08
              sta   $26a
              rts

; ----------------------------------------------------------------------
_quit:        lda   rst
              sta   l00_aptr+1
              lda   rst+1
              sta   l00_aptr+2
              sei
l00_aptr:     jmp   $dead

; ----------------------------------------------------------------------
_getkey:      jsr   $f00d
              and   #$7f
              beq   test
              cmp   #$0d
test:
              ldx   #$00
              rts

; ======================================================================
_hires_xpos:  .byte 0
_hires_ypos:  .byte 0
_hires_xsize: .byte 0
_hires_ysize: .byte 0
_hires_rop:   .byte 0
_hires_clr:   .byte 0
_hires_src:   .word 0

; ----------------------------------------------------------------------
_hires_MaskA:
              clc
              ldx   _hires_ypos
              lda   _scrnlo,x
              adc   _hires_xpos
              sta   l01_adst+1
              lda   _scrnhi,x
              adc   #0
              sta   l01_adst+2
l01_loop:         ldx   #0
              lda   _hires_rop
l01_adst:         sta   $f00d,x
              inx
              cpx   _hires_xsize
              bne   l01_adst
              clc
              lda   l01_adst+1
              adc   #40
              sta   l01_adst+1
              lda   l01_adst+2
              adc   #0
              sta   l01_adst+2
              dec   _hires_ysize
              bne   l01_loop
              rts

; ----------------------------------------------------------------------
_hires_DrawA:
              lda   _hires_src
              sta   l02_asrc+1
              lda   _hires_src+1
              sta   l02_asrc+2

              clc
              ldx   _hires_ypos
              lda   _scrnlo,x
              adc   _hires_xpos
              sta   l02_adst+1
              lda   _scrnhi,x
              adc   #0
              sta   l02_adst+2

              ldy   _hires_ysize
l02_loop:         ldx   #0
l02_asrc:         lda   $f00d,x
              ora   _hires_rop
l02_adst:         sta   $f00d,x
              inx
              cpx   _hires_xsize
              bne   l02_asrc

              clc
              ;lda   #<4
              lda   _hires_xsize
              adc   l02_asrc+1
              sta   l02_asrc+1
              lda   #>4
              adc   l02_asrc+2
              sta   l02_asrc+2

              clc
              lda   #<40
              adc   l02_adst+1
              sta   l02_adst+1
              lda   #>40
              adc   l02_adst+2
              sta   l02_adst+2

              dey
              bne   l02_loop

              rts

; ----------------------------------------------------------------------
_hires_SelectA:
              lda   _hires_src
              sta   l03_asrc+1
              lda   _hires_src+1
              sta   l03_asrc+2

              clc
              ldx   _hires_ypos
              lda   _scrnlo,x
              adc   _hires_xpos
              sta   l03_adst+1
              sta   l03_arop+1
              lda   _scrnhi,x
              adc   #0
              sta   l03_adst+2
              sta   l03_arop+2

              ldy   _hires_ysize
l03_loop:         ldx   #0
l03_asrc:         lda   $f00d,x
l03_arop:         eor   $f00d,x
              ora   #$40
l03_adst:         sta   $f00d,x
              inx
              cpx   _hires_xsize
              bne   l03_asrc

              clc
              lda   #<3*7
              adc   l03_asrc+1
              sta   l03_asrc+1
              lda   #>3*7
              adc   l03_asrc+2
              sta   l03_asrc+2

              clc
              lda   #<40
              adc   l03_adst+1
              sta   l03_adst+1
              sta   l03_arop+1
              lda   #>40
              adc   l03_adst+2
              sta   l03_adst+2
              sta   l03_arop+2

              dey
              bne   l03_loop

              rts


; ----------------------------------------------------------------------
_hires_DrawCharA:
              lda   _hires_src
              sta   l04_asrc+1
              lda   _hires_src+1
              sta   l04_asrc+2

              clc
              ldx   _hires_ypos
              lda   _scrnlo,x
              adc   _hires_xpos
              sta   l04_adst+1
              lda   _scrnhi,x
              adc   #0
              sta   l04_adst+2

              ldx   #0
l04_loop:
l04_asrc:         lda   $f00d,x
              ora   _hires_rop
l04_adst:         sta   $f00d

              clc
              lda   #<40
              adc   l04_adst+1
              sta   l04_adst+1
              lda   #>40
              adc   l04_adst+2
              sta   l04_adst+2

              inx
              cpx   #8
              bne   l04_loop

              rts

; ----------------------------------------------------------------------
_hires_DrawClrA:
              clc
              ldx   _hires_ypos
              lda   _scrnlo,x
              adc   _hires_xpos
              sta   l05_adst+1
              lda   _scrnhi,x
              adc   #0
              sta   l05_adst+2

              ldx   #8
l05_loop:         lda   _hires_clr
l05_adst:         sta   $f00d

              clc
              lda   #<40
              adc   l05_adst+1
              sta   l05_adst+1
              lda   #>40
              adc   l05_adst+2
              sta   l05_adst+2

              dex
              bne   l05_loop

              rts

; ----------------------------------------------------------------------
_hires_CharSet  = $9900

_scrnlo:
  .byte <($a000+(  0*40))
  .byte <($a000+(  1*40))
  .byte <($a000+(  2*40))
  .byte <($a000+(  3*40))
  .byte <($a000+(  4*40))
  .byte <($a000+(  5*40))
  .byte <($a000+(  6*40))
  .byte <($a000+(  7*40))
  .byte <($a000+(  8*40))
  .byte <($a000+(  9*40))
  .byte <($a000+( 10*40))
  .byte <($a000+( 11*40))
  .byte <($a000+( 12*40))
  .byte <($a000+( 13*40))
  .byte <($a000+( 14*40))
  .byte <($a000+( 15*40))
  .byte <($a000+( 16*40))
  .byte <($a000+( 17*40))
  .byte <($a000+( 18*40))
  .byte <($a000+( 19*40))
  .byte <($a000+( 20*40))
  .byte <($a000+( 21*40))
  .byte <($a000+( 22*40))
  .byte <($a000+( 23*40))
  .byte <($a000+( 24*40))
  .byte <($a000+( 25*40))
  .byte <($a000+( 26*40))
  .byte <($a000+( 27*40))
  .byte <($a000+( 28*40))
  .byte <($a000+( 29*40))
  .byte <($a000+( 30*40))
  .byte <($a000+( 31*40))
  .byte <($a000+( 32*40))
  .byte <($a000+( 33*40))
  .byte <($a000+( 34*40))
  .byte <($a000+( 35*40))
  .byte <($a000+( 36*40))
  .byte <($a000+( 37*40))
  .byte <($a000+( 38*40))
  .byte <($a000+( 39*40))
  .byte <($a000+( 40*40))
  .byte <($a000+( 41*40))
  .byte <($a000+( 42*40))
  .byte <($a000+( 43*40))
  .byte <($a000+( 44*40))
  .byte <($a000+( 45*40))
  .byte <($a000+( 46*40))
  .byte <($a000+( 47*40))
  .byte <($a000+( 48*40))
  .byte <($a000+( 49*40))
  .byte <($a000+( 50*40))
  .byte <($a000+( 51*40))
  .byte <($a000+( 52*40))
  .byte <($a000+( 53*40))
  .byte <($a000+( 54*40))
  .byte <($a000+( 55*40))
  .byte <($a000+( 56*40))
  .byte <($a000+( 57*40))
  .byte <($a000+( 58*40))
  .byte <($a000+( 59*40))
  .byte <($a000+( 60*40))
  .byte <($a000+( 61*40))
  .byte <($a000+( 62*40))
  .byte <($a000+( 63*40))
  .byte <($a000+( 64*40))
  .byte <($a000+( 65*40))
  .byte <($a000+( 66*40))
  .byte <($a000+( 67*40))
  .byte <($a000+( 68*40))
  .byte <($a000+( 69*40))
  .byte <($a000+( 70*40))
  .byte <($a000+( 71*40))
  .byte <($a000+( 72*40))
  .byte <($a000+( 73*40))
  .byte <($a000+( 74*40))
  .byte <($a000+( 75*40))
  .byte <($a000+( 76*40))
  .byte <($a000+( 77*40))
  .byte <($a000+( 78*40))
  .byte <($a000+( 79*40))
  .byte <($a000+( 80*40))
  .byte <($a000+( 81*40))
  .byte <($a000+( 82*40))
  .byte <($a000+( 83*40))
  .byte <($a000+( 84*40))
  .byte <($a000+( 85*40))
  .byte <($a000+( 86*40))
  .byte <($a000+( 87*40))
  .byte <($a000+( 88*40))
  .byte <($a000+( 89*40))
  .byte <($a000+( 90*40))
  .byte <($a000+( 91*40))
  .byte <($a000+( 92*40))
  .byte <($a000+( 93*40))
  .byte <($a000+( 94*40))
  .byte <($a000+( 95*40))
  .byte <($a000+( 96*40))
  .byte <($a000+( 97*40))
  .byte <($a000+( 98*40))
  .byte <($a000+( 99*40))
  .byte <($a000+(100*40))
  .byte <($a000+(101*40))
  .byte <($a000+(102*40))
  .byte <($a000+(103*40))
  .byte <($a000+(104*40))
  .byte <($a000+(105*40))
  .byte <($a000+(106*40))
  .byte <($a000+(107*40))
  .byte <($a000+(108*40))
  .byte <($a000+(109*40))
  .byte <($a000+(110*40))
  .byte <($a000+(111*40))
  .byte <($a000+(112*40))
  .byte <($a000+(113*40))
  .byte <($a000+(114*40))
  .byte <($a000+(115*40))
  .byte <($a000+(116*40))
  .byte <($a000+(117*40))
  .byte <($a000+(118*40))
  .byte <($a000+(119*40))
  .byte <($a000+(120*40))
  .byte <($a000+(121*40))
  .byte <($a000+(122*40))
  .byte <($a000+(123*40))
  .byte <($a000+(124*40))
  .byte <($a000+(125*40))
  .byte <($a000+(126*40))
  .byte <($a000+(127*40))
  .byte <($a000+(128*40))
  .byte <($a000+(129*40))
  .byte <($a000+(130*40))
  .byte <($a000+(131*40))
  .byte <($a000+(132*40))
  .byte <($a000+(133*40))
  .byte <($a000+(134*40))
  .byte <($a000+(135*40))
  .byte <($a000+(136*40))
  .byte <($a000+(137*40))
  .byte <($a000+(138*40))
  .byte <($a000+(139*40))
  .byte <($a000+(140*40))
  .byte <($a000+(141*40))
  .byte <($a000+(142*40))
  .byte <($a000+(143*40))
  .byte <($a000+(144*40))
  .byte <($a000+(145*40))
  .byte <($a000+(146*40))
  .byte <($a000+(147*40))
  .byte <($a000+(148*40))
  .byte <($a000+(149*40))
  .byte <($a000+(150*40))
  .byte <($a000+(151*40))
  .byte <($a000+(152*40))
  .byte <($a000+(153*40))
  .byte <($a000+(154*40))
  .byte <($a000+(155*40))
  .byte <($a000+(156*40))
  .byte <($a000+(157*40))
  .byte <($a000+(158*40))
  .byte <($a000+(159*40))
  .byte <($a000+(160*40))
  .byte <($a000+(161*40))
  .byte <($a000+(162*40))
  .byte <($a000+(163*40))
  .byte <($a000+(164*40))
  .byte <($a000+(165*40))
  .byte <($a000+(166*40))
  .byte <($a000+(167*40))
  .byte <($a000+(168*40))
  .byte <($a000+(169*40))
  .byte <($a000+(170*40))
  .byte <($a000+(171*40))
  .byte <($a000+(172*40))
  .byte <($a000+(173*40))
  .byte <($a000+(174*40))
  .byte <($a000+(175*40))
  .byte <($a000+(176*40))
  .byte <($a000+(177*40))
  .byte <($a000+(178*40))
  .byte <($a000+(179*40))
  .byte <($a000+(180*40))
  .byte <($a000+(181*40))
  .byte <($a000+(182*40))
  .byte <($a000+(183*40))
  .byte <($a000+(184*40))
  .byte <($a000+(185*40))
  .byte <($a000+(186*40))
  .byte <($a000+(187*40))
  .byte <($a000+(188*40))
  .byte <($a000+(189*40))
  .byte <($a000+(190*40))
  .byte <($a000+(191*40))
  .byte <($a000+(192*40))
  .byte <($a000+(193*40))
  .byte <($a000+(194*40))
  .byte <($a000+(195*40))
  .byte <($a000+(196*40))
  .byte <($a000+(197*40))
  .byte <($a000+(198*40))
  .byte <($a000+(199*40))

_scrnhi:
  .byte >($a000+(  0*40))
  .byte >($a000+(  1*40))
  .byte >($a000+(  2*40))
  .byte >($a000+(  3*40))
  .byte >($a000+(  4*40))
  .byte >($a000+(  5*40))
  .byte >($a000+(  6*40))
  .byte >($a000+(  7*40))
  .byte >($a000+(  8*40))
  .byte >($a000+(  9*40))
  .byte >($a000+( 10*40))
  .byte >($a000+( 11*40))
  .byte >($a000+( 12*40))
  .byte >($a000+( 13*40))
  .byte >($a000+( 14*40))
  .byte >($a000+( 15*40))
  .byte >($a000+( 16*40))
  .byte >($a000+( 17*40))
  .byte >($a000+( 18*40))
  .byte >($a000+( 19*40))
  .byte >($a000+( 20*40))
  .byte >($a000+( 21*40))
  .byte >($a000+( 22*40))
  .byte >($a000+( 23*40))
  .byte >($a000+( 24*40))
  .byte >($a000+( 25*40))
  .byte >($a000+( 26*40))
  .byte >($a000+( 27*40))
  .byte >($a000+( 28*40))
  .byte >($a000+( 29*40))
  .byte >($a000+( 30*40))
  .byte >($a000+( 31*40))
  .byte >($a000+( 32*40))
  .byte >($a000+( 33*40))
  .byte >($a000+( 34*40))
  .byte >($a000+( 35*40))
  .byte >($a000+( 36*40))
  .byte >($a000+( 37*40))
  .byte >($a000+( 38*40))
  .byte >($a000+( 39*40))
  .byte >($a000+( 40*40))
  .byte >($a000+( 41*40))
  .byte >($a000+( 42*40))
  .byte >($a000+( 43*40))
  .byte >($a000+( 44*40))
  .byte >($a000+( 45*40))
  .byte >($a000+( 46*40))
  .byte >($a000+( 47*40))
  .byte >($a000+( 48*40))
  .byte >($a000+( 49*40))
  .byte >($a000+( 50*40))
  .byte >($a000+( 51*40))
  .byte >($a000+( 52*40))
  .byte >($a000+( 53*40))
  .byte >($a000+( 54*40))
  .byte >($a000+( 55*40))
  .byte >($a000+( 56*40))
  .byte >($a000+( 57*40))
  .byte >($a000+( 58*40))
  .byte >($a000+( 59*40))
  .byte >($a000+( 60*40))
  .byte >($a000+( 61*40))
  .byte >($a000+( 62*40))
  .byte >($a000+( 63*40))
  .byte >($a000+( 64*40))
  .byte >($a000+( 65*40))
  .byte >($a000+( 66*40))
  .byte >($a000+( 67*40))
  .byte >($a000+( 68*40))
  .byte >($a000+( 69*40))
  .byte >($a000+( 70*40))
  .byte >($a000+( 71*40))
  .byte >($a000+( 72*40))
  .byte >($a000+( 73*40))
  .byte >($a000+( 74*40))
  .byte >($a000+( 75*40))
  .byte >($a000+( 76*40))
  .byte >($a000+( 77*40))
  .byte >($a000+( 78*40))
  .byte >($a000+( 79*40))
  .byte >($a000+( 80*40))
  .byte >($a000+( 81*40))
  .byte >($a000+( 82*40))
  .byte >($a000+( 83*40))
  .byte >($a000+( 84*40))
  .byte >($a000+( 85*40))
  .byte >($a000+( 86*40))
  .byte >($a000+( 87*40))
  .byte >($a000+( 88*40))
  .byte >($a000+( 89*40))
  .byte >($a000+( 90*40))
  .byte >($a000+( 91*40))
  .byte >($a000+( 92*40))
  .byte >($a000+( 93*40))
  .byte >($a000+( 94*40))
  .byte >($a000+( 95*40))
  .byte >($a000+( 96*40))
  .byte >($a000+( 97*40))
  .byte >($a000+( 98*40))
  .byte >($a000+( 99*40))
  .byte >($a000+(100*40))
  .byte >($a000+(101*40))
  .byte >($a000+(102*40))
  .byte >($a000+(103*40))
  .byte >($a000+(104*40))
  .byte >($a000+(105*40))
  .byte >($a000+(106*40))
  .byte >($a000+(107*40))
  .byte >($a000+(108*40))
  .byte >($a000+(109*40))
  .byte >($a000+(110*40))
  .byte >($a000+(111*40))
  .byte >($a000+(112*40))
  .byte >($a000+(113*40))
  .byte >($a000+(114*40))
  .byte >($a000+(115*40))
  .byte >($a000+(116*40))
  .byte >($a000+(117*40))
  .byte >($a000+(118*40))
  .byte >($a000+(119*40))
  .byte >($a000+(120*40))
  .byte >($a000+(121*40))
  .byte >($a000+(122*40))
  .byte >($a000+(123*40))
  .byte >($a000+(124*40))
  .byte >($a000+(125*40))
  .byte >($a000+(126*40))
  .byte >($a000+(127*40))
  .byte >($a000+(128*40))
  .byte >($a000+(129*40))
  .byte >($a000+(130*40))
  .byte >($a000+(131*40))
  .byte >($a000+(132*40))
  .byte >($a000+(133*40))
  .byte >($a000+(134*40))
  .byte >($a000+(135*40))
  .byte >($a000+(136*40))
  .byte >($a000+(137*40))
  .byte >($a000+(138*40))
  .byte >($a000+(139*40))
  .byte >($a000+(140*40))
  .byte >($a000+(141*40))
  .byte >($a000+(142*40))
  .byte >($a000+(143*40))
  .byte >($a000+(144*40))
  .byte >($a000+(145*40))
  .byte >($a000+(146*40))
  .byte >($a000+(147*40))
  .byte >($a000+(148*40))
  .byte >($a000+(149*40))
  .byte >($a000+(150*40))
  .byte >($a000+(151*40))
  .byte >($a000+(152*40))
  .byte >($a000+(153*40))
  .byte >($a000+(154*40))
  .byte >($a000+(155*40))
  .byte >($a000+(156*40))
  .byte >($a000+(157*40))
  .byte >($a000+(158*40))
  .byte >($a000+(159*40))
  .byte >($a000+(160*40))
  .byte >($a000+(161*40))
  .byte >($a000+(162*40))
  .byte >($a000+(163*40))
  .byte >($a000+(164*40))
  .byte >($a000+(165*40))
  .byte >($a000+(166*40))
  .byte >($a000+(167*40))
  .byte >($a000+(168*40))
  .byte >($a000+(169*40))
  .byte >($a000+(170*40))
  .byte >($a000+(171*40))
  .byte >($a000+(172*40))
  .byte >($a000+(173*40))
  .byte >($a000+(174*40))
  .byte >($a000+(175*40))
  .byte >($a000+(176*40))
  .byte >($a000+(177*40))
  .byte >($a000+(178*40))
  .byte >($a000+(179*40))
  .byte >($a000+(180*40))
  .byte >($a000+(181*40))
  .byte >($a000+(182*40))
  .byte >($a000+(183*40))
  .byte >($a000+(184*40))
  .byte >($a000+(185*40))
  .byte >($a000+(186*40))
  .byte >($a000+(187*40))
  .byte >($a000+(188*40))
  .byte >($a000+(189*40))
  .byte >($a000+(190*40))
  .byte >($a000+(191*40))
  .byte >($a000+(192*40))
  .byte >($a000+(193*40))
  .byte >($a000+(194*40))
  .byte >($a000+(195*40))
  .byte >($a000+(196*40))
  .byte >($a000+(197*40))
  .byte >($a000+(198*40))
  .byte >($a000+(199*40))


.export _getkey
.export _hires_CharSet
.export _hires_DrawA
.export _hires_DrawCharA
.export _hires_DrawClrA
.export _hires_MaskA
.export _hires_SelectA
.export _hires_clr
.export _hires_rop
.export _hires_src
.export _hires_xpos
.export _hires_xsize
.export _hires_ypos
.export _hires_ysize
.export _quit
.export _setup
.export _hires
