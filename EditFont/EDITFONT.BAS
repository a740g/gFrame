' VGA font editor
' Copyright (c) Samuel Gomes (Blade), 1999-2003
' mailto: blade_go@hotmail.com

'$INCLUDE: 'basx.bi'

DECLARE SUB ChangeFontHeight (BYVAL h AS INTEGER)
DECLARE FUNCTION ChooseCharacter () AS INTEGER
DECLARE FUNCTION DoCommandLine () AS INTEGER
DECLARE SUB DrawCharacter (BYVAL ch AS INTEGER, BYVAL sx AS INTEGER, BYVAL sy AS INTEGER)
DECLARE FUNCTION EditCharacter () AS INTEGER
DECLARE FUNCTION GetFileName (sFile AS STRING) AS STRING
DECLARE FUNCTION GetFilePath (sFile AS STRING) AS STRING
DECLARE FUNCTION GetFontXY (BYVAL c AS INTEGER, BYVAL x AS INTEGER, BYVAL y AS INTEGER) AS INTEGER
DECLARE FUNCTION LoadVGAFont () AS INTEGER
DECLARE SUB LoadVGAROMFont (BYVAL iFontCode AS INTEGER)
DECLARE SUB ReadFont (sFile AS STRING)
DECLARE FUNCTION SaveVGAFont () AS INTEGER
DECLARE SUB WriteFont (sFile AS STRING)
DECLARE SUB DrawCharBitmap (BYVAL ch AS INTEGER)
DECLARE SUB DrawCharSelector (BYVAL xp AS INTEGER, BYVAL yp AS INTEGER, BYVAL c AS INTEGER)
DECLARE SUB DrawBmpSelector (BYVAL xe AS INTEGER, BYVAL ye AS INTEGER, BYVAL c AS INTEGER)

' Font file ID
CONST FONT_ID = "FONT"

' Program events
CONST EVENT_QUIT = 0
CONST EVENT_SAVE = 1
CONST EVENT_LOAD = 2
CONST EVENT_CHOOSE = 3
CONST EVENT_EDIT = 4
CONST EVENT_COMMAND = 5

DIM iEvent AS INTEGER, iReturnCode AS INTEGER
DIM SHARED sFontFile AS STRING
DIM SHARED cFont(0 TO 255) AS STRING
DIM SHARED iFontHeight AS INTEGER
DIM SHARED iFontCharacter AS INTEGER
DIM SHARED bFontChanged AS INTEGER
DIM SHARED sClipboard AS STRING

' Check the command line
sFontFile = CommandLine()

' Check if any command line help is needed
IF (sFontFile = "/?") THEN
	PRINT
	PRINT "EDITFONT: VGA Font Editor. A DOS gFrame tool. Version 2.1"
	PRINT "Syntax: EDITFONT [fontfile[.fnt]]"
	PRINT
	PRINT "Copyright (c) Samuel Gomes (Blade), 1999-2003"
	PRINT "mailto: blade_go@hotmail.com || blade_g2000@yahoo.com"
	PRINT
	END ExitFailure
END IF

' Set screen attributes.
SCREEN.ControlPanel(7) = 0
SCREEN.ControlPanel(16) = 5

' Assume successful return
iReturnCode = ExitSuccess

' Default's to command line event on program entry
iEvent = EVENT_COMMAND

' Event loop
DO
	SELECT CASE (iEvent)
		CASE EVENT_COMMAND
			iEvent = DoCommandLine()

		CASE EVENT_LOAD
			iEvent = LoadVGAFont()

		CASE EVENT_SAVE
			iEvent = SaveVGAFont()

		CASE EVENT_CHOOSE
			iEvent = ChooseCharacter()

		CASE EVENT_EDIT
			iEvent = EditCharacter()

		CASE EVENT_QUIT
			IF (bFontChanged) THEN
				SELECT CASE (MSGBOX("The font has changed. Save it now?", MB_YESNOCANCEL, "Save Font"))
					CASE IDYES
						iEvent = SaveVGAFont()
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
			' Will never come here :(
			iReturnCode = ExitFailure
			EXIT DO
	END SELECT
LOOP

CLS
END iReturnCode

SUB ChangeFontHeight (BYVAL h AS INTEGER)
	DIM i AS INTEGER

	iFontHeight = h

	IF (LEN(sClipboard) <> iFontHeight) THEN
		sClipboard = STRING$(iFontHeight, 0)
	END IF

	FOR i = 0 TO 255
		cFont(i) = STRING$(iFontHeight, 0)
	NEXT
END SUB

