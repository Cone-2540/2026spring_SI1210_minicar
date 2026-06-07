ORG     0000H
LJMP    MAIN

ORG     0003H
LJMP    MINE_BOMB

ORG     000BH
LJMP    TIMER

ORG     0013H
LJMP    HUOER

ORG     001BH
LJMP    SPEED

;**************************************************
ORG     0030H
;**************************************************
MINE_BOMB:
        PUSH    A
        PUSH    PSW

        INC     3AH

        MOV     A,40H
        MOV     3BH,A

        MOV     A,41H
        MOV     3CH,A

        CLR    20H.3
        SETB   P3.4
        SETB   P3.5

        POP     PSW
        POP     A
RETI

;**************************************************
TIMER:
        PUSH    A
        PUSH    PSW

        MOV       TL0,#0F0H
        MOV       TH0,#0D8H

        INC       37H
        MOV       A,37H

        CJNE      A,#100,TIMEREND
;is 1second

IS_HUN:
        MOV       37H,#0
        ;            IS C_MINE,STOP AND THAN  XUAN
        JB       20H.0,QI_LAI        ;THIS TIME,THE CAR IS WAITTING IN THE END,
        DEC      39H
        MOV      A,39H
        CJNE     A,#0H,TIMEREND
        SETB     20H.0
        LJMP     TIMEREND

QI_LAI:
        JB       20H.1,WAKE_UP
        DEC      38H
        CPL      P3.4
        MOV      A,38H
        CJNE     A,#0H,WAKE_UP
        SETB     20H.1
        CLR      P3.4
        SETB     TR1
        MOV      5AH,#23

WAKE_UP:
        INC       43H
        MOV       A,43H
        CJNE      A,#30,GOGOGO_DOWN
        MOV       A,42H
        JZ        GOGOGO_DOWN
        LJMP      HOME
GOGOGO_DOWN:
        MOV       A,43H
        CJNE      A,#60,TIMEREND
        MOV       43H,#0

        INC       42H
        MOV       A,42H
        CJNE      A,#60,TIMEREND
        MOV       42H,#0


TIMEREND:
        POP     PSW
        POP     A

RETI

;**************************************************
HUOER:
        PUSH    A
        PUSH    PSW

        MOV     A,3FH
        CLR     C
        ADD     A,#64    ;加上6.3毫米
        CJNE    A,#100,CHECK_CM
CHECK_CM:
        JC      ADD_CM
        SUBB    A,#100    ;大于10个毫米了，要作调整
        MOV     3FH,A
        INC     41H

ADD_CM:                   ;厘米上要加2
        INC     41H
        INC     41H

        MOV     A,41H
        CJNE    A,#100,CHECK_M
CHECK_M:
        JC      HUOER_END
        SUBB    A,#100    ;大于一米了，要调整一下
        MOV     41H,A
        INC     40H

        ;查看是否是两米了，是的话就算到了B区
        MOV     A,40H
        CJNE    A,#2,HUOER_END
        MOV     36H,#0BH

HUOER_END:
        POP     PSW
        POP     A
RETI

;**************************************************
SPEED:
        PUSH    A
        PUSH    PSW

        MOV       TL1,#0B0H
        MOV       TH1,#03CH

        INC       5BH
        MOV       A,5BH

        CJNE      A,5AH,SPEED_NO_EQUAL   ; 5A中放了速度值，比5A大，刚置为停，小则行
        CLR       P3.0                   ; 等于5A了，置为停
        LJMP      SP_END

SPEED_NO_EQUAL:                          ; 不是5A

        JC       IS_GOGOGO               ; 小于5A，则向前走
        CJNE     A,#40,SP_BIG_TOTAL      ; 是不是比总脉冲值大
        SETB     P3.0
        MOV      5BH,#0H
        LJMP     SP_END
IS_GOGOGO:                               ;是行，就置行
        SETB     P3.0
        LJMP     SP_END

SP_BIG_TOTAL:                            ; 大于5A，不是40
        CLR      P3.0
        JC       SP_END                  ;比40小则是停，就不管
        MOV      5AH,#0H
SP_END:

        POP     PSW
        POP     A
RETI

;**************************************************
;**************************************************


MAIN:
        MOV     SP,#5FH
        LCALL   INIT
    ;   LJMP    HOME;\\\\\\\\\

SIX_SECOND:
        JB      20H.0,GO_MINE_ZEON
        MOV     R1,#39H
        LCALL   SETCODEALL
        LCALL   OUT
        LJMP    SIX_SECOND

GO_MINE_ZEON:
        SETB     TR1
        LJMP     MINE_WAY
RET

;**************************************************
;**************************************************

INIT:
     MOV        R1,#36H
     MOV        R2,#34
