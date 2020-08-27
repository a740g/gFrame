' VGA font editor
' Copyright (c) Samuel Gomes, 2020
' Updated to complie with QB64 on Windows

OPTION _EXPLICIT
OPTION BASE 0

DECLARE DYNAMIC LIBRARY "user32"
    FUNCTION MessageBoxA& (BYVAL hwnd AS LONG, Message AS STRING, Title AS STRING, BYVAL MBType AS _UNSIGNED LONG)
END DECLARE

CONST FALSE% = 0
CONST TRUE% = NOT FALSE

CONST NULLSTRING = ""

'Message Box Constant values as defined by Microsoft (MBType)
CONST MB_OK& = 0 'OK button only
CONST MB_OKCANCEL& = 1 'OK & Cancel
CONST MB_ABORTRETRYIGNORE& = 2 'Abort, Retry & Ignore
CONST MB_YESNOCANCEL& = 3 'Yes, No & Cancel
CONST MB_YESNO& = 4 'Yes & No
CONST MB_RETRYCANCEL& = 5 'Retry & Cancel
CONST MB_CANCELTRYCONTINUE& = 6 'Cancel, Try Again & Continue
CONST MB_ICONSTOP& = 16 'Error stop sign icon
CONST MB_ICONQUESTION& = 32 'Question-mark icon
CONST MB_ICONEXCLAMATION& = 48 'Exclamation-point icon
CONST MB_ICONINFORMATION& = 64 'Letter i in a circle icon
CONST MB_DEFBUTTON1& = 0 '1st button default(left)
CONST MB_DEFBUTTON2& = 256 '2nd button default
CONST MB_DEFBUTTON3& = 512 '3rd button default(right)
CONST MB_APPLMODAL& = 0 'Message box applies to application only
CONST MB_SYSTEMMODAL& = 4096 'Message box on top of all other windows
CONST MB_SETFOCUS& = 65536 'Set message box as focus
CONST IDOK& = 1 'OK button pressed
CONST IDCANCEL& = 2 'Cancel button pressed
CONST IDABORT& = 3 'Abort button pressed
CONST IDRETRY& = 4 'Retry button pressed
CONST IDIGNORE& = 5 'Ignore button pressed
CONST IDYES& = 6 'Yes button pressed
CONST IDNO& = 7 'No button pressed
CONST IDTRYAGAIN& = 10 'Try again button pressed
CONST IDCONTINUE& = 1 'Continue button pressed

