COPY START(0),LENGTH(0) TO $F200

CHAR 64,(0,1,0,0,0)
BG SOURCE $9000,32
M=0
DO
  IF TOUCH THEN
    IF M=0 THEN CELL TOUCH.X/8,TOUCH.Y/8
    IF M=1 THEN BG COPY 2,3,4,3 TO TOUCH.X/8,TOUCH.Y/8
  END IF
  I$=INKEY$
  IF I$="D" THEN CLS
  IF I$="C" THEN CHAR 64+RND*64,
  IF I$="A" THEN CHAR ,(RND*8,1,,,)
  IF I$="M" THEN
    IF M=0 THEN M=1 ELSE M=0
  END IF
  CELL 0,0
  NUMBER 1,0,M,1
  WAIT VBL
LOOP


#0: COLOR PALETTES
07 10 24 38 00 04 08 0C 00 07 0B 0F 00 1B 2F 3F 00 34 38 3C 00 10 20 30 00 10 30 39 00 00 15 3F