FUNCTION ChooseCharacter () AS INTEGER
	STATIC xp AS INTEGER, yp AS INTEGER
	DIM x AS INTEGER, y AS INTEGER
	DIM sx AS INTEGER, sy AS INTEGER
	DIM ch AS INTEGER, in AS INTEGER
	DIM refTime AS SINGLE, blinkState AS INTEGER

	' Save the current time
	refTime = TIMER

	' Swith to graphics mode
	InitGraph (EGALO)

	SetBorderColor 7

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
		in = GetKey()

		SELECT CASE (in)
			CASE kbF1
				SCREEN , , 1, 1
				WaitKeyPress
				ClearKeyboardBuffer
				SCREEN , , 0, 0

			CASE kbLeft
				DrawCharSelector xp, yp, 0
				xp = xp - 1
				IF (xp < 1) THEN xp = 32
				DrawCharSelector xp, yp, 15

			CASE kbRight
				DrawCharSelector xp, yp, 0
				xp = xp + 1
				IF (xp > 32) THEN xp = 1
				DrawCharSelector xp, yp, 15

			CASE kbUp
				DrawCharSelector xp, yp, 0
				yp = yp - 1
				IF (yp < 1) THEN yp = 8
				DrawCharSelector xp, yp, 15

			CASE kbDown
				DrawCharSelector xp, yp, 0
				yp = yp + 1
				IF (yp > 8) THEN yp = 1
				DrawCharSelector xp, yp, 15

			CASE kbEnter
				iFontCharacter = (xp - 1) + (32 * (yp - 1))
				ChooseCharacter = EVENT_EDIT
				EXIT DO

			CASE kbF2
				ChooseCharacter = EVENT_SAVE
				EXIT DO

			CASE kbF3
				ChooseCharacter = EVENT_LOAD
				EXIT DO

			CASE kbEsc
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

				Yield
		END SELECT
	LOOP

	DoneGraph
END FUNCTION

FUNCTION DoCommandLine () AS INTEGER
	IF (sFontFile = sEmpty) THEN
		DoCommandLine = EVENT_LOAD
	ELSE
		' Adjust file name
		sFontFile = FExpand(ReplaceExtension(sFontFile, "fnt", False))
		IF NOT (FileExists(sFontFile)) THEN
			iFontHeight = 16
			DO
				iFontHeight = VAL(INPUTBOX$("Enter new font height in pixels (8 - 16):", "Font Height", STR$(iFontHeight)))
			LOOP WHILE (iFontHeight < 8 OR iFontHeight > 16)
			ChangeFontHeight (iFontHeight)
			LoadVGAROMFont iFontHeight      ' Load a default font from ROM
		ELSE
			' Read in the font
			ReadFont (sFontFile)
		END IF
		DoCommandLine = EVENT_CHOOSE
	END IF
END FUNCTION

SUB DrawBmpSelector (BYVAL xe AS INTEGER, BYVAL ye AS INTEGER, BYVAL c AS INTEGER)
	LINE (7 + (xe - 1) * 10, 7 + (ye - 1) * 10)-(17 + (xe - 1) * 10, 17 + (ye - 1) * 10), c, B
END SUB

SUB DrawCharacter (BYVAL ch AS INTEGER, BYVAL sx AS INTEGER, BYVAL sy AS INTEGER)
	DIM uy AS INTEGER, x AS INTEGER

	FOR uy = 1 TO iFontHeight
		x = ASC(StrChr(cFont(ch), uy))
		x = 256 * (x + (256 * (x > 127)))
		LINE (sx + 1, sy + uy)-(sx + 8, sy + uy), 1
		LINE (sx + 1, sy + uy)-(sx + 8, sy + uy), 14, , x
	NEXT
END SUB

SUB DrawCharBitmap (BYVAL ch AS INTEGER)
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

SUB DrawCharSelector (BYVAL xp AS INTEGER, BYVAL yp AS INTEGER, BYVAL c AS INTEGER)
	LINE ((xp - 1) * 10, (yp * 20))-(9 + (xp - 1) * 10, iFontHeight + 1 + (yp * 20)), c, B
END SUB