INIT_LOOP:
     MOV        @R1,#0
     INC        R1
     DJNZ       R2,INIT_LOOP

     MOV        20H,#0H
     SETB       20H.1
     SETB       20H.3


     MOV        39H,#6H      ;倒计时654321
     MOV        38H,#5H      ;倒计时54321
     MOV        5AH,#24      ;用于车速控制

     MOV        36H,#0AH

     MOV        P3,#0CH
     MOV        P2,#0H


INT_ZONE:
     LCALL      SET_INT

RET
;**************************************************
SET_INT:
     MOV       TMOD,#01H
     MOV       TL0,#0F0H
     MOV       TH0,#0D8H

     MOV       TL1,#0B0H
     MOV       TH1,#03CH
     SETB      TR0
     CLR       TR1

     SETB      EA
     SETB      ET0
     SETB      ET1
     SETB      EX0
     SETB      IT0

     SETB      EX1
     SETB      IT1

RET
;**************************************************

SETCODEALL:;6543210
        MOV     R0,#30H
        MOV     R2,#6
SETCODEALL_LOOP:
        MOV     A,@R1
        MOV     DPTR,#TAB
        MOVC    A,@A+DPTR
        MOV     @R0,A
        INC     R0
        DJNZ    R2,SETCODEALL_LOOP
RET

;**************************************************
;**************************************************

MINE_WAY:
        LCALL TRACK

        JB    20H.3,MINE_WAY
        MOV     C,P3.2           ;下面有雷，就是低
        JC    MINE_WAY

        ;下面是关小灯和蜂鸣器
        CLR     P3.4
        CLR     P3.5

        ;看看长度是否超过4米
        MOV     A,40H
        CJNE    A,#4H,PASS_C
        LJMP    C_POINT

PASS_C:
        JNC     C_POINT

        ;下面调用一般雷的处理方法，再返回主程序
        LCALL   NORMAL_MINE
        LJMP    MINE_WAY

        ;下面是到达C点了
C_POINT:
        CLR     20H.1
        LCALL   FIVE_SECOND
        SETB    20H.3
        SETB    TR1
        MOV     5AH,#23
        LJMP    BOLK

RET

;****************************************************

TRACK:
      MOV       A,P0
      ANL       A,#07H
      CJNE      A,#7H,SET_TURN
      LJMP      LIGHT_END
SET_TURN:
      CJNE      A,#3H,LIGHT_NEXT
RIGHT_LIGHT:         ;  看右边的光敏元件
      CLR       P2.7        ;这时就要向右转弯了
      SETB      P2.6
      LCALL     TURN_DELAY
      CLR       P2.6
      RET
LIGHT_NEXT:          ; 左转了
      CJNE      A,#06H,LIGHT_END
      SETB      P2.7
      SETB      P2.6
      LCALL     TURN_DELAY
      CLR       P2.6
      RET

LIGHT_END:
      CLR   P2.6
RET
;****************************************************

TURN_DELAY:
      MOV        5AH,#26
      MOV        C, P0.1
      JC         TURN_DELAY

      MOV        A,36H

      CJNE       A,#0AH,IS_ZONE_B
      MOV        5AH,#24
      LJMP       TURN_DELAY_END

IS_ZONE_B:
      MOV        5AH,#20
TURN_DELAY_END:

RET

;****************************************************

NORMAL_MINE:
      CLR        TR1
      CLR        P2.6

      ;以下是保存雷长度
      MOV       A,3CH
      ADD       A,#53H
      MOV       R0,A

      CLR       C
      MOV       A,41H
      ADD       A,#100
      SUBB      A,3CH
      CJNE      A,#100,NORMAL_MINE_LENGTH
NORMAL_MINE_LENGTH:
      JC        NO_CHANGE
      SUBB      A,#100
NO_CHANGE:
      MOV       @R0,A


      ;以下是计算中点距起点的距离
      MOV       B,#2H
      DIV       AB

      ADD       A,3CH
      CJNE      A,#100,SET_CENTER
SET_CENTER:
      JC        NO_P_WAY
      SUBB      A,#100
      INC       3BH
NO_P_WAY:
      MOV       3CH,A

      ;保存中点距离
      MOV       A,3AH
      DEC       A
      MOV       B,#2H
      MUL       AB

      MOV       R0,48H
      ADD       A,R0

      MOV       R0,A
      MOV       A,3BH
      MOV       @R0,A

      INC       R0
      MOV       A,3CH
      MOV       @R0,A



      CLR        P3.0
      CLR        P3.1
      CLR        P2.6

      CLR        TR1

      ;以下是延时显示程序
      LCALL      DELAY_MINE2
      CLR        P3.1
      SETB       P3.0
      SETB       TR1
      SETB       20H.3

RET