' Keyboard codes
CONST KBNOKEY% = 0
CONST KBESC% = 27
CONST KBBACKSPACE% = 8
CONST KBENTER% = 13
CONST KBTAB% = 9
CONST KBF1% = 315
CONST KBF2% = 316
CONST KBF3% = 317
CONST KBF4% = 318
CONST KBF5% = 319
CONST KBF6% = 320
CONST KBF7% = 321
CONST KBF8% = 322
CONST KBF9% = 323
CONST KBF10% = 324
CONST KBF11% = 389
CONST KBF12% = 390
CONST KBUP% = 328
CONST KBDOWN% = 336
CONST KBLEFT% = 331
CONST KBRIGHT% = 333
CONST KBINSERT% = 338
CONST KBDELETE% = 339
CONST KBHOME% = 327
CONST KBEND% = 335
CONST KBPAGEUP% = 329
CONST KBPAGEDOWN% = 337
CONST KBSHIFTTAB% = 271
CONST KBCTRLA% = 1
CONST KBCTRLB% = 2
CONST KBCTRLC% = 3
CONST KBCTRLD% = 4
CONST KBCTRLE% = 5
CONST KBCTRLF% = 6
CONST KBCTRLG% = 7
CONST KBCTRLH% = 8
CONST KBCTRLI% = 9
CONST KBCTRLJ% = 10
CONST KBCTRLK% = 11
CONST KBCTRLL% = 12
CONST KBCTRLM% = 13
CONST KBCTRLN% = 14
CONST KBCTRLO% = 15
CONST KBCTRLP% = 16
CONST KBCTRLQ% = 17
CONST KBCTRLR% = 18
CONST KBCTRLS% = 19
CONST KBCTRLT% = 20
CONST KBCTRLU% = 21
CONST KBCTRLV% = 22
CONST KBCTRLW% = 23
CONST KBCTRLX% = 24
CONST KBCTRLY% = 25
CONST KBCTRLZ% = 26
CONST KBCTRLENTER% = 10
CONST KBSPACEBAR% = 32
CONST KBCTRLBACKSPACE% = 127
CONST KBCTRLF1% = 350
CONST KBCTRLF2% = 351
CONST KBCTRLF3% = 352
CONST KBCTRLF4% = 353
CONST KBCTRLF5% = 354
CONST KBCTRLF6% = 355
CONST KBCTRLF7% = 356
CONST KBCTRLF8% = 357
CONST KBCTRLF9% = 358
CONST KBCTRLF10% = 359
CONST KBCTRLF11% = 393
CONST KBCTRLF12% = 394
CONST KBSHIFTF1% = 340
CONST KBSHIFTF2% = 341
CONST KBSHIFTF3% = 342
CONST KBSHIFTF4% = 343
CONST KBSHIFTF5% = 344
CONST KBSHIFTF6% = 345
CONST KBSHIFTF7% = 346
CONST KBSHIFTF8% = 347
CONST KBSHIFTF9% = 348
CONST KBSHIFTF10% = 349
CONST KBSHIFTF11% = 391
CONST KBSHIFTF12% = 392
CONST KBALTF1% = 360
CONST KBALTF2% = 361
CONST KBALTF3% = 362
CONST KBALTF4% = 363
CONST KBALTF5% = 364
CONST KBALTF6% = 365
CONST KBALTF7% = 366
CONST KBALTF8% = 367
CONST KBALTF9% = 368
CONST KBALTF10% = 369
CONST KBALTF11% = 395
CONST KBALTF12% = 396
CONST KBALTMINUS% = 386
CONST KBALTPLUS% = 387
CONST KBALT1% = 376
CONST KBALT2% = 377
CONST KBALT3% = 378
CONST KBALT4% = 379
CONST KBALT5% = 380
CONST KBALT6% = 381
CONST KBALT7% = 382
CONST KBALT8% = 383
CONST KBALT9% = 384
CONST KBALT0% = 385
CONST KBALTA% = 286
CONST KBALTB% = 304
CONST KBALTC% = 302
CONST KBALTD% = 288
CONST KBALTE% = 274
CONST KBALTF% = 289
CONST KBALTG% = 290
CONST KBALTH% = 291
CONST KBALTI% = 279
CONST KBALTJ% = 292
CONST KBALTK% = 293
CONST KBALTL% = 294
CONST KBALTM% = 306
CONST KBALTN% = 305
CONST KBALTO% = 280
CONST KBALTP% = 281
CONST KBALTQ% = 272
CONST KBALTR% = 275
CONST KBALTS% = 287
CONST KBALTT% = 276
CONST KBALTU% = 278
CONST KBALTV% = 303
CONST KBALTW% = 273
CONST KBALTX% = 301
CONST KBALTY% = 277
CONST KBALTZ% = 300

' Font file ID
CONST FONT_ID = "FONT"

' Program events
CONST EVENT_QUIT% = 0
CONST EVENT_SAVE% = 1
CONST EVENT_LOAD% = 2
CONST EVENT_CHOOSE% = 3
CONST EVENT_EDIT% = 4
CONST EVENT_COMMAND% = 5

DIM iEvent AS INTEGER
DIM SHARED sFontFile AS STRING
DIM SHARED cFont(0 TO 255) AS STRING
DIM SHARED iFontHeight AS INTEGER
DIM SHARED iFontCharacter AS INTEGER
DIM SHARED bFontChanged AS INTEGER
DIM SHARED sClipboard AS STRING

_CONSOLETITLE "VGA Font Editor"

' Check the command line
sFontFile = COMMAND$

' Check if any command line help is needed
IF (sFontFile = "/?") THEN
    PRINT
    PRINT "EDITFONT: VGA Font Editor. Version 3.0"
    PRINT "Syntax: EDITFONT [fontfile.fnt]"
    PRINT
    PRINT "Copyright (c) Samuel Gomes, 2020"
    PRINT "https://github.com/a740g/"
    PRINT
    END
END IF

_TITLE "VGA Font Editor"
_FULLSCREEN

' Default's to command line event on program entry
iEvent = EVENT_COMMAND

