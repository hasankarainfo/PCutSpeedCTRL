# kikit install cmd: pip install kikit --break-system-packages
# cmd: python3 panelize_multiple.py


from kikit import panelize_ui_impl as ki
from kikit.units import mm, deg
from kikit.panelize import Panel, BasicGridPosition, Origin
from pcbnewTransition.pcbnew import LoadBoard, VECTOR2I
from pcbnewTransition.pcbnew import EDA_ANGLE
from pcbnewTransition import pcbnew
from itertools import chain


################ Panel Config

# columns and rows quantity
rows = 2
cols = 4

# columns and rows distances, unit mili meter
row_dist = 3.0
col_dist = 3.0
# Edge cuts line width unit mili meter
line_width = 0.05

# unit degrees
angle = 0

# main pcb file path
file = '../PCutSpeedCTRL.kicad_pcb'
output_path = 'panelize.kicad_pcb'


################ KiKit Panel Config (Only deviations from default)
cuts = {
        "type": "mousebites",
        "drill": "0.8mm",
        "spacing": "1.4mm",
        "offset": "-0.1mm",
        "prolong": "-1mm"

    }
cuts__ = {
        "type": "vcuts",
        "offset": "0.00mm",
        "cutcurves": "True",
        "layer": "User.1"

    }
cuts_ = {
        "type": "none"
    }

framing= {
        "type": "frame",
        "hspace": "3mm",
        "vspace": "3mm",
        "width": "3mm",
        "mintotalheight": "100mm",
        "mintotalwidth": "200mm",
        "maxtotalheight": "10m",
        "maxtotalwidth": "10m",
        "slotwidth": "3mm",
        "chamferwidth": "0mm",
        "chamferheight": "0mm",
        "fillet": "0mm"
    }
framing_ = {
        "type": "tightframe"
    }

tabs__ = { #Add tabs between board and board as well as board and rail
		"type":"fixed"
	}

tabs_ = { #Add tabs between board and board as well as board and rail
		"type":"spacing", #Place them with constant width and spacing
		"spacing" : "1mm",
		"vwidth": "1mm",
        "hwidth": "1mm",
        "fillet": "0mm"
	}

tabs = { #Add tabs between board and board as well as board and rail
		"type": "fixed",
        "vwidth": "5mm",
        "hwidth": "5mm",
        "mindistance": "3mm",
        "vcount": "1",
        "hcount": "1",
        "cutout": "1mm",
        "patchcorners": "True",
        "fillet": "1.5mm"
	}

tooling = {
        "type": "4hole",
        "hoffset": "3mm",
        "voffset": "3.0mm"
    }

fiducials = {
        "type": "none",
        #"type": "4fid",
        "hoffset": "2.5mm",
        "voffset": "2.5mm",
        "opening": "2.5mm"
    }
post =  {
    "millradius": "0.5mm",
     "copperfill": "True"
    }

copperfill = {
    "type": "solid",
    }

# Obtain full config by combining above with default
preset = ki.obtainPreset([], 
                         tabs=tabs,
                         cuts=cuts,
                         framing=framing,
                         tooling=tooling ,
                         post=post,
                         fiducials=fiducials, 
                         copperfill = copperfill)

from typing import NamedTuple


panel = Panel(output_path)

netRenamer = lambda x, y: "Board_{n}-{orig}".format(n=x, orig=y)
#refRenamer = lambda x, y: "Board_{n}-{orig}".format(n=x, orig=y)
refRenamer = lambda x, y: "{orig}".format(n=x, orig=y)
placer = BasicGridPosition(0, 0) #HorSpace, VerSpace
substrateCount = len(panel.substrates) # Store number of previous boards (probably 0)


class tBoard():
    board: object

    def __init__(self,x, y, a, f):
        self.board = LoadBoard(f)
        self.sarea = ki.readSourceArea(preset["source"], self.board)
        self.aarea = panel.appendBoard(f ,  
                                       destination= VECTOR2I(int(x),int(y)), 
                                       origin=Origin.TopLeft, sourceArea=self.sarea, 
                                       netRenamer=netRenamer, refRenamer=refRenamer, 
                                       rotationAngle = EDA_ANGLE(a) ,  
                                       inheritDrc=False)
        self.bb = panel.board.ComputeBoundingBox(True)
        

Boards = []
Boards.append( tBoard(0,0,angle,file))
_mm = 1000000
if ( True):
    boardbbox = Boards[0].board.ComputeBoundingBox(True)
    boardbbox = boardbbox.GetBoundingBoxRotated( VECTOR2I(0,0), EDA_ANGLE(angle)) 
    #print(boardbbox.GetWidth(), boardbbox.GetHeight(), boardbbox.GetX(), boardbbox.GetY())
    boardxl = boardbbox.GetWidth() - 2*int(line_width * _mm)
    boardyl = boardbbox.GetHeight()- 2*int(line_width * _mm)
    print(boardxl,boardyl)

    for i in range(rows):
        for j in range(cols):
           if(i+j!=0) : Boards.append( tBoard((boardxl + col_dist*_mm)*j, 
                                              (boardyl + row_dist*_mm)*i, 
                                              angle, 
                                              file))
        
    ################ Adjusted 
    # Prepare		

    panel.inheritDesignSettings(Boards[0].board)
    panel.inheritProperties(Boards[0].board)
    panel.inheritTitleBlock(Boards[0].board)


    substrates = panel.substrates[substrateCount:] # Collect set of newly added boards

    # Prepare frame and partition
    framingSubstrates = ki.dummyFramingSubstrate(substrates, preset)
    panel.buildPartitionLineFromBB(framingSubstrates)
    backboneCuts = ki.buildBackBone(preset["layout"], panel, substrates, preset)


    ######## --------------------- Continue doPanelization

    tabCuts = ki.buildTabs(preset, panel, substrates, framingSubstrates)

    frameCuts = ki.buildFraming(preset, panel)


    ki.buildTooling(preset, panel)
    ki.buildFiducials(preset, panel)
    for textSection in ["text", "text2", "text3", "text4"]:
        ki.buildText(preset[textSection], panel)
    ki.buildPostprocessing(preset["post"], panel)

    ki.makeTabCuts(preset, panel, tabCuts)
    ki.makeOtherCuts(preset, panel, chain(backboneCuts, frameCuts))


    ki.buildCopperfill(preset["copperfill"], panel)

    ki.setStackup(preset["source"], panel)
    ki.setPageSize(preset["page"], panel, Boards[0].board)
    ki.positionPanel(preset["page"], panel)

    ki.runUserScript(preset["post"], panel)

    ki.buildDebugAnnotation(preset["debug"], panel)

    panel.save(reconstructArcs=preset["post"]["reconstructarcs"],
            refillAllZones=preset["post"]["refillzones"])