;****************************************************
DELAY_MINE1:
      MOV       R6,#2H
DELAY_LOOP_MINE1:
      LCALL      DELAY
      DJNZ       R6,DELAY_LOOP_MINE1
RET

;****************************************************
;****************************************************
DELAY_MINE2:
      LCALL     SET_CODE_MINE
      MOV       R5,#03H
DELAY_LOOP_MINE_F:
      MOV       R6,#08FH
      DELAY_LOOP_MINE2:
           LCALL      OUT
           DJNZ       R6,DELAY_LOOP_MINE2
      DJNZ     R5,DELAY_LOOP_MINE_F
RET

;****************************************************

SET_CODE_MINE:
      MOV     A,3BH
      MOV     B,#0AH
      DIV     AB
      MOV     44H,A
      MOV     45H,B

      MOV     A,3CH
      MOV     B,#0AH
      DIV     AB
      MOV     46H,A
      MOV     47H,B

      LCALL   SET_CODE
RET


;****************************************************

FIVE_SECOND:
         CLR        TR1
         CLR        P2.6
         SETB       P3.1
         SETB       P3.0
         LCALL      DELAY_MINE1
         CLR        P3.0
         CLR        P3.1
         CLR        P2.6

         SETB       P3.5
         SETB       P3.4
         CLR        TR1

STOP_AT_C:
        MOV     R1,#38H
        LCALL   SETCODEALL
        JB      20H.1,C_WAKE_LE
        LCALL   OUT
        LJMP    STOP_AT_C

C_WAKE_LE:
        SETB    TR1

RET

;****************************************************
;****************************************************

BOLK:
        SETB    TR1
        MOV     5AH,#20


        



SEEK_LAMP:
        LCALL   TURN_TO_LAMP
        MOV     C,P0.1
        JNC     HOME
        JB      20H.6,SEEK_LAMP

        MOV     C,P0.6
        JC      SEEK_LAMP
AV:     LCALL   AVOID_WHITE_BOLK
        MOV     C,P0.6
        JNC     AV
        SETB    20H.6

        LJMP    SEEK_LAMP

RET

;******************************************************

TURN_TO_LAMP:
        MOV  C,P0.3
        JNC  LOOK_RIGHT
        LCALL  TURN_LEFT_MIDDLE
        RET
LOOK_RIGHT:
        MOV     C,P0.5
        MOV     R6,#4FH
FORWARDLIGHT:
        LCALL   DELAY
        DJNZ    R6,FORWARDLIGHT
        JNC     TURN_TO_LAMP_END
        LCALL   TURN_RIGHT_MIDDLE
        RET

TURN_TO_LAMP_END:
RET

;******************************************************

;******************************************************
AVOID_WHITE_BOLK:
         CLR     TR1
         SETB    P2.7
         SETB    P2.6
         LCALL   DELAY
         SETB    TR1
         
         MOV     5AH,#30
         MOV    R6,#3FH
AVOID_LOOP:
         LCALL  DELAY
         DJNZ   R6,AVOID_LOOP

         MOV     C,P0.6
         JNC     AVOID_WHITE_BOLK
         MOV     5AH,#23
         CLR     P2.6
RET
;******************************************************

TURN_RIGHT_MIDDLE:
         MOV      5AH,#30
TURN_RIGHT_MIDDLE_START:
         CLR    P2.7
         SETB   P2.6
         MOV    C,P0.3
         JNC    TURN_RIGHT_MIDDLE_START
         CLR    P2.6
         MOV    5AH,#23
RET

;******************************************************

TURN_LEFT_MIDDLE:
          MOV    5AH,#30
TURN_LEFT_MIDDLE_START:
         SETB   P2.7
         SETB   P2.6
          MOV    C,P0.5
         JNC    TURN_RIGHT_MIDDLE_START
         CLR    P2.6
         MOV    5AH,#23H
RET

;******************************************************

DELAY:
        MOV     R7,#0FFH
DELAY_LOOP:
        DJNZ    R7,DELAY_LOOP
RET

;******************************************************



;****************************************************

HOME:
        MOV     IE,#0H
        MOV     IE,#0H

        CLR     P2.6
        CLR     P3.0

HOME_WAIT:
        CLR     P2.6
        CLR     P3.0
        SETB    P3.4
        LCALL   KEY_QUERY     ; 查询键盘
        LCALL   TIP_SET_CODE  ; 设置显示值
         MOV     R1,#44H
         LCALL   SET_CODE
        LCALL   OUT           ; LED 输出
        LJMP    HOME_WAIT
RET

;******************************************************

KEY_QUERY:
        LCALL   KEY
        JNC    KEY_QUERY_END
        LCALL   KEY
        JNC      KEY_QUERY_END
        LCALL   DEAL_KEY