' Event loop
DO
    SELECT CASE (iEvent)
        CASE EVENT_COMMAND
            iEvent = DoCommandLine

        CASE EVENT_LOAD
            iEvent = LoadVGAFont

        CASE EVENT_SAVE
            iEvent = SaveVGAFont

        CASE EVENT_CHOOSE
            iEvent = ChooseCharacter

        CASE EVENT_EDIT
            iEvent = EditCharacter

        CASE EVENT_QUIT
            IF (bFontChanged) THEN
                SELECT CASE (MsgBox("The font has changed. Save it now?", "Save Font", MB_YESNOCANCEL + MB_APPLMODAL + IDYES))
                    CASE IDYES
                        iEvent = SaveVGAFont
                        EXIT DO

                    CASE IDNO
                        EXIT DO

                    CASE ELSE
                        iEvent = EVENT_CHOOSE
                END SELECT
            ELSE
                EXIT DO
            END IF

        CASE ELSE
            Abort "Unhandled program event!"
            EXIT DO
    END SELECT
LOOP

END

SUB ChangeFontHeight (h AS INTEGER)
    DIM i AS INTEGER

    iFontHeight = h

    IF (LEN(sClipboard) <> iFontHeight) THEN
        sClipboard = STRING$(iFontHeight, 0)
    END IF

    FOR i = 0 TO 255
        cFont(i) = STRING$(iFontHeight, 0)
    NEXT
END SUB

FUNCTION ChooseCharacter% ()
    STATIC xp AS INTEGER, yp AS INTEGER
    DIM x AS INTEGER, y AS INTEGER
    DIM sx AS INTEGER, sy AS INTEGER
    DIM ch AS INTEGER, in AS INTEGER
    DIM refTime AS SINGLE, blinkState AS INTEGER

    ' Save the current time
    refTime = TIMER

    ' Swith to graphics mode
    SCREEN 7

    ' Show some info
    COLOR 4
    PRINT "Select a character to edit:"
    LOCATE 25, 1
    PRINT "Press 'F1' for help.";

    ' Draw the selector at the initial position
    IF (xp = 0 OR yp = 0) THEN
        xp = 1
        yp = 1
    END IF
    DrawCharSelector xp, yp, 15

    ' Draw the characters
    FOR y = 1 TO 8
        FOR x = 1 TO 32
            sx = (x - 1) * 10
            sy = y * 20
            DrawCharacter ch, sx, sy
            ch = ch + 1
        NEXT
    NEXT

    ' Also draw the help text on a different page
    SCREEN , , 1, 0
    COLOR 4
    PRINT "Quick help:"
    PRINT
    COLOR 15
    PRINT "Keys:"
    PRINT " Left    -   Move left"
    PRINT " Right   -   Move right"
    PRINT " Up      -   Move up"
    PRINT " Down    -   Move down"
    PRINT " Enter   -   Edit character"
    PRINT " F1      -   Show help"
    PRINT " F2      -   Save font"
    PRINT " F3      -   Load font"
    PRINT " ESC     -   Quit"
    PRINT
    COLOR 4
    PRINT "Press any key to continue..."
    SCREEN , , 0, 0

    DO
        in = GetKey

        SELECT CASE (in)
            CASE KBF1
                SCREEN , , 1, 1
                WaitKeyPress
                'ClearKeyboardBuffer
                SCREEN , , 0, 0

            CASE KBLEFT
                DrawCharSelector xp, yp, 0
                xp = xp - 1
                IF (xp < 1) THEN xp = 32
                DrawCharSelector xp, yp, 15

            CASE KBRIGHT
                DrawCharSelector xp, yp, 0
                xp = xp + 1
                IF (xp > 32) THEN xp = 1
                DrawCharSelector xp, yp, 15

            CASE KBUP
                DrawCharSelector xp, yp, 0
                yp = yp - 1
                IF (yp < 1) THEN yp = 8
                DrawCharSelector xp, yp, 15

            CASE KBDOWN
                DrawCharSelector xp, yp, 0
                yp = yp + 1
                IF (yp > 8) THEN yp = 1
                DrawCharSelector xp, yp, 15

            CASE KBENTER
                iFontCharacter = (xp - 1) + (32 * (yp - 1))
                ChooseCharacter = EVENT_EDIT
                EXIT DO

            CASE KBF2
                ChooseCharacter = EVENT_SAVE
                EXIT DO

            CASE KBF3
                ChooseCharacter = EVENT_LOAD
                EXIT DO

            CASE KBESC
                ChooseCharacter = EVENT_QUIT
                EXIT DO

            CASE ELSE
                ' Blink the selector at regular intervals
                IF ((TIMER - refTime) > .3) THEN
                    refTime = TIMER
                    blinkState = NOT blinkState

                    IF (blinkState) THEN
                        DrawCharSelector xp, yp, 15
                    ELSE
                        DrawCharSelector xp, yp, 0
                    END IF
                END IF

                SLEEP 1
        END SELECT
    LOOP

    SCREEN 0