FUNCTION EditCharacter () AS INTEGER
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
	InitGraph (EGALO)

	SetBorderColor 7

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
		in = GetKey()

		SELECT CASE (in)
			CASE kbLeft
				DrawBmpSelector xe, ye, 0
				xe = xe - 1
				IF (xe < 1) THEN xe = 8
				DrawBmpSelector xe, ye, 15

			CASE kbRight
				DrawBmpSelector xe, ye, 0
				xe = xe + 1
				IF (xe > 8) THEN xe = 1
				DrawBmpSelector xe, ye, 15

			CASE kbUp
				DrawBmpSelector xe, ye, 0
				ye = ye - 1
				IF (ye < 1) THEN ye = iFontHeight
				DrawBmpSelector xe, ye, 15

			CASE kbDown
				DrawBmpSelector xe, ye, 0
				ye = ye + 1
				IF (ye > iFontHeight) THEN ye = 1
				DrawBmpSelector xe, ye, 15

			CASE kbSpaceBar
				' Flag font changed
				bFontChanged = True

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

			CASE kbCtrlV
				' Flag font changed
				bFontChanged = True

				v = cFont(iFontCharacter)
				FOR n = 1 TO iFontHeight
					MID$(cFont(iFontCharacter), n, 1) = MID$(v, 1 + iFontHeight - n, 1)
				NEXT
				DrawCharBitmap (iFontCharacter)

			CASE kbCtrlH
				' Flag font changed
				bFontChanged = True

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

			CASE kbCtrlI
				' Flag font changed
				bFontChanged = True

				v = cFont(iFontCharacter)
				FOR n = 1 TO iFontHeight
					MID$(cFont(iFontCharacter), n, 1) = CHR$(255 - ASC(MID$(v, n, 1)))
				NEXT
				DrawCharBitmap (iFontCharacter)

			CASE kbCtrlC
				sClipboard = cFont(iFontCharacter)

			CASE kbCtrlP
				' Flag font changed
				bFontChanged = True

				cFont(iFontCharacter) = sClipboard
				DrawCharBitmap (iFontCharacter)

			CASE kbDelete
				' Flag font changed
				bFontChanged = True

				cFont(iFontCharacter) = STRING$(iFontHeight, 0)
				DrawCharBitmap (iFontCharacter)

			CASE kbEsc
				cFont(iFontCharacter) = cpy
				EditCharacter = EVENT_CHOOSE
				EXIT DO

			CASE kbEnter
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

				Yield
		END SELECT
	LOOP

	DoneGraph
END FUNCTION

FUNCTION GetFileName (sFile AS STRING) AS STRING
	DIM drv AS STRING, pth AS STRING
	DIM nam AS STRING, ext AS STRING

	FSplit sFile, drv, pth, nam, ext

	GetFileName = nam + ext
END FUNCTION

FUNCTION GetFilePath (sFile AS STRING) AS STRING
	DIM drv AS STRING, pth AS STRING
	DIM nam AS STRING, ext AS STRING

	FSplit sFile, drv, pth, nam, ext

	GetFilePath = drv + pth
END FUNCTION

FUNCTION GetFontXY (BYVAL c AS INTEGER, BYVAL x AS INTEGER, BYVAL y AS INTEGER) AS INTEGER
	DIM xv AS INTEGER, B AS INTEGER

	xv = 2 ^ (8 - x)
	B = ASC(StrChr(cFont(c), y))
	GetFontXY = SGN(B AND xv)
END FUNCTION

FUNCTION LoadVGAFont () AS INTEGER
	DIM sFile AS STRING, sPath AS STRING, bCancel AS INTEGER

	sFile = GetFileName(sFontFile)
	sPath = GetFilePath(sFontFile)

	' Get and existing font file name from the user
	FileOpen sFile, sPath, "*.fnt", "Open Font", BLACK, WHITE, 0, bCancel

	' Exit if user canceled
	IF (bCancel) THEN
		' Exit if 1st time
		IF (iFontHeight = 0) THEN
			LoadVGAFont = EVENT_QUIT
		ELSE
			LoadVGAFont = EVENT_CHOOSE
		END IF
		EXIT FUNCTION
	END IF

	sFontFile = sPath + "\" + sFile

	' Read in the font
	ReadFont (sFontFile)

	bFontChanged = False

	LoadVGAFont = EVENT_CHOOSE
END FUNCTION

SUB LoadVGAROMFont (BYVAL iFontCode AS INTEGER)
	DIM Regs AS RegTypeX
	DIM i AS INTEGER, j AS INTEGER

	Regs.AX = &H1130

	SELECT CASE iFontCode
		CASE IS = 8
			Regs.BX = &H300
			InterruptX &H10, Regs, Regs
		CASE IS = 16
			Regs.BX = &H600
			InterruptX &H10, Regs, Regs
	END SELECT

	IF (iFontCode = 8 OR iFontCode = 16) THEN
		DEF SEG = Regs.ES

		FOR i = 0 TO 255
			cFont(i) = ""
			FOR j = 1 TO iFontCode  ' height
				cFont(i) = cFont(i) + CHR$(PEEK(Regs.BP))
				Regs.BP = Regs.BP + 1   ' increment pointer
			NEXT
		NEXT

		DEF SEG

		iFontHeight = iFontCode
	END IF
END SUB

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

FUNCTION SaveVGAFont () AS INTEGER
	DIM sFile AS STRING, sPath AS STRING, bCancel AS INTEGER

	sFile = GetFileName(sFontFile)
	sPath = GetFilePath(sFontFile)

	' Get and existing font file name from the user
	FileSave sFile, sPath, "*.fnt", "Save Font", BLACK, WHITE, 0, bCancel

	SaveVGAFont = EVENT_CHOOSE

	' Exit if user canceled
	IF (bCancel) THEN EXIT FUNCTION

	sFontFile = sPath + "\" + sFile

	' Read in the font
	WriteFont (sFontFile)

	bFontChanged = False
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

