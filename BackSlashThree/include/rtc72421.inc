; Real Time Clock (RTC) model 72421/3
;   RTC registry address
;     date / time registry
RTC72421_REG_S1             equ     $0110
RTC72421_REG_S10            equ     $0111
RTC72421_REG_MI1            equ     $0112
RTC72421_REG_MI10           equ     $0113
RTC72421_REG_H1             equ     $0114
RTC72421_REG_H10            equ     $0115
RTC72421_REG_D1             equ     $0116
RTC72421_REG_D10            equ     $0117
RTC72421_REG_MO1            equ     $0118
RTC72421_REG_MO10           equ     $0119
RTC72421_REG_Y1             equ     $011A
RTC72421_REG_Y10            equ     $011B
RTC72421_REG_W              equ     $011C
;     control registry
RTC72421_REG_CD             equ     $011D
RTC72421_REG_CE             equ     $011E
RTC72421_REG_CF             equ     $011F

;   Control registry masks
;     registry CD
RTC72421_BIT_30SADJ         equ     $08
RTC72421_VAL_30SADJ_SET     equ     $08
RTC72421_VAL_30SADJ_CLEAR   equ     $00
RTC72421_BIT_IRQ_FLAG       equ     $04
RTC72421_VAL_IRQ_FLAG_SET   equ     $04
RTC72421_VAL_IRQ_FLAG_CLEAR equ     $00
RTC72421_BIT_BUSY           equ     $02
RTC72421_BIT_HOLD           equ     $01
RTC72421_VAL_HOLD_SET       equ     $01
RTC72421_VAL_HOLD_CLEAR     equ     $00
;     registry CE
RTC72421_BIT_FP_t1t0        equ     $0C
RTC72421_VAL_FP_1_64        equ     $00
RTC72421_VAL_FP_1SEC        equ     $04
RTC72421_VAL_FP_1MIN        equ     $08
RTC72421_VAL_FP_1HOUR       equ     $0C
RTC72421_BIT_ITRPT_STND     equ     $02
RTC72421_VAL_ITRPT          equ     $02
RTC72421_VAL_STND           equ     $00
RTC72421_BIT_MASK           equ     $01
RTC72421_VAL_MASK_SET       equ     $01
RTC72421_VAL_MASK_CLEAR     equ     $00
 ;     registry CF
RTC72421_BIT_TEST           equ     $08
RTC72421_VAL_TEST_SET       equ     $08
RTC72421_VAL_TEST_CLEAR     equ     $00
RTC72421_BIT_2412           equ     $04
RTC72421_VAL_2412_12        equ     $00
RTC72421_VAL_2412_24        equ     $04
RTC72421_BIT_STOP           equ     $02
RTC72421_VAL_STOP_SET       equ     $02
RTC72421_VAL_STOP_CLEAR     equ     $00
RTC72421_BIT_RESET          equ     $01
RTC72421_VAL_RESET_SET      equ     $01
RTC72421_VAL_RESET_CLEAR    equ     $00