END FUNCTION

FUNCTION DoCommandLine% ()
    IF (sFontFile = NULLSTRING) THEN
        DoCommandLine% = EVENT_LOAD
    ELSE
        IF NOT (_FILEEXISTS(sFontFile)) THEN
            iFontHeight = 16
            DO
                INPUT ; "Enter new font height in pixels (8 - 16): ", iFontHeight
            LOOP WHILE (iFontHeight < 8 OR iFontHeight > 16)
            ChangeFontHeight (iFontHeight)
            ReadFont "default.fnt"
        ELSE
            ' Read in the font
            ReadFont (sFontFile)
        END IF
        DoCommandLine% = EVENT_CHOOSE
    END IF
END FUNCTION

SUB DrawBmpSelector (xe AS INTEGER, ye AS INTEGER, c AS INTEGER)
    LINE (7 + (xe - 1) * 10, 7 + (ye - 1) * 10)-(17 + (xe - 1) * 10, 17 + (ye - 1) * 10), c, B
END SUB

SUB DrawCharacter (ch AS INTEGER, sx AS INTEGER, sy AS INTEGER)
    DIM uy AS INTEGER, x AS INTEGER

    FOR uy = 1 TO iFontHeight
        x = ASC(cFont(ch), uy)
        x = 256 * (x + (256 * (x > 127)))
        LINE (sx + 1, sy + uy)-(sx + 8, sy + uy), 1
        LINE (sx + 1, sy + uy)-(sx + 8, sy + uy), 14, , x
    NEXT
END SUB

SUB DrawCharBitmap (ch AS INTEGER)
    DIM x AS INTEGER, y AS INTEGER
    DIM xs AS INTEGER, ys AS INTEGER

    FOR y = 1 TO iFontHeight
        FOR x = 1 TO 8
            xs = 8 + (x - 1) * 10
            ys = 8 + (y - 1) * 10
            IF (GetFontXY(ch, x, y)) THEN
                LINE (xs, ys)-(xs + 8, ys + 8), 14, BF
            ELSE
                LINE (xs, ys)-(xs + 8, ys + 8), 1, BF
            END IF
        NEXT
    NEXT
END SUB

SUB DrawCharSelector (xp AS INTEGER, yp AS INTEGER, c AS INTEGER)
    LINE ((xp - 1) * 10, (yp * 20))-(9 + (xp - 1) * 10, iFontHeight + 1 + (yp * 20)), c, B
END SUB

