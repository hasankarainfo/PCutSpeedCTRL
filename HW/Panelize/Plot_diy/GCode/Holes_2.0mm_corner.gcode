; This file was generated with "Universal Gcode Sender 2.1.13"
;
; Tool: 2.99mm
; Depth per pass: 2.0mm
; Plunge speed: 400mm/min
; Safe height: 10.0mm
; Tool step over: 0.9mm

G21 ; millimeters
G90 ; absolute coordinate
G17 ; XY plane
G94 ; units per minute feed rate mode

; Ellipse - Center drill (1/4)
G0 Z10
G0 X197Y-3
G0 Z0
M3 S11700
G1 F400 Z0
G1 F400 Z-2
G0 Z0
G1 F400 Z-4
G0 Z0
G0 Z10
; Ellipse - Center drill (2/4)
G0 Z10
G0 X197Y-97
G0 Z0
M3 S11700
G1 F400 Z0
G1 F400 Z-2
G0 Z0
G1 F400 Z-4
G0 Z0
G0 Z10
; Ellipse - Center drill (3/4)
G0 Z10
G0 X3Y-3
G0 Z0
M3 S11700
G1 F400 Z0
G1 F400 Z-2
G0 Z0
G1 F400 Z-4
G0 Z0
G0 Z10
; Ellipse - Center drill (4/4)
G0 Z10
G0 X3Y-97
G0 Z0
M3 S11700
G1 F400 Z0
G1 F400 Z-2
G0 Z0
G1 F400 Z-4
G0 Z0
G0 Z10

; Turning off spindle
M5