KEY_WA:
        LCALL   KEY
        JC      KEY_WA
KEY_QUERY_END:
RET


RET

;******************************************************

TIP_SET_CODE:

        MOV  A,59H
        JZ   SET_TIME

        MOV     R0,#46H
        MOV     B,#02H
        MUL     AB

        ADD     A,R0
        MOV     R0,A
SET_NUM:
        MOV     A,@R0
        MOV     B,#0AH
        DIV     AB
        MOV     44H,A
        MOV     45H,B

        INC     R0
        MOV     B,#0AH
        MOV     A,@R0
        DIV     AB
        MOV     46H,A
        MOV     47H,B
        RET

SET_TIME:
        MOV     R0,#42H
        LJMP    SET_NUM

RET

;*****************************************************

OUT:
        MOV     R0,#30H
        MOV     R2,#6H
        MOV     R4,#0H
LOOP:
        MOV     A,@R0
        LCALL   OUTPUT
        INC     R0
        INC     R4
        DJNZ    R2,LOOP
RET

;*******************************************************

OUTPUT: ;在A中是要显示的数据，R4中是要显示的位
        MOV     R3,#8H
        CLR     P1.3

OUT_LOOP:
        RLC     A
        MOV     P1.2,C
        CLR     P1.4
        NOP
        NOP
        NOP
        SETB    P1.4
        NOP
        NOP
        NOP
        DJNZ    R3,OUT_LOOP
OUT_LOOP_END:
        MOV     P1,R4
        SETB    P1.3
        LCALL   DELAY
        CLR     P1.3
RET

;******************************************************

SET_CODE:
        MOV     R0,#44H
        MOV     R1,#30H
        MOV     R2,#4H
SET_CODE_LOOP:
        MOV     A,@R0
        MOV     DPTR,#TAB
        MOVC    A,@A+DPTR
        MOV     @R1,A
        INC     R0
        INC     R1
        DJNZ    R2,SET_CODE_LOOP

        ;以下是显示是第几个雷
        MOV     A,3AH
        MOV     DPTR,#TAB
        MOVC    A,@A+DPTR
        MOV     35H,A
        ;以下是使一个LED为暗
        MOV     34H,#0H
RET

;******************************************************

TAB:   DB 3FH,06H,5BH,4FH,66H,6DH,7DH,07H,7FH,6FH,77H,7CH,39H,5EH,79H,71H

;*****************************************************
KEY:
        MOV     58H,#0FEH;
        MOV     R1,#4H
KEY_LOOP:
        MOV     P1,58H
        MOV     A,P1
        CJNE    A,58H,KEY_NEXT1
        MOV     A,58H ;NO KEY FIND
        RL      A
        MOV     58H,A
        DJNZ    R1,KEY_LOOP
        CLR    C
        LJMP    KEY_END

KEY_NEXT1:
        SETB    C
KEY_END:

RET


DEAL_KEY:
         MOV    A,58H
         MOV    P1,#0F0H
         CJNE   A,#0FEH,TWO

ONE_1:   SETB   P1.1
         NOP
         MOV    C,P1.1
         JC    ONE_2
         MOV    59H,#0EH
         LJMP   DEAL_END

ONE_2:   SETB   P1.2
         MOV    C,P1.2
         JC    ONE_3
         MOV    59H,#07H
         LJMP   DEAL_END
ONE_3:   MOV    59H,#0FH
         LJMP   DEAL_END

TWO:     CJNE   A,#0FDH,THREE
TWO_1:   SETB   P1.0
         MOV    C,P1.0
         JC    TWO_2
         MOV    59H,#2H
         LJMP   DEAL_END

TWO_2:   SETB   P1.2
         MOV    C,P1.2
         JC    TWO_3
         MOV    59H,#6H
         LJMP   DEAL_END
TWO_3:   MOV    59H,#9H
         LJMP   DEAL_END


THREE:   CJNE   A,#0FBH,FOUR


THREE_1: SETB   P1.0
         MOV    C,P1.0
         JC    THREE_2
         MOV    59H,#1H
         LJMP   DEAL_END

THREE_2: SETB   P1.1
         MOV    C,P1.1
         JC    THREE_3
         MOV    59H,#4H
         LJMP   DEAL_END
THREE_3: MOV    59H,#8H
         LJMP   DEAL_END

FOUR:
FOUR_1:  SETB   P1.0
         MOV    C,P1.0
         JC    FOUR_2
         MOV    59H,#0H
         LJMP   DEAL_END

FOUR_2:  SETB   P1.1
         MOV    C,P1.1
         JC    FOUR_3
         MOV    59H,#3H
         LJMP   DEAL_END
FOUR_3:  MOV    59H,#5H
         LJMP   DEAL_END

DEAL_END:

RET