FUNCTION EditCharacter% ()
    DIM xe AS INTEGER, ye AS INTEGER
    DIM xs AS INTEGER, ys AS INTEGER
    DIM in AS INTEGER, xv AS INTEGER
    DIM x AS INTEGER, y AS INTEGER
    DIM v AS STRING, n AS INTEGER
    DIM cpy AS STRING
    DIM refTime AS SINGLE, blinkState AS INTEGER

    ' Save the current time
    refTime = TIMER

    ' Switch to graphics mode
    SCREEN 7

    ' Display some help
    COLOR 4
    LOCATE 2, 15: PRINT "Keys:"
    COLOR 15
    LOCATE 3, 15: PRINT " Left   - Move left"
    LOCATE 4, 15: PRINT " Right  - Move right"
    LOCATE 5, 15: PRINT " Up     - Move up"
    LOCATE 6, 15: PRINT " Down   - Move down"
    LOCATE 7, 15: PRINT " Space  - Toggle pixel"
    LOCATE 8, 15: PRINT " Enter  - Return"
    LOCATE 9, 15: PRINT " Esc    - Cancel"
    LOCATE 10, 15: PRINT " Ctrl+H - Flip horizontal"
    LOCATE 11, 15: PRINT " Ctrl+V - Flip vertical"
    LOCATE 12, 15: PRINT " Ctrl+I - Invert"
    LOCATE 13, 15: PRINT " Ctrl+C - Copy"
    LOCATE 14, 15: PRINT " Ctrl+P - Paste"
    LOCATE 15, 15: PRINT " Del    - Clear"

    ' Save a copy of this character
    cpy = cFont(iFontCharacter)

    ' Draw the initial bitmap
    DrawCharBitmap (iFontCharacter)

    xe = 1
    ye = 1
    DrawBmpSelector xe, ye, 15

    DO
        in = GetKey

        SELECT CASE (in)
            CASE KBLEFT
                DrawBmpSelector xe, ye, 0
                xe = xe - 1
                IF (xe < 1) THEN xe = 8
                DrawBmpSelector xe, ye, 15

            CASE KBRIGHT
                DrawBmpSelector xe, ye, 0
                xe = xe + 1
                IF (xe > 8) THEN xe = 1
                DrawBmpSelector xe, ye, 15

            CASE KBUP
                DrawBmpSelector xe, ye, 0
                ye = ye - 1
                IF (ye < 1) THEN ye = iFontHeight
                DrawBmpSelector xe, ye, 15

            CASE KBDOWN
                DrawBmpSelector xe, ye, 0
                ye = ye + 1
                IF (ye > iFontHeight) THEN ye = 1
                DrawBmpSelector xe, ye, 15

            CASE KBSPACEBAR
                ' Flag font changed
                bFontChanged = TRUE

                xv = 2 ^ (8 - xe)
                IF (GetFontXY(iFontCharacter, xe, ye)) THEN
                    MID$(cFont(iFontCharacter), ye, 1) = CHR$(ASC(MID$(cFont(iFontCharacter), ye, 1)) - xv)
                ELSE
                    MID$(cFont(iFontCharacter), ye, 1) = CHR$(ASC(MID$(cFont(iFontCharacter), ye, 1)) + xv)
                END IF
                xs = 8 + (xe - 1) * 10
                ys = 8 + (ye - 1) * 10
                IF (GetFontXY(iFontCharacter, xe, ye)) THEN
                    LINE (xs, ys)-(xs + 8, ys + 8), 14, BF
                ELSE
                    LINE (xs, ys)-(xs + 8, ys + 8), 1, BF
                END IF

            CASE KBCTRLV
                ' Flag font changed
                bFontChanged = TRUE

                v = cFont(iFontCharacter)
                FOR n = 1 TO iFontHeight
                    MID$(cFont(iFontCharacter), n, 1) = MID$(v, 1 + iFontHeight - n, 1)
                NEXT
                DrawCharBitmap (iFontCharacter)

            CASE KBCTRLH
                ' Flag font changed
                bFontChanged = TRUE

                v = STRING$(iFontHeight, 0)
                FOR y = 1 TO iFontHeight
                    FOR x = 1 TO 8
                        xv = 2 ^ (x - 1)
                        IF (GetFontXY(iFontCharacter, x, y)) THEN
                            MID$(v, y, 1) = CHR$(ASC(MID$(v, y, 1)) + xv)
                        END IF
                    NEXT
                NEXT
                cFont(iFontCharacter) = v
                DrawCharBitmap (iFontCharacter)

            CASE KBCTRLI
                ' Flag font changed
                bFontChanged = TRUE

                v = cFont(iFontCharacter)
                FOR n = 1 TO iFontHeight
                    MID$(cFont(iFontCharacter), n, 1) = CHR$(255 - ASC(MID$(v, n, 1)))
                NEXT
                DrawCharBitmap (iFontCharacter)

            CASE KBCTRLC
                sClipboard = cFont(iFontCharacter)

            CASE KBCTRLP
                ' Flag font changed
                bFontChanged = TRUE

                cFont(iFontCharacter) = sClipboard
                DrawCharBitmap (iFontCharacter)

            CASE KBDELETE
                ' Flag font changed
                bFontChanged = TRUE

                cFont(iFontCharacter) = STRING$(iFontHeight, 0)
                DrawCharBitmap (iFontCharacter)

            CASE KBESC
                cFont(iFontCharacter) = cpy
                EditCharacter = EVENT_CHOOSE
                EXIT DO

            CASE KBENTER
                EditCharacter = EVENT_CHOOSE
                EXIT DO

            CASE ELSE
                ' Blink the selector at regular intervals
                IF ((TIMER - refTime) > .3) THEN
                    refTime = TIMER
                    blinkState = NOT blinkState

                    IF (blinkState) THEN
                        DrawBmpSelector xe, ye, 15
                    ELSE
                        DrawBmpSelector xe, ye, 0
                    END IF
                END IF

                SLEEP 1
        END SELECT
    LOOP

    SCREEN 0
END FUNCTION

FUNCTION GetFontXY% (c AS INTEGER, x AS INTEGER, y AS INTEGER)
    DIM xv AS INTEGER, B AS INTEGER

    xv = 2 ^ (8 - x)
    B = ASC(cFont(c), y)
    GetFontXY = SGN(B AND xv)
END FUNCTION

FUNCTION LoadVGAFont% ()
    ' Get an existing font file name from the user
    INPUT ; "Open Font: ", sFontFile

    ' Exit if user canceled
    IF (sFontFile = NULLSTRING OR NOT _FILEEXISTS(sFontFile)) THEN
        ' Exit if 1st time
        IF (iFontHeight = 0) THEN
            LoadVGAFont = EVENT_QUIT
        ELSE
            LoadVGAFont = EVENT_CHOOSE
        END IF
        EXIT FUNCTION
    END IF

    ' Read in the font
    ReadFont (sFontFile)

    bFontChanged = FALSE

    LoadVGAFont = EVENT_CHOOSE
END FUNCTION

SUB ReadFont (sFile AS STRING)
    DIM hFile AS INTEGER, i AS INTEGER
    DIM sId AS STRING * 4, sFntHgt AS STRING * 1

    hFile = FREEFILE
    OPEN sFile FOR BINARY ACCESS READ AS hFile

    ' Check font id
    GET hFile, , sId
    IF (sId <> FONT_ID) THEN
        CLOSE hFile
        EXIT SUB
    END IF

    ' Check font height
    GET hFile, , sFntHgt
    IF (ASC(sFntHgt) = 0) THEN
        CLOSE hFile
        EXIT SUB
    END IF
    ChangeFontHeight ASC(sFntHgt)

    FOR i = 0 TO 255
        GET hFile, , cFont(i)
    NEXT

    CLOSE hFile
END SUB

FUNCTION SaveVGAFont% ()

    ' Get an existing font file name from the user
    INPUT ; "Save Font: ", sFontFile

    SaveVGAFont = EVENT_CHOOSE

    ' Exit if user canceled
    IF (sFontFile = NULLSTRING) THEN EXIT FUNCTION

    ' Read in the font
    WriteFont (sFontFile)

    bFontChanged = FALSE
END FUNCTION

SUB WriteFont (sFile AS STRING)
    DIM hFile AS INTEGER, i AS INTEGER
    DIM sId AS STRING * 4, sFntHgt AS STRING * 1

    hFile = FREEFILE
    OPEN sFile FOR BINARY ACCESS WRITE AS hFile

    ' Write font id
    sId = FONT_ID
    PUT hFile, , sId

    ' Write font height
    sFntHgt = CHR$(iFontHeight)
    PUT hFile, , sFntHgt

    FOR i = 0 TO 255
        PUT hFile, , cFont(i)
    NEXT

    CLOSE hFile
END SUB

FUNCTION MsgBox& (Message AS STRING, Title AS STRING, BoxType AS LONG)
    MsgBox& = MessageBoxA&(_WINDOWHANDLE, Message, Title, BoxType)
END FUNCTION

SUB Abort (sMessage AS STRING)
    DIM i AS LONG

    i = MsgBox(sMessage, "Abort", MB_OK + MB_ICONSTOP)
END SUB

FUNCTION GetKey% ()
    DIM k AS STRING, i AS INTEGER

    k = INKEY$
    i = LEN(k)

    IF i = 2 THEN
        GetKey% = 256 + ASC(RIGHT$(k, 1))
    ELSEIF i = 1 THEN
        GetKey% = ASC(k)
    ELSE
        GetKey% = KBNOKEY
    END IF
END FUNCTION

SUB WaitKeyPress
    DO
        SLEEP
    LOOP UNTIL GetKey <> KBNOKEY
END SUB

